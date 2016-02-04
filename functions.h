/**
 * @file   functions.h
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief  Classes representing mathematical functions.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */
#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_
#include <vector>
#include <map>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include <memory>
#include <iostream>
#include <math.h>
#include "replaceable.h"
#include "lognormaldist.h"

namespace SLAT {
/**
 * @brief Deterministic Functions
 *
 * This is the base class for functions which accept a single (double-precision)
 * argument, and always return the same single (double-precision) value for a
 * given input. This contrasts with probabilistic functions, whose results will
 * vary according to a probability distribution.
 */
    class DeterministicFn : public Replaceable<DeterministicFn>
    {
    private:
        int callback_id;
    protected:
        /** 
         * Default constructor; does nothing.
         */     
    DeterministicFn(void) : Replaceable<DeterministicFn>() {
            /* callback_id = this->add_callbacks( */
            /*     [this] (void) { this->notify_change(); }, */
            /*     [this] (std::shared_ptr<DeterministicFn> f) { */
            /*         this->replace(f); */
            /*     }); */
        };

        /** 
         * Evalue the function at the given input. Subclasses will override this
         * method.
         * 
         * @param x  The input at which to evaluate the function.
         * 
         * @return   The result of evaluating the function at x.
         */
        virtual double Evaluate(double x) const { return NAN; };
    public:
        ~DeterministicFn(void) { 
            /* thisremove_callbacks->callback_id(); */
        };

        /** 
         * Evaluate the function at a given input. This is the public interface,
         * which will invoke Evaluate() to perform the calculation. The class is
         * structured this way to facilitate instrumenting or caching all
         * DeterministicFn objects without having to change any of the
         * subclasses.
         * 
         * @param x  The input at which to evaluate the function.
         * 
         * @return The result of evaluating the function at x.
         */
        double ValueAt(double x) const;

        /** 
         * Return the derivative of the function at a given input. This is used when
         * integrating Exceedance curves.  The default implementation uses the Gnu
         * Scientific Library to determine the derivative, but subclasses can
         * override this method to perform their own calculations.
         * 
         * @param x  The input at which to evaluate the derivative.
         * 
         * @return  The derivative of the function at x.
         */
        virtual double DerivativeAt(double x) const;

        friend std::ostream& operator<<(std::ostream& out, const DeterministicFn& o);
        virtual std::string ToString(void) const;
    };


/**
 * @brief Deterministic functions represented by a mathematical formula.
 * 
 * This is an abstract class. Subclasses will implement specific formulae in
 * their Evaluate() method. These may require any number of parameters to be
 * provided to the constructor.
 *
 * I expect we'll implement a 'factory' to provide objects of an appropriate
 * subclass based on the function requested in an input file.
 */
    class FormulaFn : public DeterministicFn
    {
    protected:
        FormulaFn() {};   /**< Default constructor; does nothing. */
        ~FormulaFn() { }; /**< Default destructor; does nothing. */
    };

/**
 * @brief Non-linear hyperbolic exceedence curve
 *
 * This class uses three paramers (v_asy, IM_asy, and alpha) to control the
 * curve, according to the expression:
 *
 *     y = v_asy * exp(alpha / log(x / IM_asy))
 *
 * Since SLAT uses this for exceedence curves, it will limit the maximum value
 * to 1, and will return 0 for x >= IM_asy (normally, there would be a
 * discontinuity at x=IM_asy).
 */
    class NonLinearHyperbolicLaw : public FormulaFn
    {
    public:
        /** 
         * Constructor; must provide equation parameters.
         * 
         * @param v_asy   Equation parameter
         * @param IM_asy  Equation parameter
         * @param alpha   Equation parameter
         */
        NonLinearHyperbolicLaw(double v_asy, double IM_asy, double alpha);
        ~NonLinearHyperbolicLaw() {
            std::cout << "Destroying NonLinearHyperbolicLaw: " << this << std::endl;
        }; /**< Default destructor; do nothing. */

