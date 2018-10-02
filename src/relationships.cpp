
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
        CollapseRate([this] (void) {
                return this->CollapseRate_calc();
            }, name + std::string("::CollapseRate")),
        DemolitionRate([this] (void) {
                return this->DemolitionRate_calc();
            }, name + std::string("::DemolitionRate"))
    {
        this->name = name;
        
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

    std::vector<double> IM::lambda(std::vector<double> x)
    {
        std::vector<double> result(x.size());
#pragma omp parallel for
        for (size_t i=0; i < x.size(); i++) {
            result[i] = lambda(x[i]);
        }
        return result;
    }

    std::vector<double> IM::pCollapse(std::vector<double> im)
    {
        std::vector<double> result(im.size());
#pragma omp parallel for
        for (size_t i=0; i < im.size(); i++) {
            result[i] = pCollapse(im[i]);
        }
        return result;
    }

    std::vector<double> IM::pDemolition(std::vector<double> im)
    {
        std::vector<double> result(im.size());
#pragma omp parallel for
        for (size_t i=0; i < im.size(); i++) {
            result[i] = pDemolition(im[i]);
        }
        return result;
    }

    std::vector<double> IM::pRepair(std::vector<double> im)
    {
        std::vector<double> result(im.size());
#pragma omp parallel for
        for (size_t i=0; i < im.size(); i++) {
            result[i] = pRepair(im[i]);
        }
        return result;
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
        gsl_deriv_central(&F, x, 1E-8, &result, &abserror);
        if (std::isnan(result)) gsl_deriv_forward(&F, x, 1E-8, &result, &abserror);
        if (std::isnan(result)) gsl_deriv_backward(&F, x, 1E-8, &result, &abserror);
        return result;
    }

    double IM::CollapseRate_calc(void)
    {
        TempContext context([this] (std::ostream &o) {
                o << "IM::CollapseRate_calc() [" << this->name << "]";
            });
        if (collapse) {
            Integration::MAQ_RESULT result;
            result =  Integration::MAQ(
                [this] (double im) -> double {
                    double d = this->DerivativeAt(im);
                    double p = this->pCollapse(im);
                    double result = fabs(d) * p;
                    return result;
                }); 
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
        TempContext context([this] (std::ostream &o) {
                o << "IM::DemolitionRate_calc() [" << this->name << "]";
            });
        if (demolition) {
            Integration::MAQ_RESULT result;
            result =  Integration::MAQ(
                [this] (double im) -> double {
                    double d = this->DerivativeAt(im);
                    double p = this->pDemolition(im);
                    double result = fabs(d) * p;
                    return result;
                }); 
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
        TempContext context([this] (std::ostream &o) {
                o << "EDP::calc_lambda() [" << this->name << "]";
            });
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
            }); 
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

    double EDP::X_at_exceedence(double base_value, double p) const
    {
        return dependent_rate->X_at_exceedence(base_value, p);
    }
    
    std::vector<double> EDP::par_lambda(std::vector<double> args)
    {
        std::vector<double> result(args.size());
#pragma omp parallel for
        for (size_t i=0; i < args.size(); i++) {
            result[i] = lambda(args[i]);
        }
        return result;
    }
    
    double EDP::MeanLn(double base_value) const
    {
        return dependent_rate->MeanLn(base_value);
    }

    std::vector<double> EDP::MeanLn(std::vector<double> base_values) const
    {
        std::vector<double> result(base_values.size());
#pragma omp parallel for
        for (size_t i = 0; i < base_values.size(); i++) {
            result[i] = MeanLn(base_values[i]);
        }
        return result;
    }

    double EDP::Median(double base_value) const
    {
        return dependent_rate->Median(base_value);
    }

    std::vector<double> EDP::Median(std::vector<double> base_values) const
    {
        std::vector<double> result(base_values.size());
#pragma omp parallel for
        for (size_t i = 0; i < base_values.size(); i++) {
            result[i] = Median(base_values[i]);
        }
        return result;
    }

    double EDP::Mean(double base_value) const
    {
        return dependent_rate->Mean(base_value);
    }

    std::vector<double> EDP::Mean(std::vector<double> base_values) const
    {
        std::vector<double> result(base_values.size());
#pragma omp parallel for
        for (size_t i = 0; i < base_values.size(); i++) {
            result[i] = Mean(base_values[i]);
        }
        return result;
    }

    double EDP::SD_ln(double base_value) const
    {
        return dependent_rate->SD_ln(base_value);
    }

    std::vector<double> EDP::SD_ln(std::vector<double> base_values) const
    {
        std::vector<double> result(base_values.size());
#pragma omp parallel for
        for (size_t i = 0; i < base_values.size(); i++) {
            result[i] = SD_ln(base_values[i]);
        }
        return result;
    }

    double EDP::SD(double base_value) const
    {
        return dependent_rate->SD(base_value);
    }

    std::vector<double> EDP::SD(std::vector<double> base_values) const
    {
        std::vector<double> result(base_values.size());
#pragma omp parallel for
        for (size_t i = 0; i < base_values.size(); i++) {
            result[i] = SD(base_values[i]);
        }
        return result;
    }

    CompoundEDP::CompoundEDP(std::shared_ptr<IM> base_rate,
                             std::shared_ptr<ProbabilisticFn> dependent_rate_1,
                             std::shared_ptr<ProbabilisticFn> dependent_rate_2,
                             std::string name) :
        EDP(base_rate, dependent_rate_1, name)
    {
        this->second_dependent_rate = dependent_rate_2;

        second_dependent_rate_callback_id = second_dependent_rate->add_callbacks(
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

    CompoundEDP::DIRECTION CompoundEDP::WhichToUse(double base_value) const
    {
        double median_1 = dependent_rate->Median(base_value);
        double median_2 = second_dependent_rate->Median(base_value);

        return median_1 > median_2 ? FIRST : SECOND;
    }

    double CompoundEDP::Mean(double base_value) const
    {
        switch (WhichToUse(base_value)) {
        case FIRST:
            return dependent_rate->Mean(base_value);
            break;
        case SECOND:
            return second_dependent_rate->Mean(base_value);
        default:
            // FAIL
            return NAN;
        }
    }
    
    double CompoundEDP::MeanLn(double base_value) const
    {
        switch (WhichToUse(base_value)) {
        case FIRST:
            return dependent_rate->MeanLn(base_value);
            break;
        case SECOND:
            return second_dependent_rate->MeanLn(base_value);
        default:
            // FAIL
            return NAN;
        }
    }
    
    double CompoundEDP::Median(double base_value) const
    {
        switch (WhichToUse(base_value)) {
        case FIRST:
            return dependent_rate->Median(base_value);
            break;
        case SECOND:
            return second_dependent_rate->Median(base_value);
        default:
            // FAIL
            return NAN;
        }
    }
    
    double CompoundEDP::SD_ln(double base_value) const
    {
        switch (WhichToUse(base_value)) {
        case FIRST:
            return dependent_rate->SD_ln(base_value);
            break;
        case SECOND:
            return second_dependent_rate->SD_ln(base_value);
        default:
            // FAIL
            return NAN;
        }
    }
    
    double CompoundEDP::SD(double base_value) const
    {
        switch (WhichToUse(base_value)) {
        case FIRST:
            return dependent_rate->SD(base_value);
            break;
        case SECOND:
            return second_dependent_rate->SD(base_value);
        default:
            // FAIL
            return NAN;
        }
    }
}


