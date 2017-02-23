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

#include <boost/move/utility.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;

namespace SLAT {
    namespace Integration {
        extern bool verbose;
        
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
            static void Reset();

            unsigned int Get_Effective_Max_Evals(void) const;
            double Get_Effective_Tolerance(void) const;
            IntegrationSettings();
            IntegrationSettings(const IntegrationSettings &other);
        private:
            static const unsigned int EVALUATIONS_UNSPECIFIED=0;
            static constexpr double TOLERANCE_UNSPECIFIED=0.0;

            static const unsigned int EVALUATIONS_DEFAULT = 1024;
            static constexpr double TOLERANCE_DEFAULT = 1E-6;
            const IntegrationSettings *parent;

            unsigned int max_evals;
            double tolerance;

            static IntegrationSettings default_settings;
        protected:
        public: 
            static unsigned int bin_evals;
            typedef enum { BINARY_SUBDIVISION,
                           REVERSE_BINARY_SUBDIVISION,
                           LOW_FIRST_REVERSE_BINARY_SUBDIVISION,
                           SCATTERED,
                           DIRECTED
            } METHOD_TYPE;
            static METHOD_TYPE method;
            static src::logger_mt settings_logger;
        };
        
/*
 * MAQ integration algorithm
 */
        typedef struct {
            double integral;
            bool successful;
            unsigned int evaluations;
        } MAQ_RESULT;
        MAQ_RESULT MAQ(std::function<double (double)> integrand);
        MAQ_RESULT MAQ(std::function<double (double)> integrand,
                       const IntegrationSettings &settings);
    }
}
#endif
