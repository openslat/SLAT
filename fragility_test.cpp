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
     * This should succeed:
     */  
    {
        FragilityFunction fragFn({
                {0.0062, 0.1},
                {0.0230, 0.2},
                {0.0440, 0.3},
                {0.0564, 0.4}});
    }
}

BOOST_AUTO_TEST_CASE(Fragility_Initialisation)
{
/*
 *  Make sure the values are stored correctly.
 */  
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

BOOST_AUTO_TEST_CASE(Fragility_Probability)
{
    FragilityFunction fragFn({
            {log(0.0062) - 0.4*0.4/2, 0.4},
            {log(0.0230) - 0.4*0.4/2, 0.4},
            {log(0.0440) - 0.4*0.4/2, 0.4},
            {log(0.0564) - 0.4*0.4/2, 0.4}});
    
    std::vector<double> damage = fragFn.pDamage(0);

    BOOST_CHECK_EQUAL(damage.size(), 4);
    BOOST_CHECK_EQUAL(damage[0], 0.0);
    BOOST_CHECK_EQUAL(damage[1], 0.0);
    BOOST_CHECK_EQUAL(damage[2], 0.0);
    BOOST_CHECK_EQUAL(damage[3], 0.0);

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
