#include <iostream>
#include <fstream>
#include <iomanip>
#include "relationships.h"
#include "fragility.h"
#include "lognormaldist.h"
#include "loss_functions.h"
#include "comp_group.h"
#include "structure.h"
#include <chrono>

using namespace std;
using namespace SLAT;

#include <boost/move/utility.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(main_logger, src::logger_mt)
int main(int argc, char **argv)
{
    logging::add_file_log("main.log");
    logging::add_common_attributes();

    src::logger_mt& logger = main_logger::get();
    BOOST_LOG(logger) << "Starting main().";

    cout << "Welcome to SLAT" << endl;
    
    shared_ptr<DeterministicFn> im_rate_function(
        new LogLogInterpolatedFn(
            new double[25]{0.01, 0.02, 0.04, 0.06, 0.08, 
                    0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7,
                    0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4,
                    1.5, 1.6, 1.7, 1.8, 1.9, 2.0},
            new double[25]{0.376775, 0.155158, 0.054048,
                    0.027886, 0.017241, 0.011857, 0.003656,
                    0.001628, 0.000794, 0.000405, 0.000214,
                    0.000115, 6.4e-05, 3.7e-05, 2.1e-05, 
                    1.3e-05, 7e-06, 4e-06, 3e-06, 1.8e-06,
                    1e-06, 7e-07, 4e-07, 2e-07, 1e-07},
            25));

    
    shared_ptr<IM> im_rate_rel(new IM(im_rate_function));
    im_rate_rel->SetCollapse(std::make_shared<Collapse>(0.9, 0.470));
        
    shared_ptr<DeterministicFn> mu_edp1(
        new LinearInterpolatedFn(
            new double[16] { 0, 0.10000000000000001, 0.20000000000000001, 
                    0.29999999999999999, 0.40000000000000002, 0.5,
                    0.59999999999999998, 0.69999999999999996, 
                    0.80000000000000004, 0.90000000000000002, 
                    1.0, 1.1000000000000001, 1.2, 1.3, 
                    1.3999999999999999, 1.5},
            new double[16] {0, 0.15141575000000007, 0.30280000000000001,
                    0.45415, 0.62236111111111092, 0.77791666666666659,
                    0.91506666666666669, 1.069391304347826,
                    1.1753499999999999, 1.286, 1.4607857142857144,
                    1.2986249999999999, 1.3176666666666665,
                    1.4261666666666668, 1.5366666666666668, 1.5049999999999999},
            16));
            
    shared_ptr<DeterministicFn> sigma_edp1(
        new LinearInterpolatedFn(
            new double[16] { 0, 0.10000000000000001, 0.20000000000000001, 
                    0.29999999999999999, 0.40000000000000002, 0.5,
                    0.59999999999999998, 0.69999999999999996, 
                    0.80000000000000004, 0.90000000000000002, 
                    1.0, 1.1000000000000001, 1.2, 1.3, 
                    1.3999999999999999, 1.5},
            new double[16] {0, 0.067882331898944534, 0.13574168530044534, 
                    0.20367817123073703, 0.28013814477634091, 0.34989650510645898,
                    0.44142496010341198, 0.49079830509546896, 0.51822757396828345,
                    0.54302510684743366, 0.58711133508068103, 0.36751013105569136,
                    0.3287166966654822, 0.35331595869231081, 0.38202966725984339, 
                    0.41388404173149757},
            16));
    
    shared_ptr<ProbabilisticFn> edp1_im_relationship(
        new LogNormalFn(mu_edp1, LogNormalFn::MEAN_X, sigma_edp1, LogNormalFn::SIGMA_X));
    
    shared_ptr<DeterministicFn> mu_edp2(
        new LinearInterpolatedFn(
            new double[16]{0, 0.10000000000000001, 0.20000000000000001,
                    0.29999999999999999, 0.40000000000000002, 0.5, 
                    0.59999999999999998, 0.69999999999999996, 0.80000000000000004,
                    0.90000000000000002, 1.0, 1.1000000000000001, 1.2,
                    1.3, 1.3999999999999999, 1.5},
            new double[16] {0, 0.00084679749999999989, 0.0014862699999999996,
                    0.0026105500000000001, 0.0059848611111111109, 0.013708138888888893,
                    0.015993233333333339, 0.024313347826086951, 0.02825335, 
                    0.023721124999999999, 0.024528999999999999, 0.025899999999999999,
                    0.023528166666666666, 0.020886666666666668, 0.021164666666666665,
                    0.01535125},
            16));
    
    shared_ptr<DeterministicFn> sigma_edp2(
        new LinearInterpolatedFn(
            new double[16]{0, 0.10000000000000001, 0.20000000000000001,
                    0.29999999999999999, 0.40000000000000002, 0.5, 
                    0.59999999999999998, 0.69999999999999996, 0.80000000000000004,
                    0.90000000000000002, 1.0, 1.1000000000000001, 1.2,
                    1.3, 1.3999999999999999, 1.5},
            new double[16]{0, 0.00015757605162229377, 0.00042328555627831928,
                    0.0036251483219877025, 0.010133130045697424, 0.016833661956504681,
                    0.020216175421375229, 0.021347106937493674, 0.021141003163657811,
                    0.020106843617286128, 0.016895805068805596, 0.011155229395092817,
                    0.012579126367386038, 0.016765019184798644, 0.021479041614249614,
                    0.014549052051021515},
            16));


    shared_ptr<ProbabilisticFn> edp2_im_relationship(
        new LogNormalFn(mu_edp2, LogNormalFn::MEAN_X, sigma_edp2, LogNormalFn::SIGMA_X));

    std::shared_ptr<EDP> edp1 = std::make_shared<EDP>(im_rate_rel, edp1_im_relationship);
    std::shared_ptr<EDP> edp2 = std::make_shared<EDP>(im_rate_rel, edp2_im_relationship);

    std::vector<LogNormalDist> fragility_distributions(
        { LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.0062, 0.4),
                LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.0230, 0.4),
                LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.0440, 0.4),
                LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.0564, 0.4)});
    std::shared_ptr<FragilityFn> fragFn = std::make_shared<FragilityFn>(fragility_distributions);

    std::shared_ptr<LossFn> lossFn = std::make_shared<LossFn>(
        std::vector<LogNormalDist>({ LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.03, 0.4),
                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.08, 0.4),
                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.25, 0.4),
                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(1.00, 0.4)}));

    std::shared_ptr<CompGroup> cgroup1 = std::make_shared<CompGroup>(edp1, fragFn, lossFn, 1);
    std::shared_ptr<CompGroup> cgroup2 = std::make_shared<CompGroup>(edp2, fragFn, lossFn, 1);

    Structure structure;
    structure.AddCompGroup(cgroup1);
    structure.AddCompGroup(cgroup2);
    structure.setRebuildCost(LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(14E6, 0.35));

    std::vector<double> im_vals = [0.0, 0.001, 0.01, 0.1, 0.5, 1.0, 1.5, 2.0];
    for_each(im_vals.begin(), im_vals.end(), [] (double im) {
            LogNormalDist loss = structure.Loss(im, false);
            std::cout << "Structure Loss: " << loss.get_mu_lnX() << ", " << loss.get_sigma_lnX() << std::endl;

            LogNormalDist loss = structure.Loss(im, true);
            std::cout << "Structure Loss: " << loss.get_mu_lnX() << ", " << loss.get_sigma_lnX() << std::endl;
    }
}
