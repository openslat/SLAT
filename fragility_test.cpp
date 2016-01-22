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
                    {0.0230, 0.4},
                    {0.0062, 0.4}});
        }, std::invalid_argument);

    BOOST_CHECK_THROW(
        {   
            FragilityFunction fragFn({
                    {0.0062, 0.4},
                    {0.0440, 0.4},
                    {0.00230, 0.4}});
        }, std::invalid_argument);

    BOOST_CHECK_THROW(
        {   
            FragilityFunction fragFn({
                    {0.0062, 0.4},
                    {0.0230, 0.4},
                    {0.0440, 0.4},
                    {0.0564, 0.4},
                    {0.0564, 0.4}});
        }, std::invalid_argument);

    /*
      This should succeed. Make sure the values are stored correctly.
    */
    {
        FragilityFunction fragFn({
                {0.0062, 0.1},
                {0.0230, 0.2},
                {0.0440, 0.3},
                {0.0564, 0.4}});
        BOOST_CHECK_EQUAL(fragFn.n_states(), 4);

        vector<FragilityFunction::damage_state> damage_states = fragFn.get_damage_states();
        BOOST_CHECK_EQUAL(damage_states.size(), 4);
        BOOST_CHECK_EQUAL(damage_states[0].mu_lnX, 0.0062);
        BOOST_CHECK_EQUAL(damage_states[0].sigma_lnX, 0.1);

        BOOST_CHECK_EQUAL(damage_states[1].mu_lnX, 0.0230);
        BOOST_CHECK_EQUAL(damage_states[1].sigma_lnX, 0.2);

        BOOST_CHECK_EQUAL(damage_states[2].mu_lnX, 0.0440);
        BOOST_CHECK_EQUAL(damage_states[2].sigma_lnX, 0.3);

        BOOST_CHECK_EQUAL(damage_states[3].mu_lnX, 0.0564);
        BOOST_CHECK_EQUAL(damage_states[3].sigma_lnX, 0.4);
    }
}
