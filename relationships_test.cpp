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
#include "relationships.h"
#include "maq.h"
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace SLAT;

BOOST_AUTO_TEST_CASE( Simple_Rate_Relationship_Test )
{
    shared_ptr<DeterministicFunction> im_rate_function(
        new NonLinearHyperbolicLaw(1221, 29.8, 62.2));
    
    SimpleRateRelationship im_rate_rel(im_rate_function);

    for (int i=0; i < 100; i++) {
        double x = double(i)/10.0;

        double expected = im_rate_function->ValueAt(x);
        double actual = im_rate_rel.lambda(x);

        BOOST_REQUIRE(actual == expected);
        {
            BOOST_CHECK_CLOSE(im_rate_function->DerivativeAt(x),
                              im_rate_rel.DerivativeAt(x),
                              0.01);
        }
    }
}

BOOST_AUTO_TEST_CASE( Compound_Rate_Relationship_Integration_Params_Test )
{
    shared_ptr<DeterministicFunction> im_rate_function(
        new PowerLawParametricCurve(1.6E-4, -2.74));

    shared_ptr<RateRelationship> im_rate_rel(
        new SimpleRateRelationship(im_rate_function));
    
    shared_ptr<DeterministicFunction> mu_edp(
        new PowerLawParametricCurve(0.1, 1.5));

    shared_ptr<DeterministicFunction> sigma_edp(
        new PowerLawParametricCurve(0.5, 0.0));

    shared_ptr<ProbabilisticFunction> edp_im_relationship(
        new LogNormalFunction(mu_edp, sigma_edp));

    CompoundRateRelationship rel1(im_rate_rel, edp_im_relationship);
    CompoundRateRelationship rel2(im_rate_rel, edp_im_relationship);

    {
        /*
         * Make sure the class and object defaults are correct:
         */
        Integration::IntegrationSettings &class_settings = 
            RateRelationship::Get_Class_Integration_Settings();

        BOOST_REQUIRE_EQUAL(class_settings.Get_Effective_Tolerance(), 1E-6);
        BOOST_REQUIRE_EQUAL(class_settings.Get_Effective_Max_Evals(), 1024);
        
        Integration::IntegrationSettings &rel1_settings = 
            rel1.Get_Integration_Settings();
        BOOST_REQUIRE_EQUAL(rel1_settings.Get_Effective_Tolerance(), 1E-6);
        BOOST_REQUIRE_EQUAL(rel1_settings.Get_Effective_Max_Evals(), 1024);
            
        Integration::IntegrationSettings &rel2_settings = 
            rel2.Get_Integration_Settings();
        BOOST_REQUIRE_EQUAL(rel2_settings.Get_Effective_Tolerance(), 1E-6);
        BOOST_REQUIRE_EQUAL(rel2_settings.Get_Effective_Max_Evals(), 1024);

        /*
         * Change the class defaults; objects see changes:
         */
        {
            class_settings.Override_Tolerance(1E-5);
            BOOST_REQUIRE_EQUAL(class_settings.Get_Effective_Tolerance(), 1E-5);
            BOOST_REQUIRE_EQUAL(rel1_settings.Get_Effective_Tolerance(), 1E-5);
            BOOST_REQUIRE_EQUAL(rel2_settings.Get_Effective_Tolerance(), 1E-5);

            class_settings.Override_Max_Evals(2048);
            BOOST_REQUIRE_EQUAL(class_settings.Get_Effective_Max_Evals(), 2048);
            BOOST_REQUIRE_EQUAL(rel1_settings.Get_Effective_Max_Evals(), 2048);
            BOOST_REQUIRE_EQUAL(rel2_settings.Get_Effective_Max_Evals(), 2048);
        }

        /*
         * Override an object; other object and class not affected:
         */
        {
            rel1_settings.Override_Tolerance(1E-4);
            BOOST_REQUIRE_EQUAL(rel1_settings.Get_Effective_Tolerance(), 1E-4);
            BOOST_REQUIRE_EQUAL(class_settings.Get_Effective_Tolerance(), 1E-5);
            BOOST_REQUIRE_EQUAL(rel2_settings.Get_Effective_Tolerance(), 1E-5);

            rel2_settings.Override_Max_Evals(512);
            BOOST_REQUIRE_EQUAL(rel2_settings.Get_Effective_Max_Evals(), 512);
            BOOST_REQUIRE_EQUAL(class_settings.Get_Effective_Max_Evals(), 2048);
            BOOST_REQUIRE_EQUAL(rel1_settings.Get_Effective_Max_Evals(), 2048);
        }



/**
 * Test that integration parameters are used correctly in lambda(). If the
 * integration fails to converge to the required accuracy within the allotted
 * number of evaluations, it will return NAN.
 */

        /*
         * Current settings should produce a valid result:
         */
        BOOST_REQUIRE(!std::isnan(rel2.lambda(0.01)));
        
        /*
         * Too few evaluations--fails:
         */
        rel2_settings.Override_Tolerance(1E-6);
        rel2_settings.Override_Max_Evals(5);
        BOOST_REQUIRE(std::isnan(rel2.lambda(0.01)));

        /*
         * Plenty of evaluations--succeeds:
         */
        rel2_settings.Override_Max_Evals(4096);
        BOOST_REQUIRE(!std::isnan(rel2.lambda(0.01)));

        /*
         * Demand ridiculous precision; fail:
         */
        rel2_settings.Override_Tolerance(1E-30);
        BOOST_REQUIRE(std::isnan(rel2.lambda(0.01)));
        
        /*
         * Use the class default; succeed:
         */
        rel2_settings.Use_Default_Tolerance();
        rel2_settings.Use_Default_Max_Evals();
        BOOST_REQUIRE(!std::isnan(rel2.lambda(0.01)));

        /*
         * Restore class defaults before returning 
         */
        class_settings.Use_Default_Tolerance();
        class_settings.Use_Default_Max_Evals();
    }
}

