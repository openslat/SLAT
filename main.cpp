#include <iostream>
#include <fstream>
#include <iomanip>
#include "relationships.h"
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
    
    shared_ptr<DeterministicFunction> im_rate_function(
        new NonLinearHyperbolicLaw(1221, 29.8, 62.2));

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

    
    shared_ptr<DeterministicFunction> mu_edp(
        new PowerLawParametricCurve(0.1, 1.5));

    shared_ptr<DeterministicFunction> sigma_edp(
        new PowerLawParametricCurve(0.5, 0.0));

    shared_ptr<ProbabilisticFunction> edp_im_relationship(
        new LogNormalFunction(mu_edp, sigma_edp));

    {
        BOOST_LOG(logger) << "Writing IM-EDP table";
        ofstream outfile("im_edp.dat");

        outfile << setw(10) << "IM" << setw(12) << "EDP16"
                << setw(12) << "EDP50" << setw(12) << "EDP84" << endl;
        outfile << setprecision(6) << fixed;

        for (int i=0; i < 100; i++) {
            double im = i / 100.0;
            outfile << setw(10) << im
                    << setw(12) << edp_im_relationship->X_at_exceedence(im, 0.16)
                    << setw(12) << edp_im_relationship->X_at_exceedence(im, 0.50)
                    << setw(12) << edp_im_relationship->X_at_exceedence(im, 0.84)
                    << endl;
        }
        outfile.close();
        BOOST_LOG(logger) << "IM-EDP table done.";
    }

    CompoundRateRelationship rel(im_rate_rel, edp_im_relationship);
    for (int i=0; i < 5; i++) 
    {
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        BOOST_LOG(logger) << "Writing EDP-RATE table";
        ofstream outfile("edp_rate.dat");
        
        outfile << setw(10) << "EDP" << setw(12) << "RATE" << endl;
        outfile << setprecision(6) << fixed;
        for (int i=1; i < 150; i++) {
            double edp = i / 1000.0;
            outfile << setw(10) << edp << setw(12) << rel.lambda(edp) << endl;
        }
        outfile.close();
        BOOST_LOG(logger) << "EDP-RATE table written.";
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }
    BOOST_LOG(logger) << "Exiting main().";

    std::cout << rel << std::endl;
    shared_ptr<DeterministicFunction> new_im_rate_function(
        new NonLinearHyperbolicLaw(12.21, 29.8, 62.2));
    std::cout << "Replacing im_rate_function" << std::endl;
    im_rate_function->replace(new_im_rate_function);
    std::cout << rel << std::endl;
    for (int i=0; i < 5; i++) 
    {
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        BOOST_LOG(logger) << "Writing EDP-RATE table";
        ofstream outfile("edp_rate.dat");
        
        outfile << setw(10) << "EDP" << setw(12) << "RATE" << endl;
        outfile << setprecision(6) << fixed;
        for (int i=1; i < 150; i++) {
            double edp = i / 1000.0;
            outfile << setw(10) << edp << setw(12) << rel.lambda(edp) << endl;
        }
        outfile.close();
        BOOST_LOG(logger) << "EDP-RATE table written.";
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }

    std::cout << "Replacing new_im_rate_function" << std::endl;
    new_im_rate_function->replace(std::shared_ptr<DeterministicFunction>(new NonLinearHyperbolicLaw(1221, 29.8, 62.2)));

    std::cout << rel << std::endl;
    for (int i=0; i < 5; i++) 
    {
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        BOOST_LOG(logger) << "Writing EDP-RATE table";
        ofstream outfile("edp_rate.dat");
        
        outfile << setw(10) << "EDP" << setw(12) << "RATE" << endl;
        outfile << setprecision(6) << fixed;
        for (int i=1; i < 150; i++) {
            double edp = i / 1000.0;
            outfile << setw(10) << edp << setw(12) << rel.lambda(edp) << endl;
        }
        outfile.close();
        BOOST_LOG(logger) << "EDP-RATE table written.";
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }
}
