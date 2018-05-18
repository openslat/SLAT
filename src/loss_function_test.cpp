 /**
 * @file   loss_function_test.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief Unit tests for LossFunction objects.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */

#include "loss_functions.h"
#include <boost/test/unit_test.hpp>
#include <iostream>
using namespace std;
using namespace SLAT;

/*
 * BiLevelLoss class tests
 */
BOOST_AUTO_TEST_SUITE(BiLevelLoss_Suite)
struct Fixture {
    Fixture() :  b(10, 20, 1000, 500, 0.4)
    {
        BOOST_TEST_MESSAGE( "setup fixture" );
    }

    ~Fixture() {
        BOOST_TEST_MESSAGE( "teardown fixture" ); 
    }
    BiLevelLoss b;
};

// Count below min
BOOST_FIXTURE_TEST_CASE( BiLevelLoss_low, Fixture)
{
    LogNormalDist distribution = b.LossFunctionsForCount(1);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 1000, 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), 0.4, 0.1);
}

// Count at min
BOOST_FIXTURE_TEST_CASE(BiLevelLoss_min, Fixture)
{
    LogNormalDist distribution = b.LossFunctionsForCount(10);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 1000, 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), 0.4, 0.1);
}

// Count at max
BOOST_FIXTURE_TEST_CASE(BiLevelLoss_max, Fixture)
{
    LogNormalDist distribution = b.LossFunctionsForCount(20);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 500, 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), 0.4, 0.1);
}

// Count above max
BOOST_FIXTURE_TEST_CASE(BiLevelLoss_high, Fixture)
{
    LogNormalDist distribution = b.LossFunctionsForCount(30);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 500, 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), 0.4, 0.1);
}

// Counts between min and max
BOOST_FIXTURE_TEST_CASE(BiLevelLoss_mid, Fixture)
{
    LogNormalDist distribution = b.LossFunctionsForCount(12);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 900, 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), 0.4, 0.1);

    distribution = b.LossFunctionsForCount(15);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 750, 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), 0.4, 0.1);
    
    distribution = b.LossFunctionsForCount(17);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 650, 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), 0.4, 0.1);
}
BOOST_AUTO_TEST_SUITE_END()
/*
 * End of BiLevelLoss class tests
 */

/*
 * SimpleLossFn tests
 */
BOOST_AUTO_TEST_SUITE(SimpleLossFn_Suite)
struct Fixture {
    Fixture() : s(
        std::vector<LogNormalDist>({ LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.03, 0.41),
                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.08, 0.42),
                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.25, 0.43),
                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(1.00, 0.44)}))
    {
    }

    ~Fixture() {
    }
    SimpleLossFn s;
};

/*
 * Check with probability of 1 for each damage state, in turn:
 */
BOOST_FIXTURE_TEST_CASE( SimpleLossFn_1, Fixture)
{
    LogNormalDist distribution = s.CalculateUnadjustedLoss(std::vector<double>({1, 0, 0, 0}), 1);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 0.03, 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), 0.41, 0.1);

    distribution = s.CalculateUnadjustedLoss(std::vector<double>({0, 1, 0, 0}), 1);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 0.08, 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), 0.42, 0.1);
    
    distribution = s.CalculateUnadjustedLoss(std::vector<double>({0, 0, 1, 0}), 1);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 0.25, 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), 0.43, 0.1);
    
    distribution = s.CalculateUnadjustedLoss(std::vector<double>({0, 0, 0, 1}), 1);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 1.00, 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), 0.44, 0.1);
}

/*
 * Check with probability of 1 for each damage state, in turn, but with more than one component:
 */
BOOST_FIXTURE_TEST_CASE( SimpleLossFn_2, Fixture)
{
    LogNormalDist distribution = s.CalculateUnadjustedLoss(std::vector<double>({1, 0, 0, 0}), 3);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 3 * 0.03, 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), 0.41, 0.1);

    distribution = s.CalculateUnadjustedLoss(std::vector<double>({0, 1, 0, 0}), 7);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 7 * 0.08, 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), 0.42, 0.1);
    
    distribution = s.CalculateUnadjustedLoss(std::vector<double>({0, 0, 1, 0}), 11);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 11*0.25, 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), 0.43, 0.1);
    
    distribution = s.CalculateUnadjustedLoss(std::vector<double>({0, 0, 0, 1}), 13);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 13 * 1.00, 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), 0.44, 0.1);
}

/*
 * Check with a mix of probabilities, count=1:
 */
