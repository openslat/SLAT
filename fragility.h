/**
 * @file   fragility.h
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief  Classes representing fragility functions.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */
#ifndef _FRAGILITY_H_
#define _FRAGILITY_H_

#include "lognormal.h"
#include <vector>

namespace SLAT {
    class FragilityFunction {
    public:
        FragilityFunction(std::vector<LognormalFunction> onsets);
        
        ~FragilityFunction();
        std::size_t n_states(void);
        
        std::vector<double> pExceeded(double edp);
        std::vector<double> pHighest(double edp);
    private:
        std::vector<LognormalFunction> damage_states;
    };
}
#endif
