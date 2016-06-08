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
                 [&mu_nc, im] (std::shared_ptr<CompGroup> cg) {
                     mu_nc = mu_nc + cg->E_loss_IM(im);
                 });

        if (consider_collapse) {
            double pCollapse = this->im->pCollapse(im);
            double mu_c = mu_nc * (1.0 - pCollapse) + rebuild_cost.get_mean_X() * pCollapse;
            double sigma_c = 0;
            return LogNormalDist::LogNormalDist_from_mu_lnX_and_sigma_lnX(mu_c, sigma_c);
        } else {
            return LogNormalDist::LogNormalDist_from_mu_lnX_and_sigma_lnX(mu_nc, sigma_nc);
        }
    }
}
