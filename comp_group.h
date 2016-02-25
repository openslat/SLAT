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
#include "caching.h"

namespace SLAT {
    class CompGroup {
    public:
        CompGroup() {};
        CompGroup(std::shared_ptr<CompoundRateRelationship> edp,
                  std::shared_ptr<FragilityFn> frag_fn,
                  std::shared_ptr<LossFn> loss_fn, int count);
        double E_loss_EDP(double edp);
        double SD_ln_loss_EDP(double edp);
        double SD_loss_EDP(double edp);

        ~CompGroup() {};
        Integration::IntegrationSettings local_settings;
        static Integration::IntegrationSettings class_settings;
        
        Caching::CachedFunction<double, double> E_loss_IM;
        Caching::CachedFunction<double, double> SD_ln_loss_IM;
        Caching::CachedValue<double> E_annual_loss;
        Caching::CachedFunction<double, double> lambda_loss;
        Caching::CachedFunction<LogNormalDist, double> loss_EDP_dist;
        double E_loss(int years, double discount_rate);
    private:
        std::shared_ptr<CompoundRateRelationship> edp;
        std::shared_ptr<FragilityFn> frag_fn;
        std::shared_ptr<LossFn> loss_fn;
        int count;

        double E_loss_IM_calc(double im);
        double SD_ln_loss_IM_calc(double im);

        double E_annual_loss_calc(void);
        double lambda_loss_calc(double loss);
    };
}
#endif