BOOST_FIXTURE_TEST_CASE( SimpleLossFn_3, Fixture)
{
    LogNormalDist distribution = s.CalculateUnadjustedLoss(std::vector<double>({0.10, 0.20, 0.30, 0.15}), 1);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 
                      0.10 * 0.03 + 
                      0.20 * 0.08 + 
                      0.30 * 0.25 +
                      0.15 * 1.00, 0.1);

    {
        // Calculate what we expect the dispersion to be, and check it:
        double var = 0;
        double mean = 0;
        {
            // First damage state:
            LogNormalDist d = LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.03, 0.41);
            mean += d.get_mean_X() * 0.10;
            var += (d.get_mean_X() * d.get_mean_X() + d.get_sigma_X() * d.get_sigma_X()) * 0.10;
        }
        {
            // Second damage state:
            LogNormalDist d = LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.08, 0.42);
            mean += d.get_mean_X() * 0.20;
            var += (d.get_mean_X() * d.get_mean_X() + d.get_sigma_X() * d.get_sigma_X()) * 0.20;
        }
        {
            // First third state:
            LogNormalDist d = LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.25, 0.43);
            mean += d.get_mean_X() * 0.30;
            var += (d.get_mean_X() * d.get_mean_X() + d.get_sigma_X() * d.get_sigma_X()) * 0.30;
        }
        {
            // Fourth damage state:
            LogNormalDist d = LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(1.00, 0.44);
            mean += d.get_mean_X() * 0.15;
            var += (d.get_mean_X() * d.get_mean_X() + d.get_sigma_X() * d.get_sigma_X()) * 0.15;
        }
        var -= abs(mean * mean);
        LogNormalDist d = LogNormalDist::LogNormalDist_from_mean_X_and_sigma_X(mean, sqrt(var));

        BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), d.get_sigma_lnX(), 0.1);
    }
}

/*
 * Check with a mix of probabilities, count > 1:
 */
BOOST_FIXTURE_TEST_CASE( SimpleLossFn_4, Fixture)
{
    const double N=5;
    LogNormalDist distribution = s.CalculateUnadjustedLoss(std::vector<double>({0.10, 0.20, 0.30, 0.15}), N);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 
                      N * (0.10 * 0.03 + 
                           0.20 * 0.08 + 
                           0.30 * 0.25 +
                           0.15 * 1.00), 0.1);

    {
        // Calculate what we expect the dispersion to be, and check it:
        double var = 0;
        double mean = 0;
        {
            // First damage state:
            LogNormalDist d = LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.03, 0.41);
            mean += d.get_mean_X() * 0.10;
            var += (d.get_mean_X() * d.get_mean_X() + d.get_sigma_X() * d.get_sigma_X()) * 0.10;
        }
        {
            // Second damage state:
            LogNormalDist d = LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.08, 0.42);
            mean += d.get_mean_X() * 0.20;
            var += (d.get_mean_X() * d.get_mean_X() + d.get_sigma_X() * d.get_sigma_X()) * 0.20;
        }
        {
            // First third state:
            LogNormalDist d = LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.25, 0.43);
            mean += d.get_mean_X() * 0.30;
            var += (d.get_mean_X() * d.get_mean_X() + d.get_sigma_X() * d.get_sigma_X()) * 0.30;
        }
        {
            // Fourth damage state:
            LogNormalDist d = LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(1.00, 0.44);
            mean += d.get_mean_X() * 0.15;
            var += (d.get_mean_X() * d.get_mean_X() + d.get_sigma_X() * d.get_sigma_X()) * 0.15;
        }
        var -= abs(mean * mean);
        // NOTE: We expect the standard deviation(log(x)) to be the same for N
        // components as it is for one component:
        LogNormalDist d = LogNormalDist::LogNormalDist_from_mean_X_and_sigma_X(mean, sqrt(var));

        BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), d.get_sigma_lnX(), 0.1);
    }
}
BOOST_AUTO_TEST_SUITE_END()
/*
 * End of BiLevelLossFn tests
 */

/*
 * BiLevelLossFn tests
 */
BOOST_AUTO_TEST_SUITE(BiLevelLossFn_Suite)
struct Fixture {
    Fixture() : b(
        std::vector<std::shared_ptr<BiLevelLoss>>(
            { std::make_shared<BiLevelLoss>(6, 12, 8000, 5000, 0.3),
                    std::make_shared<BiLevelLoss>(5, 13, 15000, 10000, 0.4),
                    std::make_shared<BiLevelLoss>(3, 15, 60000, 45000, 0.5)}))
    {
    }

