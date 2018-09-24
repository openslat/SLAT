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
#include "nzs.h"
#include <boost/test/unit_test.hpp>
#include <iomanip>

using namespace std;
using namespace SLAT;

BOOST_AUTO_TEST_CASE( Complex_Relationship_Test )
{
    const SOIL_CLASS soil_class = C;
    const double Z = 0.22;
    const double distance = NAN;
    
    shared_ptr<DeterministicFn> base_im_rate_function 
        = NZS_Hazard_Curve(soil_class, 0.0, Z, distance);
    shared_ptr<DeterministicFn> x_im_rate_function
        = NZS_Hazard_Curve(soil_class, 1.0, Z, distance);
    shared_ptr<DeterministicFn> y_im_rate_function
        = NZS_Hazard_Curve(soil_class, 2.5, Z, distance);

    Complex_IM im(base_im_rate_function, x_im_rate_function, y_im_rate_function, "Test IM");

    BOOST_CHECK_CLOSE(im.lambda(Complex_IM::BASE, 0.1), 0.021, 1);
    BOOST_CHECK_CLOSE(im.lambda(Complex_IM::X, 0.1), 0.017, 1);
    BOOST_CHECK_CLOSE(im.lambda(Complex_IM::Y, 0.1), 0.00285, 1);

    double base_value = im.ValueFromLambda(Complex_IM::BASE, 0.001);
    BOOST_CHECK_CLOSE(im.lambda(Complex_IM::BASE, base_value), 0.001, 0.1);

    double x_value = im.ValueFromLambda(Complex_IM::X, 0.001);
    BOOST_CHECK_CLOSE(im.lambda(Complex_IM::X, x_value), 0.001, 0.1);

    double y_value = im.ValueFromLambda(Complex_IM::Y, 0.001);
    BOOST_CHECK_CLOSE(im.lambda(Complex_IM::Y, y_value), 0.001, 0.1);

    BOOST_CHECK_NE(base_value, x_value);
    BOOST_CHECK_NE(base_value, y_value);
    BOOST_CHECK_NE(x_value, y_value);

    std::cout << im.DerivativeAt(Complex_IM::BASE, base_value) << "; " 
              << im.DerivativeAt(Complex_IM::X, x_value) << "; " 
              << im.DerivativeAt(Complex_IM::Y, y_value) << std::endl;
    /*
      shared_ptr<DeterministicFn> double_base_im_rate_function(
      new NonLinearHyperbolicLaw(2 * 1221, 29.8, 62.2));
      base_im_rate_function->replace(double_base_im_rate_function); 
    */

}
