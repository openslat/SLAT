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
    
    double Structure::Loss(double im, bool consider_collapse)
    {
        double total = 0;
        for_each(components.begin(),
                 components.end(), 
                 [&total, im] (std::shared_ptr<CompGroup> cg) {
                     total = total + cg->E_loss_IM(im);
                 });
        if (consider_collapse) {
            double pCollapse = this->im->pCollapse(im);
            return total * (1.0 - pCollapse) + 14E6 * pCollapse;
        } else {
            return total;
        }
    }
}
