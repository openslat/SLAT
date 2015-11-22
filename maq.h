/**
 * @file   maq.h
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief  Functions for performing fast integration for SLAT.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */
#ifndef _MAQ_H_
#define _MAQ_H_

#include <cmath>
#include <functional>

namespace SLAT {
    namespace Integration {
        
        /*
         * These routines allow the programmer to specify defaults for some of
         * the integration parameters.
         */
        const double TOLERANCE_UNSPECIFIED=0;
        const unsigned int EVALUATIONS_UNSPECIFIED=0;
        
        void Set_Default_Tolerance(double tolerance);
        void Set_Default_Evaluations(unsigned int evaluations);
        double Get_Default_Tolerance(void);
        unsigned int Get_Default_Evaluations(void);
        
/*
 * MAQ integration algorithm
 */
        typedef struct {
            double integral;
            bool successful;
            int evaluations;
        } MAQ_RESULT;
        MAQ_RESULT MAQ(std::function<double (double)> integrand,
                       double tol=TOLERANCE_UNSPECIFIED, 
                       int maxeval=EVALUATIONS_UNSPECIFIED);

    }
}
#endif
