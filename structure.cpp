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
        std::vector<LogNormalDist> dists;
        int num_components = 0;
        for_each(components.begin(),
                 components.end(), 
                 [&num_components, &dists, im] (std::shared_ptr<CompGroup> cg) {
                     num_components++;
                     dists.push_back(cg->LossDist_IM(im));
                 });

        std::cerr << "Loss(" << im << ", " << consider_collapse << ")" << std::endl;
        for (size_t i=0; i < dists.size(); i++) {
            std::cerr << "    " << dists[i].get_mean_X() << ", " << dists[i].get_sigma_X() << std::endl;
        }
        LogNormalDist nc_dist = LogNormalDist::AddDistributions(dists);


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
