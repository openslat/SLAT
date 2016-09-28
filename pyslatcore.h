#include <vector>
#include "functions.h"
#include "relationships.h"
#include "fragility.h"
#include "loss_functions.h"

void Init_Caching(void);
void IntegrationSettings(double tolerance, unsigned int max_evals);


enum FUNCTION_TYPE { NLH=0, PLC, LIN, LOGLOG };
enum LOGNORMAL_MU_TYPE { MEAN_X, MEDIAN_X, MEAN_LN_X };
enum LOGNORMAL_SIGMA_TYPE { SD_X, SD_LN_X };

class IM;
class EDP;
class FragilityFn;
class LossFn;

class DeterministicFn {
public:
    DeterministicFn();
    DeterministicFn(std::shared_ptr<SLAT::DeterministicFn> f);
    double ValueAt(double v);
    std::shared_ptr<SLAT::DeterministicFn> function;
};

DeterministicFn *factory(FUNCTION_TYPE t, std::vector<double>x, std::vector<double> y);
DeterministicFn *factory(FUNCTION_TYPE t, std::vector<double> params);

class ProbabilisticFn {
public:
    ProbabilisticFn();
    ProbabilisticFn(std::shared_ptr<SLAT::ProbabilisticFn> f);
    double P_exceedence(double x, double min_y);
    double X_at_exceedence(double x, double p);
    double Mean(double x);
    double MeanLn(double x);
    double Median(double x);
    double SD_ln(double x);
    double SD(double x);
private:
    std::shared_ptr<SLAT::ProbabilisticFn> function;
    friend EDP *MakeEDP(IM base_wrate, ProbabilisticFn dependent_rate, std::string name);

};

ProbabilisticFn *MakeLogNormalProbabilisticFn(const DeterministicFn &mu_function,
                                              LOGNORMAL_MU_TYPE mu_type,
                                              const DeterministicFn &sigma_function,
                                              LOGNORMAL_SIGMA_TYPE sigma_type);


class LogNormalDist {
public:
    LogNormalDist(std::shared_ptr<SLAT::LogNormalDist> dist);
    LogNormalDist() {};
    double p_at_least(double x) const;
    double p_at_most(double x) const;
    double x_at_p(double p) const;
    double get_mu_lnX(void) const;
    double get_median_X(void) const;
    double get_mean_X(void) const;
    double get_sigma_lnX(void) const;
    double get_sigma_X(void) const;
    
private:
    std::shared_ptr<SLAT::LogNormalDist> dist;
    friend FragilityFn *MakeFragilityFn(std::vector<LogNormalDist *> distributions);
    friend LossFn *MakeLossFn(std::vector<LogNormalDist *> distributions);
    // friend class Structure;
    friend class IM;
    friend LogNormalDist AddDistributions(std::vector<LogNormalDist> dists);
};

LogNormalDist *MakeLogNormalDist(double mu, LOGNORMAL_MU_TYPE mu_type,
                                 double sigma, LOGNORMAL_SIGMA_TYPE sigma_type);



class IM {
public:
    IM();
    IM(std::shared_ptr<SLAT::IM> r);
    double get_lambda(double x);
    void SetCollapse(LogNormalDist c);
    double pCollapse(double im);
    double CollapseRate(void);
    void SetDemolition(LogNormalDist d);
    double pDemolition(double im);
    double DemolitionRate(void);
    double pRepair(double im);
public:
    std::shared_ptr<SLAT::IM> relationship;
        
};

IM *MakeIM(DeterministicFn f);

class EDP {
public:
    EDP();
    EDP(std::shared_ptr<SLAT::EDP> r);
    double get_lambda(double x);
    double P_exceedence(double x, double y);
    double Mean(double x);
    double MeanLn(double x);
    double Median(double x);
    double SD_ln(double x);
    double SD(double x);
    std::string get_Name(void);
    bool AreSame(const EDP &other);
public:
    std::shared_ptr<SLAT::EDP> relationship;
    // friend CompGroup *MakeCompGroup(EDP edp, FragilityFn frag_fn,
    //                                        LossFn loss_fn, int count);
};

EDP *MakeEDP(IM base_wrate, ProbabilisticFn dependent_rate, std::string name);

class FragilityFn {
public:
    FragilityFn(std::shared_ptr<SLAT::FragilityFn> function);
    std::vector<double> pExceeded(double edp);
    std::vector<double> pHighest(double edp);
    int n_states();
    bool AreSame(const FragilityFn &other);
private:
    std::shared_ptr<SLAT::FragilityFn> fragility;
    // friend CompGroup *MakeCompGroup(EDP edp, FragilityFn frag_fn,
    //                                        LossFn loss_fn, int count);
};

FragilityFn *MakeFragilityFn(std::vector<LogNormalDist *> distributions);

class LossFn {
public:
    LossFn(std::shared_ptr<SLAT::LossFn> function);
    int n_states();
private:
    std::shared_ptr<SLAT::LossFn> loss;
    
    // friend CompGroup *MakeCompGroup(EDP edp, FragilityFn frag_fn,
    //                                 LossFn loss_fn, int count);
};