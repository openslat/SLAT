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

#include "lognormaldist.h"
#include <vector>

namespace SLAT {
    class FragilityFn {
    public:
        FragilityFn(std::vector<LogNormalDist> onsets);
        
        ~FragilityFn();
        std::size_t n_states(void);
        
        std::vector<double> pExceeded(double edp);
        std::vector<double> pHighest(double edp);
    private:
        std::vector<LogNormalDist> damage_states;
    };
}
#endif
