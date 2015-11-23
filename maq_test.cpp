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
#include "maq.h"
#include <cmath>
#include <functional>
#include <iostream>
#include "gsl/gsl_randist.h"
#include <boost/test/unit_test.hpp>

using namespace SLAT::Integration;

BOOST_AUTO_TEST_CASE(Integration_Settings_Test)
{
    /*
     * Verify the system defaults:
     */
    const IntegrationSettings *root_settings;
    root_settings = IntegrationSettings::Get_Global_Settings();
    
    BOOST_REQUIRE(root_settings->Get_Effective_Max_Evals() == 1024);
    BOOST_REQUIRE(root_settings->Get_Effective_Tolerance() == 1E-6);

    /*
     * Create new settings from the system settings; effective values should not
     * change:
     */
    IntegrationSettings settings1(IntegrationSettings::Get_Global_Settings());
    BOOST_REQUIRE(settings1.Get_Effective_Max_Evals() == 1024);
    BOOST_REQUIRE(settings1.Get_Effective_Tolerance() == 1E-6);

    IntegrationSettings settings2(IntegrationSettings::Get_Global_Settings());
    BOOST_REQUIRE(settings2.Get_Effective_Max_Evals() == 1024);
    BOOST_REQUIRE(settings2.Get_Effective_Tolerance() == 1E-6);

    /*
     * Change global tolerance; should affect child objects as well:
     */
    IntegrationSettings::Set_Tolerance(1E-5);
    BOOST_REQUIRE(root_settings->Get_Effective_Tolerance() == 1E-5);
    BOOST_REQUIRE(settings1.Get_Effective_Tolerance() == 1E-5);
    BOOST_REQUIRE(settings2.Get_Effective_Tolerance() == 1E-5);

    /*
     * Max_Evals should not change, for any object:
     */
    BOOST_REQUIRE(root_settings->Get_Effective_Max_Evals() == 1024);
    BOOST_REQUIRE(settings1.Get_Effective_Max_Evals() == 1024);
    BOOST_REQUIRE(settings2.Get_Effective_Max_Evals() == 1024);

    /*
     * Change global eval limt; should affect child objects as well:
     */
    IntegrationSettings::Set_Max_Evals(256);
    BOOST_REQUIRE(root_settings->Get_Effective_Max_Evals() == 256);
    BOOST_REQUIRE(settings1.Get_Effective_Max_Evals() == 256);
    BOOST_REQUIRE(settings2.Get_Effective_Max_Evals() == 256);

    /*
     * Tolerance should not change, for any object:
     */
    BOOST_REQUIRE(root_settings->Get_Effective_Tolerance() == 1E-5);
    BOOST_REQUIRE(settings1.Get_Effective_Tolerance() == 1E-5);
    BOOST_REQUIRE(settings2.Get_Effective_Tolerance() == 1E-5);

    /*
     * Override tolerance; should not affect root settings or other settings:
     */
    settings1.Override_Tolerance(1E-4);
    BOOST_REQUIRE(settings1.Get_Effective_Max_Evals() == 256);
    BOOST_REQUIRE(settings1.Get_Effective_Tolerance() == 1E-4);
    
    BOOST_REQUIRE(settings2.Get_Effective_Max_Evals() == 256);
    BOOST_REQUIRE(settings2.Get_Effective_Tolerance() == 1E-5);

    BOOST_REQUIRE(root_settings->Get_Effective_Max_Evals() == 256);
    BOOST_REQUIRE(root_settings->Get_Effective_Tolerance() == 1E-5);

    /*
     * Override Max Evals; should not affect root settings or other settings:
     */
    settings2.Override_Max_Evals(512);
    BOOST_REQUIRE(settings2.Get_Effective_Max_Evals() == 512);
    BOOST_REQUIRE(settings2.Get_Effective_Tolerance() == 1E-5);
    
    BOOST_REQUIRE(settings1.Get_Effective_Max_Evals() == 256);
    BOOST_REQUIRE(settings1.Get_Effective_Tolerance() == 1E-4);

    BOOST_REQUIRE(root_settings->Get_Effective_Max_Evals() == 256);
    BOOST_REQUIRE(root_settings->Get_Effective_Tolerance() == 1E-5);

    /*
     * Create another level of settings:
     */
    IntegrationSettings settings3(&settings1);
    BOOST_REQUIRE(settings1.Get_Effective_Max_Evals() == 256);
    BOOST_REQUIRE(settings1.Get_Effective_Tolerance() == 1E-4);

    BOOST_REQUIRE(settings3.Get_Effective_Max_Evals() == 256);
    BOOST_REQUIRE(settings3.Get_Effective_Tolerance() == 1E-4);

    /*
     * Override Max Evals; should not affect tolerance, or parent:
     */
    settings3.Override_Max_Evals(128);
    BOOST_REQUIRE(settings3.Get_Effective_Max_Evals() == 128);
    BOOST_REQUIRE(settings3.Get_Effective_Tolerance() == 1E-4);

    BOOST_REQUIRE(settings1.Get_Effective_Max_Evals() == 256);
    BOOST_REQUIRE(settings1.Get_Effective_Tolerance() == 1E-4);
    
    /*
     * Override Max Evals in parent; child does not change:
     */
    settings1.Override_Max_Evals(300);
    BOOST_REQUIRE(settings1.Get_Effective_Max_Evals() == 300);
    BOOST_REQUIRE(settings1.Get_Effective_Tolerance() == 1E-4);
    
    BOOST_REQUIRE(settings3.Get_Effective_Max_Evals() == 128);
    BOOST_REQUIRE(settings3.Get_Effective_Tolerance() == 1E-4);

    /*
     * Override tolerance in parent; child changes as well:
     */
    settings1.Override_Tolerance(1E-3);
    BOOST_REQUIRE(settings1.Get_Effective_Max_Evals() == 300);
    BOOST_REQUIRE(settings1.Get_Effective_Tolerance() == 1E-3);
    
    BOOST_REQUIRE(settings3.Get_Effective_Max_Evals() == 128);
    BOOST_REQUIRE(settings3.Get_Effective_Tolerance() == 1E-3);

    /*
     * Return child Max Evals to UNSPECIFIED. Should now use parent's value:
     */
    settings3.Use_Default_Max_Evals();
    BOOST_REQUIRE(settings3.Get_Effective_Max_Evals() == 300);
    BOOST_REQUIRE(settings3.Get_Effective_Tolerance() == 1E-3);

    BOOST_REQUIRE(settings1.Get_Effective_Max_Evals() == 300);
    BOOST_REQUIRE(settings1.Get_Effective_Tolerance() == 1E-3);

}

