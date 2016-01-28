/**
 * @file   fragility_test.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Fri Jan 22 14:57:54 NZDT 2016
 * 
 * @brief Unit tests for classes declared in fragility.h.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */

#include "fragility.h"
#include <iostream>
#include <stdexcept>
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace SLAT;

BOOST_AUTO_TEST_CASE(Fragility_Constructor)
{
    /*
     * Must have at least one damage state:
     */
    BOOST_CHECK_THROW(
        {   
            FragilityFunction fragFn({});
        }, std::invalid_argument);

    /*
     * Damage states must be in order:
     */
    BOOST_CHECK_THROW(
        {   
            FragilityFunction fragFn({
                    LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0230, 0.4),
                        LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0062, 0.4)});
        }, std::invalid_argument);

    BOOST_CHECK_THROW(
        {   
            FragilityFunction fragFn({
                    LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0062, 0.4),
                        LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0440, 0.4),
                        LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.00230, 0.4)});
        }, std::invalid_argument);

    BOOST_CHECK_THROW(
        {
            FragilityFunction fragFn({
                    LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0062, 0.4),
                        LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0230, 0.4),
                        LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0440, 0.4),
                        LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0564, 0.4),
                        LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0564, 0.4)});
            }, std::invalid_argument);

    /*
     * This should succeed:
     */  
    {
        FragilityFunction fragFn({
                LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0062, 0.1),
                LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0230, 0.2),
                LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0440, 0.3),
                LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0564, 0.4)});
    }
}

BOOST_AUTO_TEST_CASE(Fragility_Initialisation)
{
/*
 *  Make sure the values are stored correctly.
 */  
    FragilityFunction fragFn({
            LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0062, 0.1),
            LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0230, 0.2),
            LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0440, 0.3),
            LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0564, 0.4)});
    BOOST_CHECK_EQUAL(fragFn.n_states(), 4);
}

BOOST_AUTO_TEST_CASE(Fragility_Probability)
{
    /*
     * This is the fragility function from the simplified bridge example. The
     * values in the paper represent mean_X and sigma_lnX, so convert to
     * mean_lnX and sigma_lnX:
     */
    FragilityFunction fragFn({
            LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0062, 0.4),
            LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0230, 0.4),
                LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0440, 0.4),
                LognormalFunction::Lognormal_from_mean_X_and_sigma_lnX(0.0564, 0.4)});
    
    /*
     * At EDP=0, the probability for eall damage states will be zero:
     */
    std::vector<double> damage = fragFn.pDamage(0);
    BOOST_CHECK_EQUAL(damage.size(), 4);
    BOOST_CHECK_EQUAL(damage[0], 0.0);
    BOOST_CHECK_EQUAL(damage[1], 0.0);
    BOOST_CHECK_EQUAL(damage[2], 0.0);
    BOOST_CHECK_EQUAL(damage[3], 0.0);

    /*
     * The following tests look at the probabilities at the median value for
     * each loss function (probability 50% for the corresponding damage state).
     * The other damage states are tested more loosely agains approximate
     * values.
     */
    damage = fragFn.pDamage(exp(log(0.0062) - 0.4*0.4/2));
    BOOST_CHECK_EQUAL(damage.size(), 4);
    BOOST_CHECK_CLOSE(damage[0], 0.5, 1E-1);
    BOOST_CHECK_SMALL(damage[1], 1E-1);
    BOOST_CHECK_SMALL(damage[2], 1E-1);
    BOOST_CHECK_SMALL(damage[3], 1E-1);

    damage = fragFn.pDamage(exp(log(0.0230) - 0.4*0.4/2));
    BOOST_CHECK_EQUAL(damage.size(), 4);
    BOOST_CHECK_CLOSE(damage[0], 1.00, 1E-1);
    BOOST_CHECK_CLOSE(damage[1], 0.50, 1E-1);
    BOOST_CHECK_CLOSE(damage[2], 0.05, 10);
    BOOST_CHECK_CLOSE(damage[3], 0.012, 10);

    damage = fragFn.pDamage(exp(log(0.0440) - 0.4*0.4/2));
    BOOST_CHECK_EQUAL(damage.size(), 4);
    BOOST_CHECK_CLOSE(damage[0], 1.00, 1E-1);
    BOOST_CHECK_CLOSE(damage[1], 0.95, 1);
    BOOST_CHECK_CLOSE(damage[2], 0.5, 1E1);
    BOOST_CHECK_CLOSE(damage[3], 0.25, 10);

    damage = fragFn.pDamage(exp(log(0.0564) - 0.4*0.4/2));
    BOOST_CHECK_EQUAL(damage.size(), 4);
    BOOST_CHECK_CLOSE(damage[0], 1.00, 1E-1);
    BOOST_CHECK_CLOSE(damage[1], 0.98, 1);
    BOOST_CHECK_CLOSE(damage[2], 0.75, 10);
    BOOST_CHECK_CLOSE(damage[3], 0.5, 1E-3);

    damage = fragFn.pDamage(exp(log(0.075) - 0.4*0.4/2));
    BOOST_CHECK_EQUAL(damage.size(), 4);
    BOOST_CHECK_CLOSE(damage[0], 1.00, 1E-1);
    BOOST_CHECK_CLOSE(damage[1], 1.00, 1);
    BOOST_CHECK_CLOSE(damage[2], 0.90, 5);
    BOOST_CHECK_CLOSE(damage[3], 0.75, 5);

    damage = fragFn.pDamage(exp(log(0.250) - 0.4*0.4/2));
    BOOST_CHECK_EQUAL(damage.size(), 4);
    BOOST_CHECK_CLOSE(damage[0], 1.00, 1E-1);
    BOOST_CHECK_CLOSE(damage[1], 1.00, 1E-1);
    BOOST_CHECK_CLOSE(damage[2], 1.00, 1E-1);
    BOOST_CHECK_CLOSE(damage[3], 1.00, 1E-1);
}
