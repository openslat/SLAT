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

BOOST_AUTO_TEST_CASE(BiLevelLoss1)
{
    BiLevelLoss b(10, 20, 500, 1000, 0.4);
    LogNormalDist distribution = b.LossFunctionsForCount(1);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 500, 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), 0.4, 0.1);

    distribution = b.LossFunctionsForCount(10);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 500, 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), 0.4, 0.1);

    distribution = b.LossFunctionsForCount(20);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 1000, 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), 0.4, 0.1);

    distribution = b.LossFunctionsForCount(30);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 1000, 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), 0.4, 0.1);

    distribution = b.LossFunctionsForCount(15);
    BOOST_CHECK_CLOSE(distribution.get_mean_X(), 750, 0.1);
    BOOST_CHECK_CLOSE(distribution.get_sigma_lnX(), 0.4, 0.1);
}
