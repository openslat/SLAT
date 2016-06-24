/**
 * @file   lognormal.h
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief  Classes representing lognormal functions.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */
#ifndef _LOGNORMAL_H_
#define _LOGNORMAL_H_

#include <vector>

namespace SLAT {
    class LogNormalDist {
    public:
        LogNormalDist();
        LogNormalDist(const LogNormalDist &other);
        static LogNormalDist LogNormalDist_from_mu_lnX_and_sigma_lnX(double mu_lnX, double sigma_lnX);
        static LogNormalDist LogNormalDist_from_mean_X_and_sigma_lnX(double mean_X, double sigma_lnX);
        static LogNormalDist LogNormalDist_from_median_X_and_sigma_lnX(double median_X, double sigma_lnX);
        static LogNormalDist LogNormalDist_from_mean_X_and_sigma_X(double mean_X, double sigma_X);
        ~LogNormalDist() {};

        double p_at_least(double x) const;
        double p_at_most(double x) const;

        double x_at_p(double p) const;

        double get_mu_lnX(void) const;
        double get_median_X(void) const;
        double get_mean_X(void) const;
        double get_sigma_lnX(void) const;
        double get_sigma_X(void) const;

        LogNormalDist WeighDistribution(double weight);

        static LogNormalDist AddWeightedDistributions(
            const std::vector<LogNormalDist> distributions, 
            const std::vector<double> weights);
        static LogNormalDist AddDistributions(
            const std::vector<LogNormalDist> distributions);
    private:
        double mu_lnX;
        double sigma_lnX;
    };
}
#endif