    ~Fixture() {
    }
    BiLevelLossFn b;
};

// One component
BOOST_FIXTURE_TEST_CASE( BiLevelLossFn_1, Fixture)
{
    std::vector<double> p({0.10, 0.20, 0.40});
    LogNormalDist distribution = b.CalculateUnadjustedLoss(p, 1);

    // Create a simple loss function that uses the same values at the
    // BiLevelLossFn will use for one component:
    SimpleLossFn s(
        std::vector<LogNormalDist>({
                LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(8000, 0.03),
                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(15000, 0.4),
                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(60000, 0.5)}));
    LogNormalDist d  = s.CalculateUnadjustedLoss(p, 1);

    BOOST_CHECK_CLOSE(distribution.get_mean_X(), d.get_mean_X(), 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), d.get_sigma_lnX(), 0.1);
}

// One component, with and adjustment factor
BOOST_FIXTURE_TEST_CASE( BiLevelLossFn_1_adj, Fixture)
{
    const double adjustment_factor = 3.14;
    std::vector<double> p({0.10, 0.20, 0.40});
    LogNormalDist distribution = b.CalculateLoss(p, 1, adjustment_factor);
    
    // Create a simple loss function that uses the same values at the
    // BiLevelLossFn will use for one component:
    SimpleLossFn s(
        std::vector<LogNormalDist>({
                LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(8000, 0.03),
                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(15000, 0.4),
                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(60000, 0.5)}));
    LogNormalDist d  = s.CalculateLoss(p, 1, adjustment_factor);

    BOOST_CHECK_CLOSE(distribution.get_mean_X(), d.get_mean_X(), 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), d.get_sigma_lnX(), 0.1);
}

// More components than the max for each loss function
BOOST_FIXTURE_TEST_CASE( BiLevelLossFn_2, Fixture)
{
    LogNormalDist distribution = b.CalculateUnadjustedLoss(std::vector<double>({0.10, 0.20, 0.40}), 20);
    
    // Create a simple loss function that uses the same values at the
    // BiLevelLossFn will use for one component:
    SimpleLossFn s(
        std::vector<LogNormalDist>({
                LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(5000, 0.03),
                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(10000, 0.4),
                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(45000, 0.5)}));
    LogNormalDist d  = s.CalculateUnadjustedLoss(std::vector<double>({0.10, 0.20, 0.40}), 20);

    BOOST_CHECK_CLOSE(distribution.get_mean_X(), d.get_mean_X(), 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), d.get_sigma_lnX(), 0.1);
}

// Number of components somewhere in the middle:
BOOST_FIXTURE_TEST_CASE( BiLevelLossFn_3, Fixture)
{
    LogNormalDist distribution = b.CalculateUnadjustedLoss(std::vector<double>({0.10, 0.20, 0.40}), 8);
    
    // Create a simple loss function that uses the same values at the
    // BiLevelLossFn will use for one component:
    SimpleLossFn s(
        std::vector<LogNormalDist>({
                LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(7000, 0.03),
                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(13125, 0.4),
                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(53750, 0.5)}));
    LogNormalDist d  = s.CalculateUnadjustedLoss(std::vector<double>({0.10, 0.20, 0.40}), 8);

    BOOST_CHECK_CLOSE(distribution.get_mean_X(), d.get_mean_X(), 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), d.get_sigma_lnX(), 0.1);
}

// Number of components somewhere in the middle:
BOOST_FIXTURE_TEST_CASE( BiLevelLossFn_5, Fixture)
{
    LogNormalDist distribution = b.CalculateUnadjustedLoss(std::vector<double>({0.10, 0.20, 0.40}), 5);
    
    // Create a simple loss function that uses the same values at the
    // BiLevelLossFn will use for one component:
    SimpleLossFn s(
        std::vector<LogNormalDist>({
                LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(8000, 0.03),
                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(15000, 0.4),
                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(57500, 0.5)}));
    LogNormalDist d  = s.CalculateUnadjustedLoss(std::vector<double>({0.10, 0.20, 0.40}), 5);

    BOOST_CHECK_CLOSE(distribution.get_mean_X(), d.get_mean_X(), 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), d.get_sigma_lnX(), 0.1);

    distribution = b.CalculateUnadjustedLoss(std::vector<double>({0.10, 0.20, 0.40}), 5);    
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), d.get_mean_X(), 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), d.get_sigma_lnX(), 0.1);
}

BOOST_AUTO_TEST_SUITE_END()
/*
 * End of BiLevelLossFn tests
 */