BOOST_AUTO_TEST_CASE( Compound_Rate_Relationship_Test )
{
    shared_ptr<DeterministicFunction> im_rate_function(
        new PowerLawParametricCurve(1.6E-4, -2.74));

    shared_ptr<RateRelationship> im_rate_rel(
        new SimpleRateRelationship(im_rate_function));
    
    shared_ptr<DeterministicFunction> mu_edp(
        new PowerLawParametricCurve(0.1, 1.5));

    shared_ptr<DeterministicFunction> sigma_edp(
        new PowerLawParametricCurve(0.5, 0.0));

    shared_ptr<ProbabilisticFunction> edp_im_relationship(
        new LogNormalFunction(mu_edp, sigma_edp));

    CompoundRateRelationship rel(im_rate_rel, edp_im_relationship);

    for (int i=1; i < 150; i++) {
        double x = i / 1000.0;

        double ko=1.6E-4, k=2.74, a=0.1, b=1.5, beta=0.5;

        double expected = ko * pow(x/a, -k/b)*exp(0.5 * k*k/(b*b)*beta*beta);
        BOOST_CHECK_CLOSE(rel.lambda(x), expected, 0.1);
    }

    {
        std::shared_ptr<CompoundRateRelationship> rel_ptr(
            new CompoundRateRelationship(im_rate_rel, edp_im_relationship));

        for (int i=1; i < 150; i++) {
            double x = i / 1000.0;

            double ko=1.6E-4, k=2.74, a=0.1, b=1.5, beta=0.5;

            double expected = ko * pow(x/a, -k/b)*exp(0.5 * k*k/(b*b)*beta*beta);
            BOOST_CHECK_CLOSE(rel_ptr->lambda(x), expected, 0.1);
        }
    }

    {
        std::shared_ptr<RateRelationship> rel_ptr(
            new CompoundRateRelationship(im_rate_rel, edp_im_relationship));

        for (int i=1; i < 150; i++) {
            double x = i / 1000.0;

            double ko=1.6E-4, k=2.74, a=0.1, b=1.5, beta=0.5;

            double expected = ko * pow(x/a, -k/b)*exp(0.5 * k*k/(b*b)*beta*beta);
            BOOST_REQUIRE_CLOSE(rel_ptr->lambda(x), expected, 0.1);
        }
    }
    {
        RateRelationship *rel_ptr = new CompoundRateRelationship(im_rate_rel, edp_im_relationship);

        for (int i=1; i < 150; i++) {
            double x = i / 1000.0;

            double ko=1.6E-4, k=2.74, a=0.1, b=1.5, beta=0.5;

            double expected = ko * pow(x/a, -k/b)*exp(0.5 * k*k/(b*b)*beta*beta);
            BOOST_CHECK_CLOSE(rel_ptr->lambda(x), expected, 0.1);
        }
    }
}

