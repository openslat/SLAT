/**
 * @file   maq.h
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief  Functions for performing fast integration for SLAT.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */
#include <iostream>
#include <stack>
#include <cmath>
#include <limits>
#include "maq.h"

namespace SLAT {
    namespace Integration {

/**
 * Default tolearance for integration.
 */
        static double default_tolerance = 1E-6;

/**
 * Default evaluation limit for integration.
 */
        static unsigned int default_evaluations = 1024;

        void Set_Default_Tolerance(double tolerance) 
        {
            if (tolerance == TOLERANCE_UNSPECIFIED) {
                std::cerr << "Can't set tolerance to 'unspecified'; ignoring."
                          << std::endl;
            } else if (tolerance < 0) {
                std::cerr << "Can't set tolerance <0; using absolute value instead."
                          << std::endl;
                default_tolerance = std::abs(tolerance);
            } else {
                default_tolerance = tolerance;
            }
        }
        
        void Set_Default_Evaluations(unsigned int evaluations)
        {
            if (evaluations == EVALUATIONS_UNSPECIFIED) {
                std::cerr << "Can't set evaluations limit to 'unspecified'."
                          << std::endl;
            } else {
                default_evaluations = evaluations;
            }
        }
        
        double Get_Default_Tolerance(void)
        {
            return default_tolerance;
        }
        
        unsigned int Get_Default_Evaluations(void)
        {
            return default_evaluations;
        }
        
/*
 * Functions for mapping between the x and t domains:
 */
        inline double x_from_t(double t) { return (t==0 ? INFINITY : 1.0/t - 1.0); };
        inline double t_from_x(double x) { return 1.0/(x + 1.0); };

/*
 * Local structure definition for the limits of integration.
 */
        typedef struct { double a, b; int evaluations; } search_result_t;
    

/*
 * Try to divide the range [0, ∞) to bracket something interesting for integration.
 */
        search_result_t binary_subdivision(std::function<double (double)> f, int max_evals)
        {
            double a=0; // Maps to x=∞
            double b=1.0; // Maps to x=0
            double c = (a + b)/2.0;

            double fb = f(x_from_t(b));
            double fc = f(x_from_t(c));
    
            int evaluations = 2;

            if (fb <= std::numeric_limits<double>::epsilon() && 
                fc <= std::numeric_limits<double>::epsilon()) 
            {
                long int intervals = 4;
                while (evaluations <= max_evals && fc <= std::numeric_limits<double>::epsilon()) {
                    for (int i=1; i < intervals; i += 2) {
                        c = float(i)/intervals;
                        fc = f(x_from_t(c));
                        evaluations++;

                        if (fc > std::numeric_limits<double>::epsilon()) {
                            a = (float(i) - 1.0) / intervals;
                            b = (float(i) + 1.0) / intervals;
                            break;
                        }
                    }
                    intervals *= 2;
                }

                if (fc <= std::numeric_limits<double>::epsilon()) {
                    return {NAN, NAN, evaluations};
                }
            }
    
            return {a, b, evaluations};
        }

/*
 * MAQ integration algorithm
 */
        inline double quad(double a, double b, double c, 
                           double fa, double fb, double fc)
        {
            return (b - a) / 6 * (fa + 4 * fc + fb);
        }

        MAQ_RESULT MAQ(std::function<double (double)> integrand,
                       double tol, int maxeval)
        {
            if (tol == TOLERANCE_UNSPECIFIED) {
                tol = default_tolerance;
            }
            
            if (maxeval == EVALUATIONS_UNSPECIFIED) {
                maxeval = default_evaluations;
            }

            bool success = true;
            // Initialisation
            int counter;
            double integral = 0.0;

            // Stack for keeping track of what still needs to be evaluated:
            typedef struct {
                double a, b, c;
                double fa, fb, fc; 
                double r;
            } maq_todo;

            std::stack<maq_todo> region_stack;

            /*
             * Determine the range for integration:
             */
            double a, b;
            {
                search_result_t r = binary_subdivision(integrand, maxeval);
                a = r.a;
                b = r.b;
                counter = r.evaluations;
            }
   
            double fa = integrand(x_from_t(a))/(a == 0 ? 1 : a*a);
            double fb = integrand(x_from_t(b))/(b*b);

            double c = (a + b)/2.0;
            double fc = integrand(x_from_t(c))/(c*c);
            counter += 3;

            double r1 = quad(a, b, c, fa, fb, fc);
            region_stack.push({a, b, c, fa, fb, fc, r1});

            while (!region_stack.empty()) {
                counter++;
                if (counter > maxeval) {
                    success = false;
                    break;
                }
        
                maq_todo todo = region_stack.top();
                region_stack.pop();
                a = todo.a;
                b = todo.b;
                c = todo.c;
        
                fa = todo.fa;
                fb = todo.fb;
                fc = todo.fc;

                r1 = todo.r;

                // cout << "Popped: " << a << ", " << b << ", " << c << "; " 
                //      << fa << ", " << fb << ", " << fc << "; " << r1
                //      << endl;
        
                double d = (a + c)/2.0;
                double e = (b + c)/2.0;
        
                double fd = integrand(x_from_t(d))/(d*d);
                double fe = integrand(x_from_t(e))/(e*e);

                double r2 = quad(a, c, d, fa, fc, fd);
                double r3 = quad(c, b, e, fc, fb, fe);

                double q1 = r1;
                double q2 = r2 + r3;

                double abserr = std::abs(q2 - q1);
                if (abserr < std::abs(tol * q2) || abserr < std::abs(tol * integral)) {
                    integral = integral + q2 + (q2 - q1)/15;
                } else if (std::abs(r2) > std::abs(r3)) {
                    // cout << "Push: " << c << ", " << b << ", " << e << "; " 
                    //      << fc << ", " << fb << ", " << fe << "; " << r3
                    //      << endl;
                    region_stack.push({c, b, e, fc, fb , fe, r3});
                    // cout << "Push: " << a << ", " << c << ", " << d << "; " 
                    //      << fa << ", " << fc << ", " << fd << "; " << r2
                    //      << endl;
                    region_stack.push({a, c, d, fa, fc, fd, r2});
                } else {
                    region_stack.push({a, c, d, fa, fc, fd, r2});
                    // cout << "Push: " << a << ", " << c << ", " << d << "; " 
                    //      << fa << ", " << fc << ", " << fd << "; " << r2
                    //      << endl;
                    region_stack.push({c, b, e, fc, fb , fe, r3});
                    // cout << "Push: " << c << ", " << b << ", " << e << "; " 
                    //      << fc << ", " << fb << ", " << fe << "; " << r3
                    //      << endl;
                }
            }
            //std::cout << integral << "   " << success << "  " << counter << std::endl;
            return {integral, success, counter};
        }

    }
}
