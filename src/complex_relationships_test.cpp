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

    shared_ptr<DeterministicFn> double_base_im_rate_function(
        new NonLinearHyperbolicLaw(2 * 1221, 29.8, 62.2));
    base_im_rate_function->replace(double_base_im_rate_function); 

    x_im_rate_function->notify_change();
        
    BOOST_CHECK_CLOSE(im.ValueFromLambda(Complex_IM::BASE, im.lambda(Complex_IM::BASE, 0.001)),
                      0.001,
        0.1);
    BOOST_CHECK_EQUAL(1, 1);
}
