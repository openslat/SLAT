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
#include "relationships.h"
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace SLAT;

BOOST_AUTO_TEST_CASE(comp_group_edp_test)
{
    shared_ptr<DeterministicFn> im_rate_function(
        new NonLinearHyperbolicLaw(1221, 29.8, 62.2));
    shared_ptr<IM> im_rate_rel(
        new IM(im_rate_function));

    shared_ptr<DeterministicFn> mu_edp(
        new PowerLawParametricCurve(0.1, 1.5));
    shared_ptr<DeterministicFn> sigma_edp(
        new PowerLawParametricCurve(0.5, 0.0));
    shared_ptr<ProbabilisticFn> edp_im_relationship(
        new LogNormalFn(mu_edp, LogNormalFn::MEAN_X, sigma_edp, LogNormalFn::SIGMA_LN_X));

    std::shared_ptr<EDP> rel(new EDP(im_rate_rel, edp_im_relationship));

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


    // {
    //     im_rate_function->replace(std::make_shared<NonLinearHyperbolicLaw>(2 * 1221, 29.8, 62.2));
    //     mu_edp->replace(std::make_shared<PowerLawParametricCurve>(0.2, 1.5));
    // }
    //Caching::Clear_Caches();
    
    for (size_t i=0; i < sizeof(test_data)/sizeof(test_data[0]); i++) {
        BOOST_CHECK_CLOSE(component_group.E_cost_EDP(test_data[i].edp), 
                          test_data[i].mu_loss,
                          0.1);
       BOOST_CHECK_CLOSE(component_group.SD_ln_cost_EDP(test_data[i].edp), 
                          test_data[i].sd_loss,
                          0.5);
    }

    const struct { double im, mu_loss, sd_loss; } im_test_data[] = {
    { 1.0000E-03, 0.000, 0.000 }, { 2.6242E-02, 3.1234E-07, 3.411 }, 
    { 5.1485E-02, 1.1151E-04, 2.399 }, { 7.6727E-02, 1.2258E-03, 1.836 },    
    { 0.1020, 4.2789E-03, 1.470 }, { 0.1272, 8.9319E-03, 1.247 }, 
    { 0.1525, 1.4380E-02, 1.163 }, { 0.1777, 2.0246E-02, 1.191 },    
    { 0.2029, 2.6711E-02, 1.264 }, { 0.2282, 3.4360E-02, 1.331 }, 
    { 0.2534, 4.3815E-02, 1.373 }, { 0.2787, 5.5687E-02, 1.390 },    
    { 0.3039, 7.0450E-02, 1.384 }, { 0.3292, 8.8370E-02, 1.363 },
    { 0.3544, 0.1095, 1.331 }, { 0.3796, 0.1338, 1.293 },    
    { 0.4049, 0.1611, 1.251 }, { 0.4301, 0.1908, 1.208 }, { 0.4554, 0.2227, 1.164 }, { 0.4806, 0.2563, 1.120 },    
    { 0.5058, 0.2912, 1.078 }, { 0.5311, 0.3268, 1.037 }, { 0.5563, 0.3627, 0.9971 }, { 0.5816, 0.3985, 0.9593 },    
    { 0.6068, 0.4340, 0.9232 }, { 0.6321, 0.4688, 0.8890 }, { 0.6573, 0.5027, 0.8566 }, { 0.6825, 0.5355, 0.8259 },    
    { 0.7078, 0.5669, 0.7970 }, { 0.7330, 0.5971, 0.7696 }, { 0.7583, 0.6258, 0.7439 }, { 0.7835, 0.6529, 0.7197 },    
    { 0.8088, 0.6786, 0.6969 }, { 0.8340, 0.7028, 0.6755 }, { 0.8592, 0.7254, 0.6555 }, { 0.8845, 0.7466, 0.6368 },    
    { 0.9097, 0.7664, 0.6193 }, { 0.9350, 0.7848, 0.6029 }, { 0.9602, 0.8019, 0.5876 }, { 0.9855, 0.8179, 0.5733 },    
    { 1.011, 0.8326, 0.5600 }, { 1.036, 0.8462, 0.5477 }, { 1.061, 0.8587, 0.5362 }, { 1.086, 0.8703, 0.5256 },    
    { 1.112, 0.8810, 0.5157 }, { 1.137, 0.8909, 0.5065 }, { 1.162, 0.8999, 0.4980 }, { 1.187, 0.9082, 0.4902 },    
    { 1.213, 0.9159, 0.4829 }, { 1.238, 0.9229, 0.4762 }, { 1.263, 0.9294, 0.4701 }, { 1.288, 0.9350, 0.4645 },    
    { 1.314, 0.9404, 0.4592 }, { 1.339, 0.9454, 0.4544 }, { 1.364, 0.9500, 0.4500 }, { 1.389, 0.9541, 0.4459 },    
    { 1.415, 0.9580, 0.4421 }, { 1.440, 0.9615, 0.4386 }, { 1.465, 0.9647, 0.4355 }, { 1.490, 0.9679, 0.4324 },    
    { 1.516, 0.9706, 0.4298 }, { 1.541, 0.9730, 0.4273 }, { 1.566, 0.9753, 0.4251 }, { 1.591, 0.9773, 0.4230 },    
    { 1.617, 0.9792, 0.4211 }, { 1.642, 0.9808, 0.4194 }, { 1.667, 0.9824, 0.4178 }, { 1.692, 0.9839, 0.4163 },    
    { 1.717, 0.9852, 0.4150 }, { 1.743, 0.9861, 0.4138 }, { 1.768, 0.9872, 0.4127 }, { 1.793, 0.9882, 0.4117 },    
    { 1.818, 0.9895, 0.4107 }, { 1.844, 0.9904, 0.4094 }, { 1.869, 0.9911, 0.4090 }, { 1.894, 0.9918, 0.4083 },    
    { 1.919, 0.9924, 0.4076 }, { 1.945, 0.9930, 0.4070 }, { 1.970, 0.9936, 0.4065 }, { 1.995, 0.9941, 0.4060 },    
    { 2.020, 0.9946, 0.4055 }, { 2.046, 0.9950, 0.4051 }, { 2.071, 0.9954, 0.4047 }, { 2.096, 0.9957, 0.4043 },    
    { 2.121, 0.9961, 0.4039 }, { 2.147, 0.9964, 0.4036 }, { 2.172, 0.9967, 0.4033 }, { 2.197, 0.9969, 0.4030 },    
    { 2.222, 0.9972, 0.4028 }, { 2.248, 0.9974, 0.4026 }, { 2.273, 0.9976, 0.4024 }, { 2.298, 0.9978, 0.4022 },    
    { 2.323, 0.9979, 0.4021 }, { 2.349, 0.9981, 0.4019 }, { 2.374, 0.9982, 0.4018 }, { 2.399, 0.9984, 0.4016 },    
    { 2.424, 0.9985, 0.4015 }, { 2.450, 0.9986, 0.4014 }, { 2.475, 0.9987, 0.4013 }, { 2.500, 0.9988, 0.4012 }    
    };    

    //component_group = CompGroup(rel, fragFn, lossFn, 1);
    // Skip first entry, which returns NAN instead of 0, since the integrator can't find anything interesting to look at
    for (size_t i=0; i < sizeof(test_data)/sizeof(test_data[0]); i++) {
        BOOST_CHECK_CLOSE(component_group.E_cost_IM(im_test_data[i].im), 
                          im_test_data[i].mu_loss,
                          0.2);
        BOOST_CHECK_CLOSE(component_group.SD_ln_cost_IM(im_test_data[i].im), 
                          im_test_data[i].sd_loss,
                          0.5);
    }


    //@ TODO: Tests where number of components > 1
    struct { double t, e_annual, e_yearly; } annual_loss_test_data[] = {
        { 1.000, 6.5623E-04, 6.5623E-04 }, { 2.000, 1.2742E-03, 6.3712E-04 }, { 3.000, 1.8563E-03, 6.1876E-04 }, 
        { 4.000, 2.4044E-03, 6.0110E-04 }, { 5.000, 2.9206E-03, 5.8412E-04 }, { 6.000, 3.4067E-03, 5.6779E-04 },
        { 7.000, 3.8646E-03, 5.5208E-04 }, { 8.000, 4.2958E-03, 5.3697E-04 }, { 9.000, 4.7018E-03, 5.2242E-04 },
        { 10.00, 5.0842E-03, 5.0842E-04 }, { 11.00, 5.4444E-03, 4.9494E-04 }, { 12.00, 5.7836E-03, 4.8196E-04 },
        { 13.00, 6.1030E-03, 4.6946E-04 }, { 14.00, 6.4038E-03, 4.5741E-04 }, { 15.00, 6.6871E-03, 4.4581E-04 },
        { 16.00, 6.9539E-03, 4.3462E-04 }, { 17.00, 7.2052E-03, 4.2383E-04 }, { 18.00, 7.4418E-03, 4.1343E-04 },
        { 19.00, 7.6647E-03, 4.0340E-04 }, { 20.00, 7.8745E-03, 3.9373E-04 }, { 21.00, 8.0722E-03, 3.8439E-04 },
        { 22.00, 8.2583E-03, 3.7538E-04 }, { 23.00, 8.4336E-03, 3.6668E-04 }, { 24.00, 8.5987E-03, 3.5828E-04 },
        { 25.00, 8.7542E-03, 3.5017E-04 }, { 26.00, 8.9006E-03, 3.4233E-04 }, { 27.00, 9.0385E-03, 3.3476E-04 },
        { 28.00, 9.1684E-03, 3.2744E-04 }, { 29.00, 9.2907E-03, 3.2037E-04 }, { 30.00, 9.4059E-03, 3.1353E-04 },
        { 31.00, 9.5143E-03, 3.0691E-04 }, { 32.00, 9.6165E-03, 3.0052E-04 }, { 33.00, 9.7127E-03, 2.9432E-04 },
        { 34.00, 9.8033E-03, 2.8833E-04 }, { 35.00, 9.8886E-03, 2.8253E-04 }, { 36.00, 9.9690E-03, 2.7692E-04 },
        { 37.00, 1.0045E-02, 2.7148E-04 }, { 38.00, 1.0116E-02, 2.6621E-04 }, { 39.00, 1.0183E-02, 2.6110E-04 },
        { 40.00, 1.0246E-02, 2.5616E-04 }, { 41.00, 1.0306E-02, 2.5136E-04 }, { 42.00, 1.0362E-02, 2.4671E-04 },
        { 43.00, 1.0415E-02, 2.4220E-04 }, { 44.00, 1.0464E-02, 2.3783E-04 }, { 45.00, 1.0511E-02, 2.3358E-04 },
        { 46.00, 1.0555E-02, 2.2946E-04 }, { 47.00, 1.0597E-02, 2.2547E-04 }, { 48.00, 1.0636E-02, 2.2158E-04 },
        { 49.00, 1.0673E-02, 2.1781E-04 }, { 50.00, 1.0708E-02, 2.1415E-04 }, { 51.00, 1.0740E-02, 2.1059E-04 },
        { 52.00, 1.0771E-02, 2.0713E-04 }, { 53.00, 1.0800E-02, 2.0377E-04 }, { 54.00, 1.0827E-02, 2.0050E-04 },
        { 55.00, 1.0853E-02, 1.9733E-04 }, { 56.00, 1.0877E-02, 1.9423E-04 }, { 57.00, 1.0900E-02, 1.9123E-04 },
        { 58.00, 1.0921E-02, 1.8830E-04 }, { 59.00, 1.0942E-02, 1.8545E-04 }, { 60.00, 1.0961E-02, 1.8268E-04 },
        { 61.00, 1.0979E-02, 1.7998E-04 }, { 62.00, 1.0995E-02, 1.7735E-04 }, { 63.00, 1.1011E-02, 1.7478E-04 },
        { 64.00, 1.1026E-02, 1.7229E-04 }, { 65.00, 1.1040E-02, 1.6985E-04 }, { 66.00, 1.1054E-02, 1.6748E-04 },
        { 67.00, 1.1066E-02, 1.6517E-04 }, { 68.00, 1.1078E-02, 1.6291E-04 }, { 69.00, 1.1089E-02, 1.6071E-04 },
        { 70.00, 1.1100E-02, 1.5857E-04 }, { 71.00, 1.1109E-02, 1.5647E-04 }, { 72.00, 1.1119E-02, 1.5443E-04 },
        { 73.00, 1.1127E-02, 1.5243E-04 }, { 74.00, 1.1136E-02, 1.5048E-04 }, { 75.00, 1.1143E-02, 1.4858E-04 },
        { 76.00, 1.1151E-02, 1.4672E-04 }, { 77.00, 1.1158E-02, 1.4490E-04 }, { 78.00, 1.1164E-02, 1.4313E-04 },
        { 79.00, 1.1170E-02, 1.4139E-04 }, { 80.00, 1.1176E-02, 1.3970E-04 }, { 81.00, 1.1181E-02, 1.3804E-04 },
        { 82.00, 1.1186E-02, 1.3642E-04 }, { 83.00, 1.1191E-02, 1.3483E-04 }, { 84.00, 1.1196E-02, 1.3328E-04 },
        { 85.00, 1.1200E-02, 1.3176E-04 }, { 86.00, 1.1204E-02, 1.3028E-04 }, { 87.00, 1.1208E-02, 1.2882E-04 },
        { 88.00, 1.1211E-02, 1.2740E-04 }, { 89.00, 1.1215E-02, 1.2601E-04 }, { 90.00, 1.1218E-02, 1.2464E-04 },
        { 91.00, 1.1221E-02, 1.2330E-04 }, { 92.00, 1.1223E-02, 1.2199E-04 }, { 93.00, 1.1226E-02, 1.2071E-04 },
        { 94.00, 1.1229E-02, 1.1945E-04 }, { 95.00, 1.1231E-02, 1.1822E-04 }, { 96.00, 1.1233E-02, 1.1701E-04 },
        { 97.00, 1.1235E-02, 1.1583E-04 }, { 98.00, 1.1237E-02, 1.1466E-04 }, { 99.00, 1.1239E-02, 1.1352E-04 },
        { 100.0, 1.1241E-02, 1.1241E-04 },     
    };
    for (size_t i=0; i < sizeof(annual_loss_test_data)/sizeof(annual_loss_test_data[0]); i++) {
        double years = annual_loss_test_data[i].t;
        double annual_loss = component_group.E_cost(years, 0.06);
        BOOST_CHECK_CLOSE(annual_loss, annual_loss_test_data[i].e_annual, 0.2);
        BOOST_CHECK_CLOSE(annual_loss/years, annual_loss_test_data[i].e_yearly, 0.2);
    }

    struct {double loss, rate; } loss_rate_test_data[] = {
        { 1.0000E-04, 4.7032E-02 }, { 1.2089E-04, 4.5311E-02 }, { 1.4616E-04, 4.3607E-02 }, { 1.7669E-04, 4.1920E-02 }, 
        { 2.1361E-04, 4.0249E-02 }, { 2.5825E-04, 3.8593E-02 }, { 3.1221E-04, 3.6950E-02 }, { 3.7745E-04, 3.5347E-02 }, 
        { 4.5631E-04, 3.3703E-02 }, { 5.5166E-04, 3.2095E-02 }, { 6.6693E-04, 3.0496E-02 }, { 8.0628E-04, 2.8903E-02 }, 
        { 9.7475E-04, 2.7315E-02 }, { 1.1784E-03, 2.5730E-02 }, { 1.4246E-03, 2.4149E-02 }, { 1.7223E-03, 2.2572E-02 }, 
        { 2.0822E-03, 2.0976E-02 }, { 2.5173E-03, 1.9434E-02 }, { 3.0433E-03, 1.7883E-02 }, { 3.6791E-03, 1.6353E-02 }, 
        { 4.4479E-03, 1.4853E-02 }, { 5.3773E-03, 1.3394E-02 }, { 6.5008E-03, 1.1987E-02 }, { 7.8592E-03, 1.0643E-02 }, 
        { 9.5013E-03, 9.3747E-03 }, { 1.1487E-02, 8.1887E-03 }, { 1.3887E-02, 7.0982E-03 }, { 1.6788E-02, 6.1064E-03 }, 
        { 2.0296E-02, 5.2145E-03 }, { 2.4537E-02, 4.4236E-03 }, { 2.9664E-02, 3.7300E-03 }, { 3.5862E-02, 3.1292E-03 }, 
        { 4.3356E-02, 2.6140E-03 }, { 5.2415E-02, 2.1758E-03 }, { 6.3367E-02, 1.8063E-03 }, { 7.6607E-02, 1.4962E-03 }, 
        { 9.2614E-02, 1.2369E-03 }, { 0.1120, 1.0205E-03 }, { 0.1354, 8.3996E-04 }, { 0.1636, 6.8915E-04 }, 
        { 0.1978, 5.6286E-04 }, { 0.2392, 4.5683E-04 }, { 0.2892, 3.6776E-04 }, { 0.3496, 2.9251E-04 }, 
        { 0.4226, 2.2919E-04 }, { 0.5109, 1.7614E-04 }, { 0.6177, 1.3213E-04 }, { 0.7467, 9.6224E-05 }, 
        { 0.9028, 6.7716E-05 }, { 1.091, 4.5935E-05 },
    };

    for (size_t i=0; i < sizeof(loss_rate_test_data)/sizeof(loss_rate_test_data[0]); i++) {
        BOOST_CHECK_CLOSE(loss_rate_test_data[i].rate, component_group.lambda_cost(loss_rate_test_data[i].loss), 0.2);
    }
}


