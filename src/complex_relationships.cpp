/**
 * @file   complex_relationships.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Wed Sep 19 14:03:27 NZST 2018
 * 
 * @brief Implementation of classes representing IM and EDP objects with different
 *        functions defining their behaviour in the X and Y directions.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2018 Canterbury University
 */
#include "complex_relationships.h"

namespace SLAT {
    Complex_IM::Complex_IM()
    {
        std::cout << "> Complex_IM()" << std::endl;
        std::cout << "< Complex_IM()" << std::endl;
    };

    Complex_IM::~Complex_IM() {
        std::cout << "> ~Complex_IM()" << std::endl;
        std::cout << "< ~Complex_IM()" << std::endl;
    };
}
