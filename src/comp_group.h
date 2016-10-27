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

#include <vector>
#include "maq.h"
#include "relationships.h"
#include "fragility.h"
#include "loss_functions.h"
#include "caching.h"

namespace SLAT {
    class CompGroup {
    public:
        CompGroup(std::shared_ptr<EDP> edp,
                  std::shared_ptr<FragilityFn> frag_fn,
                  std::shared_ptr<LossFn> cost_fn, 
                  std::shared_ptr<LossFn> delay_fn, 
                  int count,
                  std::string name);
        CompGroup(std::shared_ptr<EDP> edp,
                  std::shared_ptr<FragilityFn> frag_fn,
                  std::shared_ptr<LossFn> cost_fn, 
                  std::shared_ptr<LossFn> delay_fn, 
                  int count)
            : CompGroup(edp, frag_fn, cost_fn, delay_fn, count, "Anonymous CompGroup")
        {
        }

        double E_cost_EDP(double edp);
        double SD_ln_cost_EDP(double edp);
        double SD_cost_EDP(double edp);

        double E_delay_EDP(double edp);
        double SD_ln_delay_EDP(double edp);
        
        ~CompGroup() {};
        Integration::IntegrationSettings local_settings;
        static Integration::IntegrationSettings class_settings;
        
        Caching::CachedFunction<double, double> E_cost_IM;
        Caching::CachedFunction<double, double> SD_ln_cost_IM;
        Caching::CachedValue<double> E_annual_cost;
        Caching::CachedFunction<double, double> lambda_cost;
        Caching::CachedFunction<LogNormalDist, double> cost_EDP_dist;
        Caching::CachedFunction<LogNormalDist, double> delay_EDP_dist;
        std::shared_ptr<FragilityFn> FragFn(void);
        double E_cost(int years, double discount_rate);
        double mean_ln_cost_EDP(double edp);
        LogNormalDist CostDist_IM(double im);
        std::shared_ptr<IM> getIM(void) { return edp->Base_Rate(); };
        std::shared_ptr<EDP> get_EDP(void) { return edp; };
        std::shared_ptr<FragilityFn> get_Fragility(void) { return frag_fn; };
        std::string get_Name(void) { return name; };
        Caching::CachedValue<std::vector<double>> Rate; 
        std::vector<double> pDS_IM(double im);
    private:
        std::string name;
        std::shared_ptr<EDP> edp;
        std::shared_ptr<FragilityFn> frag_fn;
        std::shared_ptr<LossFn> cost_fn;
        std::shared_ptr<LossFn> delay_fn;
        int count;

        double E_cost_IM_calc(double im);
        double SD_ln_cost_IM_calc(double im);

        double E_annual_cost_calc(void);
        double lambda_cost_calc(double cost);

        double pDS_IM_calc(std::pair<int, double> params);
        std::vector<double> calc_Rate(void);
    };
}
#endif
