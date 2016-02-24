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
            caches[cache] = clear_func;
        }

        /**
         * Remove a cached function from the list.
         */
        void Remove_Cache(void *cache) {
            caches.erase(cache);
        }

        /**
         * Clear all (registered) function caches.
         */
        void Clear_Caches(void) {
            for (auto it=caches.begin(); it != caches.end(); it++) {
                it->second();
            }
        }
    };
}
