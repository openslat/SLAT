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
#include "nzs.h"
#include <boost/test/unit_test.hpp>
#include <iomanip>

using namespace std;
using namespace SLAT;
BOOST_AUTO_TEST_CASE( NZS_Test )
{
    std::shared_ptr<DeterministicFn> f = NZS_Hazard_Curve(C, 0.0, 0.22, NAN);
    BOOST_CHECK_CLOSE(f->ValueAt(0.05852), 1./20, 1);
    BOOST_CHECK_CLOSE(f->ValueAt(0.07), 0.042, 1);
    BOOST_CHECK_CLOSE(f->ValueAt(0.1), 0.021, 1);
    BOOST_CHECK_CLOSE(f->ValueAt(0.15), 0.0095, 1);
    BOOST_CHECK_CLOSE(f->ValueAt(0.25), 0.0029, 1);
}
