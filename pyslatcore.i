%module pyslatcore
%{
#include "pyslatcore.h"
%}
%include <std_string.i>
%include <std_vector.i>
%include <std_map.i>
namespace std{
    %template(VectorDouble) std::vector<double>;
    %template(VectorLogNormal) std::vector<LogNormalDist *>;
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

LogNormalDist * MakeLogNormalDist(double mu, LOGNORMAL_MU_TYPE mu_type,
                                  double sigma, LOGNORMAL_SIGMA_TYPE sigma_type);

class IM {
public:
    IM();
    double get_lambda(double x);
    void SetCollapse(LogNormalDist c);
    double pCollapse(double im);
    double CollapseRate(void);
    void SetDemolition(LogNormalDist d);
    double pDemolition(double im);
    double DemolitionRate(void);
    double pRepair(double im);
        
};

IM *MakeIM(DeterministicFn f);

class EDP {
public:
    EDP();
    double get_lambda(double x);
    double P_exceedence(double x, double y);
    double Mean(double x);
    double MeanLn(double x);
    double Median(double x);
    double SD_ln(double x);
    double SD(double x);
    std::string get_Name(void);
    bool AreSame(const EDP &other);
};

EDP *MakeEDP(IM base_wrate, ProbabilisticFn dependent_rate, std::string name);

class FragilityFn {
public:
    FragilityFn(std::shared_ptr<SLAT::FragilityFn> function);
    std::vector<double> pExceeded(double edp);
    std::vector<double> pHighest(double edp);
    int n_states();
    bool AreSame(const FragilityFn &other);
};

FragilityFn *MakeFragilityFn(std::vector<LogNormalDist *> distributions);
