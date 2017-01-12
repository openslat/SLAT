/**
 * @file   structure.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Wed Feb 03 10:04:50 NZDT 2016
 *
 * @brief  Classes representing structures (composed on component groups).
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */

#include <memory>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <utility>
#include "comp_group.h"
#include "structure.h"
#include "maq.h"

namespace SLAT {
    Structure::Structure(std::string name) : AnnualCost([this] (void) { return this->calc_AnnualCost(); })
    {
        this->name = name;
    };
    
    void  Structure::AddCompGroup(std::shared_ptr<CompGroup> cg)
    {
        //std::cout << "> Structure::AddCompGroup(): " << cg << std::endl;
        if (components.size() == 0) {
            //std::cout << "...getIM()" << std::endl;
            im = cg->getIM();
            //std::cout << "...done()" << std::endl;
        }
        if (cg->getIM() != im) {
            throw std::invalid_argument("WRONG IM");
        }
        components.push_back(cg);
    }
    
    LogNormalDist Structure::CostNC(double im)
    {
        std::vector<LogNormalDist> dists;
        int num_components = 0;
        for_each(components.begin(),
                 components.end(), 
                 [&num_components, &dists, im] (std::shared_ptr<CompGroup> cg) {
                     num_components++;
                     LogNormalDist dist = cg->CostDist_IM(im);
                     dists.push_back(dist);
                 });
        return LogNormalDist::AddDistributions(dists);
    }

    LogNormalDist Structure::Cost(double im, bool consider_collapse)
    {
        LogNormalDist nc_dist = CostNC(im);

        if (consider_collapse) {
            std::vector<LogNormalDist> dists = { nc_dist, rebuild_cost };
            if (this->im != NULL) {
                double pCollapse = this->im->pCollapse(im);
                std::vector<double> weights = { 1.0 - pCollapse, pCollapse };
            
                LogNormalDist c_dist = LogNormalDist::AddWeightedDistributions(dists, weights); 
                return c_dist;
            } else {
                return LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(NAN, NAN);
            };
        } else {
            return nc_dist;
        }
    }

    LogNormalDist Structure::TotalCost(double im)
    {
        LogNormalDist cost_repair = CostNC(im);
        LogNormalDist cost_demolition = demolition_cost;
        LogNormalDist cost_collapse = rebuild_cost;
        
        double E_cost = cost_repair.get_mean_X() * this->im->pRepair(im)
            + cost_demolition.get_mean_X() * this->im->pDemolition(im)
            + cost_collapse.get_mean_X() * this->im->pCollapse(im); 
        
        double E_sq_cost_repair = cost_repair.get_mean_X() * cost_repair.get_mean_X() +
            cost_repair.get_sigma_X() * cost_repair.get_sigma_X();
        double E_sq_cost_demolition = cost_demolition.get_mean_X() * cost_demolition.get_mean_X() +
            cost_demolition.get_sigma_X() * cost_demolition.get_sigma_X();
        double E_sq_cost_collapse = cost_collapse.get_mean_X() * cost_collapse.get_mean_X() +
            cost_collapse.get_sigma_X() * cost_collapse.get_sigma_X();

        double E_sq_cost = E_sq_cost_repair * this->im->pRepair(im)
            + E_sq_cost_demolition * this->im->pDemolition(im)
            + E_sq_cost_collapse * this->im->pCollapse(im);
        
        double var_cost = E_sq_cost - E_cost * E_cost;
        double sigma_cost = sqrt(var_cost);
        return LogNormalDist::LogNormalDist_from_mean_X_and_sigma_X(E_cost, sigma_cost);
    }

    std::pair<LogNormalDist, LogNormalDist> Structure::DeaggregatedCost(double im)
    {
        LogNormalDist cost_nc = CostNC(im);
        LogNormalDist cost_c = rebuild_cost;
        LogNormalDist cost_demo = demolition_cost;
        double p = this->im->pCollapse(im);

        return std::make_pair(cost_nc.WeighDistribution(1.0 - p),
                              cost_c.WeighDistribution(p));
    }

    Structure::COSTS Structure::CostsByFate(double im)
    {
        COSTS result;
        result.repair = CostNC(im).WeighDistribution(this->im->pRepair(im));
        result.demolition = demolition_cost.WeighDistribution(this->im->pDemolition(im));
        result.collapse = rebuild_cost.WeighDistribution(this->im->pCollapse(im));
        return result;
    }

    LogNormalDist Structure::calc_AnnualCost(void)
    {
        double mu=NAN, beta=NAN;
#pragma omp parallel sections
        {
#pragma omp section
            {
                Integration::MAQ_RESULT result;
                result = Integration::MAQ(
                    [this] (double im) -> double {
                        double cost = this->Cost(im, true).get_mean_X();
                        double deriv = std::abs(this->im->DerivativeAt(im));
                        return cost * deriv;
                    });
                if (result.successful) {
                    mu = result.integral;
                } 
            }

#pragma omp section
            {
                Integration::MAQ_RESULT result;
                result = Integration::MAQ(
                    [this] (double im) -> double {
                        double cost = this->Cost(im, true).get_mean_X();
                        double sd = this->Cost(im, true).get_sigma_X();
                        double deriv = std::abs(this->im->DerivativeAt(im));
                        return (cost * cost + sd * sd) * deriv ;
                    });
                if (result.successful) {
                    beta = result.integral;
                } 
            }
        }
        beta = sqrt(std::abs(beta - mu * mu));        
        return LogNormalDist::LogNormalDist_from_mean_X_and_sigma_X(mu, beta);
    }
}
