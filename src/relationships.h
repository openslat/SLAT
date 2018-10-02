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
        int callback_id;
        std::shared_ptr<DeterministicFn> f;
        std::shared_ptr<LogNormalDist> collapse = NULL;
        std::shared_ptr<LogNormalDist> demolition = NULL;
    public:
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
         * Returns the probability of exceedence at given values.
         * 
         * @param x The values for which we want to know the probability.
         * 
         * @return The probabilities the of exceedence.
         */
        std::vector<double> lambda(std::vector<double> x);
        
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
            CollapseRate.ClearCache();
            collapse = new_collapse;
            notify_change();
        }

        void SetCollapse(LogNormalDist new_collapse)
        {
            CollapseRate.ClearCache();
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

        std::vector<double> pDemolition(std::vector<double> im);
        std::vector<double> pCollapse(std::vector<double> im);
        std::vector<double> pRepair(std::vector<double> im);

        double pRepair(double im) {
            return 1.0 - (pDemolition(im) + pCollapse(im));
        }

        Caching::CachedValue<double> CollapseRate;

        void SetDemolition(std::shared_ptr<LogNormalDist> new_demolition)
        {
            DemolitionRate.ClearCache();
            demolition = new_demolition;
            notify_change();
        }

        void SetDemolition(LogNormalDist new_demolition)
        {
            DemolitionRate.ClearCache();
            demolition = std::make_shared<LogNormalDist>(new_demolition);
            notify_change();
        }

        double pDemolition(double im) {
            if (demolition) {
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
        int callback_id;

    public:
        Caching::CachedFunction<double, double> lambda;
        std::vector<double> par_lambda(std::vector<double> args);
        
        /** 
         * Returns the derivative of lambda at the given point.
         * 
         * @param x The value at which we want to know the derivative.
         * 
         * @return The derivative of the relationship at x.
         */
        virtual double DerivativeAt(double x) ;

        virtual std::string ToString(void) const;
            
        friend std::ostream& operator<<(std::ostream& out, const EDP& o);

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
        double X_at_exceedence(double x, double p) const;
        
        virtual double Mean(double base_value) const;
        virtual double MeanLn(double base_value) const;
        virtual double Median(double base_value) const;
        virtual double SD_ln(double base_value) const;
        virtual double SD(double base_value) const;

        std::vector<double> Mean(std::vector<double>base_value) const;
        std::vector<double> MeanLn(std::vector<double>base_value) const;
        std::vector<double> Median(std::vector<double>base_value) const;
        std::vector<double> SD_ln(std::vector<double>base_value) const;
        std::vector<double> SD(std::vector<double>base_value) const;
        
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

/**
 * @brief CompoundEDP
 *
 * A 'CompoundEDP' serves the same function as an EDP object, but uses the maximum
 * of two EDPs as its demand function.
 */
    class CompoundEDP : public EDP
    {
    protected:
        enum DIRECTION { FIRST, SECOND };
        DIRECTION WhichToUse(double im) const;
        int second_dependent_rate_callback_id;
        std::shared_ptr<ProbabilisticFn> second_dependent_rate; /**< Dependent function */

    public:
        /** 
         * Create a compound EDP relationship given shared pointers to a rate
         * relationship and two probabilistic functions.
         * 
         * @param base_rate        A shared pointer to a rate relationship
         * @param dependent_rate_1  A shared pointer to a probabilistic function.
         * @param dependent_rate_2  A shared pointer to a probabilistic function.
         * 
         * @return 
         */
        CompoundEDP(std::shared_ptr<IM> base_rate,
                     std::shared_ptr<ProbabilisticFn> dependent_rate_1,
                     std::shared_ptr<ProbabilisticFn> dependent_rate_2,
                     std::string name);
        CompoundEDP(std::shared_ptr<IM> base_rate,
                     std::shared_ptr<ProbabilisticFn> dependent_rate_1,
                     std::shared_ptr<ProbabilisticFn> dependent_rate_2)
        : CompoundEDP(base_rate, dependent_rate_1,
                       dependent_rate_2, "Anonymous Compound_EDP") {};

        virtual ~CompoundEDP() {
            base_rate->remove_callbacks(base_rate_callback_id);
            dependent_rate->remove_callbacks(dependent_rate_callback_id);
            second_dependent_rate->remove_callbacks(second_dependent_rate_callback_id);
        }; /**< Destructor; unregister callbacks */

        virtual double Mean(double base_value) const;
        virtual double MeanLn(double base_value) const;
        virtual double Median(double base_value) const;
        virtual double SD_ln(double base_value) const;
        virtual double SD(double base_value) const;
    };
}
#endif
