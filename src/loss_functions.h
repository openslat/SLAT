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
#include "caching.h"
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

    class BiLevelLoss {
    public:
        BiLevelLoss(int count_min, int count_max, double cost_at_min, double cost_at_max, double dispersion)
            : LossFunctionsForCount([this] (int count) {
                    return this->LossFunctionsForCount_calc(count);
                }, std::string("BiLevelLoss::LossFunctionsForCount"))
        {
            this->count_min = count_min;
            this->count_max = count_max;
            this->cost_at_min = cost_at_min;
            this->cost_at_max = cost_at_max;
            this->dispersion = dispersion;
        };
        ~BiLevelLoss() {};

        int count_min, count_max;
        double cost_at_min, cost_at_max, dispersion;

        friend std::ostream& operator<<(std::ostream& out, BiLevelLoss& o);

        Caching::CachedFunction<LogNormalDist, int> LossFunctionsForCount;
    private:
        LogNormalDist LossFunctionsForCount_calc(int count);
    };
    
    class BiLevelLossFn: public LossFn {
        BiLevelLossFn();
        BiLevelLossFn(std::vector<BiLevelLoss> distributions);
        
        ~BiLevelLossFn() {};
        std::size_t n_states(void);

        LogNormalDist CalculateLoss(std::vector<double> probabilities, int count);

        /**
         * Overloaded << operator for printing a LossFn to a stream. Intended
         * for debugging.
         */
        friend std::ostream& operator<<(std::ostream& out, BiLevelLossFn& o);
    private:
        std::vector<BiLevelLoss> distributions;
    };
}
#endif
