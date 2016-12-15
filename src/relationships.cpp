
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
#include <string>
#include <iostream>

namespace SLAT {
    Integration::IntegrationSettings EDP::class_settings(
        Integration::IntegrationSettings::Get_Global_Settings());

    Integration::IntegrationSettings &EDP::Get_Class_Integration_Settings(void)
    {
        return class_settings;
    }

    Integration::IntegrationSettings &EDP::Get_Integration_Settings(void)
    {
        return local_settings;
    }

    Integration::IntegrationSettings IM::class_settings(
        Integration::IntegrationSettings::Get_Global_Settings());

    Integration::IntegrationSettings &IM::Get_Class_Integration_Settings(void)
    {
        return class_settings;
    }

    Integration::IntegrationSettings &IM::Get_Integration_Settings(void)
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
    double wrapper(double x,  std::function<double (double)> *f)
    {
        return (*f)(x);
    }

    IM::IM( std::shared_ptr<DeterministicFn> func, std::string name) :
        local_settings(&class_settings),
        CollapseRate([this] (void) {
                return this->CollapseRate_calc();
            }, name + std::string("::CollapseRate")),
        DemolitionRate([this] (void) {
                return this->DemolitionRate_calc();
            }, name + std::string("::DemolitionRate"))
    {
        this->name = name;
        // std::cout << "IM: " << name << " " << local_settings.Get_Effective_Max_Evals() << "; "
        //           << Get_Class_Integration_Settings().Get_Effective_Max_Evals() << "; "
        //           << Integration::IntegrationSettings::Get_Global_Settings()->Get_Effective_Max_Evals() << std::endl;
        
        f = func;
        callback_id = f->add_callbacks(
            [this] (void) {
                this->notify_change();
            },
            [this] (std::shared_ptr<DeterministicFn> new_f) {
                this->f = new_f;
                this->notify_change();
            });
    }


    double IM::lambda(double x)
    {
        return f->ValueAt(x);
    }

    std::string IM::ToString(void) const 
    {
        return "IM(" + f->ToString() + ")";
    }

    std::string EDP::ToString(void) const 
    {
        return "EDP((" + base_rate->ToString()
            + ", " + dependent_rate->ToString() + ")";
    }

    std::ostream& operator<<(std::ostream& out, const EDP& o)
    {
        out << o.ToString();
        return out;
    }

    double IM::DerivativeAt(double x)
    {
        /*
         * Encapsulate the function in a lambda, that we can pass to the GSL through
         * the function 'wrapper()' (above).
         */
        std::function<double (double)> local_lambda = [this] (double x) {
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
         * Just return the result reported by the GSL--we're ignoring the absolute
         * error, and any error codes, at least for now.
         */
        double result, abserror;
        gsl_deriv_forward(&F, x, 1E-8, &result, &abserror);
        return result;
    }

    double IM::CollapseRate_calc(void)
    {
        if (collapse) {
            // std::cout << "IM::CollapseRate_calc " << name << " " << local_settings.Get_Effective_Max_Evals() << "; "
            //           << Get_Class_Integration_Settings().Get_Effective_Max_Evals() << "; "
            //           << Integration::IntegrationSettings::Get_Global_Settings()->Get_Effective_Max_Evals() << std::endl;

            Integration::MAQ_RESULT result;
            result =  Integration::MAQ(
                [this] (double im) -> double {
                    double d = this->DerivativeAt(im);
                    double p = this->pCollapse(im);
                    double result = fabs(d) * p;
                    return result;
                }, local_settings); 
            //std::cout << "...." << std::endl;
            if (result.successful) {
                return result.integral;
            } else {
                // Log error
                return NAN;
            };
        } else {
            return 0;
        }
    }

    double IM::DemolitionRate_calc(void)
    {
        if (demolition) {
            Integration::MAQ_RESULT result;
            result =  Integration::MAQ(
                [this] (double im) -> double {
                    double d = this->DerivativeAt(im);
                    double p = this->pDemolition(im);
                    double result = fabs(d) * p;
                    return result;
                }, local_settings); 
            if (result.successful) {
                return result.integral;
            } else {
                // Log error
                return NAN;
            };
        } else {
            return 0;
        }
    }


/*
 * Uses the GSL to calculate the derivative; can be overridden by subclasses.
 */
    double EDP::DerivativeAt(double x)
    {
        /*
         * Encapsulate the function in a lambda, that we can pass to the GSL through
         * the function 'wrapper()' (above).
         */
        std::function<double (double)> local_lambda = [this] (double x) {
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
         * Just return the result reported by the GSL--we're ignoring the absolute
         * error, and any error codes, at least for now.
         */
        double result, abserror;
        gsl_deriv_forward(&F, x, 1E-8, &result, &abserror);
        return result;
    }

    EDP::EDP(std::shared_ptr<IM> base_rate,
             std::shared_ptr<ProbabilisticFn> dependent_rate, 
             std::string name) :
        local_settings(&class_settings),
        callback_id(0),
        lambda([this] (double x) { return this->calc_lambda(x); }, name + "::lambda") 
    {
        this->name = name;
        this->base_rate = base_rate;
        this->dependent_rate = dependent_rate;

        base_rate_callback_id = base_rate->add_callbacks(
            [this] (void) {
                this->lambda.ClearCache();
                this->notify_change();
            },
            [this] (std::shared_ptr<IM> new_base_rate) {
                this->lambda.ClearCache();
                this->base_rate = new_base_rate;
                this->notify_change();
            });

        dependent_rate_callback_id = dependent_rate->add_callbacks(
            [this] (void) {
                this->lambda.ClearCache();
                this->notify_change();
            },
            [this] (std::shared_ptr<ProbabilisticFn> new_dependent_rate) {
                this->lambda.ClearCache();
                this->dependent_rate = new_dependent_rate;
                this->notify_change();
            });
    }

    double EDP::calc_lambda(double min_y) 
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
                    result = (p * base_rate->pRepair(x2) + 
                              (1.0 - base_rate->pRepair(x2))) * std::abs(d);
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

    double EDP::P_exceedence(double base_value, double min_dependent_value) const
    {
        return dependent_rate->P_exceedence(base_value, min_dependent_value);
    }

    double EDP::MeanLn(double base_value) const
    {
        return dependent_rate->MeanLn(base_value);
    }

    double EDP::Median(double base_value) const
    {
        return dependent_rate->Median(base_value);
    }

    double EDP::Mean(double base_value) const
    {
        return dependent_rate->Mean(base_value);
    }

    double EDP::SD_ln(double base_value) const
    {
        return dependent_rate->SD_ln(base_value);
    }

    double EDP::SD(double base_value) const
    {
        return dependent_rate->SD(base_value);
    }

}


