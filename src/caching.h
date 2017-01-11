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
#include <boost/log/trivial.hpp>
#include <sstream>
#include "context.h"

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
         * Instances register themselves with the caching module in the
         * constructor, and unregister themselves in the destructor. This allows
         * a programmer to clear all caches by invoking
         * Caching::Clear_Caches(). The cache for an instance can be cleared by
         * calling its ClearCache() method.
         *
         * This class supports parallel processing by using OpenMP mechanisms to
         * protect the cache from competing threads.
         */
        template <class T, class V> class CachedFunction {
        public:
            /**
             * Constructor requires a function object; name string and activate
             * flag are optional, and only inteneded to assist with development
             * and debugging.
             *
             * This constructor will register the instance with the Caching
             * module.
             */
            CachedFunction(std::function<T (V)> base_func, 
                           std::string name="Anonymous", 
                           bool activate_cache=true) 
            { 
                /*
                 * Make sure the caching module is initialised--this function
                 * can be safely called multiple times; only the first will have
                 * any effect.
                 */
                Caching::Private::Init_Caching();

                omp_init_lock(&lock); // Initalise the lock for this object

                // Set member variables:
                this->name = name;
                cache_active = activate_cache;
                hits = 0;
                total_calls = 0;
                func = base_func; 

                // Tell the caching module about this object:
                Caching::Private::Add_Cache(this, [this] (void) { 
                        this->ClearCache(); });
            };

            /**
             * Destructor removes the instance from the Caching module.
             */
            ~CachedFunction() {
                Caching::Private::Remove_Cache(this);
            }

            /**
             * The () operator checks the cache using the function argument as a
             * key. If the value exists, it is returned; otherwise, the
             * std::function provided to the constructor is invoked, and result
             * is added to the cache before being returned.
             */
            T operator()(V v) { 
                //std::stringstream s;
                //s << "CachedFunction[" << this->name << "](" << v << ")";
                //Context::PushText(s.str());
                // TempContext context([this] (std::ostream &o) {
                //         o << "CachedFunction(): " << this->name;
                //     });
                //BOOST_LOG_TRIVIAL(fatal) << Context::GetText();

                /*
                 * Lock to prevent conflicts if different threads try to use
                 * this cache at the same time.
                 */
                omp_set_lock(&lock); 
                total_calls++;  // For development diagnostics
                
                /*
                 * Only use the cache if it's been enabled, AND the argument is
                 * not NAN (NAN causes problems in the look up).  
                 *
                 * The isnan() test might break something if we try use this
                 * template with a non-numeric parameter type.
                 */
                if (cache_active && !std::isnan(v)) {

                    /*
                     * It may look like this code could be shortened, but the
                     * use of temporary variables are necessary to correctly
                     * support parallel processing.
                     *
                     * Use count() to check whether the value we're after has
                     * been cached. If we just try to look up the value instead
                     * (e.g., refer to cache[v]), it will be inserted if it
                     * doesn't already exist.
                     */
                    bool cached = cache.count(v) != 0;

                    if (!cached) {
                        /*
                         * The value isn't cached, so we need to calculate
                         * it. For efficiency, we only want to calculate it
                         * once. We use a map 'in_process' to record which
                         * values are being actively calculated at a given
                         * time. Each entry in 'in_process' has a lock and a
                         * count.
                         */
                        bool happening = in_process.count(v) != 0;
                        if (happening) {
                            /*
                             *  The calculation is already being
                             *  performed. Increment the count, release the lock
                             *  for this object, and then take corresponding
                             *  'in_process' lock.
                             */
                            in_process[v].count++;
                            omp_lock_t *calc_lock = &in_process[v].lock;
                            omp_unset_lock(&lock);
                            omp_set_lock(calc_lock);

                            /*
                             * Once we have calc_lock, we know the value is in
                             * the cache. Wait for the object lock, then
                             * decrement the 'in_process' count for the
                             * requested value. If it isn't zero, there's
                             * someone else waiting, so release the lock. If
                             * we're the last one, erase the appropriate
                             * 'in_process' element.
                             */
                            omp_set_lock(&lock);
                            in_process[v].count--;
                            if (in_process[v].count > 0) {
                                omp_unset_lock(&in_process[v].lock);
                            } else {
                                in_process.erase(v);
                            }
                        } else {
                            /*
                             * We're the first one to request this
                             * calculation. Add an entry to 'in_process', then
                             * release the object while the calculation is
                             * performed.
                             */
                            omp_init_lock(&in_process[v].lock);
                            omp_set_lock(&in_process[v].lock);
                            in_process[v].count = 0;
                            omp_unset_lock(&lock);

                            T result = func(v);  // Perform calculation
                            
                            omp_set_lock(&lock); // Get object lock
                            
                            /*
                             * This is a sanity check. If when we get here, the
                             * value is in the cache already, something is wrong
                             * with the synchronisation in this code. That is,
                             * somebody else wanted the same result, and
                             * calculated it instead of waiting for us.
                             */
                            if (cache.count(v) != 0) {
                                std::cout << "DUPLICATE (f): " << name << " at " << v << std::endl;
                                std::cout << in_process[v].count << std::endl;
                                raise(SIGINT);
                            }

                            cache[v] = result; // Save the result

                            /*
                             * If no one else is interested in the result,
                             * remove the 'in_process' entry; otherwise, release
                             * the 'in_process' lock to signal other interested
                             * parties.
                             */
                            if (in_process[v].count == 0) {
                                in_process.erase(v);
                            } else {
                                omp_unset_lock(&in_process[v].lock);
                            }
                        }
                    } else {
                        // Record cache hit
                        hits++;
                    }
                    
                    /*
                     * At this point, the value is in the cache; grab it before
                     * releasing the object lock, then return the value.
                     */
                    T result = cache[v];
                    omp_unset_lock(&lock);
                    //Context::PopText();
                    return result;
                } else {
                    /*
                     *  Cache disabled, or parameter is NAN; always evaluate
                     *  function.
                     */
                    omp_unset_lock(&lock);
                    //Context::PopText();
                    return func(v);
                }
            }

            /**
             * Clear the cache for just this object.
             */
            void ClearCache(void) {
                cache.clear(); 
            };
        private:
            /// This will perform the calculation that we're caching.
            std::function<T (V)> func;

            /// The results cache
            std::unordered_map<V, T> cache;

            /**
             * Flag controlling whether the cache is acutally used. Intended for
             * use in debugging and characterisation.
             */
            bool cache_active;

            /// Structure for recording calcuations in process.
            struct WAITING {
                omp_lock_t lock; ///< Lock for this record
                int count;       ///< Count of waiting clients
            };

            /// Map of calculations in process.
            std::unordered_map<V, struct WAITING> in_process;

            /// OpenMP lock for this object
            omp_lock_t lock;

            /**
             * (Optional) name for this object; used in error and debugging
             * messages.
             */
            std::string name;

            int hits; ///< Counter of cache hits (for development)
            int total_calls; ///< Counter of total calls (for development)
        };


        
        /**
         * Template for cached values which presumably take some effort to
         *  calculate.  The template is instantiated with a std::function<T ()>,
         *  which is called to evaluate the function. This is similar to the
         *  CachedFunction template, but no parameter is involved in the
         *  calculation.  The constructor can also be given a name string, which
         *  will be included in debug and error messages. To further assist
         *  developers, the template also records the number of times the
         *  function is called, and the number of cache hits.
         *
         * Instances register themselves with the caching module in the
         * constructor, and unregister themselves in the destructor. This allows
         * a programmer to clear all caches by invoking
         * Caching::Clear_Caches(). The cache for an instance can be cleared by
         * calling its ClearCache() method.
         *
         * This class supports parallel processing by using OpenMP mechanisms to
         * protect the cache from competing threads.
         */
        template <class T> class CachedValue {
        private:
            bool in_process; ///< Flag indicating the value is being calculated

            /// Lock for clients waiting for the calculation to finish:
            omp_lock_t waiting_lock;

            omp_lock_t lock; ///< Lock for this object

            std::function<T (void)> func; ///< Block performing the calculation.

            T cached_value;  ///< The result of the calculation

            bool cache_valid; ///< Is the stored result valid?
            
            int hits;        ///< Hit counter (for development)
            int total_calls; ///< Count of total calls (for development)
            std::string name;  ///< (Optional) object name

        public:
            /**
             * Constructor requires a function object; name string is optional,
             * and only inteneded to assist with development and debugging.
             *
             * This constructor will register the instance with the Caching
             * module.
             */
             CachedValue(std::function<T (void)> base_func, std::string name = "Anonymous") { 
                /*
                 * Make sure the caching module is initialised--this function
                 * can be safely called multiple times; only the first will have
                 * any effect.
                 */
                Caching::Private::Init_Caching();

                omp_init_lock(&lock); // Initialise the lock for this object

                /**
                 * Set up the lock that's used to ensure the value is calculated
                 * only once.
                 */
                omp_init_lock(&waiting_lock);
                omp_set_lock(&waiting_lock);

                // Set member variables:
                in_process = false; 
                this->name = name;
                cache_valid = false;
                total_calls = 0;     
                hits = 0;
                func = base_func; 

                // Tell the caching module about this object:
                Caching::Private::Add_Cache(this, [this] (void) { 
                        this->ClearCache(); });
            };

            /**
             * Destructor removes the instance from the Caching module.
             */
            ~CachedValue() {
                Caching::Private::Remove_Cache(this);
            };

            /**
             * The first time the () operator is calls, it invokes the
             * std::function and saves the value. After that, the saved value is
             * returned.
             */
            T operator()(void) { 
                // std::stringstream s;
                // s << "CachedValue[" << this->name << "]";
                // Context::PushText(s.str());
                // TempContext context([this] (std::ostream &o) {
                //         o << "CachedValue(): " << this->name;
                //     });
                //BOOST_LOG_TRIVIAL(fatal) << Context::GetText();

                /*
                 * Lock to prevent conflicts if different threads try to use
                 * this cache at the same time.
                 */
                omp_set_lock(&lock);
                total_calls++;  // For development diagnostics
                
                /*
                 * If the value hasn't been yet been calculated, we need to
                 * calculate it; if the calculation is already in process, wait
                 * for it to finish.
                 */
                if (!cache_valid) {
                    /*
                     * The calculating is being performed. Release the object
                     * lock, then wait for the 'waiting_lock' to indicate the
                     * calculation has been completed.
                     */
                    if (in_process) {
                        omp_unset_lock(&lock);
                        omp_set_lock(&waiting_lock);
                        omp_unset_lock(&waiting_lock); // Release for anyone else who's waiting
                        omp_set_lock(&lock); // Re-take the lock before returning the value
                    } else {
                        /*
                         * This is the first request--calculate the value.
                         */
                        in_process = true;
                        omp_unset_lock(&lock); // Release the lock while calculating
                        
                        T result = func();
                        
                        omp_set_lock(&lock); // Lock before saving to cache
                        /*
                         * Sanity check--if cache_valid is set, the calculation
                         * has been performed more than once.
                         */
                        if (cache_valid) {
                            std::cout << "DUPLICATE (v): " << name << std::endl;
                            raise(SIGINT);
                        }
                        
                        // Save the result, and release the 'waiting_lock' to anyone waiting
                        cached_value = result;
                        cache_valid = true;
                        in_process = false;
                        omp_unset_lock(&waiting_lock);
                    }                        
                } else {
                    // Already cached; no need to calculate
                    hits++;
                }

                /*
                 * At this point, we have the lock, and the value is stored in
                 * the cache. Release the lock and return the cached value.
                 */
                T result = cached_value;
                omp_unset_lock(&lock);
                //Context::PopText();
                return result;
            }

            /**
             * Clear the cache for just this object.
             */
            void ClearCache(void) {
                cache_valid = false;
            };
        };
    }
}
#endif
