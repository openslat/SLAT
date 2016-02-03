/**
 * @file   comp_group_test.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief Unit tests for classes declared in comp_group.h.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */

#include "comp_group.h"
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace SLAT;

BOOST_AUTO_TEST_CASE(comp_group_test)
{
    shared_ptr<DeterministicFn> im_rate_function(
        new NonLinearHyperbolicLaw(1221, 29.8, 62.2));
    shared_ptr<RateRelationship> im_rate_rel(
        new SimpleRateRelationship(im_rate_function));

    shared_ptr<DeterministicFn> mu_edp(
        new PowerLawParametricCurve(0.1, 1.5));
    shared_ptr<DeterministicFn> sigma_edp(
        new PowerLawParametricCurve(0.5, 0.0));
    shared_ptr<ProbabilisticFn> edp_im_relationship(
        new LogNormalFn(mu_edp, LogNormalFn::MEAN_X, sigma_edp, LogNormalFn::SIGMA_LN_X));

    std::shared_ptr<CompoundRateRelationship> rel(new CompoundRateRelationship(im_rate_rel, edp_im_relationship));

    std::shared_ptr<FragilityFn> fragFn(new FragilityFn(
                                            { LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.0062, 0.4),
                                                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.0230, 0.4),
                                                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.0440, 0.4),
                                                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.0564, 0.4)}));
    std::shared_ptr<LossFn> lossFn(new LossFn(
                                       { LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.03, 0.4),
                                               LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.08, 0.4),
                                               LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.25, 0.4),
                                               LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(1.00, 0.4)}));
    
    CompGroup  component_group(rel, fragFn, lossFn, 1);

    const struct { double edp, mu_loss, sd_loss; } test_data[] = {
        { 1.0000E-03, 1.9387E-07, 3.480 }, { 2.5051E-03, 5.8299E-04, 2.025 }, 
        { 4.0101E-03, 5.6081E-03, 1.355 }, { 5.5152E-03, 1.3912E-02, 0.9655 },
        { 7.0202E-03, 2.0998E-02, 0.7317 }, { 8.5253E-03, 2.5787E-02, 0.6118 }, 
        { 1.0030E-02, 2.9164E-02, 0.5897 }, { 1.1535E-02, 3.2186E-02, 0.6446 }, 
        { 1.3040E-02, 3.5567E-02, 0.7437 }, { 1.4545E-02, 3.9722E-02, 0.8559 }, 
        { 1.6051E-02, 4.4906E-02, 0.9608 }, { 1.7556E-02, 5.1312E-02, 1.048 }, { 1.9061E-02, 5.9124E-02, 1.115 }, 
        { 2.0566E-02, 6.8530E-02, 1.162 }, { 2.2071E-02, 7.9705E-02, 1.192 }, { 2.3576E-02, 9.2797E-02, 1.206 }, 
        { 2.5081E-02, 0.1079, 1.209 }, { 2.6586E-02, 0.1251, 1.202 }, { 2.8091E-02, 0.1443, 1.187 }, 
        { 2.9596E-02, 0.1655, 1.168 }, { 3.1101E-02, 0.1885, 1.145 }, { 3.2606E-02, 0.2132, 1.119 }, 
        { 3.4111E-02, 0.2393, 1.091 }, { 3.5616E-02, 0.2667, 1.062 }, { 3.7121E-02, 0.2950, 1.033 }, 
        { 3.8626E-02, 0.3239, 1.003 }, { 4.0131E-02, 0.3534, 0.9744 }, { 4.1636E-02, 0.3830, 0.9457 }, 
        { 4.3141E-02, 0.4127, 0.9176 }, { 4.4646E-02, 0.4421, 0.8904 }, { 4.6152E-02, 0.4712, 0.8640 }, 
        { 4.7657E-02, 0.4997, 0.8385 }, { 4.9162E-02, 0.5275, 0.8139 }, { 5.0667E-02, 0.5546, 0.7903 }, 
        { 5.2172E-02, 0.5808, 0.7678 }, { 5.3677E-02, 0.6060, 0.7462 }, { 5.5182E-02, 0.6302, 0.7255 }, 
        { 5.6687E-02, 0.6534, 0.7059 }, { 5.8192E-02, 0.6755, 0.6872 }, { 5.9697E-02, 0.6966, 0.6694 }, 
        { 6.1202E-02, 0.7166, 0.6525 }, { 6.2707E-02, 0.7355, 0.6365 }, { 6.4212E-02, 0.7534, 0.6214 }, 
        { 6.5717E-02, 0.7703, 0.6071 }, { 6.7222E-02, 0.7861, 0.5935 }, { 6.8727E-02, 0.8011, 0.5808 }, 
        { 7.0232E-02, 0.8151, 0.5687 }, { 7.1737E-02, 0.8282, 0.5574 }, { 7.3242E-02, 0.8404, 0.5467 }, 
        { 7.4747E-02, 0.8519, 0.5367 }, { 7.6253E-02, 0.8626, 0.5273 }, { 7.7758E-02, 0.8726, 0.5185 }, 
        { 7.9263E-02, 0.8819, 0.5102 }, { 8.0768E-02, 0.8906, 0.5024 }, { 8.2273E-02, 0.8986, 0.4952 }, 
        { 8.3778E-02, 0.9061, 0.4884 }, { 8.5283E-02, 0.9131, 0.4821 }, { 8.6788E-02, 0.9196, 0.4762 }, 
        { 8.8293E-02, 0.9255, 0.4707 }, { 8.9798E-02, 0.9311, 0.4656 }, { 9.1303E-02, 0.9363, 0.4608 }, 
        { 9.2808E-02, 0.9410, 0.4564 }, { 9.4313E-02, 0.9455, 0.4522 }, { 9.5818E-02, 0.9496, 0.4484 }, 
        { 9.7323E-02, 0.9534, 0.4448 }, { 9.8828E-02, 0.9569, 0.4415 }, { 0.1003, 0.9601, 0.4384 }, 
        { 0.1018, 0.9631, 0.4356 }, { 0.1033, 0.9659, 0.4329 }, { 0.1048, 0.9685, 0.4305 }, 
        { 0.1064, 0.9708, 0.4282 }, { 0.1079, 0.9730, 0.4261 }, { 0.1094, 0.9750, 0.4242 }, 
        { 0.1109, 0.9769, 0.4224 }, { 0.1124, 0.9787, 0.4207 }, { 0.1139, 0.9803, 0.4192 }, 
        { 0.1154, 0.9817, 0.4177 }, { 0.1169, 0.9831, 0.4164 }, { 0.1184, 0.9844, 0.4152 }, 
        { 0.1199, 0.9855, 0.4140 }, { 0.1214, 0.9866, 0.4130 }, { 0.1229, 0.9876, 0.4120 }, 
        { 0.1244, 0.9885, 0.4111 }, { 0.1259, 0.9894, 0.4103 }, { 0.1274, 0.9902, 0.4095 }, 
        { 0.1289, 0.9909, 0.4088 }, { 0.1304, 0.9916, 0.4082 }, { 0.1319, 0.9922, 0.4076 }, 
        { 0.1334, 0.9928, 0.4070 }, { 0.1349, 0.9933, 0.4065 }, { 0.1365, 0.9938, 0.4060 }, 
        { 0.1380, 0.9942, 0.4056 }, { 0.1395, 0.9947, 0.4052 }, { 0.1410, 0.9951, 0.4048 }, 
        { 0.1425, 0.9954, 0.4044 }, { 0.1440, 0.9957, 0.4041 }, { 0.1455, 0.9961, 0.4038 }, 
        { 0.1470, 0.9963, 0.4035 }, { 0.1485, 0.9966, 0.4033 }, { 0.1500, 0.9968, 0.4031 }, 
    };

    for (size_t i=0; i < sizeof(test_data)/sizeof(test_data[0]); i++) {
        BOOST_CHECK_CLOSE(component_group.E_loss_EDP(test_data[i].edp), 
                          test_data[i].mu_loss,
                          0.1);
       BOOST_CHECK_CLOSE(component_group.SD_ln_loss_EDP(test_data[i].edp), 
                          test_data[i].sd_loss,
                          0.5);
    }
}

