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

#include <vector>

namespace SLAT {
    class FragilityFunction {
    public:
        typedef struct {
            double mu_lnX;
            double sigma_lnX;
        } damage_state ;

        FragilityFunction(std::vector<std::pair<double, double>> onsets);
        
        ~FragilityFunction();
        int n_states(void);
        
        double pDamage(int state, double edp);
        std::vector<double> pDamage(double edp);
        const std::vector<damage_state> get_damage_states(void);
    private:
        std::vector<damage_state> damage_states;
    };
}
#endif
