/**
 * @file   complex_relationships.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Wed Sep 19 14:03:27 NZST 2018
 * 
 * @brief Implementation of classes representing IM and EDP objects with different
 *        functions defining their behaviour in the X and Y directions.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2018 Canterbury University
 */
#include "complex_relationships.h"

namespace SLAT {
    Complex_IM::Complex_IM(std::shared_ptr<DeterministicFn> fn_base,
                   std::shared_ptr<DeterministicFn> fn_x,
                   std::shared_ptr<DeterministicFn> fn_y,
                   std::string name)
    {
        std::cerr << "> Complex_IM()" << std::endl;
        this->name = name;
        
        this->fn_base = fn_base;
        if (fn_base != NULL) {
            this->callback_id_base = fn_base->add_callbacks(
                [this] (void) {
                    std::cout << "Complex_IM '" << this->name << "' base function changed." << std::endl;
                    this->notify_change();
                },
                [this] (std::shared_ptr<DeterministicFn> new_f) {
                    std::cout << "Complex_IM '" << this->name << "' base function replaced." << std::endl;
                    this->fn_base = new_f;
                    this->notify_change();
                });
        }
        
        this->fn_x = fn_x;
        if (fn_x != NULL) {
            this->callback_id_x = fn_x->add_callbacks(
                [this] (void) {
                    std::cout << "Complex_IM '" << this->name << "' X function changed." << std::endl;
                    this->notify_change();
                },
                [this] (std::shared_ptr<DeterministicFn> new_f) {
                    std::cout << "Complex_IM '" << this->name << "' X function replaced." << std::endl;
                    this->fn_x = new_f;
                    this->notify_change();
                });
        }

        this->fn_y = fn_y;
        if (fn_y != NULL) {
            this->callback_id_y = fn_y->add_callbacks(
                [this] (void) {
                    std::cout << "Complex_IM '" << this->name << "' Y function changed." << std::endl;
                    this->notify_change();
                },
                [this] (std::shared_ptr<DeterministicFn> new_f) {
                    std::cout << "Complex_IM '" << this->name << "' Y function replaced." << std::endl;
                    this->fn_y = new_f;
                    this->notify_change();
                });
        }
        std::cerr << "< Complex_IM()" << std::endl;
    };

    Complex_IM::~Complex_IM() {
        std::cerr << "> ~Complex_IM()" << std::endl;
        fn_base->remove_callbacks(callback_id_base);
        fn_x->remove_callbacks(callback_id_x);
        fn_y->remove_callbacks(callback_id_y);
        std::cerr << "< ~Complex_IM()" << std::endl;
    };

    std::string Complex_IM::ToString(void) const
    {
        return "Complex_IM '" + name + "': "
            + fn_base->ToString() + ", "
            + fn_x->ToString() + ", "
            + fn_y->ToString();
    }
    
    std::ostream& operator<<(std::ostream& out, const Complex_IM& o)
    {
        out << o.ToString();
        return out;
    }

    double Complex_IM::lambda(Component which, double where)
    {
        switch (which) {
        case BASE:
            return fn_base->ValueAt(where);
        case X:
            return fn_x->ValueAt(where);
        case Y:
            return fn_y->ValueAt(where);
        default:
            return NAN;
        }
    }

    double Complex_IM::value(Component which, double lambda)
    {
        switch (which) {
        case BASE:
            return fn_base->solve_for(lambda);
        case X:
            return fn_x->solve_for(lambda);
        case Y:
            return fn_y->solve_for(lambda);
        default:
            return NAN;
        }
    }
}