        /** 
         * Evaluate the function at x.
         * 
         * @param x  The input value at which to evaluate the function.
         * 
         * @return   The value of the function at x.
         */
        double Evaluate(double x) const;
        virtual std::string ToString(void) const;
    private:
        double v_asy;   /**< Equation parameter */
        double IM_asy;  /**< Equation parameter */
        double alpha;   /**< Equation parameter */
    };

/**
 * @brief Power-law curve
 *
 * This class uses two parameters to generate a curve, according to the
 * expression:
 *
 *     y = a * x^b
 */
    class PowerLawParametricCurve :public FormulaFn
    {
    public:
        /** 
         * Constructor; must provide equation parameters.
         * 
         * @param a  Equation parameter.
         * @param b  Equation parameter.     
         * 
         * @return 
         */
        PowerLawParametricCurve(double a, double b);
        ~PowerLawParametricCurve() { }; /**< Default destructor; do nothing. */

        /** 
         * Evaluate the function at x.
         * 
         * @param x  The input value at which to evaluate the function.
         * 
         * @return  The value of the function at x.
         */
        double Evaluate(double x) const; 
        virtual std::string ToString(void) const;
    private:
        double a;  /**< Equation parameter */
        double b;  /**< Equation parameter */
    };

/**
 * @brief Interopolated Function
 * 
 * This is the base class for interpolated functions, which are based on a set
 * of (input, output) std::pairs provided to the constructor, and an interpolation
 * algorithm.
 */
    class InterpolatedFn : public DeterministicFn
    {
    protected:
        /**
         * The default constructor stores the provided (x, y) std::pairs here, but at
         * this point this is done solely to facilitate development and
         * debugging. None of the subclasses need this data beyond their constructor.
         */
        std::vector<std::pair<double, double> > data; 
    public:
        /**
         * The default constructor takes arrays of corresponding input and output values.
         */
        InterpolatedFn(double x[], double y[], size_t size);
        ~InterpolatedFn() { };  /**< Default destructor; does nothing. */

        /** 
         * Subclasses will override this method to perform interpolation.
         * 
         * @param x  The value at which to evaluate the function.
         * 
         * @return   The interpolated value at x.
         */
        virtual double Evaluate(double x) const { return NAN; };
        virtual std::string ToString(void) const { return "Interpolated Function"; };
    };

/**
 * @brief Linearly-interpolated Function
 * 
 * This class linearly interpolates between the provided data points.
 */
    class LinearInterpolatedFn : public InterpolatedFn
    {
    public: 
        /** 
         * Construct a function using linear interpolation, given a set of data
         * points.
         * 
         * @param x      An array of input values for the function
         * @param y      An array of corresponding output values
         * @param size   The size of the arrays
         */
        LinearInterpolatedFn(double x[], double y[], size_t size);

        /** 
         * Linearly interpolate the function value, using the points provided in the
         * constructor.
         * 
         * @param x  The input value to interpolate at.
         * 
         * @return The interpolated value for x, or NAN if x is out of range.
         */
        virtual double Evaluate(double x) const;

        ~LinearInterpolatedFn();  /**< Destructor; releases GSL structures. */
        virtual std::string ToString(void) const;
    protected: 
        /**
         * GSL spline used for interpolation. This is allocated in the constructor, and
         * maintained for the lifetime of the object.
         */
        gsl_spline *interp;

        /**
         * GSL interpolation accelator. This is allocated in the constructor, and
         * maintained for the lifetime of the object.
         */
        gsl_interp_accel *accel;
    };

/**
 * @brief Log-log-interpolated Function
 * 
 * This class linearly interpolates between the logs of the provided data
 * points.
 */
    class LogLogInterpolatedFn : public InterpolatedFn
    {
    public: 
        /** 
         * Construct a function using log-log interpolation, given a set of data
         * points.
         * 
         * @param x      An array of input values for the function
         * @param y      An array of corresponding output values
         * @param size   The size of the arrays
         */
        LogLogInterpolatedFn(double x[], double y[], size_t size);

