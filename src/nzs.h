/**
 * @file   nzs.h
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Sep 24 13:04:59 NZST 2018
 * 
 * @brief Generate a deterministic function for a demand curve, using the
 *         instructions in NZS 1170.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2018 Canterbury University
 */
#ifndef _NZS_H_
#define _NZS_H_
#include "functions.h"
namespace SLAT {
    typedef enum { A, B, C, D, E } SOIL_CLASS;
    std::shared_ptr<DeterministicFn> NZS_Hazard_Curve(SOIL_CLASS soil,
                                                      double period,
                                                      double Z, 
                                                      double distance);
}
#endif
