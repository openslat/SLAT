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
    /*
     * Test against the IM used in the simplified bridge example, as calculated
     * by the original SLAT program.
     */
    shared_ptr<DeterministicFn> im_rate_function(
        new NonLinearHyperbolicLaw(1221, 29.8, 62.2));

    SimpleRateRelationship im_rate_rel(im_rate_function);

    struct { double im, rate; } test_data[] = {
        { 1.0000E-03, 2.915 }, { 1.0822E-03, 2.782 }, { 1.1712E-03, 2.653 }, { 1.2676E-03, 2.529 },
        { 1.3718E-03, 2.408 }, { 1.4846E-03, 2.291 }, { 1.6067E-03, 2.178 }, { 1.7388E-03, 2.070 },
        { 1.8818E-03, 1.964 }, { 2.0366E-03, 1.863 }, { 2.2041E-03, 1.765 }, { 2.3853E-03, 1.671 },
        { 2.5815E-03, 1.581 }, { 2.7938E-03, 1.494 }, { 3.0235E-03, 1.410 }, { 3.2722E-03, 1.330 },
        { 3.5413E-03, 1.253 }, { 3.8325E-03, 1.179 }, { 4.1477E-03, 1.108 }, { 4.4888E-03, 1.041 },
        { 4.8579E-03, 0.9760 }, { 5.2575E-03, 0.9144 }, { 5.6898E-03, 0.8557 }, { 6.1577E-03, 0.7997 },
        { 6.6641E-03, 0.7464 }, { 7.2122E-03, 0.6958 }, { 7.8053E-03, 0.6477 }, { 8.4472E-03, 0.6021 },
        { 9.1418E-03, 0.5590 }, { 9.8936E-03, 0.5181 }, { 1.0707E-02, 0.4796 }, { 1.1588E-02, 0.4432 },
        { 1.2541E-02, 0.4089 }, { 1.3572E-02, 0.3766 }, { 1.4688E-02, 0.3463 }, { 1.5896E-02, 0.3179 },
        { 1.7203E-02, 0.2913 }, { 1.8618E-02, 0.2664 }, { 2.0149E-02, 0.2431 }, { 2.1806E-02, 0.2215 },
        { 2.3600E-02, 0.2013 }, { 2.5540E-02, 0.1826 }, { 2.7641E-02, 0.1653 }, { 2.9914E-02, 0.1493 },
        { 3.2374E-02, 0.1345 }, { 3.5036E-02, 0.1209 }, { 3.7918E-02, 0.1084 }, { 4.1036E-02, 9.6884E-02 },
        { 4.4411E-02, 8.6390E-02 }, { 4.8063E-02, 7.6816E-02 }, { 5.2015E-02, 6.8103E-02 }, { 5.6293E-02, 6.0196E-02 },
        { 6.0922E-02, 5.3040E-02 }, { 6.5932E-02, 4.6581E-02 }, { 7.1355E-02, 4.0770E-02 }, { 7.7223E-02, 3.5558E-02 },
        { 8.3573E-02, 3.0899E-02 }, { 9.0446E-02, 2.6747E-02 }, { 9.7884E-02, 2.3061E-02 }, { 0.1059, 1.9801E-02 },
        { 0.1146, 1.6928E-02 }, { 0.1241, 1.4407E-02 }, { 0.1343, 1.2203E-02 }, { 0.1453, 1.0286E-02 },
        { 0.1573, 8.6251E-03 }, { 0.1702, 7.1937E-03 }, { 0.1842, 5.9661E-03 }, { 0.1993, 4.9188E-03 },
        { 0.2157, 4.0304E-03 }, { 0.2335, 3.2811E-03 }, { 0.2527, 2.6529E-03 }, { 0.2735, 2.1297E-03 },
        { 0.2960, 1.6968E-03 }, { 0.3203, 1.3413E-03 }, { 0.3466, 1.0514E-03 }, { 0.3751, 8.1701E-04 },
        { 0.4060, 6.2898E-04 }, { 0.4394, 4.7950E-04 }, { 0.4755, 3.6178E-04 }, { 0.5146, 2.6998E-04 },
        { 0.5569, 1.9914E-04 }, { 0.6027, 1.4509E-04 }, { 0.6523, 1.0434E-04 }, { 0.7060, 7.3992E-05 },
        { 0.7640, 5.1700E-05 }, { 0.8268, 3.5558E-05 }, { 0.8948, 2.4046E-05 }, { 0.9684, 1.5971E-05 },
        { 1.048, 1.0404E-05 }, { 1.134, 6.6389E-06 }, { 1.228, 4.1430E-06 }, { 1.328, 2.5242E-06 },
        { 1.438, 1.4987E-06 }, { 1.556, 8.6532E-07 }, { 1.684, 4.8476E-07 }, { 1.822, 2.6281E-07 },
        { 1.972, 1.3749E-07 }, { 2.134, 6.9188E-08 }, { 2.310, 3.3371E-08 }, { 2.500, 1.5364E-08 }
    };

    for (size_t i=0; i < sizeof(test_data)/sizeof(test_data[0]); i++) {
        double rate = test_data[i].rate;
        BOOST_CHECK_CLOSE( rate, im_rate_rel.lambda(test_data[i].im), 0.5);

        /*
         * Do a quick sanity check on the derivative function:
         */
        {
            double deriv = im_rate_rel.DerivativeAt(test_data[i].im);
            double epsilon = 1E-4;
            BOOST_CHECK_CLOSE( rate + deriv * epsilon, 
                               im_rate_rel.lambda(test_data[i].im + epsilon),
                               0.5);
        }
    }

    {
        shared_ptr<DeterministicFn> double_im_rate_function(
            new NonLinearHyperbolicLaw(2 * 1221, 29.8, 62.2));
        im_rate_function->replace(double_im_rate_function);
        im_rate_function = double_im_rate_function;
    }
    for (size_t i=0; i < sizeof(test_data)/sizeof(test_data[0]); i++) {
        double rate = 2.0 * test_data[i].rate;
        BOOST_CHECK_CLOSE( rate, im_rate_rel.lambda(test_data[i].im), 0.5);

        /*
         * Do a quick sanity check on the derivative function:
         */
        {
            double deriv = im_rate_rel.DerivativeAt(test_data[i].im);
            double epsilon = 1E-4;
            BOOST_CHECK_CLOSE( rate + deriv * epsilon, 
                               im_rate_rel.lambda(test_data[i].im + epsilon),
                               0.5);
        }
    }
}

