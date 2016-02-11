/**
 * @file   comp_group.h
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Wed Feb 03 10:04:50 NZDT 2016
 *
 * @brief  Classes representing component groups.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */
#ifndef _COMP_GROUP_H_
#define _COMP_GROUP_H_

#include "maq.h"
#include "relationships.h"
#include "fragility.h"
#include "loss_functions.h"

namespace SLAT {
    class CompGroup {
    public:
        CompGroup() {};
        CompGroup(std::shared_ptr<CompoundRateRelationship> edp,
                  std::shared_ptr<FragilityFn> frag_fn,
                  std::shared_ptr<LossFn> loss_fn, int count);
        double E_loss_EDP(double edp);
        double SD_ln_loss_EDP(double edp);

        double E_loss_IM(double im);
        double SD_ln_loss_IM(double im);
        ~CompGroup() {};
        Integration::IntegrationSettings local_settings;
        static Integration::IntegrationSettings class_settings;
    private:
        std::shared_ptr<CompoundRateRelationship> edp;
        std::shared_ptr<FragilityFn> frag_fn;
        std::shared_ptr<LossFn> loss_fn;
        int count;
    };
}
#endif
