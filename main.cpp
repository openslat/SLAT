#include <iostream>
#include <fstream>
#include <iomanip>
#include "relationships.h"
#include "fragility.h"
#include "lognormaldist.h"
#include "loss_functions.h"
#include "comp_group.h"
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
    
    wrapped_NonLinearHyperbolicLaw im_rate_function(1221, 29.8, 62.2);

    shared_ptr<RateRelationship> im_rate_rel(
        new SimpleRateRelationship(im_rate_function));
    {
        BOOST_LOG(logger) << "Writing IM-RATE table";
        ofstream outfile("im_rate.dat");
        outfile << setw(10) << "IM" << setw(12) << "RATE" << endl;
        outfile << setprecision(6) << fixed;
        for (int i=1; i < 100; i++) {
            double im = i / 100.0;
            outfile << setw(10) << im << setw(12) << im_rate_rel->lambda(im) << endl;
        }
        outfile.close();
        BOOST_LOG(logger) << "IM-RATE table written";
    }

    
    wrapped_PowerLawParametricCurve mu_edp(0.1, 1.5);
    wrapped_PowerLawParametricCurve sigma_edp(0.5, 0.0);

    wrapped_LogNormalFn edp_im_relationship(mu_edp, wrapped_LogNormalFn::MEAN_X, sigma_edp, wrapped_LogNormalFn::SIGMA_LN_X);

    {
        BOOST_LOG(logger) << "Writing IM-EDP table";
        ofstream outfile("im_edp.dat");

        outfile << setw(10) << "IM" << setw(12) << "EDP16"
                << setw(12) << "EDP50" << setw(12) << "EDP84" << endl;
        outfile << setprecision(6) << fixed;

        for (int i=0; i < 100; i++) {
            double im = i / 100.0;
            outfile << setw(10) << im
                    << setw(12) << edp_im_relationship.X_at_exceedence(im, 0.16)
                    << setw(12) << edp_im_relationship.Mean(im)
                    << setw(12) << edp_im_relationship.X_at_exceedence(im, 0.84)
                    << endl;
        }
        outfile.close();
        BOOST_LOG(logger) << "IM-EDP table done.";
    }

    std::shared_ptr<CompoundRateRelationship> rel(new CompoundRateRelationship(im_rate_rel, edp_im_relationship));
    for (int i=0; i < 5; i++) 
    {
        //std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        BOOST_LOG(logger) << "Writing EDP-RATE table";
        ofstream outfile("edp_rate.dat");
        
        outfile << setw(10) << "EDP" << setw(12) << "RATE" << endl;
        outfile << setprecision(6) << fixed;
        for (int i=1; i < 150; i++) {
            double edp = i / 1000.0;
            outfile << setw(10) << edp << setw(12) << rel->lambda(edp) << endl;
        }
        outfile.close();
        BOOST_LOG(logger) << "EDP-RATE table written.";
        //std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        //std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }


    std::cout << *rel << std::endl;
    wrapped_NonLinearHyperbolicLaw new_im_rate_function(12.21, 29.8, 62.2);
    std::cout << "Replacing im_rate_function" << std::endl;
    im_rate_function.replace(new_im_rate_function);
    std::cout << *rel << std::endl;
    for (int i=0; i < 5; i++) 
    {
        //std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        BOOST_LOG(logger) << "Writing EDP-RATE table";
        ofstream outfile("edp_rate.dat");
        
        outfile << setw(10) << "EDP" << setw(12) << "RATE" << endl;
        outfile << setprecision(6) << fixed;
        for (int i=1; i < 150; i++) {
            double edp = i / 1000.0;
            outfile << setw(10) << edp << setw(12) << rel->lambda(edp) << endl;
        }
        outfile.close();
        BOOST_LOG(logger) << "EDP-RATE table written.";
        //std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        //std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }

    std::cout << "Replacing new_im_rate_function" << std::endl;
    new_im_rate_function.replace(wrapped_NonLinearHyperbolicLaw(1221, 29.8, 62.2));

    std::cout << *rel << std::endl;
    for (int i=0; i < 5; i++) 
    {
        //std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        BOOST_LOG(logger) << "Writing EDP-RATE table";
        ofstream outfile("edp_rate.dat");
        
        outfile << setw(10) << "EDP" << setw(12) << "RATE" << endl;
        outfile << setprecision(6) << fixed;
        for (int i=1; i < 150; i++) {
            double edp = i / 1000.0;
            outfile << setw(10) << edp << setw(12) << rel->lambda(edp) << endl;
        }
        outfile.close();
        BOOST_LOG(logger) << "EDP-RATE table written.";
        //std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        //std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }


    std::shared_ptr<FragilityFn> fragFn(new FragilityFn(
        { LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.0062, 0.4),
                LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.0230, 0.4),
                LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.0440, 0.4),
                LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.0564, 0.4)}));
    {
        BOOST_LOG(logger) << "Writing DS-EDP table";
        ofstream outfile("ds_edp.dat");
        
        outfile << setw(10) << "EDP";
        for (int i=0; i < 4; i++) {
            outfile << setw(11) << "DS" << setw(1) << i;
        }
        outfile << endl;
        outfile << setprecision(6) << fixed;
        for (int i=0; i < 200; i++) {
            double edp = i / 1000.0;
            outfile << setw(10) << edp;

            std::vector<double> damage = fragFn->pExceeded(edp);
            for (int j=0; j<4; j++) {
                outfile << setw(12) << damage[j];
            }
            outfile << endl;            
        }
        outfile.close();
        BOOST_LOG(logger) << "DS-DSP table written.";
    }

    std::shared_ptr<LossFn> lossFn(new LossFn(
        { LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.03, 0.4),
                LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.08, 0.4),
                LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.25, 0.4),
                LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(1.00, 0.4)}));
    
    {
        CompGroup  component_group(rel, fragFn, lossFn, 1);
        ofstream outfile("loss_edp.dat");
    
        outfile << setw(10) << "EDP" << setw(12) << "Loss"
                << setw(12) << "SD(ln)" << endl;

        for (int i=1; i < 200; i++) {
            double edp = i / 1000.;
            component_group.E_loss_EDP(edp);

            outfile << setw(10) << edp << setw(12) << component_group.E_loss_EDP(edp)
                    << setw(12) << component_group.SD_ln_loss_EDP(edp) << endl;
        }
        outfile.close();
        BOOST_LOG(logger) << "LOSS-EDP table written." << endl;
    }
}
