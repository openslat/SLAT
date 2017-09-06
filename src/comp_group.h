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
    /**
     * Represents a 'component group' in an analysis. Each group consists of:
     *   - a number of components
     *   - an EDP, describing the engineering demand placed 
     *     on the components
     *   - a FragilityFn, describing the relationship between
     *      engineering demand and the onset of each damage 
     *      state
     *   - A LossFn, describing the cost for each damage
     *     state (may be NULL)
     *   - A LossFn, describing the delay time for each
     *     damage state (may be NULL)
     *   - An optional name, which will be used in error 
     *     and debugging messages
     */
    class CompGroup {
    public:
        /**
         * Full constructor, requiring all parameters including name
         */
        CompGroup(std::shared_ptr<EDP> edp,
                  std::shared_ptr<FragilityFn> frag_fn,
                  std::shared_ptr<LossFn> cost_fn, 
                  std::shared_ptr<LossFn> delay_fn, 
                  int count,
                  std::string name);

        /**
         * Abbreviated constructor; used default name ("Anonymous CompGroup").
         */
        CompGroup(std::shared_ptr<EDP> edp,
                  std::shared_ptr<FragilityFn> frag_fn,
                  std::shared_ptr<LossFn> cost_fn, 
                  std::shared_ptr<LossFn> delay_fn, 
                  int count)
            : CompGroup(edp, frag_fn, cost_fn, delay_fn, count, "Anonymous CompGroup")
        {
        }

        /**
         * Unregister callbacks on destruction.
         */
        ~CompGroup()
        {
            
            edp->remove_callbacks(edp_callback_id);
            frag_fn->remove_callbacks(frag_fn_callback_id);

            if (cost_fn) {
                cost_fn->remove_callbacks(cost_fn_callback_id);
            }

            if (delay_fn) {
                delay_fn->remove_callbacks(delay_fn_callback_id);
            }
        };

        /*
         * Relationships between EDP, and cost and delay. Since these are simple
         * calculations, they are not cached.
         */
        /**
         * Expected (mean) cost, given EDP.
         */
        double E_cost_EDP(double edp);

        /**
         * The standard deviation of ln(cost), given EDP.
         */
        double SD_ln_cost_EDP(double edp);

        /**
         * The standard deviation of cost, given EDP.
         */
        double SD_cost_EDP(double edp);

        /**
         * Expected (mean) delay, given EDP.
         */
        double E_delay_EDP(double edp);

        /**
         * The standard deviation of ln(delay), given EDP.
         */
        double SD_ln_delay_EDP(double edp);
        
        /**
         * The standard deviation of cost, given EDP.
         */
        double SD_delay_EDP(double edp);
        
        
        /*
         * Relationships between IM, and cost and delay. These values require
         * integration, and so are cached.
         */
        /**
         * Expected (mean) cost, given IM.
         */
        Caching::CachedFunction<double, double> E_cost_IM;

        /**
         * Standard deviation of ln(cost), given IM.
         */
        Caching::CachedFunction<double, double> SD_ln_cost_IM;

        /**
         * Expected (mean) delay, given IM.
         */
        Caching::CachedFunction<double, double> E_delay_IM;

        /**
         * Standard deviation of ln(delay), given IM.
         */
        Caching::CachedFunction<double, double> SD_ln_delay_IM;


        /**
         * Expected (mean) annual cost (a cached value)
         */
        Caching::CachedValue<double> E_annual_cost;

        /**
         * The expected rate of exceedence of cost (a cached value).
         */
        Caching::CachedFunction<double, double> lambda_cost;

        /**
         * The (lognormal) distribution of cost given EDP.
         */
        Caching::CachedFunction<LogNormalDist, double> cost_EDP_dist;

        /**
         * The (lognormal) distribution of delay given EDP.
         */
        Caching::CachedFunction<LogNormalDist, double> delay_EDP_dist;

        /**
         * Mean ln(cost), given EDP.
         */
        double mean_ln_cost_EDP(double edp);

        /**
         * Provide a vector of the expected rate of occurance for each damage state. 
         */
        Caching::CachedValue<std::vector<double>> Rate;
        
        /**
         * Provide a vector of the probability of exceeding the onset of each
         * damaage state, given IM.
         */
        std::vector<double> pDS_IM(double im);


        /**
         * Return the (lognormal) distribution of cost, given IM.
         */
        LogNormalDist CostDist_IM(double im);


        /*
         * Access functions
         */
        /**
         * Retrieve the fragility function of the component group.
         * @return The FragilityFn.
         */
        std::shared_ptr<FragilityFn> FragFn(void);

        /**
         * Retrieve the cost function of the component group.
         * @return The cost_fn.
         */
        std::shared_ptr<LossFn> CostFn(void);
        
        /**
         * Retrieve the delay function of the component group.
         * @return The delay_fn.
         */
        std::shared_ptr<LossFn> DelayFn(void);
        
        /**
         * Retrieve eh intensity measure that affects the component group.
         * @return The IM.
         */
        std::shared_ptr<IM> getIM(void) { return edp->Base_Rate(); };

        /**
         * Retrieve the demand parameter that effects the component group.
         * @return The EDP.
         */
        std::shared_ptr<EDP> get_EDP(void) { return edp; };

        /**
         * Retrieve the fragility function of the component group.
         * @return The FragilityFn.
         * @todo Is this redundant, given FragFn()?
         */
        std::shared_ptr<FragilityFn> get_Fragility(void) { return frag_fn; };

        /**
         * Retrive the string identifying the component group.
         * @return The name string.
         */
        std::string get_Name(void) const { return name; };

        /**
         * Overloaded << operator for printing a CompGroup to a stream. Intended
         * for debugging.
         */
        friend std::ostream& operator<<(std::ostream& out, const CompGroup& o);

    private:
        /** 
         * A string identifying the object; used in debugging, warning, and
         * error messages.
         */
        std::string name;

        /**
         * The EDP affecting this component group.
         */
        std::shared_ptr<EDP> edp;

        /**
         * The fragility function, describing the likelihood of each damage 
         * state based on the engineering demand.
         */
        std::shared_ptr<FragilityFn> frag_fn;

        /**
         * A loss function describing the cost of repair for each damage state.
         */
        std::shared_ptr<LossFn> cost_fn;

        /**
         * A loss function describing the delay incurred by each damage state.
         */
        std::shared_ptr<LossFn> delay_fn;

        /**
         * The number of components in the group.
         */
        int count;

        /**
         * ID returned when installing a callback for the EDP function. When the
         * EDP is changed or replaced, clear the caches.
         */
        int edp_callback_id;

        /**
         * ID returned when installing a callback for the fragility function. When the
         * fragility function is changed or replaced, clear the caches.
         */
        int frag_fn_callback_id;

        /**
         * ID returned when installing a callback for the cost function. When the
         * cost function is changed or replaced, clear the caches.
         */
        int cost_fn_callback_id;

        /**
         * ID returned when installing a callback for the delay function. When the
         * delay function is changed or replaced, clear the caches.
         */
        int delay_fn_callback_id;
        
        /*
         * Functions used to calculate cached values 
         */
        double E_cost_IM_calc(double im); ///< Calculate mean(cost) | IM
        double SD_ln_cost_IM_calc(double im); ///< Calculate sd(ln(cost)) | IM

        double E_delay_IM_calc(double im); ///< Calculate mean(delay) | IM
        double SD_ln_delay_IM_calc(double im); ///< Calculate sd(ln(cost)) | IM
        
        double E_annual_cost_calc(void); ///< Calculate mean(annual cost) | IM
        double lambda_cost_calc(double cost); ///< Calculate rate of cost

        /**
         * Calculate the probability of exceeding the onset value for each
         * damage state given IM.
         */
        double pDS_IM_calc(std::pair<int, double> params);

        std::vector<double> calc_Rate(void); ///< Calculate the rate of
                                             ///exceedence of each damage state
    };
}
#endif
