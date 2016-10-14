/**
 * @file   relationships.h
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief  Classes representing relationships involving mathematical functions.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */
#ifndef _RELATIONSHIPS_H_
#define _RELATIONSHIPS_H_

#include <unordered_map>
#include "functions.h"
#include "maq.h"
#include "replaceable.h"
#include "caching.h"
#include "lognormaldist.h"
#include <iostream>

namespace SLAT {

/**
 * @brief IM
 * 
 * An 'IM' object represents an intensity measurement function. It is
 * constructed from a deterministic function, and can report the rate of
 * exceedence, and derivative.
 */
    class IM : public Replaceable<IM>
    {
    protected:
        Integration::IntegrationSettings local_settings;
        static Integration::IntegrationSettings class_settings;
        
        int callback_id;
        std::shared_ptr<DeterministicFn> f;
        std::shared_ptr<LogNormalDist> collapse = NULL;
        std::shared_ptr<LogNormalDist> demolition = NULL;
    public:
        /**
         * Returns class integration settings:
         */
        static Integration::IntegrationSettings &Get_Class_Integration_Settings(void);

        /**
         * Returns object integration settings:
         */
        Integration::IntegrationSettings &Get_Integration_Settings(void);

        IM(std::shared_ptr<DeterministicFn> func, std::string name);
        IM(std::shared_ptr<DeterministicFn> func) : IM(func, "Anonymous IM") {};

        ~IM() {
            f->remove_callbacks(callback_id);
        }; /**< Destructor; uninstall callbacks */

        /** 
         * Returns the probability of exceedence at a given value.
         * 
         * @param x The value for which we want to know the probability.
         * 
         * @return The probability the of exceedence.
         */
        double lambda(double x);
        
        /** 
         * Returns the derivative of lambda at the given point.
         * 
         * @param x The value at which we want to know the derivative.
         * 
         * @return The derivative of the relationship at x.
         */
        virtual double DerivativeAt(double x) ;

        void SetCollapse(std::shared_ptr<LogNormalDist> new_collapse)
        {
            collapse = new_collapse;
            notify_change();
        }

        void SetCollapse(LogNormalDist new_collapse)
        {
            collapse = std::make_shared<LogNormalDist>(new_collapse);
            notify_change();
        }

        double pCollapse(double im) {
            if (collapse) {
                return collapse->p_at_most(im);
            } else {
                return 0;
            }
        }

        double pRepair(double im) {
            return 1.0 - (pDemolition(im) + pCollapse(im));
        }

        Caching::CachedValue<double> CollapseRate;

        void SetDemolition(std::shared_ptr<LogNormalDist> new_demolition)
        {
            demolition = new_demolition;
            notify_change();
        }

        void SetDemolition(LogNormalDist new_demolition)
        {
            demolition = std::make_shared<LogNormalDist>(new_demolition);
            notify_change();
        }

        double pDemolition(double im) {
            if (demolition) {
                // @TODO Make sure this is at least zero!
                return demolition->p_at_most(im) - pCollapse(im);
            } else {
                return 0;
            }
        }

        Caching::CachedValue<double> DemolitionRate;
        
        virtual std::string ToString(void) const;
            
        friend std::ostream& operator<<(std::ostream& out, const IM& o);
    private:
        double CollapseRate_calc(void);
        double DemolitionRate_calc(void);
        std::string name;
    };

    
/**
 * @brief EDP
 * 
 * An 'EDP' object represents the relationship between a probabilistic EDP
 * function, and the related IM.
 */
    class EDP : public Replaceable<EDP>
    {
    protected:
        Integration::IntegrationSettings local_settings;
        static Integration::IntegrationSettings class_settings;
        int callback_id;

    public:
        Caching::CachedFunction<double, double> lambda;
        
        /** 
         * Returns the derivative of lambda at the given point.
         * 
         * @param x The value at which we want to know the derivative.
         * 
         * @return The derivative of the relationship at x.
         */
        virtual double DerivativeAt(double x) ;

        /**
         * Returns class integration settings:
         */
        static Integration::IntegrationSettings &Get_Class_Integration_Settings(void);

        /**
         * Returns object integration settings:
         */
        Integration::IntegrationSettings &Get_Integration_Settings(void);

        virtual std::string ToString(void) const;
            
        friend std::ostream& operator<<(std::ostream& out, const EDP& o);

        /** 
         * Set the default tolerance and evaluations limit used by the
         * integrator when calculating lambda for CompoundRateRelationships.
         * 
         * @param tol   Tolerance
         * @param evals Maximum number of evaluations
         */
        static void SetDefaultIntegrationParameters(double tol, unsigned int evals);

        /** 
         * Get the default tolerance and evaluations limit used by the
         * integrator when calculating lambda for CompoundRateRelationships.
         * 
         * @param tol    Returned with the tolerance.
         * @param evals  Returned with the evaluations limit.
         */
        static void GetDefaultIntegrationParameters(double &tol, unsigned int &evals);

        /** 
         * Create a compound relationship given shared pointers to a rate
         * relationship and a probabilistic function.
         * 
         * @param base_rate       A shared pointer to a rate relationship
         * @param dependent_rate  A shared pointer to a probabilistic function.
         * 
         * @return 
         */
        EDP(std::shared_ptr<IM> base_rate,
            std::shared_ptr<ProbabilisticFn> dependent_rate,
            std::string name);
        EDP(std::shared_ptr<IM> base_rate,
            std::shared_ptr<ProbabilisticFn> dependent_rate)
            : EDP(base_rate, dependent_rate, "Anonymous EDP") {};

        virtual ~EDP() {
            base_rate->remove_callbacks(base_rate_callback_id);
            dependent_rate->remove_callbacks(dependent_rate_callback_id);
        }; /**< Destructor; unregister callbacks */

        /** 
         * Return the probability of exceeding a given value in the complex
         * relationship. This is calculated by looking at all values of the base
         * rate, and determing 1) how likely that value is to be encountered, and 2)
         * given that base rate, how likely is the dependent function to be
         * exceeded?
         * 
         * @param x  The value at which we want to know the probability.
         * 
         * @return The probability that the value will exceed x.
         */
        double calc_lambda(double x);

        double P_exceedence(double base_value, double min_dependent_value) const;
        
        double Mean(double base_value) const;
        double MeanLn(double base_value) const;
        double Median(double base_value) const;
        double SD_ln(double base_value) const;
        double SD(double base_value) const;
        
        std::shared_ptr<IM> Base_Rate(void) { return base_rate; };
        
        std::string get_Name(void) { return name; }
    protected:
        std::string name;
        std::shared_ptr<IM> base_rate; /**< Base rate relationship */
        std::shared_ptr<ProbabilisticFn> dependent_rate; /**< Dependent function */

        double tolerance;
        unsigned int max_evaluations;

        int base_rate_callback_id, dependent_rate_callback_id;

        static double default_tolerance;
        static unsigned int default_max_evaluations;
    };
}
#endif
