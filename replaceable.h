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
    template <class T> class Replaceable /*: public std::enable_shared_from_this<T>*/ {
    private:
        typedef struct {std::function<void (void)> changed_cb;
            std::function<void (std::shared_ptr<T>)> replace_cb;
        } callback_struct;
        std::map<int, callback_struct> callbacks;
        int id_counter;
    public:
        Replaceable()  { 
            id_counter = 0;
            std::cout << "Replaceable() " << callbacks.size() << std::endl;
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
            std::cout << "> notify_change() " << this << std::endl;
            std::cout << callbacks.size() << std::endl;
            for (auto it=callbacks.begin(); it != callbacks.end(); it++) {
                std::cout << "<" << std::endl;
                std::cout << it->first << std::endl;

                
                std::cout << &(it->second.replace_cb) << std::endl;
                it->second.changed_cb();
                std::cout << ">" << std::endl;
            }
            std::cout << "> notify_change() " << this << std::endl;
        }

        void replace(std::shared_ptr<T> replacement) {
            std::cout << "replace<>() " << this << " with " << replacement << std::endl;
            replacement->callbacks = callbacks;
            std::cout << callbacks.size() << " callbacks" << std::endl;
            for (auto it=callbacks.begin(); it != callbacks.end();) {
                std::cout << ". " << std::endl;
                it->second.replace_cb(replacement);
                it = callbacks.erase(it);
            }
            std::cout << "==== replacing done ====" << std::endl;
        }
    };
}
#endif
