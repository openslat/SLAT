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
#include "functions.h"
#include "maq.h"
#ifndef _RELATIONSHIPS_H_
#define _RELATIONSHIPS_H_

namespace SLAT {

/**
 * @brief Rate Relationship
 * 
 * A 'Rate Relationship' represents the probability of a function exceeding a
 * given value. This is an abstract class.
 */
    class RateRelationship
    {
    public:
        int id;
    protected:
        RateRelationship(void); /**< Default constructor. */
        ~RateRelationship() { };   /**< Default destructor; does nothing. */
        Integration::IntegrationSettings local_settings;
        static Integration::IntegrationSettings class_settings;
    public:
        /** 
         * Returns the probability of exceedence at a given value.
         * 
         * @param x The value for which we want to know the probability.
         * 
         * @return The probability the of exceedence.
         */
        virtual double lambda(double x) const {
            //std::cout << "RateRelationship::lambda()" << std::endl;
            return NAN;
        };
        
        /** 
         * Returns the derivative of lambda at the given point.
         * 
         * @param x The value at which we want to know the derivative.
         * 
         * @return The derivative of the relationship at x.
         */
        virtual double DerivativeAt(double x) const;

        /**
         * Returns class integration settings:
         */
        static Integration::IntegrationSettings &Get_Class_Integration_Settings(void);

        /**
         * Returns object integration settings:
         */
        Integration::IntegrationSettings &Get_Integration_Settings(void);
    };

/**
 * @brief Simple Rate Relationship
 * 
 * A simple rate relationship is defined by a single, deterministic
 * function. The function should describe the probability that a value will
 * exceed a given value. This should be 1 at 0 (i.e., the value is guaranteed to
 * be at least zero), monotonically decreasing to 0.
 */
    class SimpleRateRelationship : public RateRelationship
    {
    public:
        /** 
         * Construct a SimpleRateRelationship given a shared pointer to a
         * deterministic function that describes the rate relationship.
         * 
         * @param func A shared pointer to the function defining the relationship.
         */
        SimpleRateRelationship(std::shared_ptr<DeterministicFunction> func);
        ~SimpleRateRelationship() { }; /**< Destructor does not need to do
                                        * anything. */
        /** 
         * Return the probability of exceeding a given value.
         * 
         * @param x  The value at which we want to know the probability.
         * 
         * @return The probability that the value will exceed x.
         */
        virtual double lambda(double x) const;
    protected:
        std::shared_ptr<DeterministicFunction> f; /**< Function describing the
                                                   * relationship. */
    };

/**
 * @brief Comound Rate Relationship
 * 
 * A compound rate relationship is defined by a rate relationship, and a
 * probabilistic function. For example, given an rate relationship for IM, and a
 * probabilistic function describing the relationship between IM and EDP, a
 * compound relationship will combine them to describe the rate relationship for
 * EDP.
 */
    class CompoundRateRelationship : public RateRelationship
    {
    public:
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
        CompoundRateRelationship(std::shared_ptr<RateRelationship> base_rate,
                                 std::shared_ptr<ProbabilisticFunction> dependent_rate);

        ~CompoundRateRelationship() { }; /**< Destructor; does nothing. */


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
        virtual double lambda(double x);

    protected:
        std::shared_ptr<RateRelationship> base_rate; /**< Base rate relationship */
        std::shared_ptr<ProbabilisticFunction> dependent_rate; /**< Dependent function */

        double tolerance;
        unsigned int max_evaluations;

        static double default_tolerance;
        static unsigned int default_max_evaluations;
    };
}
#endif
