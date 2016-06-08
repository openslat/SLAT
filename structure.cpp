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
#include "comp_group.h"
#include "structure.h"

namespace SLAT {
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
    
    LogNormalDist Structure::Loss(double im, bool consider_collapse)
    {
        double mu_nc, sigma_nc = 0;
        for_each(components.begin(),
                 components.end(), 
                 [&mu_nc, &sigma_nc, im] (std::shared_ptr<CompGroup> cg) {
                     mu_nc = mu_nc + cg->E_loss_IM(im);
                     sigma_nc = sigma_nc + cg->SD_ln_loss_IM(im) *
                         cg->SD_ln_loss_IM(im);
                 });
        LogNormalDist nc_dist = LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(mu_nc, sqrt(sigma_nc));


        if (consider_collapse) {
            std::vector<LogNormalDist> dists = { nc_dist, rebuild_cost };
            double pCollapse = this->im->pCollapse(im);
            std::vector<double> weights = { 1.0 - pCollapse, pCollapse };
            
            LogNormalDist c_dist = LogNormalDist::AddWeightedDistributions(dists, weights); 
            return c_dist;
        } else {
            return nc_dist;
        }
    }
}
