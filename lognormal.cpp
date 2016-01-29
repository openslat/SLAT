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

    static inline double sigma_lnX_from_mean_X_and_sigma_X(double mean_X, double sigma_X)
    {
        return sqrt(log(1.0 + (sigma_X * sigma_X) / (mean_X * mean_X)));
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

    const LognormalFunction LognormalFunction::Lognormal_from_mean_X_and_sigma_X(double mean_X, double sigma_X)
    {
        LognormalFunction result;
        result.sigma_lnX = sigma_lnX_from_mean_X_and_sigma_X(mean_X, sigma_X);
        result.mu_lnX = mu_lnX_from_mean_X_and_sigma_lnX(mean_X, result.sigma_lnX);
        return result;
    };

    const LognormalFunction LognormalFunction::Lognormal_from_mu_lnX_and_sigma_lnX(double mu_lnX, double sigma_lnX)
    {
        LognormalFunction result;
        result.mu_lnX = mu_lnX;
        result.sigma_lnX = sigma_lnX;
        return result;
    };

    double LognormalFunction::p_at_least(double x) const
    {
        return gsl_cdf_lognormal_Q(x, mu_lnX, sigma_lnX);
    }

    double LognormalFunction::p_at_most(double x) const
    {
        return gsl_cdf_lognormal_P(x, mu_lnX, sigma_lnX);
    }

    double LognormalFunction::x_at_p(double p) const
    {
        return gsl_cdf_lognormal_Pinv(p, mu_lnX, sigma_lnX);
    }


    double LognormalFunction::get_mu_lnX(void) const
    {
        return mu_lnX;
    }

    double LognormalFunction::get_median_X(void) const
    {
        return exp(mu_lnX);
    }

    double LognormalFunction::get_mean_X(void) const
    {
        return exp(mu_lnX + sigma_lnX * sigma_lnX /2);
    }

    double LognormalFunction::get_sigma_lnX(void) const
    {
        return sigma_lnX;
    }

    double LognormalFunction::get_sigma_X(void) const
    {
        return  get_mean_X() * sqrt(exp(get_sigma_lnX() * get_sigma_lnX()) - 1);
    }

    LognormalFunction LognormalFunction::AddWeightedDistributions(
        const std::vector<LognormalFunction> distributions, 
        const std::vector<double> weights)
    {
        if (distributions.size() != weights.size()) {
            throw std::invalid_argument("sizes differ");
        }

        double new_mean_X = 0;
        double new_var_X = 0;

        for (size_t i=0; i < distributions.size(); i++) {
            double mean_X = distributions[i].get_mean_X();
            double sd_X = distributions[i].get_sigma_X();

            new_mean_X += mean_X * weights[i];
            new_var_X += (mean_X * mean_X + sd_X * sd_X) * weights[i];
        }
        new_var_X -= new_mean_X * new_mean_X;
        return Lognormal_from_mean_X_and_sigma_X(new_mean_X, sqrt(new_var_X));
    }

}
