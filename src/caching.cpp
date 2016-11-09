/**
 * @file   caching.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief  Facilitates for caching function results.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */

#include "caching.h"

namespace SLAT {
    namespace Caching {
        namespace Private {
            omp_lock_t lock;

        void Init_Caching(void)
        {
            static bool first_call = true;
            if (first_call) {
                omp_init_lock(&lock);
            };
            first_call = false;
        }

        /**
         * Maintaina map of all cached functions, so we can clear all caches
         * with a single function call (e.g., when changing the integration
         * method or parameters).
         */
        std::unordered_map<void *, std::function<void (void)>> caches;
        
        /**
         * Add a cached function to the list.
         */
        void Add_Cache(void *cache, std::function<void (void)> clear_func) {
            omp_set_lock(&lock);
            caches[cache] = clear_func;
            omp_unset_lock(&lock);
        }

        /**
         * Remove a cached function from the list.
         */
        void Remove_Cache(void *cache) {
            omp_set_lock(&lock);
            caches.erase(cache);
            omp_unset_lock(&lock);
        }
        }
        /**
         * Clear all (registered) function caches.
         */
        void Clear_Caches(void) {
            omp_set_lock(&Private::lock);
            for (auto it=Private::caches.begin(); it != Private::caches.end(); it++) {
                it->second();
            }
            omp_unset_lock(&Private::lock);
        }
    };
}
