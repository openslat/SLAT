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
#include "lognormaldist.h"

namespace SLAT {
    std::string DeterministicFn::ToString(void) const 
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

    std::string LinearInterpolatedFn::ToString(void) const 
    {
        return "LinearInterpolatedFn";
    }

    std::string LogLogInterpolatedFn::ToString(void) const 
    {
        return "LogLogInterpolatedFn";
    }

    std::ostream& operator<<(std::ostream& out, const DeterministicFn& o)
    {
        out << o.ToString();
        return out;
    }

    std::string LogNormalFn::ToString(void) const 
    {
        return "LogNormal(" + mu_function.ToString() + ", " + 
            sigma_function.ToString() + ")";

    }

    std::ostream& operator<<(std::ostream& out, const ProbabilisticFn& o)
    {
        out << o.ToString();
        return out;
    }

    
    double DeterministicFn::ValueAt(double x) const
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
    double DeterministicFn::DerivativeAt(double x) const
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
 * Base constructor for InterpolatedFn
 *
 * Just stashes the data describing the function, in case we want to refer to it
 * later (e.g., during debugging).
 */
    InterpolatedFn::InterpolatedFn(double x[], double y[], size_t size) : 
        DeterministicFn()
    {
        for (unsigned int i=0; i < size; i++) {
            data.push_back(std::pair<double, double>(x[i], y[i]));
        }
    }


/*
 * LinearInterpolatedFn constructor
 *
 * Allocate and initialise the GSL data structures used for interpolation.
 */
    LinearInterpolatedFn::LinearInterpolatedFn(double x[], double y[], size_t size) :
        InterpolatedFn(x, y, size)
    {
        accel = gsl_interp_accel_alloc();
        interp = gsl_spline_alloc (gsl_interp_linear, size);
        gsl_spline_init(interp, x, y, size);
    }

/*
 * LinearInterpolatedFn destructor
 *
 * Release the GSL data structures used for interpolation.
 */
    LinearInterpolatedFn::~LinearInterpolatedFn()
    {
        gsl_spline_free(interp);
        gsl_interp_accel_free(accel);
    }

    double LinearInterpolatedFn::Evaluate(double x) const
    {
        // Use the GSL interpolator allocated by the constructor:
        double y;
        (void)gsl_spline_eval_e(interp, x, accel, &y);
        return y;
    }

/*
 * LogLogInterpolatedFn constructor
 *
 * Allocate and initialise the GSL data structures used for interpolation.
 */
    LogLogInterpolatedFn::LogLogInterpolatedFn(double x[], double y[], size_t size) :
        InterpolatedFn(x, y, size)
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
 * LogLogInterpolatedFn destructor
 *
 * Release the GSL data structures used for interpolation.
 */
    LogLogInterpolatedFn::~LogLogInterpolatedFn()
    {
        gsl_spline_free(interp);
        gsl_interp_accel_free(accel);
    }

/*
 * Perform the log-log interpolation, using the GSL interpolator set up in the
 * constructor.
 */
    double LogLogInterpolatedFn::Evaluate(double x) const
    {
        double y;
        (void)gsl_spline_eval_e(interp, log(x), accel, &y);
        return exp(y);
    }


    ProbabilisticFn::ProbabilisticFn(wrapped_DeterministicFn &mu_function,
                                     wrapped_DeterministicFn &sigma_function)
    {
        this->mu_function = mu_function;
        this->sigma_function = sigma_function;

        mu_function_callback_id = mu_function.add_callbacks(
            [this] (void) {
                this->notify_change();
            },
            [this] (wrapped_DeterministicFn &new_mu_function) {
                this->mu_function = new_mu_function;
                this->notify_change();
            });

        sigma_function_callback_id = sigma_function.add_callbacks(
            [this] (void) {
                this->notify_change();
            },
            [this] (wrapped_DeterministicFn &new_sigma_function) {
                this->sigma_function = new_sigma_function;
                this->notify_change();
            });
    }

    ProbabilisticFn::~ProbabilisticFn() 
    {
        mu_function.remove_callbacks(mu_function_callback_id);
        sigma_function.remove_callbacks(sigma_function_callback_id);
    }

