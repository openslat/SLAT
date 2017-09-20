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
    
    const std::vector<std::shared_ptr<CompGroup>> Structure::Components(void)
    {
        std::vector<std::shared_ptr<CompGroup>> result(components.size());
        for (size_t i=0; i < components.size(); i++) {
            result[i] = components[i]->first;
        }        
        return result;
    };
    
    double Structure::pdf(double im) 
    {
        return TotalCost(im).get_mean_X() * std::abs(this->im->DerivativeAt(im));
    };


    void  Structure::AddCompGroup(std::shared_ptr<CompGroup> cg)
    {
        if (components.size() == 0) {
            im = cg->getIM();
            im->add_callbacks(
                [this] (void) {
                    this->Clear_Cache();
                    std::cout << "Structure: IM Changed; clearing cache." << std::endl;
                },
                [this] (std::shared_ptr<IM> new_im) {
                    this->Clear_Cache();
                    this->im = new_im;
                    std::cout << "Structure: IM Replaced; clearing cache." << std::endl;
                });
        }
        if (cg->getIM() != im) {
            throw std::invalid_argument("WRONG IM");
        }
        std::shared_ptr<CG_ENTRY> entry = std::make_shared<CG_ENTRY>(cg, 0);
        entry->second = cg->add_callbacks(
            [this] (void) {
                this->Clear_Cache();
            },
            [this, entry] (std::shared_ptr<CompGroup> new_cg) {
                this->Clear_Cache();
                entry->first = new_cg;
            });
        components.push_back(entry);
        this->Clear_Cache();
    }
    
    void Structure::RemoveCompGroup(std::shared_ptr<CompGroup> cg)
    {
        for (size_t i=0; i < components.size(); i++) {
            if (components[i]->first == cg) {
                std::cout << "REMOVING " << components[i]->first << std::endl;
                components.erase(components.begin()+i);
                break;
            }
        }
    }
    
    LogNormalDist Structure::CostNC(double im)
    {
        std::vector<LogNormalDist> dists;
        int num_components = 0;
        for_each(components.begin(),
                 components.end(), 
                 [&num_components, &dists, im] (std::shared_ptr<CG_ENTRY> cg_entry) {
                     num_components++;
                     LogNormalDist dist = cg_entry->first->CostDist_IM(im);
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

    double Structure::E_cost(int year, double discount_rate)
    {
        return ((1.0 - exp(-discount_rate * year)) / discount_rate) * AnnualCost().get_mean_X();
    }

    std::vector<double> Structure::E_cost(std::vector<int> years, double discount_rate)
    {
        std::vector<double> result(years.size());
#pragma omp parallel for
        for (size_t i=0; i < years.size(); i++) {
            result[i] = E_cost(years[i], discount_rate);
        }
        return result;
    }

    std::pair<LogNormalDist, LogNormalDist> Structure::DeaggregatedCost(double im)
    {
        LogNormalDist cost_nc = CostNC(im);
        LogNormalDist cost_c = rebuild_cost;
        LogNormalDist cost_demo = demolition_cost;
        double p = this->im->pCollapse(im);

        return std::make_pair(cost_nc.ScaleDistributionByProbability(1.0 - p),
                              cost_c.ScaleDistributionByProbability(p));
    }

    Structure::COSTS Structure::CostsByFate(double im)
    {
        COSTS result;
        if (this->im != NULL) {
            result.repair = CostNC(im).ScaleDistributionByProbability(this->im->pRepair(im));
            result.demolition = demolition_cost.ScaleDistributionByProbability(this->im->pDemolition(im));
            result.collapse = rebuild_cost.ScaleDistributionByProbability(this->im->pCollapse(im));
        }
        return result;
    }

    LogNormalDist Structure::calc_AnnualCost(void)
    {
        TempContext context([this] (std::ostream &o) {
                o << "Structure::calc_AnnualCost() [" << this->name << "]";
            });
        double mu=NAN, beta=NAN;
        if (components.size() > 0) {
#pragma omp parallel sections
            {
#pragma omp section
                {
                    Integration::MAQ_RESULT result;
                    result = Integration::MAQ(
                        [this] (double im) -> double {
                            double cost = this->TotalCost(im).get_mean_X();
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
                            double cost = this->TotalCost(im).get_mean_X();
                            double sd = this->TotalCost(im).get_sigma_X();
                            double deriv = std::abs(this->im->DerivativeAt(im));
                            return (cost * cost + sd * sd) * deriv ;
                        });
                    if (result.successful) {
                        beta = result.integral;
                    } 
                }
            }
            beta = sqrt(std::abs(beta - mu * mu));        
        }
        return LogNormalDist::LogNormalDist_from_mean_X_and_sigma_X(mu, beta);
    }

    void Structure::Clear_Cache(void)
    {
        AnnualCost.ClearCache();
    }
}
