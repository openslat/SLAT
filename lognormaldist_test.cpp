/**
 * @file   lognormaldist_test.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief Unit tests for LogNormalDist class.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */

#include <cmath>
#include "lognormaldist.h"
#include <boost/test/unit_test.hpp>
#include <iostream>
using namespace std;
using namespace SLAT;

/*
 * Arbitrary values for testing conversions between parameters:
 */
const double MU_LN_X = 10.0;
const double SIGMA_LN_X = 1.5;
const double MEDIAN_X = exp(MU_LN_X);
const double MEAN_X = exp(MU_LN_X + SIGMA_LN_X * SIGMA_LN_X / 2.0);
const double SIGMA_X = MEAN_X * sqrt(exp(SIGMA_LN_X * SIGMA_LN_X) - 1.0);

BOOST_AUTO_TEST_CASE(mu_lnX_and_sigma_lnX)
{
    /*
     * Create a LogNormalDist from mu_lnX and sigma_lnX, and verify all ways of
     * representing the distribution are correct:
     */
    LogNormalDist distribution = LogNormalDist::LogNormalDist_from_mu_lnX_and_sigma_lnX(MU_LN_X, SIGMA_LN_X);
    BOOST_CHECK_EQUAL(distribution.get_mu_lnX(), MU_LN_X);
    BOOST_CHECK_EQUAL(distribution.get_sigma_lnX(), SIGMA_LN_X);
    BOOST_CHECK_EQUAL(distribution.get_median_X(), MEDIAN_X);
    BOOST_CHECK_EQUAL(distribution.get_mean_X(), MEAN_X);
    BOOST_CHECK_EQUAL(distribution.get_sigma_X(), SIGMA_X);
}

BOOST_AUTO_TEST_CASE(median_X_and_sigma_lnX)
{
    /*
     * Create a LogNormalDist from median_X and sigma_lnX, and verify all ways
     * of representing the distribution are correct:
     */
    LogNormalDist distribution = LogNormalDist::LogNormalDist_from_median_X_and_sigma_lnX(MEDIAN_X, SIGMA_LN_X);
    BOOST_CHECK_EQUAL(distribution.get_mu_lnX(), MU_LN_X);
    BOOST_CHECK_EQUAL(distribution.get_sigma_lnX(), SIGMA_LN_X);
    BOOST_CHECK_EQUAL(distribution.get_median_X(), MEDIAN_X);
    BOOST_CHECK_EQUAL(distribution.get_mean_X(), MEAN_X);
    BOOST_CHECK_EQUAL(distribution.get_sigma_X(), SIGMA_X);
}

BOOST_AUTO_TEST_CASE(mean_X_and_sigma_lnX)
{
    /*
     * Create a LogNormalDist from mean_X and sigma_lnX, and verify all ways of
     * representing the distribution are correct:
     */
    LogNormalDist distribution = LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(MEAN_X, SIGMA_LN_X);
    BOOST_CHECK_EQUAL(distribution.get_mu_lnX(), MU_LN_X);
    BOOST_CHECK_EQUAL(distribution.get_sigma_lnX(), SIGMA_LN_X);
    BOOST_CHECK_EQUAL(distribution.get_median_X(), MEDIAN_X);
    BOOST_CHECK_EQUAL(distribution.get_mean_X(), MEAN_X);
    BOOST_CHECK_EQUAL(distribution.get_sigma_X(), SIGMA_X);
}

BOOST_AUTO_TEST_CASE(mean_X_and_sigma_X)
{
    /*
     * Create a LogNormalDist from mean_X and sigma_X, and verify all ways of
     * representing the distribution are correct:
     */
    LogNormalDist distribution = LogNormalDist::LogNormalDist_from_mean_X_and_sigma_X(MEAN_X, SIGMA_X);
    BOOST_CHECK_EQUAL(distribution.get_mu_lnX(), MU_LN_X);
    BOOST_CHECK_EQUAL(distribution.get_sigma_lnX(), SIGMA_LN_X);
    BOOST_CHECK_EQUAL(distribution.get_median_X(), MEDIAN_X);
    BOOST_CHECK_EQUAL(distribution.get_mean_X(), MEAN_X);
    BOOST_CHECK_EQUAL(distribution.get_sigma_X(), SIGMA_X);
}

BOOST_AUTO_TEST_CASE(p)
{
    /*
     * Sanity check on probability functions. Probability of being "at least" or "at most" MEDIAN is 50%.
     * 
     * Below MEDIAN, probability of "at most" is less than probability of "at least". Above MEDAIN,
     * the opposite is true. Everywhere, the sum of "at most" and "at least" is one.
     */
    LogNormalDist distribution = LogNormalDist::LogNormalDist_from_mean_X_and_sigma_X(MEAN_X, SIGMA_X);
    BOOST_CHECK_EQUAL(distribution.p_at_least(MEDIAN_X), 0.50);
    BOOST_CHECK_EQUAL(distribution.p_at_most(MEDIAN_X), 0.50);

    for (int i=0; i < 100; i++) {
        double x = MEDIAN_X * i / 100.0;
        BOOST_CHECK(distribution.p_at_most(x) < distribution.p_at_least(x));
        BOOST_CHECK_EQUAL(distribution.p_at_most(x) + distribution.p_at_least(x), 1.0);

        x = MEDIAN_X + MEDIAN_X * (i + 1)/ 100.0;
        BOOST_CHECK(distribution.p_at_most(x) > distribution.p_at_least(x));
        BOOST_CHECK_EQUAL(distribution.p_at_most(x) + distribution.p_at_least(x), 1.0);
    }
}

BOOST_AUTO_TEST_CASE(x)
{
    /*
     * Really quick sanity check on x_at_p():
     *    - Probability is zero for x==0
     *    - Probability is 50% for the median
     *    - Probaility of 100% corresponds to infinity
     */
    LogNormalDist distribution = LogNormalDist::LogNormalDist_from_mean_X_and_sigma_X(MEAN_X, SIGMA_X);
    BOOST_CHECK_EQUAL(distribution.x_at_p(0.0), 0.0);
    BOOST_CHECK_EQUAL(distribution.x_at_p(0.50), MEDIAN_X);
    BOOST_CHECK_EQUAL(distribution.x_at_p(1.00), INFINITY);
}
