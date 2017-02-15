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
    FragilityFn::FragilityFn(std::vector<LogNormalDist> onsets)
    {
        omp_init_lock(&lock);
        omp_set_lock(&lock);
        if (onsets.size() == 0) {
            omp_unset_lock(&lock);
            throw std::invalid_argument("onsets");
        } else {
            damage_states.resize(onsets.size());
            double mean = NAN;
            for (unsigned int i=0; i < onsets.size(); i++) {
                if (onsets[i].get_mu_lnX() <= mean) {
                    throw std::invalid_argument("onsets");
                    break;
                } else {
                    damage_states[i] = onsets[i];
                    mean = damage_states[i].get_mu_lnX();
                }
            }   
        }
        omp_unset_lock(&lock);
    };

    FragilityFn::~FragilityFn()
    {
    };

    std::size_t FragilityFn::n_states(void)
    {
        omp_set_lock(&lock);
        std::size_t result = damage_states.size();
        omp_unset_lock(&lock);
        return result;
    };

    std::vector<double> FragilityFn::pExceeded(double edp)
    {
        std::vector<double> result(n_states());
        for (size_t i=0; i < n_states(); i++) {
            result[i] = damage_states[i].p_at_most(edp);
        }
        return result;
    }

    std::vector<double> FragilityFn::pHighest(double edp)
    {
        std::vector<double> result = pExceeded(edp);
        for (size_t i=1; i < n_states(); i++) {
            result[i-1] -= result[i];
            if (result[i-1] < 0) {
                result[i-1] = 0;
            }
        }
        return result;
    }

    std::ostream& operator<<(std::ostream& out, FragilityFn& o)
    {
        out << "FragilityFn: ";
        for (size_t i=0; i < o.n_states(); i++) {
            out << o.damage_states[i] << "; ";
        }
        out << std::endl;
        return out;
    }
}