BOOST_AUTO_TEST_CASE( Compound_Rate_Relationship_Integration_Params_Test )
{
    shared_ptr<DeterministicFn> im_rate_function(
        new PowerLawParametricCurve(1.6E-4, -2.74));

    shared_ptr<RateRelationship> im_rate_rel(
        new SimpleRateRelationship(im_rate_function));
    
    shared_ptr<DeterministicFn> mu_edp(
        new PowerLawParametricCurve(0.1, 1.5));

    shared_ptr<DeterministicFn> sigma_edp(
        new PowerLawParametricCurve(0.5, 0.0));

    shared_ptr<ProbabilisticFn> edp_im_relationship(
        new LogNormalFn(mu_edp, LogNormalFn::MEAN_LN_X,  sigma_edp, LogNormalFn::SIGMA_LN_X));

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
        rel2.lambda.ClearCache();
        BOOST_REQUIRE(std::isnan(rel2.lambda(0.01)));

        /*
         * Plenty of evaluations--succeeds:
         */
        rel2_settings.Override_Max_Evals(4096);
        rel2.lambda.ClearCache();
        BOOST_REQUIRE(!std::isnan(rel2.lambda(0.01)));

        /*
         * Demand ridiculous precision; fail:
         */
        rel2_settings.Override_Tolerance(1E-30);
        rel2.lambda.ClearCache();
        BOOST_REQUIRE(std::isnan(rel2.lambda(0.01)));
        
        /*
         * Use the class default; succeed:
         */
        rel2_settings.Use_Default_Tolerance();
        rel2_settings.Use_Default_Max_Evals();
        rel2.lambda.ClearCache();
        BOOST_REQUIRE(!std::isnan(rel2.lambda(0.01)));

        /*
         * Restore class defaults before returning 
         */
        Caching::Clear_Caches();
        class_settings.Use_Default_Tolerance();
        class_settings.Use_Default_Max_Evals();
    }
}

