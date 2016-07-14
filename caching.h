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
#include <iomanip>
#include <string>
#include <omp.h>

namespace SLAT {
    namespace Caching {
        void Init_Caching(void);
        void Add_Cache(void *cache, std::function<void (void)> clear_func);
        void Remove_Cache(void *cache);
        void Clear_Caches(void);
            
        template <class T, class V> class CachedFunction {
        public:
            omp_lock_t lock;
            std::function<T (V)> func;
            std::unordered_map<V, T> cache;
            bool cache_active;
        public:
            CachedFunction() {
                name = "Anonymous";
                hits = 0;
                total_calls = 0;
                omp_init_lock(&lock);
            };
            CachedFunction(std::function<T (V)> base_func, std::string name="Anonymous", bool activate_cache=true) { 
                omp_init_lock(&lock);
                this->name = name;
                cache_active = activate_cache;
                hits = 0;
                total_calls = 0;
                func = base_func; 
                Add_Cache(this, [this] (void) { 
                        this->ClearCache(); });
            };
            ~CachedFunction() {
                std::cout << std::setw(50) << name
                          << std::setw(10) << hits
                          << std::setw(10) << total_calls 
                          << std::endl;
                Remove_Cache(this);
            }
            T operator()(V v) { 
                omp_set_lock(&lock);
                total_calls++;
                if (cache_active) {
                    bool cached = cache.count(v) != 0;
                    if (!cached) {
                        omp_unset_lock(&lock);
                        T result = func(v); 
                        omp_set_lock(&lock);
                        if (cache.count(v) != 0) {
                            std::cout << "DUPLICATE (f): " << name << std::endl;
                        }
                        cache[v] = result;
                    } else {
                        hits++;
                    }
                    T result = cache[v];
                    omp_unset_lock(&lock);
                    return result;
                } else {
                    omp_unset_lock(&lock);
                    return func(v);
                }
            }
            void ClearCache(void) {
                cache.clear(); 
            };
        private:
            std::string name;
            int hits;
            int total_calls;
        };

        template <class T> class CachedValue {
        private:
            omp_lock_t lock;
            std::function<T (void)> func;
            T cached_value;
            bool cache_valid;
            int hits, total_calls;
            std::string name;
        public:
        CachedValue() : cache_valid(false) { 
                name = "Anonymous";
                total_calls = 0;
                hits = 0;
                omp_init_lock(&lock);
            };
            CachedValue(std::function<T (void)> base_func, std::string name = "Anonymous") { 
                omp_init_lock(&lock);
                this->name = name;
                cache_valid = false;
                total_calls = 0;
                hits = 0;
                func = base_func; 
                Add_Cache(this, [this] (void) { 
                        this->ClearCache(); });
            };
            ~CachedValue() {
                std::cout << std::setw(50) << name
                          << std::setw(10) << hits
                          << std::setw(10) << total_calls 
                          << std::endl;
            };
            T operator()(void) { 
                omp_set_lock(&lock);
                total_calls++;
                if (!cache_valid) {
                    omp_unset_lock(&lock);
                    T result = func();
                    omp_set_lock(&lock);
                    if (cache_valid) {
                        std::cout << "DUPLICATE (v): " << name << std::endl;
                    }
                    cached_value = result;
                    cache_valid = true;
                } else {
                    hits++;
                }
                T result = cached_value;
                omp_unset_lock(&lock);
                return result;
            }
            void ClearCache(void) {
                cache_valid = false;
            };
        };
    }
}
#endif
