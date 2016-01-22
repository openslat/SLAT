/**
 * @file   fragility.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief  Classes representing fragility functions.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */
#include "fragility.h"
#include <stdexcept>
#include <cmath>
#include <iostream>
using namespace std;

namespace SLAT {
    FragilityFunction::FragilityFunction(std::vector<std::pair<double, double>> onsets)
    {
        if (onsets.size() == 0) {
            throw std::invalid_argument("onsets");
        } else {
            damage_states.resize(onsets.size());
            double mean = NAN;
            for (uint i=0; i < onsets.size(); i++) {
                if (onsets[i].first <= mean) {
                    throw std::invalid_argument("onsets");
                    break;
                } else {
                    damage_states[i].mu_lnX = onsets[i].first;
                    damage_states[i].sigma_lnX = onsets[i].second;
                    mean = damage_states[i].mu_lnX;
                }
            }   
        }
    };

    FragilityFunction::~FragilityFunction()
    {
    };

    int FragilityFunction::n_states(void)
    {
        return damage_states.size();
    };

    const std::vector<FragilityFunction::damage_state> 
    FragilityFunction::get_damage_states(void)
    {
        return damage_states;
    }
}