// boost_AUTO_TEST_CASE(MAQ_Defaults)
// {
//     /*
//      * Save the default values so we can restore them later:
//      */
//     double tolerance = Get_Default_Tolerance();
//     unsigned int evaluations = Get_Default_Evaluations();
    
//     /*
//      * Make sure the default values are valid:
//      */
//     BOOST_CHECK_NE(Get_Default_Tolerance(), TOLERANCE_UNSPECIFIED);
//     BOOST_CHECK(Get_Default_Tolerance() > 0.0);
//     BOOST_CHECK(Get_Default_Tolerance() < 1.0);
//     BOOST_CHECK_NE(Get_Default_Evaluations(), EVALUATIONS_UNSPECIFIED);
//     BOOST_CHECK(Get_Default_Evaluations() > 0);

//     /*
//      * Not allowed to change the default tolerance to UNSPECIFIED.
//      */
//     Set_Default_Tolerance(TOLERANCE_UNSPECIFIED);
//     BOOST_CHECK_NE(Get_Default_Tolerance(), TOLERANCE_UNSPECIFIED);

//     /*
//      * Set default tolerance to a positive value:
//      */
//     Set_Default_Tolerance(-0.123);
//     BOOST_CHECK_NE(Get_Default_Tolerance(), TOLERANCE_UNSPECIFIED);
//     BOOST_CHECK_EQUAL(Get_Default_Tolerance(), 0.123);

//     /*
//      * If we try to set default tolerance to a negative value, the absolute
//      * value is used instead:
//      */
//     Set_Default_Tolerance(1.5E-3);
//     BOOST_CHECK_EQUAL(Get_Default_Tolerance(), 1.5E-3);

//     /*
//      * Not allowed to change the default evaluations limit to UNSPECIFIED.
//      */
//     Set_Default_Evaluations(EVALUATIONS_UNSPECIFIED);
//     BOOST_CHECK_NE(Get_Default_Evaluations(), EVALUATIONS_UNSPECIFIED);

//     /*
//      * Set evaluations to a valid value:
//      */
//     Set_Default_Evaluations(17);
//     BOOST_CHECK_EQUAL(Get_Default_Evaluations(), 17);

//     /*    
//      * Restore defaults:
//      */
//     Set_Default_Tolerance(tolerance);
//     Set_Default_Evaluations(evaluations);
// }

// BOOST_AUTO_TEST_CASE(MAQ_Test)
// {
//     MAQ_RESULT result;

//     /*
//      * Using the default tolerance and number of evaluations, the integration
//      * succeeds.
//      */
//     result = MAQ([] (double x) -> double {
//             return gsl_ran_lognormal_pdf(x, log(1), 1);
//         });
//     BOOST_REQUIRE(result.successful);
//     BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations < 100);
//     BOOST_CHECK_CLOSE(result.integral, 1.0, 1E-4);

