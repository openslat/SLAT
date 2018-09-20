/**
 * @file   functions_test.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief Unit tests for classes declared in functions.h.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */

#include "functions.h"
#include <boost/test/unit_test.hpp>
#include <iomanip>

using namespace std;
using namespace SLAT;

BOOST_AUTO_TEST_CASE( Power_Law_Curve_1_2 )
{
    PowerLawParametricCurve f(1,2);

    for (int i=0; i < 100; i++) {
        double x = double(i)/10.0;
        
        BOOST_CHECK_EQUAL( f.ValueAt(x), x*x);
        if (x == 0) {
            BOOST_CHECK_SMALL( f.DerivativeAt(x), 1E-10);
        } else {
            BOOST_CHECK_CLOSE( f.DerivativeAt(x), 2.0 * x, 0.1);
        }
    }
}

BOOST_AUTO_TEST_CASE( Power_Law_Curve_025_05)
{
    PowerLawParametricCurve f(0.25, 0.5);

    for (int i=0; i < 100; i++) {
        double x = double(i)/10.0;
        
        BOOST_CHECK_EQUAL( f.ValueAt(x), 0.25 * sqrt(x));
        if (x > 0) {
            BOOST_CHECK_CLOSE( f.DerivativeAt(x), 1.0 / (8.0 * sqrt(x)), 0.1);
        }
    }
}

BOOST_AUTO_TEST_CASE( Non_Linear_Hyberbolic_Curve )
{
    NonLinearHyperbolicLaw curve(1221, 29.8, 62.2);

    for (int i=0; i < 100; i++) {
        double x = double(i)/10.0;

        double expected;
        if (x >= 29.8) {
            expected = 0;
        } else {
            expected = 1221 * exp(62.2 / log(x/29.8));
        }

        double actual = curve.ValueAt(x);

        BOOST_CHECK_EQUAL(actual, expected);
        if (x > 0) {
            double derivative = (curve.ValueAt(x + 1E-6) - curve.ValueAt(x)) / 1E-6;
            BOOST_CHECK_CLOSE(curve.DerivativeAt(x), derivative, 1);
        }
    }
}

BOOST_AUTO_TEST_CASE( Linear_Interp_Test_1 )
{
    double x[] = { 1, 10, 20, 50 };
    double y[] = { 1, 10, 25, 85 };
    LinearInterpolatedFn f(x, y, sizeof(x)/sizeof(x[0]));

    BOOST_CHECK_EQUAL(f.ValueAt(0.0), 1.0);
    BOOST_CHECK_EQUAL(f.ValueAt(1.0), 1.0);
    BOOST_CHECK_CLOSE(f.DerivativeAt(1.0), 0.5, 0.1);
    BOOST_CHECK_EQUAL(f.ValueAt(2.0), 2.0);
    BOOST_CHECK_CLOSE(f.DerivativeAt(2.0), 1.0, 0.1);
    BOOST_CHECK_CLOSE(f.DerivativeAt(9.9), 1.0, 0.1);
    BOOST_CHECK_EQUAL(f.ValueAt(10.0), 10.0);
    BOOST_CHECK_CLOSE(f.DerivativeAt(10.0), 1.25, 0.1);
    BOOST_CHECK_CLOSE(f.DerivativeAt(10.1), 1.5, 0.1);
    BOOST_CHECK_EQUAL(f.ValueAt(15.0), 17.5);
    BOOST_CHECK_CLOSE(f.DerivativeAt(15.0), 1.5, 0.1);
    BOOST_CHECK_CLOSE(f.DerivativeAt(19.9), 1.5, 0.1);
    BOOST_CHECK_EQUAL(f.ValueAt(20.0), 25.0);
    BOOST_CHECK_CLOSE(f.DerivativeAt(20.0), 1.75, 0.1);
    BOOST_CHECK_EQUAL(f.ValueAt(30.0), 45.0);
    BOOST_CHECK_CLOSE(f.DerivativeAt(30.0), 2.0, 0.1);
    BOOST_CHECK_EQUAL(f.ValueAt(50.0), 85.0);
    BOOST_CHECK_CLOSE(f.DerivativeAt(50.0), 1.0, 0.1);
    BOOST_CHECK_CLOSE(f.ValueAt(55.0), 85.0, 0.1);
}

