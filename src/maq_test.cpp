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

BOOST_AUTO_TEST_CASE(MAQ_Defaults)
{
    /*
     * Reset the values to their defaults, and make sure then are valid:
     */
    const IntegrationSettings *global_settings = IntegrationSettings::Get_Global_Settings();
    IntegrationSettings::Reset();

    double tolerance = global_settings->Get_Effective_Tolerance();
    unsigned int evaluations = global_settings->Get_Effective_Max_Evals();
    
    /*
     * Make sure the default values are valid:
     */
    BOOST_CHECK(tolerance > 0.0);
    BOOST_CHECK(tolerance < 1.0);
    BOOST_CHECK(evaluations > 0);

    /*
     * Not allowed to change the default tolerance to zero:
     */
    IntegrationSettings::Set_Tolerance(0.0);
    BOOST_CHECK_EQUAL(global_settings->Get_Effective_Tolerance(), tolerance);

    /*
     * Try setting default tolerance to a negative value; it shouldn't change:
     */
    IntegrationSettings::Set_Tolerance(-0.123);
    BOOST_CHECK_EQUAL(global_settings->Get_Effective_Tolerance(), tolerance);

    /*
     * Set default tolerance to a positive value:
     */
    IntegrationSettings::Set_Tolerance(1.23E-4);
    BOOST_CHECK_EQUAL(global_settings->Get_Effective_Tolerance(), 1.23E-4);

    
    /*
     * Not allowed to change the default evaluations limit to 0:
     */
    IntegrationSettings::Set_Max_Evals(0);
    BOOST_CHECK_EQUAL(global_settings->Get_Effective_Max_Evals(), evaluations);

    /*
     * Set evaluation limit to a valid value:
     */
    IntegrationSettings::Set_Max_Evals(299);
    BOOST_CHECK_EQUAL(global_settings->Get_Effective_Max_Evals(), 299);

    /*    
     * Restore defaults:
     */
    IntegrationSettings::Reset();
    BOOST_CHECK_EQUAL(global_settings->Get_Effective_Tolerance(), tolerance);
    BOOST_CHECK_EQUAL(global_settings->Get_Effective_Max_Evals(), evaluations);
}

