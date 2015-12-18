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
#include "gsl/gsl_cdf.h"
#include "gsl/gsl_deriv.h"

#include "relationships.h"
#include "maq.h"

namespace SLAT {
    namespace Caching {
        std::unordered_map<void *, std::function<void (void)>> caches;
        
        void Add_Cache(void *cache, std::function<void (void)> clear_func) {
            caches[cache] = clear_func;
        }

        void Remove_Cache(void *cache) {
            caches.erase(cache);
        }

        void Clear_Caches(void) {
            for (auto it=caches.begin(); it != caches.end(); it++) {
                it->second();
            }
        }
    };
        
    Integration::IntegrationSettings RateRelationship::class_settings(
        Integration::IntegrationSettings::Get_Global_Settings());

    RateRelationship::RateRelationship() : local_settings(&class_settings),
                                         lambda([this] (double x) { return this->calc_lambda(x); })
    {
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
        callback_id = f->add_callbacks(
            [this] (void) {this->notify_change();},
            [this] (std::shared_ptr<DeterministicFunction> new_f) {
                this->f = new_f;
                this->notify_change();
            });
    }


    double SimpleRateRelationship::calc_lambda(double x)
    {
        return f->ValueAt(x);
    }

    std::string RateRelationship::ToString(void) const 
    {
        return "Rate Relationship: ";
    }

    std::string SimpleRateRelationship::ToString(void) const 
    {
        return "Simple(" + f->ToString() + ")";
    }

    std::string CompoundRateRelationship::ToString(void) const 
    {
        return "Compound(" + base_rate->ToString()
            + ", " + dependent_rate->ToString() + ")";
    }

    std::ostream& operator<<(std::ostream& out, const RateRelationship& o)
    {
        out << o.ToString();
        return out;
    }
/*
 * Uses the GSL to calculate the derivative; can be overridden by subclasses.
 */
    double RateRelationship::DerivativeAt(double x)
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

        base_rate_callback_id = base_rate->add_callbacks(
            [this] (void) {this->notify_change();},
            [this] (std::shared_ptr<RateRelationship> new_base_rate) {
                this->base_rate = new_base_rate;
                this->notify_change();
            });

        dependent_rate_callback_id = dependent_rate->add_callbacks(
            [this] (void) {this->notify_change();},
            [this] (std::shared_ptr<ProbabilisticFunction> new_dependent_rate) {
                this->dependent_rate = new_dependent_rate;
                this->notify_change();
            });
    }

    double CompoundRateRelationship::calc_lambda(double min_y) 
    {
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
