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
    FragilityFunction::FragilityFunction(std::vector<LognormalFunction> onsets)
    {
        if (onsets.size() == 0) {
            throw std::invalid_argument("onsets");
        } else {
            damage_states.resize(onsets.size());
            double mean = NAN;
            for (uint i=0; i < onsets.size(); i++) {
                if (onsets[i].get_mu_lnX() <= mean) {
                    throw std::invalid_argument("onsets");
                    break;
                } else {
                    damage_states[i] = onsets[i];
                    mean = damage_states[i].get_mu_lnX();
                }
            }   
        }
    };

    FragilityFunction::~FragilityFunction()
    {
    };

    std::size_t FragilityFunction::n_states(void)
    {
        return damage_states.size();
    };

    std::vector<double> FragilityFunction::pDamage(double edp)
    {
        std::vector<double> result(n_states());
        for (size_t i=0; i < n_states(); i++) {
            result[i] = damage_states[i].p_at_most(edp);
        }
        return result;
    }
}
