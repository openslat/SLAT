/**
 * @file   comp_group.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Wed Feb 03 10:04:50 NZDT 2016
 *
 * @brief  Classes representing component groups.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */

#include "comp_group.h"
#include <vector>

namespace SLAT {
    CompGroup::CompGroup(std::shared_ptr<RateRelationship> edp,
                         std::shared_ptr<FragilityFn> frag_fn, 
                         std::shared_ptr<LossFn> loss_fn,
                         int count)
    {
        this->edp = edp;
        this->frag_fn = frag_fn;
        this->loss_fn = loss_fn;
        this->count = count;
    };

    double CompGroup::E_loss_EDP(double edp)
    {
        LogNormalDist ln_fn = LogNormalDist::AddWeightedDistributions(
            loss_fn->LossFns(), frag_fn->pHighest(edp));
        return ln_fn.get_mean_X();
    }

    double CompGroup::SD_ln_loss_EDP(double edp)
    {
        LogNormalDist ln_fn = LogNormalDist::AddWeightedDistributions(
            loss_fn->LossFns(), frag_fn->pHighest(edp));
        return ln_fn.get_sigma_lnX();
    }
}
