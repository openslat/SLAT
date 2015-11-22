/**
 * @file   relationships.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief Implementation of classes representing relationships involving
 * mathematical functions.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */
#include <iostream>
#include "gsl/gsl_cdf.h"
#include "gsl/gsl_deriv.h"

#include "relationships.h"
#include "maq.h"

namespace SLAT {

    double CompoundRateRelationship::default_tolerance =
        Integration::TOLERANCE_UNSPECIFIED;

    unsigned int CompoundRateRelationship::default_max_evaluations =
        Integration::EVALUATIONS_UNSPECIFIED;
    
/** 
 * Local wrapper function for use with GSL. 
 *
 * Lets us pass a C++ lambda (associated with a particular object) to GSL
 * functions.
 * 
 * @param x A value at which to evaluate a function
 * @param f The function to be evaluated
 * 
 * @return The value of f(x).
 */
    static double wrapper(double x,  std::function<double (double)> *f)
    {
        return (*f)(x);
    }

    SimpleRateRelationship::SimpleRateRelationship(std::shared_ptr<DeterministicFunction> func) 
    {
        f = func;
    }


    double SimpleRateRelationship::lambda(double x) const
    {
        return f->ValueAt(x);
    }

/*
 * Uses the GSL to calculate the derivative; can be overridden by subclasses.
 */
    double RateRelationship::DerivativeAt(double x) const
    {
        /*
         * Encapsulate the function in a lambda, that we can pass to the GSL through
         * the function 'wrapper()' (above).
         */
        std::function<double (double, void*)> local_lambda = [this] (double x, void *) {
            double result = this->lambda(x);
            return result;
        };

        /*
         * Set up a 'gsl_function'--we'll pass the lambda as an additional
         * parameter, as allowed by the GSL.
         */
        gsl_function F;
        F.function = (double (*)(double, void *))wrapper;
        F.params = &local_lambda;
    
        /*
         * Just return the result reported by the GSL--we're ignoring the absolution
         * error, and any error codes, at least for now.
         */
        double result, abserror;
        gsl_deriv_forward(&F, x, 1E-8, &result, &abserror);
        return result;
    }

    CompoundRateRelationship::CompoundRateRelationship(
        std::shared_ptr<RateRelationship> base_rate,
        std::shared_ptr<ProbabilisticFunction> dependent_rate)
    {
        this->base_rate = base_rate;
        this->dependent_rate = dependent_rate;

        this->SetIntegrationParameters(Integration::TOLERANCE_UNSPECIFIED,
                                       Integration::EVALUATIONS_UNSPECIFIED);
    }

    double CompoundRateRelationship::lambda(double min_y)
    {
        /*
         * Decide which parameters to use:
         */
        double my_tol, class_tol;
        unsigned int my_max_evals, class_max_evals;
        GetIntegrationParameters(my_tol, my_max_evals);
        GetDefaultIntegrationParameters(class_tol, class_max_evals);
        
        Integration::MAQ_RESULT result;
        result =  Integration::MAQ(
            [this, min_y] (double x2) -> double {
                double result;
                if (x2 == 0) {
                    result = 0;
                } else {
                    double d = this->base_rate->DerivativeAt(x2);
                    double p = this->dependent_rate->P_exceedence(x2, min_y);
                    result = p * std::abs(d);
                }
                return result;
            }, 
            my_tol != Integration::TOLERANCE_UNSPECIFIED ? my_tol : class_tol,
            my_max_evals != Integration::EVALUATIONS_UNSPECIFIED 
            ? my_max_evals : class_max_evals);
        if (result.successful) {
            return result.integral;
        } else {
            // Log error
            return NAN;
        };
    }

    void CompoundRateRelationship::SetDefaultIntegrationParameters(
        double tol, unsigned int evals)
    {
        default_tolerance = tol;
        default_max_evaluations = evals;
    }
    
    void CompoundRateRelationship::GetDefaultIntegrationParameters(
        double &tol, unsigned int &evals)
    {
        tol = default_tolerance;
        evals = default_max_evaluations;
    }

    void CompoundRateRelationship::SetIntegrationParameters(
        double tol, unsigned int evals)
    {
        tolerance = tol;
        max_evaluations = evals;
    }
    
    void CompoundRateRelationship::GetIntegrationParameters(
        double &tol, unsigned int &evals)
    {
        tol = tolerance;
        evals = max_evaluations;
    }
}