        /** 
         * Logarithmicly interpolate the function value, using the points provided
         * in the constructor.
         * 
         * @param x  The input value to interpolate at.
         * 
         * @return The interpolated value for x, or NAN if x is out of range.
         */
        virtual double Evaluate(double x) const;

        ~LogLogInterpolatedFn();  /**< Destructor; releases GSL structures. */
        virtual std::string ToString(void) const;
    protected:
        /**
         * GSL spline used for interpolation. This is allocated in the constructor, and
         * maintained for the lifetime of the object.
         */
        gsl_spline *interp;

        /**
         * GSL interpolation accelator. This is allocated in the constructor, and
         * maintained for the lifetime of the object.
         */
        gsl_interp_accel *accel;
    };

/**
 * @brief Probalistic Functions
 * 
 * This is the base class for functions which accept a single (double-precision)
 * argument, and return a single (double-precision) result. The result will not
 * always be the same, but varies pseudo-randomly according to a probality
 * distribution set when the object is created.
 *
 * This contrasts with deterministic functions, which always return the same
 * value for a given input.
 */
    class ProbabilisticFn : public Replaceable<ProbabilisticFn>
    {
    public:
        /** 
         * @brief General constructor
         *
         * This constructor takes two shared pointers to deterministic functions, which
         * define the distribution of the function result for a given input value.
         *
         * @param mu_function     A function describing the 'mu' of the distribution.
         * @param sigma_function  A function describing the 'sigma' of the distribution.
         */
        ProbabilisticFn(std::shared_ptr<DeterministicFn> mu_function,
                              std::shared_ptr<DeterministicFn> sigma_function);

        ProbabilisticFn ProbabilisticFn_mu_lnX_sigma_lnX(std::shared_ptr<DeterministicFn> mu_function,
                                               std::shared_ptr<DeterministicFn> sigma_function);


        /** 
         * @brief Probability of Exceedence 
         * 
         * Returns the probability (from zero to one) that the result of the
         * function at input x will be at least min_y.
         * 
         * @param x      The input point at which to evaluate the function.
         * @param min_y  The minimum result we're interested in.
         * 
         * @return The probability that f(x) >= min_y.
         */
        virtual double P_exceedence(double x, double min_y) const { return NAN; };

        /** 
         * @brief Inverse Exceedence
         * 
         * Given an input value 'x' and a probability 'p', returns the output value
         * 'y' such that f(x) <= y with probability p. For example, if p == 0.10, 
         * then we'll return a value y such that f(x) <= y 10% of the time.
         *
         * @param x The input point at which to evaluate the function.
         * @param p The probability we're interested in.
         * 
         * @return y, such that f(x) <= y with probability p.
         */
        virtual double X_at_exceedence(double x, double p) const { return NAN; };

        /** 
         * @brief Mean Exceedence
         * 
         * Given an input value 'x',  returns the mean of f(x).
          *
         * @param x The input point at which to evaluate the function.
         * 
         * @return mean(f(x))
         */
        virtual double Mean(double x) const { return NAN; };

        ~ProbabilisticFn();
        
        friend std::ostream& operator<<(std::ostream& out, const ProbabilisticFn& o);
        virtual std::string ToString(void) const { return "ProbabilisticFn"; };
    protected:
        /**
         * Shared pointer to the mu function.
         */
        std::shared_ptr<DeterministicFn> mu_function;

        /**
         * Shared pointer to the sigma function.
         */
        std::shared_ptr<DeterministicFn> sigma_function;

