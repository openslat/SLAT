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
#include <cmath>
#include <vector>
#include <functional>
#include <gsl/gsl_deriv.h>
#include <gsl/gsl_roots.h>
#include <omp.h>
#include <boost/log/trivial.hpp>
#include "functions.h"
#include "lognormaldist.h"
#include "context.h"
#include <iomanip>

namespace SLAT {
    DeterministicFn::DeterministicFn(void): Replaceable<DeterministicFn>(),
            SolveFor([this] (double x) { 
                    return this->SolveFor_calc(x);
                }) 
            {};

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
        return "LogNormal(" + mu_function->ToString() + ", " + 
            sigma_function->ToString() + ")";

    }

    std::ostream& operator<<(std::ostream& out, const ProbabilisticFn& o)
    {
        out << o.ToString();
        return out;
    }

    
    double DeterministicFn::ValueAt(double x) const
    {
        // std::stringstream s;
        // s << "DeterministicFn::ValueAt(" << x << ")";
        //Context::PushText(s.str());
        // Context::PushText([] (std::ostream &o) {
        //         o << "ValueAt()";
        //     });
        //BOOST_LOG_TRIVIAL(fatal) << Context::GetText();
        double result = this->Evaluate(x);
        // Context::PopText();
        return result;
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
        std::function<double (double)> local_lambda = [this] (double x) {
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
        if (std::isnan(result)) gsl_deriv_forward(&F, x, 1E-8, &result, &abserror);
        if (std::isnan(result)) gsl_deriv_backward(&F, x, 1E-8, &result, &abserror);

        return result;
    }

/*
 * Uses the GSL to find a root; can be overridden by subclasses.
 */
    double DeterministicFn::SolveFor_calc(double y) 
    {
        /*
         * Encapsulate the function in a lambda, that we can pass to the GSL through
         * the function 'wrapper()' (above).
         */
        std::function<double (double)> local_lambda = [this, y] (double x) {
            double result = this->ValueAt(x) - y;
            return result;
        };

        /*
         * Set up a 'gsl_function'--we'll pass the lambda as an additional
         * parameter, as allowed by the GSL.
         */
        gsl_function F;
        F.function = (double (*)(double, void *))wrapper;
        F.params = &local_lambda;

        const gsl_root_fsolver_type * T  = gsl_root_fsolver_brent;
        gsl_root_fsolver * s = gsl_root_fsolver_alloc (T);
        double x_low = 0, x_high=1/y;
        if (std::isnan(x_high)) x_high = 1.0;
        double y_low = this->ValueAt(x_low);
        double y_high = this->ValueAt(x_high);
        //std::cout << "solving for " << y << std::endl;
        //std::cout << x_low << ", " << y_low << "; " << x_high << ", " << y_high << std::endl;
        if (y_low < y and y_high < y) {
            while (y_high < y) {
                x_low = x_high;
                y_low = y_high;

                x_high = 2 * x_low;
                y_high = this->ValueAt(x_high);
            }
        } else if (y_low > y and y_high > y) {
            while (y_high > y) {
                x_low = x_high;
                y_low = y_high;

                x_high = 2 * x_low;
                y_high = this->ValueAt(x_high);
            }
        } else if (y_low == y) {
            return x_low; 
        } else if (y_high == y) {
            return x_high;
        }
        gsl_root_fsolver_set (s, &F, x_low, x_high);
        /*
        std::cout << x_low << ", " << y_low << "; " << x_high << ", " << y_high << std::endl;
        
        std::cout << "using " << gsl_root_fsolver_name(s) << "method" << std::endl;

        std::cout << std::setw(5) << "iter"
                  << std::setw(9) << "root" 
                  << std::setw(10) << "err"
                  << std::setw(9) << "err(est)" 
                  << std::endl;
        */
        int iter=0;
        const int max_iter = 100;
        int status;

        do
        {
            iter++;
            status = gsl_root_fsolver_iterate(s);
            double r = gsl_root_fsolver_root(s);
            double x_lo = gsl_root_fsolver_x_lower(s);
            double x_hi = gsl_root_fsolver_x_upper(s);
            status = gsl_root_test_interval(x_lo, x_hi,
                                            0, 0.001);
            /*
            if (status == GSL_SUCCESS) {
                std::cout << "Converged: " << std::endl;
            }
            
            std::cout << std::setw(5) << iter << " ["
                      << std::setw(7) << x_low << ", "
                      << std::setw(7) << x_hi << "] "
                      << std::setw(7) << r
                      << std::setw(7) << x_hi - x_low
                      << std::endl;
            */
            if (status == GSL_SUCCESS) {
                gsl_root_fsolver_free (s);
                return r;
            }
            
            if (status != GSL_CONTINUE) {
                std::cout << "FAIL: " << x_low << ", " << x_high << "; " << status << std::endl;
            }
        }
        while (status == GSL_CONTINUE && iter < max_iter);
        std::cout << "ITER: " << iter << std::endl;
        gsl_root_fsolver_free (s);
        
        return NAN;
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
        if (x >= this->IM_asy) {
            return 0;
        } else {
            return v_asy * exp(alpha / log(x / IM_asy));
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
        min_x = std::pair<double, double>(x[0], y[0]);
        max_x = std::pair<double, double>(x[size - 1], y[size - 1]);
        
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
        if (x < min_x.first) {
            //std::cout << "CLIPPED at " << x << std::endl;
            y = min_x.second;
        } else if (x > max_x.first) {
            //std::cout << "CLIPPED at " << x << std::endl;
            y = max_x.second;
        } else {
            (void)gsl_spline_eval_e(interp, x, accel, &y);
        }
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
        if (x < min_x.first) {
            y = min_x.second;
        } else if (x > max_x.first) {
            y = max_x.second;
        } else {
            (void)gsl_spline_eval_e(interp, log(x), accel, &y);
            y = exp(y);

        }
        return y;
    }


    ProbabilisticFn::ProbabilisticFn(std::shared_ptr<DeterministicFn> mu_function,
                                                 std::shared_ptr<DeterministicFn> sigma_function)
    {
        this->mu_function = mu_function;
        this->sigma_function = sigma_function;

        mu_function_callback_id = mu_function->add_callbacks(
            [this] (void) {
                this->notify_change();
            },
            [this] (std::shared_ptr<DeterministicFn> new_mu_function) {
                this->mu_function = new_mu_function;
                this->notify_change();
            });

        sigma_function_callback_id = sigma_function->add_callbacks(
            [this] (void) {
                this->notify_change();
            },
            [this] (std::shared_ptr<DeterministicFn> new_sigma_function) {
                this->sigma_function = new_sigma_function;
                this->notify_change();
            });
    }

    ProbabilisticFn::~ProbabilisticFn() 
    {
        mu_function->remove_callbacks(mu_function_callback_id);
        sigma_function->remove_callbacks(sigma_function_callback_id);
    }

    LogNormalFn::LogNormalFn(std::shared_ptr<DeterministicFn> mu_function, M_TYPE m_type,
                                         std::shared_ptr<DeterministicFn> sigma_function, S_TYPE s_type)
        : ProbabilisticFn(mu_function, sigma_function)
    {
        switch (m_type) {
        case MEAN_LN_X:
            mean_lnX = [this] (double x) -> double { return this->mu_function->ValueAt(x); };
            median_X = [this] (double x) -> double { return exp(this->mean_lnX(x)); };
            mean_X = [this] (double x) -> double { 
                return exp(this->mean_lnX(x) + this->sigma_lnX(x) * this->sigma_lnX(x) / 2);
            };
            break;
        case MEAN_X:
            mean_X = [this] (double x) -> double { return this->mu_function->ValueAt(x); };
            mean_lnX = [this] (double x) -> double {
                return log(this->mean_X(x)) - this->sigma_lnX(x) * this->sigma_lnX(x) / 2; 
            };
            median_X = [this] (double x) -> double { return exp(this->mean_lnX(x)); };
            break;
        case MEDIAN_X:
            median_X = [this] (double x) -> double { return this->mu_function->ValueAt(x); };
            mean_lnX = [this] (double x) -> double { return log(this->median_X(x)); };
            mean_X = [this] (double x) -> double { 
                return exp(this->mean_lnX(x) + this->sigma_lnX(x) * this->sigma_lnX(x) /2);
            };
            break;
        case MEAN_INVALID:
        default:
            throw std::invalid_argument("MEAN");
        };
        
        switch (s_type) {
        case SIGMA_LN_X:
            sigma_lnX = [this] (double x) -> double{ return this->sigma_function->ValueAt(x); };
            sigma_X = [this] (double x) -> double{
                return this->mean_X(x) * sqrt(exp(this->sigma_lnX(x) * this->sigma_lnX(x)) - 1); 
            };
            break;
        case SIGMA_X:
            sigma_X = [this] (double x) -> double { return this->sigma_function->ValueAt(x); };
            sigma_lnX = [this] (double x) -> double { 
                double sd = this->sigma_X(x);
                double mean = this->mean_X(x);
                return sqrt(log(1.0 + (sd * sd)/(mean * mean))); 
            };
            break;
        case SIGMA_INVALID:
        default:
            throw std::invalid_argument("SIGMA");
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

    double LogNormalFn::MeanLn(double x) const
    {
        return distribution(x).get_mu_lnX();
    }

    double LogNormalFn::Median(double x) const
    {
        return distribution(x).get_median_X();
    }

    double LogNormalFn::SD_ln(double x) const
    {
        return distribution(x).get_sigma_lnX();
    }

    double LogNormalFn::SD(double x) const
    {
        return distribution(x).get_sigma_X();
    }

    LogNormalDist LogNormalFn::distribution(double x) const
    {
        LogNormalDist result = LogNormalDist::LogNormalDist_from_mu_lnX_and_sigma_lnX(mean_lnX(x), sigma_lnX(x));
        return result;
    }
}
