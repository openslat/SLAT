#include <iostream>
#include <fstream>
#include <iomanip>
#include "relationships.h"

using namespace std;
using namespace SLAT;

int main(int argc, char **argv)
{
    cout << "Welcome to SLAT" << endl;
    
    shared_ptr<DeterministicFunction> im_rate_function(
        new NonLinearHyperbolicLaw(1221, 29.8, 62.2));

    shared_ptr<RateRelationship> im_rate_rel(
        new SimpleRateRelationship(im_rate_function));

    {
        ofstream outfile("im_rate.dat");
        outfile << setw(10) << "IM" << setw(12) << "RATE" << endl;
        outfile << setprecision(6) << fixed;
        for (int i=1; i < 100; i++) {
            double im = i / 100.0;
            outfile << setw(10) << im << setw(12) << im_rate_rel->lambda(im) << endl;
        }
        outfile.close();
    }

    
    shared_ptr<DeterministicFunction> mu_edp(
        new PowerLawParametricCurve(0.1, 1.5));

    shared_ptr<DeterministicFunction> sigma_edp(
        new PowerLawParametricCurve(0.5, 0.0));

    shared_ptr<ProbabilisticFunction> edp_im_relationship(
        new LogNormalFunction(mu_edp, sigma_edp));

    {
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
    }

    CompoundRateRelationship rel(im_rate_rel, edp_im_relationship);
    {
        ofstream outfile("edp_rate.dat");
        
        outfile << setw(10) << "EDP" << setw(12) << "RATE" << endl;
        outfile << setprecision(6) << fixed;
        for (int i=1; i < 150; i++) {
            double edp = i / 1000.0;
            outfile << setw(10) << edp << setw(12) << rel.lambda(edp) << endl;
        }
        outfile.close();
    }
    
}
