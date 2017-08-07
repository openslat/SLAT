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
    protected:
        LossFn() {};
    public:
        ~LossFn() {};

        virtual LogNormalDist CalculateLoss(std::vector<double> probabilities, int count);

        virtual std::size_t n_states(void) {
            throw std::invalid_argument("LossFn::n_states()");
            return 0;
        };

        
        /**
         * Overloaded << operator for printing a LossFn to a stream. Intended
         * for debugging.
         */
        friend std::ostream& operator<<(std::ostream& out, LossFn& o);
    };
    
    class SimpleLossFn: public LossFn {
    public:
        SimpleLossFn() {};
        SimpleLossFn(std::vector<LogNormalDist> distributions);
        
        ~SimpleLossFn() {};
        std::size_t n_states(void);

        LogNormalDist CalculateLoss(std::vector<double> probabilities, int count);

        /**
         * Overloaded << operator for printing a LossFn to a stream. Intended
         * for debugging.
         */
        friend std::ostream& operator<<(std::ostream& out, SimpleLossFn& o);
    private:
        std::vector<LogNormalDist> loss_functions;
    };
}
#endif
