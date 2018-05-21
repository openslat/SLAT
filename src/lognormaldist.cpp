/**
 * @file   lognormal.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief  Classes representing lognormal functions.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */
#include <gsl/gsl_cdf.h>
#include "lognormaldist.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
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
        if (mean_X == 0) {
            return 0.0;
        } else {
            return sqrt(log(1.0 + (sigma_X * sigma_X) / (mean_X * mean_X)));
        }
    }

    LogNormalDist::LogNormalDist(void) {
        mu_lnX = NAN;
        sigma_lnX = NAN;
    }

    LogNormalDist::LogNormalDist(const LogNormalDist &other) {
        mu_lnX = other.mu_lnX;
        sigma_lnX = other.sigma_lnX;
    }

    LogNormalDist LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(double mean_lnX, double sigma_lnX)
    {
        LogNormalDist result;
        result.mu_lnX = mu_lnX_from_mean_X_and_sigma_lnX(mean_lnX, sigma_lnX);
        result.sigma_lnX = sigma_lnX;
        return result;
    };

    LogNormalDist LogNormalDist::LogNormalDist_from_mean_X_and_sigma_X(double mean_X, double sigma_X)
    {
        LogNormalDist result;
        result.sigma_lnX = sigma_lnX_from_mean_X_and_sigma_X(mean_X, sigma_X);
        result.mu_lnX = mu_lnX_from_mean_X_and_sigma_lnX(mean_X, result.sigma_lnX);
        return result;
    };

    LogNormalDist LogNormalDist::LogNormalDist_from_median_X_and_sigma_lnX(double median_X, double sigma_lnX)
    {
        LogNormalDist result;
        result.sigma_lnX = sigma_lnX;
        result.mu_lnX = log(median_X);
        return result;
    };

    LogNormalDist LogNormalDist::LogNormalDist_from_mu_lnX_and_sigma_lnX(double mu_lnX, double sigma_lnX)
    {
        LogNormalDist result;
        result.mu_lnX = mu_lnX;
        result.sigma_lnX = sigma_lnX;
        return result;
    };

    double LogNormalDist::p_at_least(double x) const
    {
        /*
         * If mu_lnX is NAN, we'll assume it's because mean(x) is 0, and return
         * 0 for the rate of exceedence. 
         */
        if (std::isnan(mu_lnX)) {
            return 0;
        } else {
            double result = gsl_cdf_lognormal_Q(x, mu_lnX, sigma_lnX);
            if (result < 0) {
                return 0.0;
            } else {
                return result;
            }
        }
    }

    double LogNormalDist::p_at_most(double x) const
    {
        double result = gsl_cdf_lognormal_P(x, mu_lnX, sigma_lnX);
        if (result < 0) {
            //std::cout << "CLIPPING p_at_most" << std::endl;
            return 0.0;
        } else {
            return result;
        }
    }

    double LogNormalDist::x_at_p(double p) const
    {
        double result = gsl_cdf_lognormal_Pinv(p, mu_lnX, sigma_lnX);
        if (result < 0.0) {
            return 0.0;
        } else {
            return result;
        }
    }


    double LogNormalDist::get_mu_lnX(void) const
    {
        return mu_lnX;
    }

    double LogNormalDist::get_median_X(void) const
    {
        return exp(mu_lnX);
    }

    double LogNormalDist::get_mean_X(void) const
    {
        if (mu_lnX == -INFINITY) {
            //std::cout << "MINUS INFINITY" << std::endl;
            return 0;
        }
        return exp(mu_lnX + sigma_lnX * sigma_lnX /2);
    }

    double LogNormalDist::get_sigma_lnX(void) const
    {
        return sigma_lnX;
    }

    double LogNormalDist::get_sigma_X(void) const
    {
        if (mu_lnX == -INFINITY) {
            return 0;
        } else {
            double result = get_mean_X() * sqrt(exp(get_sigma_lnX() * get_sigma_lnX()) - 1);
            return result;
        }
    }

    LogNormalDist LogNormalDist::AddWeightedDistributions(
        const std::vector<LogNormalDist> distributions, 
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
        new_var_X = abs(new_var_X);
        return LogNormalDist_from_mean_X_and_sigma_X(new_mean_X, sqrt(new_var_X));
    }

    LogNormalDist LogNormalDist::ScaleDistributionByProbability(double probability)
    {
        return ApplyAdjustmentFactor(probability);
    }
    
    LogNormalDist LogNormalDist::ScaleDistributionByNumComponents(double count)
    {
        return ApplyAdjustmentFactor(count);
    }
    
    LogNormalDist LogNormalDist::ApplyAdjustmentFactor(double adjustment_factor)
    {
        return LogNormalDist_from_mean_X_and_sigma_lnX(
            adjustment_factor * get_mean_X(), 
            get_sigma_lnX());
    }

    LogNormalDist LogNormalDist::AddDistributions(
        const std::vector<LogNormalDist> distributions) 
    {
        double new_mean_X = 0;
        double new_var_X = 0;

        for (size_t i=0; i < distributions.size(); i++) {
            double mean_X = distributions[i].get_mean_X();
            new_mean_X += mean_X;

            double sd_X_i = distributions[i].get_sigma_X();
            
            for (size_t j=i; j < distributions.size(); j++) {
                double sd_X_j = distributions[j].get_sigma_X();

                if (i == j) {
                    new_var_X += sd_X_i * sd_X_j;
                } else {
                    new_var_X += 0 * sd_X_i * sd_X_j;
                }
            }
        }
        return LogNormalDist_from_mean_X_and_sigma_X(new_mean_X, sqrt(new_var_X));
    }

    std::ostream &operator<<(std::ostream &s, const LogNormalDist &dist)
    {
        return s << "(lognormal mean=" << dist.get_mean_X()
                 << ", sd_ln=" << dist.get_sigma_lnX()
                 << ")";
    }
}
