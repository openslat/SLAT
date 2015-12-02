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

    Integration::IntegrationSettings RateRelationship::class_settings(
        Integration::IntegrationSettings::Get_Global_Settings());

    RateRelationship::RateRelationship() : local_settings(&class_settings) {
        id = 0;
    };

    Integration::IntegrationSettings &RateRelationship::Get_Class_Integration_Settings(void)
    {
        return class_settings;
    }

    Integration::IntegrationSettings &RateRelationship::Get_Integration_Settings(void)
    {
        return local_settings;
    }

    
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
        : RateRelationship()
    {
        f = func;
        id = 1;
    }


    double SimpleRateRelationship::lambda(double x) const
    {
        //std::cout << "SimpleRateRelationship::lambda()" << std::endl;
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
        : RateRelationship()
    {
        this->base_rate = base_rate;
        this->dependent_rate = dependent_rate;
        id = 2;
    }

    double CompoundRateRelationship::lambda(double min_y) const
    {
        std::cout << "CompoundRateRelationship::lambda()" << std::endl;
        /*
         * Decide which parameters to use:
         */
        double my_tol, class_tol;
        unsigned int my_max_evals, class_max_evals;
        
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
            }, local_settings); 
        if (result.successful) {
            return result.integral;
        } else {
            // Log error
            return NAN;
        };
    }
}
