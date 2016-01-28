/**
 * @file   functions.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief Implementation of classes representing mathematical functions. xxxx
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */
#include <sstream>
#include <math.h>
#include <vector>
#include <functional>
#include <gsl/gsl_deriv.h>
#include "functions.h"
#include "lognormal.h"

namespace SLAT {
    std::string DeterministicFunction::ToString(void) const 
    {
        return "Deterministic Function";
    }

    std::string NonLinearHyperbolicLaw::ToString(void) const 
    {
        std::stringstream result;
        result << "Hyperbolic(" << v_asy << ", " << IM_asy
               << ", " << alpha << ")";
        return result.str();
    }
    

    std::string PowerLawParametricCurve::ToString(void) const 
    {
        std::stringstream result;
        result << "Power(" << a << ", " << b << ")";
        return result.str();
    }

    std::string LinearInterpolatedFunction::ToString(void) const 
    {
        return "LinearInterpolatedFunction";
    }

    std::string LogLogInterpolatedFunction::ToString(void) const 
    {
        return "LogLogInterpolatedFunction";
    }

    std::ostream& operator<<(std::ostream& out, const DeterministicFunction& o)
    {
        out << o.ToString();
        return out;
    }

    std::string LogNormalFunction::ToString(void) const 
    {
        return "LogNormal(" + mu_function->ToString() + ", " + 
            sigma_function->ToString() + ")";

    }

