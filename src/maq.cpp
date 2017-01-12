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
#include <iomanip>
#include <stack>
#include <cmath>
#include <limits>
#include <omp.h>
#include <boost/log/trivial.hpp>
#include "maq.h"
#include "context.h"

unsigned int max_count = 0;
unsigned int max_bin = 0;
unsigned int max_successful_bin = 0;
unsigned long bin_evals = 0;
unsigned long maq_evals = 0;
size_t calls = 0;
size_t successes = 0;
size_t fails = 0;
size_t nans = 0;
size_t bin_fails = 0;
const size_t N_BINS = 16;
size_t bin_Bins[N_BINS] = {0};
//size_t bin_near_fails = 0;

void ResetIntegrationStats()
{
    max_count = 0;
    max_bin = 0;
    max_successful_bin = 0;
    maq_evals = 0;
    bin_evals = 0;
    calls = 0;
    successes = 0;
    fails = 0;
    nans = 0;
    bin_fails = 0;
    for (size_t i=0; i < N_BINS; i++) {
        bin_Bins[i] = 0;
    }
//    bin_near_fails = 0;
}

void DumpIntegrationStats()
{
#pragma omp critical
    std::cout << "Max Count: " << max_count << std::endl
              << "Max Bin: " << max_bin << std::endl
              << "Max Successful Bin: " << max_successful_bin << std::endl
              << "Bin Evals: " << bin_evals << std::endl
              << "MAQ Evals: " << maq_evals << std::endl
              << "Total Count: " << (bin_evals + maq_evals) << std::endl
              << "Calls: " << calls << std::endl
              << "Successes: " << successes << std::endl
              << "Fails: " << fails << std::endl
              << "NANs: " << nans << std::endl
              << "Bin_Fails: " << bin_fails << std::endl;

    std::cout << "Bins: ";
    for (size_t i=0; i < N_BINS; i++) {
        std::cout << bin_Bins[i] << "  ";
    }
    std::cout << std::endl;

//              << "Bin_Near_Fails: " << bin_near_fails << std::endl;
}

namespace SLAT {
    namespace Integration {

        unsigned int IntegrationSettings::bin_evals = 0;
        IntegrationSettings::METHOD_TYPE  IntegrationSettings::method = OLD;

        
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
        typedef struct { double a, b; unsigned int evaluations; } search_result_t;
    

/*
 * Try to divide the range [0, ∞) to bracket something interesting for integration.
 */
        search_result_t binary_subdivision(std::function<double (double)> f, unsigned int max_evals)
        {
            double a=0; // Maps to x=∞
            double b=1.0; // Maps to x=0
            double c = (a + b)/2.0;

            double fb = f(x_from_t(b));
            double fc = f(x_from_t(c));
    
            unsigned int evaluations = 2;

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
                    // std::cout << "----------" << std::endl;
                    // double last_y = NAN;
                    // for (int i=0; i < intervals; i++) {
                    //     double t = float(i)/intervals;
                    //     double y = f(x_from_t(t));
                    //     if (y != last_y) {
                    //         std::cout << std::setw(5) << i << "/" << std::setw(5) << intervals
                    //                   << std::setw(12) << std::setprecision(5) << t
                    //                   << std::setw(12) << std::setprecision(5) << y
                    //                   << std::endl;
                    //         last_y = y;
                    //     }
                    // }
                    // std::cout << "----------" << std::endl;
                    return {NAN, NAN, evaluations};
                }

            }
            return {a, b, evaluations};
        }