BOOST_AUTO_TEST_CASE( Compound_Rate_Relationship_Test )
{
    shared_ptr<DeterministicFn> im_rate_function = std::make_shared<NonLinearHyperbolicLaw>(1221, 29.8, 62.2);

    shared_ptr<RateRelationship> im_rate_rel = std::make_shared<SimpleRateRelationship>(im_rate_function);
    
    shared_ptr<DeterministicFn> mu_edp = std::make_shared<PowerLawParametricCurve>(0.1, 1.5);

    shared_ptr<DeterministicFn> sigma_edp = std::make_shared<PowerLawParametricCurve>(0.5, 0.0);

    shared_ptr<ProbabilisticFn> edp_im_relationship = 
        std::make_shared<LogNormalFn>(mu_edp, LogNormalFn::MEAN_X, sigma_edp, LogNormalFn::SIGMA_LN_X);

    CompoundRateRelationship rel(im_rate_rel, edp_im_relationship);

    const struct { double edp, rate; } test_data[] = {
        { 1.0000E-03, 7.8804E-02 }, { 1.0519E-03, 7.4990E-02 }, { 1.1065E-03, 7.1321E-02 }, { 1.1640E-03, 6.7801E-02 },
        { 1.2244E-03, 6.4421E-02 }, { 1.2880E-03, 6.1178E-02 }, { 1.3548E-03, 5.8068E-02 }, { 1.4252E-03, 5.5087E-02 },
        { 1.4992E-03, 5.2231E-02 }, { 1.5770E-03, 4.9495E-02 }, { 1.6589E-03, 4.6877E-02 }, { 1.7450E-03, 4.4373E-02 },
        { 1.8356E-03, 4.1978E-02 }, { 1.9309E-03, 3.9688E-02 }, { 2.0311E-03, 3.7502E-02 }, { 2.1365E-03, 3.5416E-02 },
        { 2.2475E-03, 3.3425E-02 }, { 2.3641E-03, 3.1527E-02 }, { 2.4869E-03, 2.9728E-02 }, { 2.6160E-03, 2.7994E-02 },
        { 2.7518E-03, 2.6354E-02 }, { 2.8946E-03, 2.4761E-02 }, { 3.0449E-03, 2.3311E-02 }, { 3.2030E-03, 2.1901E-02 },
        { 3.3693E-03, 2.0563E-02 }, { 3.5442E-03, 1.9293E-02 }, { 3.7282E-03, 1.8088E-02 }, { 3.9217E-03, 1.6947E-02 },
        { 4.1253E-03, 1.5866E-02 }, { 4.3395E-03, 1.4843E-02 }, { 4.5648E-03, 1.3875E-02 }, { 4.8018E-03, 1.2961E-02 },
        { 5.0511E-03, 1.2098E-02 }, { 5.3133E-03, 1.1283E-02 }, { 5.5891E-03, 1.0515E-02 }, { 5.8793E-03, 9.7908E-03 },
        { 6.1845E-03, 9.1079E-03 }, { 6.5056E-03, 8.4670E-03 }, { 6.8433E-03, 7.8642E-03 }, { 7.1986E-03, 7.2979E-03 },
        { 7.5723E-03, 6.7664E-03 }, { 7.9654E-03, 6.2680E-03 }, { 8.3790E-03, 5.8011E-03 }, { 8.8140E-03, 5.3639E-03 },
        { 9.2715E-03, 4.9550E-03 }, { 9.7529E-03, 4.5730E-03 }, { 1.0259E-02, 4.2163E-03 }, { 1.0792E-02, 3.8837E-03 },
        { 1.1352E-02, 3.5737E-03 }, { 1.1941E-02, 3.2809E-03 }, { 1.2561E-02, 3.0166E-03 }, { 1.3213E-02, 2.7671E-03 },
        { 1.3899E-02, 2.5356E-03 }, { 1.4621E-02, 2.3209E-03 }, { 1.5380E-02, 2.1220E-03 }, { 1.6179E-02, 1.9380E-03 },
        { 1.7018E-02, 1.7679E-03 }, { 1.7902E-02, 1.6109E-03 }, { 1.8831E-02, 1.4661E-03 }, { 1.9809E-02, 1.3326E-03 },
        { 2.0837E-02, 1.2099E-03 }, { 2.1919E-02, 1.0971E-03 }, { 2.3057E-02, 9.9351E-04 }, { 2.4254E-02, 8.9857E-04 },
        { 2.5513E-02, 8.1174E-04 }, { 2.6838E-02, 7.3222E-04 }, { 2.8231E-02, 6.5957E-04 }, { 2.9697E-02, 5.9334E-04 },
        { 3.1238E-02, 5.3301E-04 }, { 3.2860E-02, 4.7813E-04 }, { 3.4566E-02, 4.2828E-04 }, { 3.6361E-02, 3.8306E-04 },
        { 3.8248E-02, 3.4210E-04 }, { 4.0234E-02, 3.0505E-04 }, { 4.2323E-02, 2.7160E-04 }, { 4.4520E-02, 2.4143E-04 },
        { 4.6831E-02, 2.1426E-04 }, { 4.9262E-02, 1.8984E-04 }, { 5.1820E-02, 1.6792E-04 }, { 5.4510E-02, 1.4829E-04 },
        { 5.7340E-02, 1.3072E-04 }, { 6.0317E-02, 1.1504E-04 }, { 6.3448E-02, 1.0105E-04 }, { 6.6742E-02, 8.8606E-05 },
        { 7.0207E-02, 7.7550E-05 }, { 7.3852E-02, 6.7704E-05 }, { 7.7686E-02, 5.9070E-05 }, { 8.1719E-02, 5.1404E-05 },
        { 8.5961E-02, 4.4645E-05 }, { 9.0424E-02, 3.8696E-05 }, { 9.5118E-02, 3.3472E-05 }, { 0.1001, 2.8913E-05 },
        { 0.1053, 2.4887E-05 }, { 0.1107, 2.1391E-05 }, { 0.1165, 1.8346E-05 }, { 0.1225, 1.5699E-05 },
        { 0.1289, 1.3404E-05 }, { 0.1356, 1.1419E-05 }, { 0.1426, 9.7043E-06 }, { 0.1500, 8.2273E-06 },
    };
    for (size_t i=0; i < sizeof(test_data)/sizeof(test_data[0]); i++) {
        BOOST_CHECK_CLOSE(rel.lambda(test_data[i].edp), test_data[i].rate, 0.2);
    }

    {
        std::cout << "im_rate_function: " << im_rate_function  << std::endl
                  << "im_rate_rel: " << im_rate_rel << std::endl
                  << "mu_edp: " << mu_edp << std::endl
                  << "sigma_edp: " << sigma_edp << std::endl
                  << "edp_im_relationship: " << edp_im_relationship << std::endl
                  << "rel: " << &rel << std::endl;
    }

    {
        std::shared_ptr<CompoundRateRelationship> rel_ptr(
            new CompoundRateRelationship(im_rate_rel, edp_im_relationship));
        
        for (size_t i=0; i < sizeof(test_data)/sizeof(test_data[0]); i++) {
            BOOST_CHECK_CLOSE(rel_ptr->lambda(test_data[i].edp), test_data[i].rate, 0.2);
        }
        shared_ptr<DeterministicFn> double_im_rate_function = 
            std::make_shared<NonLinearHyperbolicLaw>(1 * 1221, 29.8, 62.2);
        std::cout << "double_im_rate_function: " << double_im_rate_function << std::endl;
        std::cout << im_rate_function << std::endl;
        im_rate_function->replace(double_im_rate_function);
        im_rate_function = double_im_rate_function;
    }

    {
        std::shared_ptr<RateRelationship> rel_ptr = std::make_shared<CompoundRateRelationship>(im_rate_rel, edp_im_relationship);
        std::cout << "*** " << rel_ptr.use_count() << " ***" << std::endl;
        {
            std::shared_ptr<RateRelationship> ptr2 = rel_ptr;
            std::cout << "*** " << rel_ptr.use_count() << ", " << ptr2.use_count() << " ***" << std::endl;
        }
        std::cout << "*** " << rel_ptr.use_count() << " ***" << std::endl;

        for (size_t i=0; i < sizeof(test_data)/sizeof(test_data[0]); i++) {
            BOOST_REQUIRE_CLOSE(rel_ptr->lambda(test_data[i].edp), test_data[i].rate, 0.2);
        }
    }

    {
        RateRelationship *rel_ptr = new CompoundRateRelationship(im_rate_rel, edp_im_relationship);

        for (size_t i=0; i < sizeof(test_data)/sizeof(test_data[0]); i++) {
            BOOST_REQUIRE_CLOSE(rel_ptr->lambda(test_data[i].edp), test_data[i].rate, 0.2);
        }
        delete(rel_ptr);
    }

    std::cout << ">>>> im_rate_function: " << im_rate_function << std::endl;
    {
        shared_ptr<DeterministicFn> new_im_rate_function = std::make_shared<NonLinearHyperbolicLaw>(1221, 29.8, 62.2);

        shared_ptr<RateRelationship> new_im_rate_rel = std::make_shared<SimpleRateRelationship>(new_im_rate_function);
    
        shared_ptr<DeterministicFn> new_mu_edp = std::make_shared<PowerLawParametricCurve>(0.1, 1.5);

        shared_ptr<DeterministicFn> new_sigma_edp = std::make_shared<PowerLawParametricCurve>(0.5, 0.0);

        shared_ptr<ProbabilisticFn> new_edp_im_relationship = 
            std::make_shared<LogNormalFn>(new_mu_edp, LogNormalFn::MEAN_X, new_sigma_edp, LogNormalFn::SIGMA_LN_X);

         CompoundRateRelationship new_rel(new_im_rate_rel, new_edp_im_relationship);
        {
            shared_ptr<DeterministicFn> double_im_rate_function = 
                std::make_shared<NonLinearHyperbolicLaw>(2 * 1221, 29.8, 62.2);
            std::cout << *im_rate_function << ", " << *double_im_rate_function << std::endl;
           im_rate_function->replace(double_im_rate_function);
           im_rate_function = double_im_rate_function;
            std::cout << "***********" << std::endl;
            std::cout << im_rate_function << std::endl;
           std::cout << double_im_rate_function << std::endl;
       }
        std::cout << "-------------" << std::endl;
        std::cout << rel << std::endl;
        std::cout << "-------------" << std::endl;
        Caching::Clear_Caches();
        for (size_t i=0; i < sizeof(test_data)/sizeof(test_data[0]); i++) {
            BOOST_CHECK_CLOSE(rel.lambda(test_data[i].edp), 2.0 * test_data[i].rate, 0.2);
       }
        std::cout << "+++++" << std::endl;
  }
        std::cout << "+++++" << std::endl;
}


