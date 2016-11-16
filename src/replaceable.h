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
    /**
     * This template class is intended to be improve efficiency by allowing an
     * object in a simulation to change, and notify other affected objects of
     * the change. This allows only the affected objects to clear their caches,
     * minimising calculations when updating results.
     *
     * To implement a Replaceable class, be sure to call notify_change()
     * whenever the object changes so as to affect clients. 
     *
     * To use a Replaceable object, include a shared_ptr to it in your class,
     * and install callbacks (via add_callbacks()). The changed_cb will
     * typically clear the cache; if the client is also Replaceable, it will
     * notify its clients of the change.  The replace_cb needs to replace the
     * shared_ptr with the object provided; it will also typically clear the
     * cache (and, if the client is Replaceable, notify its clients of the
     * change).
     */
    template <class T> class Replaceable {
    private:
        /**
         * callback_struct stores a pair of std::function objects. 
         */
        typedef struct {
            /*
             * changed_cb is a void(void) function, to be invoked by
             * notify_change().
             */
            std::function<void (void)> changed_cb;

            /*
             * replace_cb is a void(shared_ptr<T>) function, to be invoked by
             * replace().
             */
            std::function<void (std::shared_ptr<T>)> replace_cb;
        } callback_struct;
        
        /**
         * callbacks maps clients to their callback functions. Each client
         * should invoke add_callbacks() in its constructor, saving the return
         * value, and passing it to remove_callbacks() in its destructor.
         */
        std::map<int, callback_struct> callbacks;

        /**
         * id_counter is used as the key for clients. It is incremented by
         * add_callbacks().  Client ids do not need to be sequential; just
         * unique. This is an easy way to ensure that, but other mechanisms
         * could be used without affecting anything outside this template.
         */
        int id_counter;
    public:
        Replaceable()  { 
            id_counter = 0;
        };
        
        int add_callbacks(std::function<void (void)> changed,
                          std::function<void (std::shared_ptr<T>)> replace)
        {
            id_counter++;
            callbacks[id_counter] = {changed, replace};
            return id_counter;
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
            replacement->callbacks = callbacks;
            for (auto it=callbacks.begin(); it != callbacks.end();) {
                it->second.replace_cb(replacement);
                it = callbacks.erase(it);
            }
        }
    };
}
#endif