BOOST_AUTO_TEST_CASE(MAQ_Test)
{
    IntegrationSettings::Reset();

    MAQ_RESULT result;
    IntegrationSettings local_settings(IntegrationSettings::Get_Global_Settings());

    /*
     * Using the default tolerance and number of evaluations, the integration
     * succeeds.
     */
    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        });
    BOOST_REQUIRE(result.successful);
    BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations < 100);
    BOOST_CHECK_CLOSE(result.integral, 1.0, 1E-4);

    /*
     * local_settings should be the same as the defaults:
     */
    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        }, local_settings);
    BOOST_REQUIRE(result.successful);
    BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations < 100);
    BOOST_CHECK_CLOSE(result.integral, 1.0, 1E-4);

    /*
     * With a ridiculously small evaluations limit, the integration should fail
     * after hitting the limit. The result will probably be close to zero, but
     * we don't really care.
     */

    /*
     * Override MAX_EVALS in the local settings. Integration should fail when we
     * use local_settings, but still succeed with the defaults:
     */
    local_settings.Override_Max_Evals(3);
    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        }, local_settings);
    BOOST_REQUIRE(!result.successful);
    BOOST_REQUIRE(result.evaluations >= 3);
    BOOST_CHECK(isnan(result.integral));

    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        });
    BOOST_REQUIRE(result.successful);
    BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations < 100);
    BOOST_CHECK_CLOSE(result.integral, 1.0, 1E-4);

    /*
     * Cancel the override; local_settings should succeed:
     */
    local_settings.Use_Default_Max_Evals();
    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        }, local_settings);
    BOOST_REQUIRE(result.successful);
    BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations < 100);
    BOOST_CHECK_CLOSE(result.integral, 1.0, 1E-4);

    /*
     * Set the default MAX_EVALS to 3; both should fail:
     */
    IntegrationSettings::Set_Max_Evals(3);
    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        });
    BOOST_REQUIRE(!result.successful);
    BOOST_REQUIRE(result.evaluations >= 3);
    BOOST_CHECK(isnan(result.integral));

    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        }, local_settings);
    BOOST_REQUIRE(!result.successful);
    BOOST_REQUIRE(result.evaluations >= 3);
    BOOST_CHECK(isnan(result.integral));

    /*
     * Override again; local_settings should succeed, and defaults fail:
     */
    local_settings.Override_Max_Evals(200);
    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        }, local_settings);
    BOOST_REQUIRE(result.successful);
    BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations < 100);
    BOOST_CHECK_CLOSE(result.integral, 1.0, 1E-4);
    
    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        });
    BOOST_REQUIRE(!result.successful);
    BOOST_REQUIRE(result.evaluations >= 3);
    BOOST_CHECK(isnan(result.integral));

    /*
     * Set the default tolerance too tight, and the integration will fail, but 
     * provide a good result anyway:
     */
    IntegrationSettings::Reset();
    IntegrationSettings::Set_Tolerance(2E-13);
    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        });
    BOOST_REQUIRE(!result.successful);
    BOOST_REQUIRE(result.evaluations >= 
                  IntegrationSettings::Get_Global_Settings()->Get_Effective_Max_Evals());
    BOOST_CHECK(isnan(result.integral));

    /*
     * Using the defaults, local settings should also fail:
     */
    local_settings.Use_Default_Tolerance();
    local_settings.Use_Default_Max_Evals();
    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        }, local_settings);
    BOOST_REQUIRE(!result.successful);
    BOOST_REQUIRE(result.evaluations >= 
                  IntegrationSettings::Get_Global_Settings()->Get_Effective_Max_Evals());
    BOOST_CHECK(isnan(result.integral));

    /*
     * Override the local settings, and it should succeed, while the defaults
     * will still fail:
     */
    local_settings.Override_Tolerance(1E-6);
    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        }, local_settings);
    BOOST_REQUIRE(result.successful);
    BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations < 100);
    BOOST_CHECK_CLOSE(result.integral, 1.0, 1E-4);

    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        });
    BOOST_REQUIRE(!result.successful);
    BOOST_REQUIRE(result.evaluations >= 
                  IntegrationSettings::Get_Global_Settings()->Get_Effective_Max_Evals());
    BOOST_CHECK(isnan(result.integral));

    /*
     * Change local_settings to use only 40 evaluations. The integration will
     * fail, but the result is within * 0.1% of 1.0.
     */
    local_settings.Override_Max_Evals(40);
    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        }, local_settings);
    BOOST_REQUIRE(!result.successful);
    BOOST_REQUIRE(result.evaluations >= 40);
    BOOST_CHECK(isnan(result.integral));

    /*
     * If we keep the same limit, but lower the required accuracy, the
     * integration succeeds:
     */
    local_settings.Override_Tolerance(1E-4);
    result = MAQ([] (double x) -> double {
                     return gsl_ran_lognormal_pdf(x, log(1), 1);
        }, local_settings);
    BOOST_REQUIRE(result.successful);
    BOOST_REQUIRE(result.evaluations <= 50);
    BOOST_CHECK_CLOSE(result.integral, 1.0, 0.1);

    /*
     * Given a much higher limit, the integration is successful, and accurate to
     * within 0.0001%.
     */
    local_settings.Override_Tolerance(1E-6);
    local_settings.Override_Max_Evals(1024);
    result = MAQ([] (double x) -> double {
            return gsl_ran_lognormal_pdf(x, log(1), 1);
        }, local_settings);
    BOOST_REQUIRE(result.successful);
    BOOST_REQUIRE(result.evaluations >= 1 && result.evaluations < 100);
    BOOST_CHECK_CLOSE(result.integral, 1.0, 1E-4);


    /*
     * The remaining integrations should all succeed well within the allocated
     * number of evaluations, to the requested accuracy.
     */
    IntegrationSettings::Set_Tolerance(1E-6);
    IntegrationSettings::Set_Max_Evals(1024);
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
