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
        double def_tol, tol;
        unsigned int def_max_evals, max_evals;
        
        /*
         * Make sure the class and object defaults are correct:
         */
        {
            CompoundRateRelationship::GetDefaultIntegrationParameters(def_tol, def_max_evals);
            BOOST_REQUIRE_EQUAL(def_tol, Integration::TOLERANCE_UNSPECIFIED);
            BOOST_REQUIRE_EQUAL(def_max_evals, Integration::EVALUATIONS_UNSPECIFIED);

            rel1.GetIntegrationParameters(tol, max_evals);
            BOOST_REQUIRE_EQUAL(tol, Integration::TOLERANCE_UNSPECIFIED);
            BOOST_REQUIRE_EQUAL(max_evals, Integration::EVALUATIONS_UNSPECIFIED);

            rel2.GetIntegrationParameters(tol, max_evals);
            BOOST_REQUIRE_EQUAL(tol, Integration::TOLERANCE_UNSPECIFIED);
            BOOST_REQUIRE_EQUAL(max_evals, Integration::EVALUATIONS_UNSPECIFIED);
        }


        /*
         * Change the class defaults; objects don't change:
         */
        {
            CompoundRateRelationship::SetDefaultIntegrationParameters(1.2, 125);
            double temp_tol;
            unsigned int temp_evals;
            CompoundRateRelationship::GetDefaultIntegrationParameters(temp_tol,
                                                                      temp_evals);
            BOOST_REQUIRE_EQUAL(temp_tol, 1.2);
            BOOST_REQUIRE_EQUAL(temp_evals, 125);
            
            /*
             * Make sure the object defaults are correct:
             */
            rel1.GetIntegrationParameters(tol, max_evals);
            BOOST_REQUIRE_EQUAL(tol, Integration::TOLERANCE_UNSPECIFIED);
            BOOST_REQUIRE_EQUAL(max_evals, Integration::EVALUATIONS_UNSPECIFIED);
            
            rel2.GetIntegrationParameters(tol, max_evals);
            BOOST_REQUIRE_EQUAL(tol, Integration::TOLERANCE_UNSPECIFIED);
            BOOST_REQUIRE_EQUAL(max_evals, Integration::EVALUATIONS_UNSPECIFIED);
        }

        /*
         * Change an object defaults; others don't change:
         */
        {
            double temp_tol;
            unsigned int temp_evals;

            rel1.SetIntegrationParameters(0.7, 500);
            rel1.GetIntegrationParameters(temp_tol, temp_evals);
            BOOST_REQUIRE_EQUAL(temp_tol, 0.7);
            BOOST_REQUIRE_EQUAL(temp_evals, 500);
            
            CompoundRateRelationship::GetDefaultIntegrationParameters(temp_tol,
                                                                      temp_evals);
            BOOST_REQUIRE_EQUAL(temp_tol, 1.2);
            BOOST_REQUIRE_EQUAL(temp_evals, 125);
            
            rel2.GetIntegrationParameters(tol, max_evals);
            BOOST_REQUIRE_EQUAL(tol, Integration::TOLERANCE_UNSPECIFIED);
            BOOST_REQUIRE_EQUAL(max_evals, Integration::EVALUATIONS_UNSPECIFIED);
        }

/**
 * TODO: Test that default parameters are used correctly in lambda().
 */
        BOOST_REQUIRE(!std::isnan(rel2.lambda(0.01)));
        
        rel2.SetIntegrationParameters(1E-6, 5);
        BOOST_REQUIRE(std::isnan(rel2.lambda(0.01)));

        CompoundRateRelationship::SetDefaultIntegrationParameters(1E-6, 5);
        rel2.SetIntegrationParameters(Integration::TOLERANCE_UNSPECIFIED,
                                      Integration::EVALUATIONS_UNSPECIFIED);
        BOOST_REQUIRE(std::isnan(rel2.lambda(0.01)));
        

        CompoundRateRelationship::SetDefaultIntegrationParameters(1E-6, 4096);
        BOOST_REQUIRE(!std::isnan(rel2.lambda(0.01)));
        
        CompoundRateRelationship::SetDefaultIntegrationParameters(
            Integration::TOLERANCE_UNSPECIFIED,
            Integration::EVALUATIONS_UNSPECIFIED);
        BOOST_REQUIRE(!std::isnan(rel2.lambda(0.01)));

        /*
         * Restore class defaults before returning 
         */
        CompoundRateRelationship::SetDefaultIntegrationParameters(def_tol, def_max_evals);
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
}

