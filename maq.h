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
#include <boost/shared_ptr.hpp>

namespace SLAT {
    namespace Integration {
        
        class IntegrationSettings
        {
        public:
            IntegrationSettings(const IntegrationSettings *other);
            static const IntegrationSettings *Get_Global_Settings(void);
            
            void Override_Tolerance(double tolerance);
            void Override_Max_Evals(unsigned int max_evals);  
            void Use_Default_Tolerance(void); // Restore default
            void Use_Default_Max_Evals(void); // Restore default

            /*
             * Change global defaults:
             */
            static void Set_Tolerance(double tolerance);
            static void Set_Max_Evals(unsigned int max_evals);  

            unsigned int Get_Effective_Max_Evals(void) const;
            double Get_Effective_Tolerance(void) const;
        private:
            IntegrationSettings(const IntegrationSettings &other);
            static const unsigned int EVALUATIONS_UNSPECIFIED=0;
            static constexpr double TOLERANCE_UNSPECIFIED=0.0;

            static const unsigned int EVALUATIONS_DEFAULT = 1024;
            static constexpr double TOLERANCE_DEFAULT = 1E-6;
            const IntegrationSettings *parent;
            
            unsigned int max_evals;
            double tolerance;

            IntegrationSettings();
            static IntegrationSettings default_settings;
        };
        
/*
 * MAQ integration algorithm
 */
        typedef struct {
            double integral;
            bool successful;
            int evaluations;
        } MAQ_RESULT;
        MAQ_RESULT MAQ(std::function<double (double)> integrand);
        MAQ_RESULT MAQ(std::function<double (double)> integrand,
                       const IntegrationSettings &settings);
    }
}
#endif
