/**
 * @file   relationships_test.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief Unit tests for classes declared in relationships.h.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */
#include "complex_relationships.h"
#include <boost/test/unit_test.hpp>
#include <iomanip>

using namespace std;
using namespace SLAT;

BOOST_AUTO_TEST_CASE( Complex_Relationship_Test )
{
    shared_ptr<DeterministicFn> base_im_rate_function(
        new NonLinearHyperbolicLaw(1221, 29.8, 62.2));
    shared_ptr<DeterministicFn> x_im_rate_function(
        new NonLinearHyperbolicLaw(1221 * 0.95, 29.8, 62.2));
    shared_ptr<DeterministicFn> y_im_rate_function(
        new NonLinearHyperbolicLaw(1221 * 0.90, 29.8, 62.2));

    Complex_IM im(base_im_rate_function, x_im_rate_function, y_im_rate_function, "Test IM");
    std::cout << im << std::endl;

    const std::array<double, 7> values = { 1E-3, 2E-3, 5E-3, 1E-2, 2E-2, 5E-2, 1E-1 };
    const std::array<double, 8> rates = { 10, 1, 0.1, 0.05, 0.01, 0.001, 0.0001, 1E-6 };

    std::cout << std::setw(15) << "im" 
              << std::setw(15) << "base_rate"
              << std::setw(15) << "x_rate"
              << std::setw(15) << "y_rate"
              << std::endl;
    for (const auto& im_value : values) {
        double base_rate = im.lambda(Complex_IM::BASE, im_value);
        double x_rate = im.lambda(Complex_IM::X, im_value);
        double y_rate = im.lambda(Complex_IM::Y, im_value);
        std::cout << std::setw(15) << im_value
                  << std::setw(15) << base_rate
                  << std::setw(15) << x_rate
                  << std::setw(15) << y_rate
                  << std::endl;
    }
    std::cout << "============================" << std::endl;
    std::cout << std::setw(15) << "lambda" 
              << std::setw(15) << "base"
              << std::setw(15) << "x"
              << std::setw(15) << "y"
              << std::endl;
    for (const auto& lambda : rates) {
        double base = im.value(Complex_IM::BASE, lambda);
        double x = im.value(Complex_IM::X, lambda);
        double y = im.value(Complex_IM::Y, lambda);
        std::cout << std::setw(15) << lambda
                  << std::setw(15) << base
                  << std::setw(15) << x
                  << std::setw(15) << y
                  << std::endl;
    }
    std::cout << "============================" << std::endl;

    std::cout << im.lambda(Complex_IM::BASE, 0.001) << std::endl
              << im.lambda(Complex_IM::X, 0.001) << std::endl
              << im.lambda(Complex_IM::Y, 0.001) << std::endl
              << "-----------" << std::endl;
        
    shared_ptr<DeterministicFn> double_base_im_rate_function(
        new NonLinearHyperbolicLaw(2 * 1221, 29.8, 62.2));
    base_im_rate_function->replace(double_base_im_rate_function); 

    x_im_rate_function->notify_change();
        
    std::cout << im.lambda(Complex_IM::BASE, 0.001) << std::endl
              << im.lambda(Complex_IM::X, 0.001) << std::endl
              << im.lambda(Complex_IM::Y, 0.001) << std::endl
              << "-----------" << std::endl;
        
    {
        double lambda = 0.002;
        double base = im.value(Complex_IM::BASE, lambda);
        double x = base * 0.95;
        double y = base * 0.90;

        std::cout << lambda << ": " << base << std::endl
                  << "     " << x << ": " << im.lambda(Complex_IM::X, x) << std::endl
                  << "     " << y << ": " << im.lambda(Complex_IM::Y, y) << std::endl;
    }
    BOOST_CHECK_CLOSE(im.value(Complex_IM::BASE, im.lambda(Complex_IM::BASE, 0.001)),
                      0.001,
        0.1);
    BOOST_CHECK_EQUAL(1, 1);
}
