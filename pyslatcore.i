%module pyslatcore
%{
#include "pyslatcore.h"
%}
%include <std_vector.i>
%include <std_map.i>
namespace std{
    %template(VectorDouble) std::vector<double>;
}


void Init_Caching(void);
void IntegrationSettings(double tolerance, unsigned int max_evals);

enum FUNCTION_TYPE { NLH, PLC, LIN, LOGLOG };
enum LOGNORMAL_MU_TYPE { MEAN_X, MEDIAN_X, MEAN_LN_X };
enum LOGNORMAL_SIGMA_TYPE { SD_X, SD_LN_X };

class DeterministicFn {
public:
    double ValueAt(double v);
};

DeterministicFn *factory(FUNCTION_TYPE t, std::vector<double>x, std::vector<double> y);
DeterministicFn *factory(FUNCTION_TYPE t, std::vector<double> params);

ProbabilisticFn *MakeLogNormalProbabilisticFn(const DeterministicFn &mu_function,
                                                  LOGNORMAL_MU_TYPE mu_type,
                                                  const DeterministicFn &sigma_function,
                                                  LOGNORMAL_SIGMA_TYPE sigma_type);

class LogNormalDist {
public:
    double p_at_least(double x) const;
    double p_at_most(double x) const;
    double x_at_p(double p) const;
    double get_mu_lnX(void) const;
    double get_median_X(void) const;
    double get_mean_X(void) const;
    double get_sigma_lnX(void) const;
    double get_sigma_X(void) const;
};
