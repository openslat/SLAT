/**
 * @file   loss_functions.h
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief  Classes representing loss functions.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */
#ifndef _LOSS_FUNCTIONS_H_
#define _LOSS_FUNCTIONS_H_

#include "lognormaldist.h"
#include <memory>
#include <vector>

namespace SLAT {
    class LossFn {
    public:
        LossFn() {};
        LossFn(std::vector<LogNormalDist> distributions);
        
        ~LossFn() {};
        std::size_t n_states(void);
        const std::vector<LogNormalDist> LossFns(void);

        /**
         * Overloaded << operator for printing a LossFn to a stream. Intended
         * for debugging.
         */
        friend std::ostream& operator<<(std::ostream& out, LossFn& o);
    private:
        std::vector<LogNormalDist> loss_functions;
    };
}
#endif
