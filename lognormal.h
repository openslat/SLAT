/**
 * @file   lognormal.h
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief  Classes representing lognormal functions.
 * 
 * This file part of SLAT (the Seismic Lognormal Assessment Tool).
 *
 * ©2015 Canterbury University
 */
#ifndef _LOGNORMAL_H_
#define _LOGNORMAL_H_

namespace SLAT {
    class LognormalFunction {
    public:
        LognormalFunction();
        static const LognormalFunction Lognormal_from_mu_lnX_and_sigma_lnX(double mu_lnX, double sigma_lnX);
        static const LognormalFunction Lognormal_from_mean_X_and_sigma_lnX(double mean_X, double sigma_lnX);
        static const LognormalFunction Lognormal_from_median_X_and_sigma_lnX(double median_X, double sigma_lnX);
        static const LognormalFunction Lognormal_from_mean_X_and_sigma_X(double mean_X, double sigma_X);
        ~LognormalFunction() {};

        double p_at_least(double x) const;
        double p_at_most(double x) const;

        double x_at_p(double p) const;

        double get_mu_lnX(void) const;

    private:
        double mu_lnX;
        double sigma_lnX;
    };
}
#endif
