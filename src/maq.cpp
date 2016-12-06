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

        src::logger_mt IntegrationSettings::settings_logger;
        IntegrationSettings IntegrationSettings::default_settings;
        
        void IntegrationSettings::Reset(void)
        {
            default_settings.tolerance = TOLERANCE_DEFAULT;
            default_settings.max_evals = EVALUATIONS_DEFAULT;
        }

        IntegrationSettings::IntegrationSettings(void)
        {
            tolerance = TOLERANCE_DEFAULT;
            max_evals = EVALUATIONS_DEFAULT;
            parent = NULL;
        }

        IntegrationSettings::IntegrationSettings(const IntegrationSettings *other)

        {
            tolerance = TOLERANCE_UNSPECIFIED;
            max_evals = EVALUATIONS_UNSPECIFIED;
            parent = other;
        }

        const IntegrationSettings *IntegrationSettings::Get_Global_Settings(void) 
        {
            return &IntegrationSettings::default_settings;
        }

        unsigned int IntegrationSettings::Get_Effective_Max_Evals(void) const
        {
            const IntegrationSettings *ptr = this;
            while (ptr->parent != NULL && ptr->max_evals == EVALUATIONS_UNSPECIFIED) {
                ptr = ptr->parent;
            }
            return ptr->max_evals;
        }


        double IntegrationSettings::Get_Effective_Tolerance(void) const
        {
            const IntegrationSettings *ptr = this;
            while (ptr->parent != NULL && ptr->tolerance == TOLERANCE_UNSPECIFIED) {
                ptr = ptr->parent;
            }
            return ptr->tolerance;
        }

        void IntegrationSettings::Override_Tolerance(double tol) 
        {
            tolerance = tol;
        }

        void IntegrationSettings::Override_Max_Evals(unsigned int evals) 
        {
            max_evals = evals;
        }

        void IntegrationSettings::Use_Default_Tolerance() 
        {
            tolerance = TOLERANCE_UNSPECIFIED;
        }

        void IntegrationSettings::Use_Default_Max_Evals() 
        {
            max_evals = EVALUATIONS_UNSPECIFIED;
        }

        void IntegrationSettings::Set_Tolerance(double tol) 
        {
            if (tol > 0.0) {
                default_settings.Override_Tolerance(tol);
            } else {
                BOOST_LOG(settings_logger) << "Can't set tolerance to " << tol
                                           << "; must be > 0.0.";
            }
        }

        void IntegrationSettings::Set_Max_Evals(unsigned int evals)
        {
            if (evals > 0) {
                default_settings.Override_Max_Evals(evals);
            } else {
                BOOST_LOG(settings_logger) << "Can't set max evaluatons to " << evals
                                           << "; must be > 0.0.";
            }
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
                while (evaluations < max_evals && fc <= std::numeric_limits<double>::epsilon()) {
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
                    //std::cout << fc << ", " << std::numeric_limits<double>::epsilon() << ", " << evaluations << std::endl;
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

        MAQ_RESULT MAQ(std::function<double (double)> integrand) 
        {
            return MAQ(integrand, *IntegrationSettings::Get_Global_Settings());
        }



        /**
         * @todo Determine how many of the allowed evaluations to use
         * for finding something interesting (and whether it is constant, or
         * programmer-controlled).
         * @todo If binary_subdivision() fails to find something
         * interesting, the programmer should be able to control
         * what happens (treat it as zero, issue a warning and treat
         * it as zero, or fail).
         * @todo Allow programmer to control what happens when number of
         * evaluations is exceeded (warn and use latest approximation, abort).
         */
        MAQ_RESULT MAQ(std::function<double (double)> integrand,
                       const IntegrationSettings &settings)
        {
            double tol = settings.Get_Effective_Tolerance();
            unsigned int maxeval = settings.Get_Effective_Max_Evals();

            bool success = true;
            // Initialisation
            unsigned int counter;
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
                /**
                 * Use no more than 1/4 of the allowed evaluations to find something
                 * interesting.
                 */
                search_result_t r = binary_subdivision(integrand, maxeval/4);
                if (std::isnan(r.a)) {
                    //std::cout << "binary_subdivision failed" << std::endl;
                    return {0, true, (unsigned int)r.evaluations}; 
                }
                a = r.a;
                b = r.b;
                counter = r.evaluations;
            }
            //int temp_count = counter;
   
            double fa = integrand(x_from_t(a))/(a == 0 ? 1 : a*a);
            double fb = integrand(x_from_t(b))/(b*b);

            if (std::isnan(fa)) {
                //std::cout << "fa is NAN" << std::endl;
                return {NAN, false, counter};
            }

            if (std::isnan(fb)) {
                //std::cout << "fb is NAN" << std::endl;
                return {NAN, false, counter};
            }
            
            double c = (a + b)/2.0;
            double fc = integrand(x_from_t(c))/(c*c);
            if (std::isnan(fc)) {
                //std::cout << "fc is NAN" << std::endl;
                return {NAN, false, counter};
            }
            
            counter += 3;

            double r1 = quad(a, b, c, fa, fb, fc);
            region_stack.push({a, b, c, fa, fb, fc, r1});

            while (!region_stack.empty()) {
                counter++;
                if (counter > maxeval) {
                    /*
                     * What happens when the number of evaluations is exceeded
                     * without achieving the required accuracy?
                     */
#if false
                    /*
                     * This branch ignores the error, and returns 0
                     */
                    success = true;
                    integral = 0;
                    break;
#elseif false
                    /*
                     * This branch generates an error, by returning NAN.
                     */
                    success = false;
                    integral = NAN;
                    break;
#else
                    /*
                     * This branch returns the current approximation, even
                     * though it isn't as good as requested. We should also
                     * generate a warning when this occurs.
                     */
                    success = true;
                    maq_todo todo = region_stack.top();
                    region_stack.pop();
                    integral += todo.r;
                    continue;
#endif
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

                if (std::isnan(fd)) {
                    //std::cout << "fd is NAN" << std::endl;
                    return {NAN, false, counter};
                }
            
                if (std::isnan(fe)) {
                    //std::cout << "fe is NAN" << std::endl;
                    return {NAN, false, counter};
                }

                double r2 = quad(a, c, d, fa, fc, fd);
                double r3 = quad(c, b, e, fc, fb, fe);

                double q1 = r1;
                double q2 = r2 + r3;

                double abserr = std::abs(q2 - q1);
                if (abserr <= std::abs(tol * q2) || abserr <= std::abs(tol * integral)) {
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
            if (!success || std::isnan(integral)) {
                //std::cout << integral << "   " << success << "  " << counter << "   " << temp_count << maxeval << std::endl;
            }
            return {integral, success, counter};
        }

    }
}
