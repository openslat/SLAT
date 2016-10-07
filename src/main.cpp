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
    Caching::Init_Caching();

    src::logger_mt& logger = main_logger::get();
    BOOST_LOG(logger) << "Starting main().";

    cout << "Welcome to SLAT" << endl;
    
    shared_ptr<DeterministicFn> im_rate_function(
        new NonLinearHyperbolicLaw(1221, 29.8, 62.2));

    shared_ptr<IM> im_rate_rel(new IM(im_rate_function));
    {
        BOOST_LOG(logger) << "Writing IM-RATE table";
        ofstream outfile("im_rate.dat");
        outfile << setw(10) << "IM" << setw(15) << "RATE" << endl;
        outfile << setprecision(6) << fixed;
        for (int i=1; i < 100; i++) {
            double im = i / 100.0;
            outfile << setw(10) << im << setw(15) << im_rate_rel->lambda(im) << endl;
        }
        outfile.close();
        BOOST_LOG(logger) << "IM-RATE table written";
    }

    shared_ptr<DeterministicFn> mu_edp(
        new PowerLawParametricCurve(0.1, 1.5));

    shared_ptr<DeterministicFn> sigma_edp(
        new PowerLawParametricCurve(0.5, 0.0));

    shared_ptr<ProbabilisticFn> edp_im_relationship(
        new LogNormalFn(mu_edp, LogNormalFn::MEAN_X, sigma_edp, LogNormalFn::SIGMA_LN_X));

    {
        BOOST_LOG(logger) << "Writing IM-EDP table";
        ofstream outfile("im_edp.dat");

        outfile << setw(10) << "IM" << setw(15) << "EDP16"
                << setw(15) << "EDP50" << setw(15) << "EDP84" << endl;
        outfile << setprecision(6) << fixed;

        for (int i=0; i < 100; i++) {
            double im = i / 100.0;
            outfile << setw(10) << im
                    << setw(15) << edp_im_relationship->X_at_exceedence(im, 0.16)
                    << setw(15) << edp_im_relationship->Mean(im)
                    << setw(15) << edp_im_relationship->X_at_exceedence(im, 0.84)
                    << endl;
        }
        outfile.close();
        BOOST_LOG(logger) << "IM-EDP table done.";
    }

    std::shared_ptr<EDP> rel = std::make_shared<EDP>(im_rate_rel, edp_im_relationship);
    for (int i=0; i < 5; i++) 
    {
        //std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        BOOST_LOG(logger) << "Writing EDP-RATE table";
        ofstream outfile("edp_rate.dat");
        
        outfile << setw(10) << "EDP" << setw(15) << "RATE" << endl;
        outfile << setprecision(6) << fixed;
        for (int i=1; i < 150; i++) {
            double edp = i / 1000.0;
            outfile << setw(10) << edp << setw(15) << rel->lambda(edp) << endl;
        }
        outfile.close();
        BOOST_LOG(logger) << "EDP-RATE table written.";
        //std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        //std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }

    std::cout << *rel << std::endl;
    shared_ptr<DeterministicFn> new_im_rate_function = std::make_shared<NonLinearHyperbolicLaw>(12.21, 29.8, 62.2);
    std::cout << "Replacing im_rate_function" << std::endl;
    im_rate_function->replace(new_im_rate_function);
    im_rate_rel->SetCollapse(LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.9, 0.470));

    std::cout << "Rate of Collapse: " << im_rate_rel->CollapseRate() << std::endl;
    std::cout << *rel << std::endl;
    for (int i=0; i < 5; i++) 
    {
        //std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        BOOST_LOG(logger) << "Writing EDP-RATE table";
        ofstream outfile("edp_rate.dat");
        
        outfile << setw(10) << "EDP" << setw(15) << "RATE" << endl;
        outfile << setprecision(6) << fixed;
        for (int i=1; i < 150; i++) {
            double edp = i / 1000.0;
            outfile << setw(10) << edp << setw(15) << rel->lambda(edp) << endl;
        }
        outfile.close();
        BOOST_LOG(logger) << "EDP-RATE table written.";
        //std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        //std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }

    std::cout << "Replacing new_im_rate_function" << std::endl;
    new_im_rate_function->replace(std::make_shared<NonLinearHyperbolicLaw>(1221, 29.8, 62.2));

    std::cout << *rel << std::endl;
    for (int i=0; i < 5; i++) 
    {
        //std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        BOOST_LOG(logger) << "Writing EDP-RATE table";
        ofstream outfile("edp_rate.dat");
        
        outfile << setw(10) << "EDP" << setw(15) << "RATE" << endl;
        outfile << setprecision(6) << fixed;
        for (int i=1; i < 150; i++) {
            double edp = i / 1000.0;
            outfile << setw(10) << edp << setw(15) << rel->lambda(edp) << endl;
        }
        outfile.close();
        BOOST_LOG(logger) << "EDP-RATE table written.";
        //std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        //std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }

    std::vector<LogNormalDist> fragility_distributions({ LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.0062, 0.4),
                LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.0230, 0.4),
                LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.0440, 0.4),
                LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.0564, 0.4)});
    std::shared_ptr<FragilityFn> fragFn = std::make_shared<FragilityFn>(fragility_distributions);
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
                outfile << setw(15) << damage[j];
            }
            outfile << endl;            
        }
        outfile.close();
        BOOST_LOG(logger) << "DS-EDP table written.";
    }

    std::shared_ptr<LossFn> lossFn = std::make_shared<LossFn>(
        std::vector<LogNormalDist>({ LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.03, 0.4),
                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.08, 0.4),
                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(0.25, 0.4),
                    LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(1.00, 0.4)}));

    {
        std::shared_ptr<CompGroup> component_group = std::make_shared<CompGroup>(rel, fragFn, lossFn, 1);
        ofstream outfile("loss_edp.dat");
    
        outfile << setw(10) << "EDP" << setw(15) << "Loss"
                << setw(15) << "SD(ln)" << endl;

        for (int i=1; i < 200; i++) {
            double edp = i / 1000.;
            double loss = component_group->E_loss_EDP(edp);

            outfile << setw(10) << edp << setw(15) << loss
                    << setw(15) << component_group->SD_ln_loss_EDP(edp) << endl;
        }
        outfile.close();
        BOOST_LOG(logger) << "LOSS-EDP table written." << endl;

        outfile.open("loss_im.dat");
        outfile << setw(10) << "IM" << setw(15) << "Loss"
                << setw(15) << "SD(ln)" << endl;
        for (int i=0; i < 250; i++) {
            double im = (i + 1)/ 100.;
            outfile << setw(10) << im << setw(15) << component_group->E_loss_IM(im)
                    << setw(15) << component_group->SD_ln_loss_IM(im) << endl;
        }
        outfile.close();
        BOOST_LOG(logger) << "LOSS-IM table written." << endl;

        std::cout << "Expected Annual Loss: " << component_group->E_annual_loss() << std::endl;
        
        outfile.open("annual_loss.dat");
        outfile << setw(10) << "Year" << setw(15) << "Loss" << endl;
        for (int year=0; year <= 100; year++) {
            outfile << setw(10) << year << setw(15) << component_group->E_loss(year, 0.06) << endl;
        }
        outfile.close();
        BOOST_LOG(logger) << "annual loss table written." << endl;

        outfile.open("loss_rate.dat");
        outfile << setw(10) << "Loss" << setw(15) << "Rate" << std::endl;
        for (int i=0; i < 250; i++) {
            double loss = 1E-4 + i * (1.2 - 1E-4) / 250;
            outfile << setw(10) << loss << setw(15) << component_group->lambda_loss(loss) << std::endl;
        }
        outfile.close();
        BOOST_LOG(logger) << "LOSS-RATE table written." << endl;


        Structure structure("structure");
        structure.AddCompGroup(component_group);
        structure.setRebuildCost(LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(14E6, 0.35));
        {
            LogNormalDist loss = structure.Loss(0, false);
            std::cout << "Structure Loss: " << loss.get_mu_lnX() << ", " << loss.get_sigma_lnX() << std::endl;
        }
        {
            LogNormalDist loss = structure.Loss(0, true);
            std::cout << "Structure Loss: " << loss.get_mu_lnX() << ", " << loss.get_sigma_lnX() << std::endl;
        }
        {
            LogNormalDist loss = structure.Loss(1, false);
            std::cout << "Structure Loss: " << loss.get_mu_lnX() << ", " << loss.get_sigma_lnX() << std::endl;
        }
        {
            LogNormalDist loss = structure.Loss(1, true);
            std::cout << "Structure Loss: " << loss.get_mu_lnX() << ", " << loss.get_sigma_lnX() << std::endl;
        }
    }
}
