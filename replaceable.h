/**
 * @file   replaceable.h
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief  Template classes for replaceable functions.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */
#ifndef _REPLACEABLE_H_
#define _REPLACEABLE_H_
#include <map>
#include <memory>
#include <iostream>

namespace SLAT {
    template <class T> class Replaceable {
    private:
        typedef struct {std::function<void (void)> changed_cb;
            std::function<void (std::shared_ptr<T>)> replace_cb;
        } callback_struct;
        std::map<int, callback_struct> callbacks;
        int id_counter;
    public:
        Replaceable() { 
            id_counter = 0;
        };
        
        int add_callbacks(std::function<void (void)> changed,
                          std::function<void (std::shared_ptr<T>)> replace)
        {
            callbacks[id_counter] = {changed, replace};
            return id_counter++;
        }

        void remove_callbacks(int id) {
            callbacks.erase(id);
        }

        void notify_change(void) {
            for (auto it=callbacks.begin(); it != callbacks.end(); it++) {
                it->second.changed_cb();
            }
        }

        void replace(std::shared_ptr<T> replacement) {
            for (auto it=callbacks.begin(); it != callbacks.end();) {
                it->second.replace_cb(replacement);
                it = callbacks.erase(it);
            }
        }
    };
}
#endif