BOOST_AUTO_TEST_CASE( Log_Log_Interp_Test_1 )
{
    const size_t size = 30;
    double x[size], y[size];
    NonLinearHyperbolicLaw h(1221, 29.8, 62.2);

    for (unsigned int i=0; i < size; i++) {
        x[i] = double(i)/(size -1) * 28;
        if (x[i] == 0) x[i] = 0.001;
        y[i] = h.ValueAt(x[i]);
    }
    LogLogInterpolatedFn f(x, y, size);

    BOOST_CHECK_CLOSE(f.ValueAt(0.0), h.ValueAt(x[0]), 0.01);
    BOOST_CHECK_SMALL(f.ValueAt(30.0), 1E-5);

    for (int i=0; i <= 20; i++) {
        double x = double(i)/20 * 28;
        if (x == 0) x = 0.001;
        double expected = h.ValueAt(x);
        double actual = f.ValueAt(x);

        if (expected > 1E-5) {
            BOOST_CHECK_CLOSE(expected, actual, 0.01);
            if (i > 0) {
                BOOST_CHECK_CLOSE(f.DerivativeAt(x), h.DerivativeAt(x), 0.01);
            }
        } else {
            BOOST_CHECK_SMALL(actual, 1E-5);
        }
    }
}

BOOST_AUTO_TEST_CASE(Log_Normal_Function_Test)
{
    shared_ptr<DeterministicFn> mu_func(new PowerLawParametricCurve(0.1, 1.5));
    shared_ptr<DeterministicFn> sigma_func(new PowerLawParametricCurve(0.5, 0));

    LogNormalFn edp_im(mu_func, LogNormalFn::MEDIAN_X, sigma_func, LogNormalFn::SIGMA_LN_X);
    
    struct { double im; double pct; double edp; } test_data[] = {
        { 0.10, 0.16, 1.923E-03 }, { 0.10, 0.50, 3.162E-03 }, { 0.10, 0.84, 5.199E-03 },
        { 0.20, 0.16, 5.440E-03 }, { 0.20, 0.50, 8.944E-03 }, { 0.20, 0.84, 1.471E-02 },
        { 0.30, 0.16, 9.994E-03 }, { 0.30, 0.50, 1.643E-02 }, { 0.30, 0.84, 2.702E-02 },
        { 0.40, 0.16, 1.539E-02 }, { 0.40, 0.50, 2.530E-02 }, { 0.40, 0.84, 4.159E-02 },
        { 0.50, 0.16, 2.150E-02 }, { 0.50, 0.50, 3.536E-02 }, { 0.50, 0.84, 5.813E-02 },
        { 0.60, 0.16, 2.827E-02 }, { 0.60, 0.50, 4.648E-02 }, { 0.60, 0.84, 7.641E-02 },
        { 0.70, 0.16, 3.562E-02 }, { 0.70, 0.50, 5.857E-02 }, { 0.70, 0.84, 9.629E-02 },
        { 0.80, 0.16, 4.352E-02 }, { 0.80, 0.50, 7.155E-02 }, { 0.80, 0.84, 1.176E-01 },
        { 0.90, 0.16, 5.193E-02 }, { 0.90, 0.50, 8.538E-02 }, { 0.90, 0.84, 1.404E-01 },
        { 1.00, 0.16, 6.082E-02 }, { 1.00, 0.50, 1.000E-01 }, { 1.00, 0.84, 1.644E-01 },
        { 1.10, 0.16, 7.017E-02 }, { 1.10, 0.50, 1.154E-01 }, { 1.10, 0.84, 1.897E-01 },
        { 1.20, 0.16, 7.995E-02 }, { 1.20, 0.50, 1.315E-01 }, { 1.20, 0.84, 2.161E-01 },
        { 1.30, 0.16, 9.015E-02 }, { 1.30, 0.50, 1.482E-01 }, { 1.30, 0.84, 2.437E-01 },
        { 1.40, 0.16, 1.008E-01 }, { 1.40, 0.50, 1.657E-01 }, { 1.40, 0.84, 2.724E-01 },
        { 1.50, 0.16, 1.117E-01 }, { 1.50, 0.50, 1.837E-01 }, { 1.50, 0.84, 3.021E-01 },
        { 1.60, 0.16, 1.231E-01 }, { 1.60, 0.50, 2.024E-01 }, { 1.60, 0.84, 3.328E-01 },
        { 1.70, 0.16, 1.348E-01 }, { 1.70, 0.50, 2.217E-01 }, { 1.70, 0.84, 3.644E-01 },
        { 1.80, 0.16, 1.469E-01 }, { 1.80, 0.50, 2.415E-01 }, { 1.80, 0.84, 3.971E-01 },
        { 1.90, 0.16, 1.593E-01 }, { 1.90, 0.50, 2.619E-01 }, { 1.90, 0.84, 4.306E-01 },
        { 2.00, 0.16, 1.720E-01 }, { 2.00, 0.50, 2.828E-01 }, { 2.00, 0.84, 4.650E-01 },
        { 2.10, 0.16, 1.851E-01 }, { 2.10, 0.50, 3.043E-01 }, { 2.10, 0.84, 5.003E-01 },
        { 2.20, 0.16, 1.985E-01 }, { 2.20, 0.50, 3.263E-01 }, { 2.20, 0.84, 5.365E-01 },
        { 2.30, 0.16, 2.122E-01 }, { 2.30, 0.50, 3.488E-01 }, { 2.30, 0.84, 5.735E-01 },
        { 2.40, 0.16, 2.261E-01 }, { 2.40, 0.50, 3.718E-01 }, { 2.40, 0.84, 6.113E-01 },
        { 2.50, 0.16, 2.404E-01 }, { 2.50, 0.50, 3.953E-01 }, { 2.50, 0.84, 6.499E-01 },
    };
    for (unsigned int i=0; i < sizeof(test_data)/sizeof(test_data[0]); i++) {
        double im = test_data[i].im;
        double expected_pct = test_data[i].pct;
        double expected_edp = test_data[i].edp;

        double actual_edp = edp_im.X_at_exceedence(im, expected_pct);
        double actual_pct = 1.0 - edp_im.P_exceedence(im, expected_edp);

        BOOST_CHECK_CLOSE(expected_pct, actual_pct, 0.5);
        BOOST_CHECK_CLOSE(expected_edp, actual_edp, 1.0);
    }

    /*
     * Make sure we get the right overloaded function when we access a LogNormalFn
     * through a pointer:
     */
    std::shared_ptr<ProbabilisticFn> f(new LogNormalFn(mu_func, LogNormalFn::MEDIAN_X, 
                                                                   sigma_func, LogNormalFn::SIGMA_LN_X));
    for (unsigned int i=0; i < sizeof(test_data)/sizeof(test_data[0]); i++) {
        double im = test_data[i].im;
        double expected_pct = test_data[i].pct;
        double expected_edp = test_data[i].edp;

        double actual_edp = f->X_at_exceedence(im, expected_pct);
        double actual_pct = 1.0 - f->P_exceedence(im, expected_edp);

        BOOST_CHECK_CLOSE(expected_pct, actual_pct, 0.5);
        BOOST_CHECK_CLOSE(expected_edp, actual_edp, 1.0);
    }
}

