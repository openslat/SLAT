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

namespace SLAT {
    namespace Caching {
        void Add_Cache(void *cache, std::function<void (void)> clear_func);
        void Remove_Cache(void *cache);
        void Clear_Caches(void);
            
        template <class T, class V> class CachedFunction {
        private:
            std::function<T (V)> func;
            std::unordered_map<V, T> cache;
            bool cache_active;
            CachedFunction() {};
        public:
            CachedFunction(std::function<T (V)> base_func, bool activate_cache=true) { 
                cache_active = activate_cache;
                func = base_func; 
                Add_Cache(this, [this] (void) { 
                        this->ClearCache(); });
            };
            ~CachedFunction() {
                Remove_Cache(this);
            }
            T operator()(V v) { 
                if (cache_active) {
                    try {
                        T result = cache.at(v);
                        return result;
                    } catch (...) {
                        cache[v] = func(v); 
                        return cache[v];
                    };
                } else {
                    return func(v);
                }
            }
            void ClearCache(void) {
                cache.clear(); 
            };
        };
    }
}
#endif