//     /*
//      * With a ridiculously small evaluations limit, the integration should fail
//      * after hitting the limit. The result will probably be close to zero, but
//      * we don't really care.
//      */
//     result = MAQ([] (double x) -> double {
//             return gsl_ran_lognormal_pdf(x, log(1), 1);
//         }, 1E-6, 3);
//     BOOST_REQUIRE(!result.successful);
//     BOOST_REQUIRE(result.evaluations >= 3);
//     BOOST_CHECK_SMALL(result.integral, 1E-5);

//     /*
//      * Set the default tolerance too tights, and the integration will fail, but 
//      * provide a good result anyway:
//      */
//     Set_Default_Tolerance(2E-13);
//     result = MAQ([] (double x) -> double {
//             return gsl_ran_lognormal_pdf(x, log(1), 1);
//         });
//     BOOST_REQUIRE(!result.successful);
//     BOOST_REQUIRE(result.evaluations >= Get_Default_Evaluations());
//     BOOST_CHECK_CLOSE(result.integral, 1.0, 1);

//     /*
//      * Allowed more evaluations, the integration will still fail, but the result
//      * will be nearer to 1/2.
//      */
//     result = MAQ([] (double x) -> double {
//                      return gsl_ran_lognormal_pdf(x, log(1), 1);
//         }, 1E-6, 20);
//     BOOST_REQUIRE(!result.successful);
//     BOOST_REQUIRE(result.evaluations >= 20);
//     BOOST_CHECK_CLOSE(result.integral, 0.5, 10);

//     /*
//      * With a limit of 50, the integration still fails, but the result is within
//      * 0.1% of 1.0.
//      */
//     result = MAQ([] (double x) -> double {
//             return gsl_ran_lognormal_pdf(x, log(1), 1);
//         }, 1E-6, 50);
//     BOOST_REQUIRE(!result.successful);
//     BOOST_REQUIRE(result.evaluations >= 50);
//     BOOST_CHECK_CLOSE(result.integral, 1.0, 0.1);

//     /*
//      * If we keep the same limit, but lower the required accuracy, the
//      * integration succeeds:
//      */
//     result = MAQ([] (double x) -> double {
//                      return gsl_ran_lognormal_pdf(x, log(1), 1);
//         }, 1E-4, 50);
//     BOOST_REQUIRE(result.successful);
//     BOOST_REQUIRE(result.evaluations <= 50);
//     BOOST_CHECK_CLOSE(result.integral, 1.0, 0.1);

//     /*
//      * Given a much higher limit, the integration is successful, and accurate to
//      * within 0.0001%.
//      */
//     result = MAQ([] (double x) -> double {
//             return gsl_ran_lognormal_pdf(x, log(1), 1);
//         }, 1E-6, 1024);
//     BOOST_REQUIRE(result.successful);
//     BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations < 100);
//     BOOST_CHECK_CLOSE(result.integral, 1.0, 1E-4);


//     /*
//      * The remaining integrations should all succeed well within the allocated
//      * number of evaluations, to the requested accuracy.
//      */
//     result = MAQ([] (double x) -> double {
//             return gsl_ran_lognormal_pdf(x, log(1E5), 1);
//         }, 1E-6, 1024);
//     BOOST_REQUIRE(result.successful);
//     BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations <= 200);
//     BOOST_CHECK_CLOSE(result.integral, 1.0, 1E-4);

//     result = MAQ([] (double x) -> double {
//                      return gsl_ran_lognormal_pdf(x + 0.25, log(1.0), 0.5);
//         }, 1E-6, 1024);
//     BOOST_REQUIRE(result.successful);
//     BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations <= 200);
//     BOOST_CHECK_CLOSE(result.integral, 0.997219, 1E-4);

//     result = MAQ([] (double x) -> double {
//             return gsl_ran_lognormal_pdf(x, log(0.25), 0.1);
//         }, 1E-6, 1024);
//     BOOST_REQUIRE(result.successful);
//     BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations <= 200);
//     BOOST_CHECK_CLOSE(result.integral, 1.0, 1E-4);

//     result = MAQ([] (double x) -> double {
//             return gsl_ran_lognormal_pdf(x, log(10), 0.1);
//         }, 1E-6, 1024);
//     BOOST_REQUIRE(result.successful);
//     BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations <= 200);
//     BOOST_CHECK_CLOSE(result.integral, 1.0, 1E-4);

//     result = MAQ([] (double x) -> double {
//                      return gsl_ran_lognormal_pdf(x, log(1E5), 3);
//         }, 1E-6, 1024
//         );
//     BOOST_REQUIRE(result.successful);
//     BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations <= 250);
//     BOOST_CHECK_CLOSE(result.integral, 1.0, 1E-4);
// }
