/**
 * @file   complex_relationships.h
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Wed Sep 19 13:45:50 NZST 2018
 * 
 * @brief  Classes representing IM and EDP relationships, with separate
 *         functions for X and Y directions.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2018 Canterbury University
 */
#ifndef _COMPLEX_RELATIONSHIPS_H_
#define _COMPLEX_RELATIONSHIPS_H_

#include <unordered_map>
#include "functions.h"
#include "replaceable.h"
#include <iostream>

namespace SLAT {

/**
 * @brief Complex_IM
 * 
 * A 'Complex_IM' object represents an intensity measurement function, which
 * has separate functions describing the behaviour in the X and Y directions.
 *
 * It has three deterministic functions: one for each of the X and Y directions, 
 * and another, 'base' function which is used as a reference for the other two.
 *
 * At this point, I expect the three functions to be generated using NZS 1170, 
 * with the base function using a period of 0, and the X and Y functions using 
 * the fundamental periods of the structure in the respective directions.
 *
 * The functions will be 'aligned' based on the rate-of-exceedance. That is, 
 * given a value for the base acceleration, the X and Y accelerations will be
 * those which have the same rate-of-exceedance as that of the base value.
 */
    class Complex_IM : public Replaceable<Complex_IM>
    {
    public:
        enum Component { BASE, X, Y };
        Complex_IM(std::shared_ptr<DeterministicFn> fn_base,
                   std::shared_ptr<DeterministicFn> fn_x,
                   std::shared_ptr<DeterministicFn> fn_y,
                   std::string name);
        ~Complex_IM();

        double lambda(Component which, double where);
        double ValueFromLambda(Component which, double lambda);
        double DerivativeAt(Component which, double where);

        virtual std::string ToString(void) const;
        friend std::ostream& operator<<(std::ostream& out, const Complex_IM& o);
    private:
        int callback_id_base, callback_id_x, callback_id_y;
        std::shared_ptr<DeterministicFn> fn_base, fn_x, fn_y;
        std::string name;
    };
}
#endif