        int mu_function_callback_id, sigma_function_callback_id;
    };


/**
 * @brief Log Normal Probabilistic Function
 *
 * This class represents a probabilistic function that follows a log-normal
 * distribution.
 */
    class LogNormalFn : public ProbabilisticFn
    {
    private:
        std::function<double (double)> mean_X, mean_lnX, median_X, sigma_X, sigma_lnX;
        LogNormalDist distribution(double x) const;
    public:
        typedef enum { MEAN_X, MEAN_LN_X, MEDIAN_X } M_TYPE;
        typedef enum { SIGMA_X, SIGMA_LN_X } S_TYPE;
        
        /** 
         * Constructor for a log-normal probabilistic function.
         * 
         * @param mu_function     Shared pointer to the mu function.
         * @param sigma_function  Shared pointer to the sigma function.
         */
        LogNormalFn(std::shared_ptr<DeterministicFn> mu_function, M_TYPE m_type,
                          std::shared_ptr<DeterministicFn> sigma_function, S_TYPE s_type);

        /** 
         * @brief Probability of Exceedence 
         * 
         * Returns the probability (from zero to one) that the result of the
         * function at input x will be at least min_y.
         * 
         * @param x      The input point at which to evaluate the function.
         * @param min_y  The minimum result we're interested in.
         * 
         * @return The probability that f(x) >= min_y.
         */
        virtual double P_exceedence(double x, double min_y) const;

        /** 
         * @brief Inverse Exceedence
         * 
         * Given an input value 'x' and a probability 'p', returns the output value
         * 'y' such that f(x) <= y with probability p. For example, if p == 0.10,
         * then we'll return a value y such that f(x) <= y 10% of the time.
         *
         * @param x The input point at which to evaluate the function.
         * @param p The probability we're interested in.
         * 
         * @return y, such that f(x) <= y with probability p.
         */
        virtual double X_at_exceedence(double x, double p) const;

        /** 
         * @brief Mean Exceedence
         * 
         * Given an input value 'x',  returns the mean of f(x).
         *
         * @param x The input point at which to evaluate the function.
         * 
         * @return mean(f(x))
         */
        virtual double Mean(double x) const;

        ~LogNormalFn() { }; /**< Destructor; doesn't need to do anything. */
        virtual std::string ToString(void) const;
    };


    class wrapped_DeterministicFn
    {
    private:
    protected:
        std::shared_ptr<DeterministicFn> function;
        wrapped_DeterministicFn() : function() {};
    public:
        ~wrapped_DeterministicFn(void) {
            std::cout << "Destroying wrapped_DeterministicFn: " 
                      << this << "; " << function << " [" << function.use_count() << "]"
                      << std::endl;
        };

        wrapped_DeterministicFn(const wrapped_DeterministicFn &other) {
            this->function = other.function;
            std::cout << "Copy Constructor: " << this << " [" << this->function << "], " 
                      << &other << "[" << other.function << "]" << std::endl;
            std::cout << this->function.use_count() << ", " << other.function.use_count() << std::endl;
        }

        /** 
         * Evaluate the function at a given input. This is the public interface,
         * which will invoke Evaluate() to perform the calculation. The class is
         * structured this way to facilitate instrumenting or caching all
         * DeterministicFn objects without having to change any of the
         * subclasses.
         * 
         * @param x  The input at which to evaluate the function.
         * 
         * @return The result of evaluating the function at x.
         */
        double ValueAt(double x) const;

        /** 
         * Return the derivative of the function at a given input. This is used when
         * integrating Exceedance curves.  The default implementation uses the Gnu
         * Scientific Library to determine the derivative, but subclasses can
         * override this method to perform their own calculations.
         * 
         * @param x  The input at which to evaluate the derivative.
         * 
         * @return  The derivative of the function at x.
         */
        double DerivativeAt(double x) const;

        friend std::ostream& operator<<(std::ostream& out, const DeterministicFn& o);

        std::string ToString(void) const;
    };

    class wrapped_NonLinearHyperbolicLaw : public wrapped_DeterministicFn
    {
    public:
        wrapped_NonLinearHyperbolicLaw(double v_asy, double IM_asy, double alpha);
    };
}

#endif
