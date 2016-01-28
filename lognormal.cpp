/**
 * @file   lognormal.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief  Classes representing lognormal functions.
 * 
 * This file part of SLAT (the Seismic Lognormal Assessment Tool).
 *
 * ©2015 Canterbury University
 */
#include <gsl/gsl_cdf.h>
#include "lognormal.h"
#include <cmath>
#include <iostream>
using namespace std;

namespace SLAT {
    static inline double mean_X_from_mu_lnX_and_sigma_lnX(double mu_lnX, double sigma_lnX)
    {
        return exp(mu_lnX + sigma_lnX * sigma_lnX / 2.0);
    };

    static inline double mu_lnX_from_mean_X_and_sigma_lnX(double mean_X, double sigma_lnX)
    {
        return log(mean_X) - sigma_lnX * sigma_lnX / 2.0;
    }

    LognormalFunction::LognormalFunction(void) {
        mu_lnX = NAN;
        sigma_lnX = NAN;
        
    }

    const LognormalFunction LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(double mean_lnX, double sigma_lnX)
    {
        LognormalFunction result;
        result.mu_lnX = mu_lnX_from_mean_X_and_sigma_lnX(mean_lnX, sigma_lnX);
        result.sigma_lnX = sigma_lnX;
        return result;
    };

    double LognormalFunction::p_at_least(double x)
    {
        return gsl_cdf_lognormal_Q(x, mu_lnX, sigma_lnX);
    }

    double LognormalFunction::p_at_most(double x)
    {
        return gsl_cdf_lognormal_P(x, mu_lnX, sigma_lnX);
    }

    double LognormalFunction::get_mu_lnX(void)
    {
        return mu_lnX;
    }

}