BOOST_AUTO_TEST_CASE(Solver_Test)
{
    shared_ptr<DeterministicFn> f(new NonLinearHyperbolicLaw(1221, 29.8, 62.2));

    const int N=10;
    const int MAX_X = 2.5;

    for (int i=0; i <= N; i++) {
        double x = (double)i/N * MAX_X;
        double y = f->ValueAt(x);
        double z = f->solve_for(y);
        
        if (isnan(z)) std::cout << x << ", " << y << ", " << z << std::endl;
        BOOST_CHECK_CLOSE(x, z, 0.01);
    }
}

BOOST_AUTO_TEST_CASE(Linear_Solver_Test)
{
    const size_t size = 30;
    double x[size], y[size];
    NonLinearHyperbolicLaw h(1221, 29.8, 62.2);

    for (unsigned int i=0; i < size; i++) {
        x[i] = double(i)/(size -1) * 28;
        if (x[i] == 0) x[i] = 0.001;
        y[i] = h.ValueAt(x[i]);
    }
    LinearInterpolatedFn f(x, y, size);

    const int N=10;
    const int MAX_X = 2.5;

    for (int i=0; i <= N; i++) {
        double x = (double)i/N * MAX_X;
        double y = f.ValueAt(x);
        double z = f.solve_for(y);
        
        BOOST_CHECK_CLOSE(x, z, 0.01);
    }
}

BOOST_AUTO_TEST_CASE(LogLog_Solver_Test)
{
    const size_t size = 30;
    double x[size], y[size];
    NonLinearHyperbolicLaw h(1221, 29.8, 62.2);

    for (unsigned int i=0; i < size; i++) {
        x[i] = double(i)/(size -1) * 28;
        if (x[i] == 0) x[i] = 0.001;
        y[i] = h.ValueAt(x[i]);
    }
    LogLogInterpolatedFn f(x, y, size);

    double start_time = omp_get_wtime();
    for (size_t k=0; k < 1000; k++) {
        const int N=1000;
        const int MAX_X = 2.5;
        
        for (int i=0; i <= N; i++) {
            double x = (double)i/N * MAX_X;
            double y = f.ValueAt(x);
            double z = f.solve_for(y);
            
            BOOST_CHECK_CLOSE(x, z, 0.1);
        }
    }
    double end_time = omp_get_wtime();
    std::cout << "Elapsed time: " << end_time - start_time << endl;
}
