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
    BOOST_REQUIRE(IntegrationSettings::Get_Integration_Eval_Limit() == 2048);
    BOOST_REQUIRE(IntegrationSettings::Get_Tolerance() == 1E-3);

    /*
     * Change tolerance, max evals should not be effected:
     */
    IntegrationSettings::Set_Tolerance(1E-5);
    BOOST_REQUIRE(IntegrationSettings::Get_Tolerance() == 1E-5);
    BOOST_REQUIRE(IntegrationSettings::Get_Integration_Eval_Limit() == 2048);

    /*
     * Change Max Evals; should not affect root settings:
     */
    IntegrationSettings::Set_Integration_Eval_Limit(512);
    BOOST_REQUIRE(IntegrationSettings::Get_Integration_Eval_Limit() == 512);
    BOOST_REQUIRE(IntegrationSettings::Get_Tolerance() == 1E-5);

    /*
     * Reset the integration parameters & verify:
     */
    IntegrationSettings::Reset();
    BOOST_REQUIRE(IntegrationSettings::Get_Integration_Eval_Limit() == 2048);
    BOOST_REQUIRE(IntegrationSettings::Get_Tolerance() == 1E-3);
}


BOOST_AUTO_TEST_CASE(MAQ_Test)
{
    IntegrationSettings::Reset();

    MAQ_RESULT result;

    /*
     * Using the default tolerance and number of evaluations, the integration
     * succeeds.
     */
    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        });
    BOOST_REQUIRE(result.successful);
    BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations < 100);
    BOOST_CHECK_CLOSE(result.integral, 1.0, 0.1);

    /*
     * With a ridiculously small evaluations limit, the integration should fail
     * after hitting the limit. The result will probably be close to zero, but
     * we don't really care.
     */

    /*
     * Override MAX_EVALS; integration should fail.
     */
    IntegrationSettings::Set_Integration_Eval_Limit(3);
    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        });
    BOOST_REQUIRE(!result.successful);
    BOOST_REQUIRE(result.evaluations >= 3);
    BOOST_CHECK(std::isnan(result.integral));

    /*
     * Cancel the override; integration should succeed:
     */
    IntegrationSettings::Reset();
    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        });
    BOOST_REQUIRE(result.successful);
    BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations < 100);
    BOOST_CHECK_CLOSE(result.integral, 1.0, 0.1);
    /*
     * Set the default MAX_EVALS to 3; both should fail:
     */
    IntegrationSettings::Set_Integration_Eval_Limit(3);
    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        });
    BOOST_REQUIRE(!result.successful);
    BOOST_REQUIRE(result.evaluations >= 3);
    BOOST_CHECK(std::isnan(result.integral));

    /*
     * Set the tolerance too tight, and the integration will fail, but provide a
     * good result anyway:
     */
    IntegrationSettings::Reset();
    IntegrationSettings::Set_Integration_Eval_Limit(1024);
    IntegrationSettings::Set_Tolerance(2E-13);
    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        });
    BOOST_REQUIRE(!result.successful);
    BOOST_REQUIRE(result.evaluations >= 
                  IntegrationSettings::Get_Integration_Eval_Limit());
    BOOST_CHECK(std::isnan(result.integral));


    /*
     * Use only 40 evaluations. The integration will fail, but the result is
     * within * 0.1% of 1.0.
     */
    IntegrationSettings::Reset();
    IntegrationSettings::Set_Tolerance(1E-6);
    IntegrationSettings::Set_Integration_Eval_Limit(40);
    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        });
    BOOST_REQUIRE(!result.successful);
    BOOST_REQUIRE(result.evaluations >= 40);
    BOOST_CHECK(std::isnan(result.integral));

    /*
     * If we keep the same limit, but lower the required accuracy, the
     * integration succeeds:
     */
    IntegrationSettings::Set_Tolerance(1E-4);
    result = MAQ([] (double x) -> double {
                     return gsl_ran_lognormal_pdf(x, log(1), 1);
        });
    BOOST_REQUIRE(result.successful);
    BOOST_REQUIRE(result.evaluations <= 50);
    BOOST_CHECK_CLOSE(result.integral, 1.0, 0.1);

    /*
     * Given a much higher limit, the integration is successful, and accurate to
     * within 0.0001%.
     */
    IntegrationSettings::Set_Tolerance(1E-6);
    IntegrationSettings::Set_Integration_Eval_Limit(1024);
    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        });
    BOOST_REQUIRE(result.successful);
    BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations < 100);
    BOOST_CHECK_CLOSE(result.integral, 1.0, 1E-4);


    /*
     * The remaining integrations should all succeed well within the allocated
     * number of evaluations, to the requested accuracy.
     */
    IntegrationSettings::Set_Tolerance(1E-6);
    IntegrationSettings::Set_Integration_Eval_Limit(1024);
    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1E5), 1);
        });
    BOOST_REQUIRE(result.successful);
    BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations <= 200);
    BOOST_CHECK_CLOSE(result.integral, 1.0, 1E-4);
    
    result = MAQ([] (double x) -> double {
                     return gsl_ran_lognormal_pdf(x + 0.25, log(1.0), 0.5);
        });
    BOOST_REQUIRE(result.successful);
    BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations <= 200);
    BOOST_CHECK_CLOSE(result.integral, 0.997219, 1E-4);

    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(0.25), 0.1);
        });
    BOOST_REQUIRE(result.successful);
    BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations <= 200);
    BOOST_CHECK_CLOSE(result.integral, 1.0, 1E-4);

    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(10), 0.1);
        });
    BOOST_REQUIRE(result.successful);
    BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations <= 200);
    BOOST_CHECK_CLOSE(result.integral, 1.0, 1E-4);

    result = MAQ([] (double x) -> double {
                     return gsl_ran_lognormal_pdf(x, log(1E5), 3);
        });
    BOOST_REQUIRE(result.successful);
    BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations <= 250);
    BOOST_CHECK_CLOSE(result.integral, 1.0, 1E-4);

    /*    
     * Restore defaults:
     */
    IntegrationSettings::Reset();
}