    std::ostream& operator<<(std::ostream& out, const ProbabilisticFunction& o)
    {
        out << o.ToString();
        return out;
    }

    
    double DeterministicFunction::ValueAt(double x) const
    {
        return this->Evaluate(x);
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

/*
 * Uses the GSL to calculate the derivative; can be overridden by subclasses.
 */
    double DeterministicFunction::DerivativeAt(double x) const
    {
        /*
         * Encapsulate the function in a lambda, that we can pass to the GSL through
         * the function 'wrapper()' (above).
         */
        std::function<double (double, void*)> local_lambda = [this] (double x, void *) {
            double result = this->ValueAt(x);
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
        gsl_deriv_central(&F, x, 1E-8, &result, &abserror);
        if (isnan(result)) gsl_deriv_forward(&F, x, 1E-8, &result, &abserror);
        if (isnan(result)) gsl_deriv_backward(&F, x, 1E-8, &result, &abserror);

        return result;
    }

    NonLinearHyperbolicLaw::NonLinearHyperbolicLaw(double v_asy, double IM_asy, double alpha)
    {
        this->v_asy = v_asy;
        this->IM_asy = IM_asy;
        this->alpha = alpha;
    }

/*
 * Evaluate a non-linear hyperbolic curve at x, forcing the result to 0
 * after IM_asy, and clipping early values to 1.
 */
    double NonLinearHyperbolicLaw::Evaluate(double x) const
    {
        if (x <= 0) {
            return 1;
        } else if (x >= this->IM_asy) {
            return 0;
        } else {
            double result = v_asy * exp(alpha / log(x / IM_asy));
            if (result > 1.0) result = 1.0;
            return result;
        }
    }

    PowerLawParametricCurve::PowerLawParametricCurve(double a, double b)
    {
        this->a = a;
        this->b = b;
    }

    double PowerLawParametricCurve::Evaluate(double x) const
    {
        return a * pow(x, b);
    }

/*
 * Base constructor for InterpolatedFunction
 *
 * Just stashes the data describing the function, in case we want to refer to it
 * later (e.g., during debugging).
 */
    InterpolatedFunction::InterpolatedFunction(double x[], double y[], size_t size) : 
        DeterministicFunction()
    {
        for (unsigned int i=0; i < size; i++) {
            data.push_back(std::pair<double, double>(x[i], y[i]));
        }
    }


/*
 * LinearInterpolatedFunction constructor
 *
 * Allocate and initialise the GSL data structures used for interpolation.
 */
    LinearInterpolatedFunction::LinearInterpolatedFunction(double x[], double y[], size_t size) :
        InterpolatedFunction(x, y, size)
    {
        accel = gsl_interp_accel_alloc();
        interp = gsl_spline_alloc (gsl_interp_linear, size);
        gsl_spline_init(interp, x, y, size);
    }

/*
 * LinearInterpolatedFunction destructor
 *
 * Release the GSL data structures used for interpolation.
 */
    LinearInterpolatedFunction::~LinearInterpolatedFunction()
    {
        gsl_spline_free(interp);
        gsl_interp_accel_free(accel);
    }

    double LinearInterpolatedFunction::Evaluate(double x) const
    {
        // Use the GSL interpolator allocated by the constructor:
        double y;
        (void)gsl_spline_eval_e(interp, x, accel, &y);
        return y;
    }

/*
 * LogLogInterpolatedFunction constructor
 *
 * Allocate and initialise the GSL data structures used for interpolation.
 */
    LogLogInterpolatedFunction::LogLogInterpolatedFunction(double x[], double y[], size_t size) :
        InterpolatedFunction(x, y, size)
    {
        accel = gsl_interp_accel_alloc();
        interp = gsl_spline_alloc (gsl_interp_linear, size);

        // Make a copy of the data in log scale for the interpolator:
        double logx[size];
        double logy[size];
        for (unsigned int i=0; i < size; i++) {
            logx[i] = log(x[i]);
            logy[i] = log(y[i]);
        }
        gsl_spline_init(interp, logx, logy, size);
    }

/*
 * LogLogInterpolatedFunction destructor
 *
 * Release the GSL data structures used for interpolation.
 */
    LogLogInterpolatedFunction::~LogLogInterpolatedFunction()
    {
        gsl_spline_free(interp);
        gsl_interp_accel_free(accel);
    }

/*
 * Perform the log-log interpolation, using the GSL interpolator set up in the
 * constructor.
 */
    double LogLogInterpolatedFunction::Evaluate(double x) const
    {
        double y;
        (void)gsl_spline_eval_e(interp, log(x), accel, &y);
        return exp(y);
    }


    ProbabilisticFunction::ProbabilisticFunction(std::shared_ptr<DeterministicFunction> mu_function,
                                                 std::shared_ptr<DeterministicFunction> sigma_function)
    {
        this->mu_function = mu_function;
        this->sigma_function = sigma_function;

        mu_function_callback_id = mu_function->add_callbacks(
            [this] (void) {
                this->notify_change();
            },
            [this] (std::shared_ptr<DeterministicFunction> new_mu_function) {
                this->mu_function = new_mu_function;
                this->notify_change();
            });

        sigma_function_callback_id = sigma_function->add_callbacks(
            [this] (void) {
                this->notify_change();
            },
            [this] (std::shared_ptr<DeterministicFunction> new_sigma_function) {
                this->sigma_function = new_sigma_function;
                this->notify_change();
            });
    }

    ProbabilisticFunction::~ProbabilisticFunction() 
    {
        mu_function->remove_callbacks(mu_function_callback_id);
        sigma_function->remove_callbacks(sigma_function_callback_id);
    }

    LogNormalFunction::LogNormalFunction(std::shared_ptr<DeterministicFunction> mu_function,
                                         std::shared_ptr<DeterministicFunction> sigma_function)
        : ProbabilisticFunction(mu_function, sigma_function)
    {
    }

    double LogNormalFunction::P_exceedence(double x, double min_y) const
    {
        return LognormalFunction::Lognormal_from_mu_lnX_and_sigma_lnX(
            log(mu_function->ValueAt(x)), 
            sigma_function->ValueAt(x)).p_at_least(min_y);
    }

    double LogNormalFunction::X_at_exceedence(double x, double p) const
    {
        return LognormalFunction::Lognormal_from_mu_lnX_and_sigma_lnX(
            log(mu_function->ValueAt(x)), 
            sigma_function->ValueAt(x)).x_at_p(p);
    }

}
