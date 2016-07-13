/**
 * @file   caching.h
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief  Facilitates for caching function results.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */
#ifndef _CACHING_H_
#define _CACHING_H_

#include <unordered_map>
#include <functional>
#include <iostream>

namespace SLAT {
    namespace Caching {
        void Add_Cache(void *cache, std::function<void (void)> clear_func);
        void Remove_Cache(void *cache);
        void Clear_Caches(void);
            
        template <class T, class V> class CachedFunction {
        public:
            std::function<T (V)> func;
            std::unordered_map<V, T> cache;
            bool cache_active;
        public:
            CachedFunction() {
                hits = 0;
                total_calls = 0;
            };
            CachedFunction(std::function<T (V)> base_func, bool activate_cache=true) { 
                cache_active = activate_cache;
                hits = 0;
                total_calls = 0;
                func = base_func; 
                Add_Cache(this, [this] (void) { 
                        this->ClearCache(); });
            };
            ~CachedFunction() {
                Remove_Cache(this);
                std::cout << "Destroying CachedFunction: " << hits << " hits of " << total_calls << " calls." << std::endl;
            }
            T operator()(V v) { 
                total_calls++;
                if (cache_active) {
                    if (cache.count(v) == 0) {
                        cache[v] = func(v); 
                    } else {
                        hits++;
                    }
                    return cache[v];
                } else {
                    return func(v);
                }
            }
            void ClearCache(void) {
                cache.clear(); 
            };
        private:
            int hits;
            int total_calls;
        };

        template <class T> class CachedValue {
        private:
            std::function<T (void)> func;
            T cached_value;
            bool cache_valid;
            int hits, total_calls;
        public:
        CachedValue() : cache_valid(false) { 
                total_calls = 0;
                hits = 0;
            };
            CachedValue(std::function<T (void)> base_func) { 
                cache_valid = false;
                total_calls = 0;
                hits = 0;
                func = base_func; 
                Add_Cache(this, [this] (void) { 
                        this->ClearCache(); });
            };
            ~CachedValue() {
                std::cout << "Destroying cache: " << hits << " hits of " << total_calls << " calls." << std::endl;
            };
            T operator()(void) { 
                total_calls++;
                if (!cache_valid) {
                    cached_value = func();
                    cache_valid = true;
                } else {
                    hits++;
                }
                return cached_value;
            }
            void ClearCache(void) {
                cache_valid = false;
            };
        };
    }
}
#endif
