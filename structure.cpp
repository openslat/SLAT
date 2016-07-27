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
    Structure::Structure(void) : AnnualLoss([this] (void) { return this->calc_AnnualLoss(); })
    {
    };
    
    void  Structure::AddCompGroup(std::shared_ptr<CompGroup> cg)
    {
        if (components.size() == 0) {
            im = cg->getIM();
        }
        if (cg->getIM() != im) {
            throw std::invalid_argument("WRONG IM");
        }
        components.push_back(cg);
    }
    
    LogNormalDist Structure::LossNC(double im)
    {
        //std::cout << "LossNC(" << im << "):" << std::endl;
        
        std::vector<LogNormalDist> dists;
        int num_components = 0;
        for_each(components.begin(),
                 components.end(), 
                 [&num_components, &dists, im] (std::shared_ptr<CompGroup> cg) {
                     num_components++;
                     dists.push_back(cg->LossDist_IM(im));
//                     std::cout << "    " << cg->LossDist_IM(im) << std::endl;
                 });
//        std::cout << "--> " << LogNormalDist::AddDistributions(dists) << std::endl;
        return LogNormalDist::AddDistributions(dists);
    }

    LogNormalDist Structure::Loss(double im, bool consider_collapse)
    {
        LogNormalDist nc_dist = LossNC(im);

//        std::cout << "Loss(" << im << ", " << consider_collapse << ") --> "
        //                << nc_dist << "...";


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

    std::pair<LogNormalDist, LogNormalDist> Structure::DeaggregatedLoss(double im)
    {
        LogNormalDist loss_nc = LossNC(im);
        LogNormalDist locc_c = rebuild_cost;
        double p = this->im->pCollapse(im);

        return std::make_pair(loss_nc.WeighDistribution(1.0 - p),
                              locc_c.WeighDistribution(p));
    }

    LogNormalDist Structure::calc_AnnualLoss(void)
    {
        double mu=NAN, beta=NAN;
        {
            Integration::MAQ_RESULT result;
            result = Integration::MAQ(
                [this] (double im) -> double {
                    double loss = this->Loss(im, true).get_mean_X();
                    double deriv = std::abs(this->im->DerivativeAt(im));
                    // std::cout << std::setw(15) << im
                    //           << std::setw(15) << loss
                    //           << std::setw(15) << deriv
                    //           << std::setw(15) << loss * deriv
                    //           << std::endl;
                    return loss * deriv;
                });
            if (result.successful) {
                mu = result.integral;
            } 
        }

        {
            Integration::MAQ_RESULT result;
            result = Integration::MAQ(
                [this] (double im) -> double {
                    double loss = this->Loss(im, true).get_mean_X();
                    double sd = this->Loss(im, true).get_sigma_X();
                    double deriv = std::abs(this->im->DerivativeAt(im));
                    // std::cout << std::setw(15) << im
                    //           << std::setw(15) << sd
                    //           << std::setw(15) << loss
                    //           << std::setw(15) << deriv
                    //           << std::setw(15) << (loss * loss - sd * sd) * deriv
                    //           << std::endl;
                    return (loss * loss + sd * sd) * deriv ;
                });
            std::cout << "Succes? " << result.successful << "; Value: " << result.integral << std::endl;
            if (result.successful) {
                // std::cout << result.integral << "-" << (mu * mu) << " --> " 
                //           << "...." << (result.integral - mu * mu) << std::endl
                //           << "..." << std::abs(result.integral - mu * mu) << std::endl
                //           << "..." << sqrt(std::abs(result.integral - mu * mu)) << std::endl;
                beta = sqrt(std::abs(result.integral - mu * mu));
                // std::cout << "beta: " << beta << std::endl;
            } 
        }
        // std::cout << "MU: " << mu << "; beta:" << beta << std::endl;
        {
            LogNormalDist temp = LogNormalDist::LogNormalDist_from_mean_X_and_sigma_X(mu, beta);
            std::cout << temp.get_mean_X() << ", " << temp.get_sigma_lnX() << std::endl;
        }
        return LogNormalDist::LogNormalDist_from_mean_X_and_sigma_X(mu, beta);
    }
}
