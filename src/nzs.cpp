/**
 * @file   nzs.cpp
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
#include "nzs.h"
namespace SLAT {
    double C_h(SOIL_CLASS soil, double T)
    {
        if ((soil == A) || (soil == B)) {
            if (T == 0) {
                return(1.0);
            } else if (T < 0.1) {
                return(1.0 + 1.35 * (T/0.1));
            } else if ( T <= 0.3) {
                return(2.35);
            } else if ( T <= 1.5) {
                return(1.60 * pow(0.5 / T, 0.75));
            } else if ( T <=3.0) {
                return(1.05/T) ;
            } else {
                return(3.15/(T*T));
            }
        } else if ( soil == C) {
            if (T == 0.0) {
                return(1.33);
            } else if ( T < 0.1) {
                return(1.33 + 1.60*(T/0.1));
            } else if ( T < 0.3) {
                return(2.93);
            } else if ( T < 1.5) {
                return(2.0 * pow(0.5/T, 0.75));
            } else if ( T < 3) {
                return(1.32/T);
            } else  {
                return(3.96/pow(T, 2));
            }
        } else if ( soil == D) {
            if (T == 0.0) {
                return(1.12);
            } else if ( T <= 0.1) {
                return(1.12 + 1.88*(T/0.1));
            } else if ( T < 0.56) {
                return(3.0);
            } else if ( T < 1.5) {
                return(2.4 * pow(0.75/T, 0.75));
            } else if ( T < 3) {
                return(2.14/T);
            } else  {
                return(6.42/pow(T, 2));
            }
        } else if ( soil == E) {
            if (T <= 0.0) {
                return(1.12);
            } else if ( T < 0.1) {
                return(1.12 + 1.88*(T/0.1));
            } else if ( T < 1.0) {
                return(3.0);
            } else if ( T < 1.5) {
                return(3.0/pow(T, 0.75));
            } else if ( T < 3) {
                return(3.32/T);
            }
        } else {
            return(9.96/pow(T, 2));
        }
        return NAN;
    }

    double N_max(double T)
    {
        if (T <= 1.5) {
            return(1.0);
        } else if (T < 2) {
            return(1.0 + (1.12 - 1.0) * (T-1.5)/(2 - 1.5));
        } else if (T < 3) {
            return(1.12 + (1.36 - 1.12) * (T-2)/(3 - 2));
        } else if (T < 4) {
            return(1.36 + (1.60 - 1.36) * (T-3)/(4 - 3));
        } else if (T < 5) {
            return(1.60 + (1.72 - 1.60) * (T-4)/(5 - 4));
        } else {
            return(1.72);
        }
    }

    double N(double T, double D, double r) 
    {
        if (r >= 1/250 || std::isnan(D)) {
            return(1.0);
        } else if (D <= 2) {
            return(N_max(T));
        } else if (D > 20) {
            return(1.0);
        } else {
            return(1 + (N_max(T) - 1) * (20 - D)/18);
        }
    }
    

    std::shared_ptr<DeterministicFn> NZS_Hazard_Curve(SOIL_CLASS soil,
                                                    double period, 
                                                    double Z, 
                                                    double distance)
    {
        const std::map<int, double> R_mapping = {{  20, 0.20},
                                                 {  25, 0.25},
                                                 {  50, 0.35},
                                                 { 100, 0.5},
                                                 { 250, 0.75},
                                                 { 500, 1.0},
                                                 {1000, 1.3 },
                                                 {2000, 1.7 },
                                                 {2500, 1.8}};
        std::vector<double> lambdas;
        std::vector<double> intensities;
        for (auto const& mapping : R_mapping) {
            double years = mapping.first;
            lambdas.push_back(1/years);
            
            double intensity = C_h(soil, period) * 
                Z *
                mapping.second * 
                N(period, distance, years);
            intensities.push_back(intensity);
        }

        std::shared_ptr<DeterministicFn> result = 
            std::make_shared<LogLogInterpolatedFn>(intensities.data(),
                                                   lambdas.data(),
                                                   R_mapping.size());
        return result;
    }
}