/*
 * Try to divide the range [0, ∞) to bracket something interesting for integration, searching in the reverse direction.
 */
        search_result_t binary_subdivision_rev(std::function<double (double)> f, unsigned int max_evals)
        {
            double a=0; // Maps to x=∞
            double b=1.0; // Maps to x=0
            double c = 1.0 - (a + b)/2.0;

            double fb = f(x_from_t(b));
            double fc = f(x_from_t(c));
    
            unsigned int evaluations = 2;

            if (fb <= std::numeric_limits<double>::epsilon() && 
                fc <= std::numeric_limits<double>::epsilon()) 
            {
                long int intervals = 4;
                while (evaluations < max_evals && fc <= std::numeric_limits<double>::epsilon()) {
                    for (int i=1; i < intervals; i += 2) {
                        c = 1.0 - float(i)/intervals;
                        fc = f(x_from_t(c));
                        
                        evaluations++;

                        if (fc > std::numeric_limits<double>::epsilon()) {
                            a = 1.0 - (float(i) + 1.0) / intervals;
                            b = 1.0 - (float(i) - 1.0) / intervals;
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
 * Try to divide the range [0, ∞) to bracket something interesting for integration, searching in the reverse direction.
 */
        search_result_t binary_subdivision_rev2(std::function<double (double)> f, unsigned int max_evals)
        {
            double a=0; // Maps to x=∞
            double b=1.0; // Maps to x=0
            double c = 1.0 - (a + b)/2.0;

            double fb = f(x_from_t(b));
            double fc = f(x_from_t(c));
    
            unsigned int evaluations = 2;

            if (fb <= std::numeric_limits<double>::epsilon() && 
                fc <= std::numeric_limits<double>::epsilon()) 
            {
                long int intervals = 4;
                while (evaluations < max_evals/2 && fc <= std::numeric_limits<double>::epsilon()) {
                    for (int i=1; i < intervals/2; i += 2) {
                        c = 1.0 - float(i)/intervals;
                        fc = f(x_from_t(c));
                        
                        evaluations++;

                        if (fc > std::numeric_limits<double>::epsilon()) {
                            a = 1.0 - (float(i) + 1.0) / intervals;
                            b = 1.0 - (float(i) - 1.0) / intervals;
                            break;
                        }
                    }
                    intervals *= 2;
                }

                intervals = 4;
                while (evaluations < max_evals && fc <= std::numeric_limits<double>::epsilon()) {
                    for (int i=1 + intervals/2; i < intervals; i += 2) {
                        c = 1.0 - float(i)/intervals;
                        fc = f(x_from_t(c));
                        
                        evaluations++;

                        if (fc > std::numeric_limits<double>::epsilon()) {
                            a = 1.0 - (float(i) + 1.0) / intervals;
                            b = 1.0 - (float(i) - 1.0) / intervals;
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
 * Try to divide the range [0, ∞) to bracket something interesting for integration, alternate method
 */
        search_result_t alternate_binary_subdivision(std::function<double (double)> f, unsigned int max_evals)
        {
            //std::cout << "> alternate_binary_subdivision()" << std::endl;
            double a=0; // Maps to x=∞
            double b=1.0; // Maps to x=0
            double c = (a + b)/2.0;

            double fb = f(x_from_t(b));
            double fc = f(x_from_t(c));
    
            unsigned int evaluations = 2;

            if (fb <= std::numeric_limits<double>::epsilon() && 
                fc <= std::numeric_limits<double>::epsilon()) 
            {
                unsigned int init_intervals = 4;
                unsigned long long int intervals = init_intervals;
                long int k = 0;
                unsigned int init_i = 1;
                while (evaluations < max_evals && fc <= std::numeric_limits<double>::epsilon()) {
                    unsigned long long int i = init_i;
                    c = 1.0 - float(i)/intervals;
                    fc = f(x_from_t(c));
                    evaluations++;

                    //std::cout << i << "/" << intervals << " [" << x_from_t(c) << "] --> " << fc << std::endl;

                    if (fc > std::numeric_limits<double>::epsilon()) {
                        a = 1.0 - (float(i) + 1.0) / intervals;
                        b = 1.0 - (float(i) - 1.0) / intervals;
                        if (a == 0.0 && b == 1.0 && evaluations > 2) {
                            //std::cout << "[1] " << i << " / " << intervals << "; " << evaluations << std::endl;
                        }
                        //std::cout << "(1) " << c << " " << intervals << std::endl;
                        break;
                    }
                    long int p = 2 + 2 * k;
                    k = k + 2;
                    i = i + p;
                    while (i < intervals && evaluations < max_evals) {
                        c = 1.0 - float(i)/intervals;
                        fc = f(x_from_t(c));
                        evaluations++;
                        //std::cout << i << "/" << intervals << " [" << x_from_t(c) << "] --> " << fc << std::endl;

                        if (fc > std::numeric_limits<double>::epsilon()) {
                            a = 1.0 - (float(i) + 1.0) / intervals;
                            b = 1.0 - (float(i) - 1.0) / intervals;
                            if (a == 0.0 && b == 1.0 && evaluations > 2) {
                                //std::cout << "[2] " << i << " / " << intervals << "; " << evaluations << std::endl;
                            }
                            //std::cout << "(2) " << c << " " << intervals << std::endl;
                            break;
                        }
                        p = p * 2;
                        i = i + p;
                    }
                    intervals *= 2;

                    if (intervals * 2 == 0) {
                        //std::cout << "........." << std::endl;
                        if (init_i == 0) {
                            init_i +=  2;
                            if (init_i > init_intervals) {
                                init_intervals *= 2;
                            }
                        }
                        intervals = init_intervals;;
                    }
                }

                if (std::isnan(fc)) {
                    std::cout << "BREAK HERE" << std::endl;
                    std::cout << c << std::endl;
                }

                if (fc <= std::numeric_limits<double>::epsilon()) {
                    //std::cout << fc << ", " << std::numeric_limits<double>::epsilon() << ", " << evaluations << std::endl;
                    //std::cout << "< alternate_binary_subdivision(): NAN, NAN, " << evaluations << std::endl;
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
            // Context::PushText([] (std::ostream &o) {
            //         o << "MAQ()";
            //     });
//            BOOST_LOG_TRIVIAL(fatal) << Context::GetText();
//            BOOST_LOG(IntegrationSettings::settings_logger) << "> MAQ()";
#pragma omp critical
            calls++;
            
            double tol = settings.Get_Effective_Tolerance();
            unsigned int maxeval = settings.Get_Effective_Max_Evals();
            unsigned int bineval = Integration::IntegrationSettings::bin_evals;
            if (bineval == 0) bineval = maxeval;

            //BOOST_LOG(IntegrationSettings::settings_logger) << "MAX_EVALS: " << maxeval;
            // if (maxeval < 2048) {
            //     std::cout << "MAX_EVALS IS " << maxeval << std::endl;
            // }
                
            
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
#if true
                search_result_t r = {0, 0, 0};
                switch (IntegrationSettings::method) {
                case IntegrationSettings::OLD:
                    r = binary_subdivision(integrand, bineval);
                    break;
                case IntegrationSettings::REV:
                    r = binary_subdivision_rev(integrand, bineval);
                    break;
                case IntegrationSettings::REV2:
                    r = binary_subdivision_rev2(integrand, bineval);
                    break;
                case IntegrationSettings::NEW:
                    r = alternate_binary_subdivision(integrand, bineval);
                    break;
                }
#else
                search_result_t r = alternate_binary_subdivision(integrand, bineval);
                // if (!isnan(r.a) && r.evaluations > 100) {
                //     r = binary_subdivision(integrand, bineval, IntegrationSettings::settings_logger );
                // }
#endif
                bin_evals += r.evaluations;
                if (max_bin < r.evaluations) max_bin = r.evaluations;
                if (max_successful_bin < r.evaluations && r.evaluations < bineval) max_successful_bin = r.evaluations;
                if (std::isnan(r.a)) {
                    //std::cout << "binary_subdivision failed" << std::endl;
                    //BOOST_LOG_TRIVIAL(fatal) << Context::GetText();
                    //BOOST_LOG(IntegrationSettings::settings_logger) << "binary_subdivision failed to find anything for " << settings.
                        // warning_label
                        //                                             << " [" << r.evaluations << " evaluations]";
#pragma omp critical
                    bin_fails++;
                    //DumpIntegrationStats();
                    BOOST_LOG_TRIVIAL(fatal) << Context::GetText() << " binary_subdivision() found nothing";
                    //BOOST_LOG(IntegrationSettings::settings_logger) << "< MAQ(): " << 0 << ", " << true << ", " << r.evaluations;
                    // Context::PopText();
                    return {0, true, (unsigned int)r.evaluations}; 
                } else {
                    double c = (r.a + r.b) / 2;
                    size_t bin = c * N_BINS;
                    bin_Bins[bin]++;
                    // BOOST_LOG(IntegrationSettings::settings_logger) << "binary_subdivision succeeded  for " << settings.warning_label
                                                                    // << " after " << r.evaluations << " evaluations.";
                }
                a = r.a;
                b = r.b;
                counter = 0; //r.evaluations;
            }
            
            // if (counter + 4 >= maxeval) {
            //     //std::cout << "no evals left after binary_subdivision()" << std::endl;
            //     bin_near_fails++;
            //     total_count += 32;
            //     counter = maxeval - 32;
            // }
            //int temp_count = counter;
   
            double fa = integrand(x_from_t(a))/(a == 0 ? 1 : a*a);
            double fb = integrand(x_from_t(b))/(b*b);

            if (std::isnan(fa)) {
                std::cout << "fa is NAN" << std::endl;
                BOOST_LOG_TRIVIAL(fatal) << Context::GetText() << "; fa is NAN";
                //BOOST_LOG(IntegrationSettings::settings_logger) << "fa is NAN;  " << settings.warning_label;
#pragma omp critical
                nans++;
                //DumpIntegrationStats();
                maq_evals += counter;
                //BOOST_LOG(IntegrationSettings::settings_logger) << "< MAQ() NAN, " << false << ", " << counter;
                // Context::PopText();
                return {NAN, false, counter};
            }

            if (std::isnan(fb)) {
                //std::cout << "fb is NAN" << std::endl;
                BOOST_LOG_TRIVIAL(fatal) << Context::GetText() << "; fb is NAN";
                //BOOST_LOG(IntegrationSettings::settings_logger) << "fb is NAN;  " << settings.warning_label;
#pragma omp critical
                nans++;
                //DumpIntegrationStats();
                maq_evals += counter;
                //BOOST_LOG(IntegrationSettings::settings_logger) << "< MAQ() NAN, " << false << ", " << counter;
                // Context::PopText();
                return {NAN, false, counter};
            }
            
            double c = (a + b)/2.0;
            double fc = integrand(x_from_t(c))/(c*c);
            if (std::isnan(fc)) {
                BOOST_LOG_TRIVIAL(fatal) << Context::GetText() << "; fc is NAN";
                //BOOST_LOG(IntegrationSettings::settings_logger) << "fc is NAN;  " << settings.warning_label;
#pragma omp critical
                nans++;
                //DumpIntegrationStats();
                maq_evals += counter;
                //BOOST_LOG(IntegrationSettings::settings_logger) << "< MAQ() NAN, " << false << ", " << counter;
                // Context::PopText();
                return {NAN, false, counter};
            }
            
            counter += 3;

            double r1 = quad(a, b, c, fa, fb, fc);
            region_stack.push({a, b, c, fa, fb, fc, r1});

            double abserr = NAN;
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
#elif true
                    /*
                     * This branch generates an error, by returning NAN.
                     */
                    success = false;
                    integral = NAN;
                    if (!std::isnan(abserr)) {
                        std::cout << abserr << std::endl;
                    }
                    break;
#else
                    /*
                     * This branch returns the current approximation, even
                     * though it isn't as good as requested. We should also
                     * generate a warning when this occurs.
                     */
                    // BOOST_LOG(IntegrationSettings::settings_logger) << "failed to reach precision;  " << settings.warning_label
                    //                                                 << " [" << counter << " evaluations]";
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
//                    BOOST_LOG(IntegrationSettings::settings_logger) << "fd is NAN;  " << settings.warning_label;
#pragma omp critical
                    nans++;
                    //DumpIntegrationStats();
                    maq_evals += counter;
//                    BOOST_LOG(IntegrationSettings::settings_logger) << "< MAQ() NAN, " << false << ", " << counter;
                    // Context::PopText();
                    return {NAN, false, counter};
                }
            
                if (std::isnan(fe)) {
//                    BOOST_LOG(IntegrationSettings::settings_logger) << "fe is NAN;  " << settings.warning_label;
#pragma omp critical
                    nans++;
                    //DumpIntegrationStats();
                    maq_evals += counter;
//                    BOOST_LOG(IntegrationSettings::settings_logger) << "< MAQ() NAN, " << false << ", " << counter;
                    // Context::PopText();
                    return {NAN, false, counter};
                }
        
                double r2 = quad(a, c, d, fa, fc, fd);
                double r3 = quad(c, b, e, fc, fb, fe);

                double q1 = r1;
                double q2 = r2 + r3;

                abserr = std::abs(q2 - q1);
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
            // BOOST_LOG(IntegrationSettings::settings_logger) << "MAQ done;  " << settings.warning_label
            //                                                 << " [" << counter << " evaluations]";
            if (counter > max_count) {
                max_count = counter;
            }
            if (success) {
#pragma omp critical
                successes++;
            } else {
#pragma omp critical
                fails++;
            
            }
            maq_evals += counter;
//            BOOST_LOG(IntegrationSettings::settings_logger) << "< MAQ() " << integral << ", " << success << ", " << counter;
            // Context::PopText();
            return {integral, success, counter};
            }

    }
}
