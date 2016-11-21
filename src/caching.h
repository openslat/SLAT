/**
 * @file   caching.h
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief  Facilitates for caching function results.
 *
 * This module provides template classes for functions and values. When a
 * function or value is expensive to calculate, caching it can reduce the
 * overall amount of computational effort required by the program. 
 *
 * When a template is instantiated, it is given a std::function<> object,
 * which will calculate the cached function or value.
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
#include <signal.h>
#include <cmath>

namespace SLAT {
    /**
     * Functions for managing the cache.
     */
    namespace Caching {
        /**
         * Caching-related functions, which should only be used from within the
         * Caching module.
         */
        namespace Private {
            /*
             * Don't use anything from this namespace directly from outside this module:
             */

            /**
             * Initialise the caching module. This should be called from
             * constructors for any Caching class. Calling it more than once
             * will do no harm.
            */
            void Init_Caching(void); 

            /**
             * Record a cache instantiation. This should be called from the
             * constructor of any Caching class. This allows
             * Caching::Clear_Caches() to clear all the function/value caches.
             */
            void Add_Cache(void *cache, std::function<void (void)> clear_func); 

            /**
             * Tell the Caching module to forget about an instantiation. This
             * should be called form the destructor; it will prevent
             * Caching::Clear_Caches() from referring to caches which are no
             * longer valid.
             */
            void Remove_Cache(void *cache); };

        /**
         * This can be used safely anwywhere, to clear all caches:
         */
        void Clear_Caches(void);

        /**
         * Template for cached functions which accept an argument of class V,
         * and return a value of class T. The template is instantiated with a
         * std::function<T (V)>, which is called to evaluate the function on a
         * cache miss.  The construct can also be given a name string, which
         * will be included in debug and error messages, and a flag. The flag
         * defaults to true, but can be explicitly set to false to disable
         * caching for an instance. This is intended to allow developers to
         * assess the impact of caching on their code. To further assist
         * developers, the template also records the number of times the
         * function is called, and the number of cache hits.
         *
         * This class supports parallel processing by using OpenMP mechanisms to
         * protect the cache from competing threads.
         */
        template <class T, class V> class CachedFunction {
        public:
            CachedFunction(std::function<T (V)> base_func, std::string name="Anonymous", bool activate_cache=true) { 
                Caching::Private::Init_Caching();
                omp_init_lock(&lock);
                this->name = name;
                cache_active = activate_cache;
                hits = 0;
                total_calls = 0;
                func = base_func; 
                Caching::Private::Add_Cache(this, [this] (void) { 
                        this->ClearCache(); });
            };
            ~CachedFunction() {
                Caching::Private::Remove_Cache(this);
            }
            T operator()(V v) { 
                omp_set_lock(&lock);
#pragma omp flush 
                total_calls++;
                if (cache_active && !std::isnan(v)) {
                    bool cached = cache.count(v) != 0;
                    if (!cached) {
                        bool happening = in_process.count(v) != 0;
                        if (happening) {
                            in_process[v].count++;
#pragma omp flush 
                            omp_lock_t *calc_lock = &in_process[v].lock;
                            omp_unset_lock(&lock);
                            omp_set_lock(calc_lock);
                            omp_set_lock(&lock);
#pragma omp flush 
                            in_process[v].count--;
                            if (in_process[v].count > 0) {
                                omp_unset_lock(&in_process[v].lock);
                            } else {
                                in_process.erase(v);
                            }
                        } else {
                            omp_init_lock(&in_process[v].lock);
                            omp_set_lock(&in_process[v].lock);
                            in_process[v].count = 0;
#pragma omp flush 
                            omp_unset_lock(&lock);
                            T result = func(v); 
                            omp_set_lock(&lock);
#pragma omp flush 
                            if (cache.count(v) != 0) {
                                std::cout << "DUPLICATE (f): " << name << " at " << v << std::endl;
                                std::cout << in_process[v].count << std::endl;
                                raise(SIGINT);
                            }
                            cache[v] = result;
                            if (in_process[v].count == 0) {
                                in_process.erase(v);
                            } else {
                                omp_unset_lock(&in_process[v].lock);
                            }
                        }
                    } else {
                        hits++;
                    }
                    T result = cache[v];
#pragma omp flush 
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
            std::function<T (V)> func;
            std::unordered_map<V, T> cache;
            bool cache_active;
            struct WAITING {omp_lock_t lock; int count; };
            std::unordered_map<V, struct WAITING> in_process;
            omp_lock_t lock;
            std::string name;
            int hits;
            int total_calls;
        };

        template <class T> class CachedValue {
        private:
            bool in_process;
            omp_lock_t waiting_lock;
            omp_lock_t lock;
            std::function<T (void)> func;
            T cached_value;
            bool cache_valid;
            int hits, total_calls;
            std::string name;
        public:
        CachedValue() : cache_valid(false) { 
                Caching::Private::Init_Caching();
                name = "Anonymous";
                total_calls = 0;
                hits = 0;
                omp_init_lock(&lock);
                omp_init_lock(&waiting_lock);
                omp_set_lock(&waiting_lock);
                in_process = false;
                func = [this] (void) {
                    throw std::runtime_error("Function for CachedValue not provided");
                    return T();
                };
            };
            CachedValue(std::function<T (void)> base_func, std::string name = "Anonymous") { 
                Caching::Private::Init_Caching();
                omp_init_lock(&lock);
                omp_init_lock(&waiting_lock);
                omp_set_lock(&waiting_lock);
                in_process = false;
                this->name = name;
                cache_valid = false;
                total_calls = 0;
                hits = 0;
                func = base_func; 
                Caching::Private::Add_Cache(this, [this] (void) { 
                        this->ClearCache(); });
            };
            ~CachedValue() {
                Caching::Private::Remove_Cache(this);
            };
            T operator()(void) { 
                omp_set_lock(&lock);
#pragma omp flush 
                total_calls++;
                if (!cache_valid) {
                    if (in_process) {
#pragma omp flush 
                        omp_unset_lock(&lock);
                        omp_set_lock(&waiting_lock);
                        omp_unset_lock(&waiting_lock); // Release for anyone else who's waiting
                        omp_set_lock(&lock);
#pragma omp flush 
                    } else {
                        in_process = true;
#pragma omp flush 
                        omp_unset_lock(&lock);
                        T result = func();
                        omp_set_lock(&lock);
                        if (cache_valid) {
                            std::cout << "DUPLICATE (v): " << name << std::endl;
                            raise(SIGINT);
                        }
                        cached_value = result;
                        cache_valid = true;
                        in_process = false;
#pragma omp flush 
                        omp_unset_lock(&waiting_lock);
                    }                        
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