    LogNormalFn::LogNormalFn(wrapped_DeterministicFn &mu_function, M_TYPE m_type,
                             wrapped_DeterministicFn &sigma_function, S_TYPE s_type)
        : ProbabilisticFn(mu_function, sigma_function)
    {
        switch (m_type) {
        case MEAN_LN_X:
            mean_lnX = [this] (double x) -> double { return this->mu_function.ValueAt(x); };
            median_X = [this] (double x) -> double { return exp(this->mean_lnX(x)); };
            mean_X = [this] (double x) -> double { 
                return exp(this->mean_lnX(x) + this->sigma_lnX(x) * this->sigma_lnX(x) / 2);
            };
            break;
        case MEAN_X:
            mean_X = [this] (double x) -> double { return this->mu_function.ValueAt(x); };
            mean_lnX = [this] (double x) -> double {
                return log(this->mean_X(x)) - this->sigma_lnX(x) * this->sigma_lnX(x) / 2; 
            };
            median_X = [this] (double x) -> double { return exp(this->mean_lnX(x)); };
            break;
        case MEDIAN_X:
            median_X = [this] (double x) -> double { return this->mu_function.ValueAt(x); };
            mean_lnX = [this] (double x) -> double { return log(this->median_X(x)); };
            mean_X = [this] (double x) -> double { 
                return exp(this->mean_lnX(x) + this->sigma_lnX(x) * this->sigma_lnX(x) /2);
            };
            break;
        };
        
        switch (s_type) {
        case SIGMA_LN_X:
            sigma_lnX = [this] (double x) -> double{ return this->sigma_function.ValueAt(x); };
            sigma_X = [this] (double x) -> double{
                return this->mean_X(x) * sqrt(exp(this->sigma_lnX(x) * this->sigma_lnX(x)) - 1); 
            };
            break;
        case SIGMA_X:
            sigma_X = [this] (double x) -> double { return this->sigma_function.ValueAt(x); };
            sigma_lnX = [this] (double x) -> double { 
                double sd = this->sigma_X(x);
                double mean = this->mean_X(x);
                return sqrt(log(1.0 + (sd * sd)/(mean * mean))); 
            };
            break;
        };
    }

    double LogNormalFn::P_exceedence(double x, double min_y) const
    {
        return distribution(x).p_at_least(min_y);
    }

    double LogNormalFn::X_at_exceedence(double x, double p) const
    {
        return distribution(x).x_at_p(p);
    }

    double LogNormalFn::Mean(double x) const
    {
        return distribution(x).get_mean_X();
    }

    LogNormalDist LogNormalFn::distribution(double x) const
    {
        LogNormalDist result = LogNormalDist::LogNormalDist_from_mu_lnX_and_sigma_lnX(mean_lnX(x), sigma_lnX(x));
        return result;
    }

    double wrapped_DeterministicFn::ValueAt(double x) const
    {
        return function->ValueAt(x);
    }

    std::ostream& operator<<(std::ostream& out, const wrapped_DeterministicFn& o)
    {
        return operator<<(out, o.function);
    }
    
    std::string wrapped_DeterministicFn::ToString(void) const 
    {
        return function->ToString();
    }

    wrapped_NonLinearHyperbolicLaw::wrapped_NonLinearHyperbolicLaw(
        double v_asy, double IM_asy, double alpha)
    {
        this->function = std::make_shared<NonLinearHyperbolicLaw>(v_asy, IM_asy, alpha);
   }

    wrapped_PowerLawParametricCurve::wrapped_PowerLawParametricCurve(double a, double b)
    {
        this->function = std::make_shared<PowerLawParametricCurve>(a, b);
   }

    wrapped_LinearInterpolatedFn::wrapped_LinearInterpolatedFn(double x[], double y[], size_t size)
    {
        this->function = std::make_shared<LinearInterpolatedFn>(x, y, size);
   }

    wrapped_LogLogInterpolatedFn::wrapped_LogLogInterpolatedFn(double x[], double y[], size_t size)
    {
        this->function = std::make_shared<LogLogInterpolatedFn>(x, y, size);
   }

    wrapped_LogNormalFn::wrapped_LogNormalFn(wrapped_DeterministicFn &mu_function, 
                                             wrapped_LogNormalFn::M_TYPE m_type,
                                             wrapped_DeterministicFn &sigma_function, 
                                             wrapped_LogNormalFn::S_TYPE s_type) : wrapped_ProbabilisticFn()
    {
        this->function = std::make_shared<LogNormalFn>(
            mu_function, 
            (LogNormalFn::M_TYPE)m_type, 
            sigma_function, 
            (LogNormalFn::S_TYPE)s_type);
    }
}
