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
        class IntegrationSettings
        {
        public:
            typedef enum { BINARY_SUBDIVISION,
                           REVERSE_BINARY_SUBDIVISION,
                           LOW_FIRST_REVERSE_BINARY_SUBDIVISION,
                           SCATTERED,
                           DIRECTED
            } METHOD_TYPE;

            static void Use_Default_Tolerance(void); // Restore default
            static void Use_Default_Integration_Eval_Limit(void); // Restore default
            static void Use_Default_Integration_Search_Limit(void); // Restore default
            static void Use_Default_Method(void); // Restore default

            static void Set_Tolerance(double tolerance);
            static void Set_Integration_Eval_Limit(unsigned int integration_eval_limit);  
            static void Set_Integration_Search_Limit(unsigned int integration_search_limit);  
            static void Set_Integration_Method(METHOD_TYPE method);
            static void Reset();

            static unsigned int Get_Integration_Eval_Limit(void);
            static unsigned int Get_Integration_Search_Limit(void);
            static double Get_Tolerance(void);
            static METHOD_TYPE Get_Integration_Method(void);
            
            //static src::logger_mt settings_logger;
        private:
            static const unsigned int INTEGRATION_EVAL_LIMIT_DEFAULT = 1024;
            static const unsigned int INTEGRATION_SEARCH_LIMIT_DEFAULT = 1024;
            static constexpr double TOLERANCE_DEFAULT = 1E-6;
            static const METHOD_TYPE METHOD_DEFAULT = DIRECTED;
            
            static unsigned int integration_eval_limit;
            static unsigned int integration_search_limit;
            static double tolerance;
        private:
            static METHOD_TYPE integration_method;
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


        /**
         * Integration statistics
         */ 
        typedef struct {
            // Controls--these will be populated from current values when the
            // statistics are queried.
            Integration::IntegrationSettings::METHOD_TYPE method;        ///< Search method
            double tolerance;          ///< Integration tolerance
            unsigned int eval_limit;   ///< Limit on evaluations during integration
            unsigned int search_limit; ///< Limit on evaluations during search

            // Counters:
            unsigned int max_successful_integration;
            unsigned int number_integration_fails;
            unsigned long total_integrations_evals;

            unsigned int max_successful_search;
            unsigned int number_search_fails;
            unsigned long total_search_evals;

            unsigned int calls;
            unsigned int successes;
            unsigned int nans;
        } IntegrationStats;

        void reset_statistics(void);
        const IntegrationStats *get_statistics(void);
        void format_statistics(std::ostream &ostr);
    
    }
}
#endif
