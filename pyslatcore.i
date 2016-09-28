%module pyslatcore
%{
#include "pyslatcore.h"
%}
%include <std_string.i>
%include <std_vector.i>
%include <std_unordered_map.i>
namespace std{
    %template(VectorDouble) std::vector<double>;
    %template(VectorLogNormal_p) std::vector<LogNormalDist *>;
    %template(VectorLogNormal) std::vector<LogNormalDist>;
//    %template(MapEDPComp) std::unordered_map<int, int >;
//    %template(MapEDPComp) std::unordered_map<EDP *, std::vector<CompGroup *>>;
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

class LossFn {
public:
    LossFn(std::shared_ptr<SLAT::LossFn> function);
    int n_states();
};

LossFn *MakeLossFn(std::vector<LogNormalDist *> distributions);

class CompGroup {
public:
    CompGroup(std::shared_ptr<SLAT::CompGroup> group);
    double E_Loss_EDP(double edp);
    double SD_ln_loss_EDP(double edp);
    double E_Loss_IM(double edp);
    double SD_ln_loss_IM(double edp);
    double E_annual_loss(void);
    double E_loss(int years, double discount_rate);
    std::vector<double> pDS_IM(double im);
    std::vector<double> Rate(void);
    double lambda_loss(double loss);
    bool AreSame(const CompGroup &other);
private:
    std::shared_ptr<SLAT::CompGroup> wrapper;
        
    friend class Structure;
};

CompGroup *MakeCompGroup(EDP edp, FragilityFn frag_fn, LossFn loss_fn, int count);

class Structure {
public:
    Structure(std::shared_ptr<SLAT::Structure> structure);
    void AddCompGroup(CompGroup cg);
    LogNormalDist Loss(double im, bool consider_collapse);
    LogNormalDist TotalLoss(double im);
    std::vector<LogNormalDist> DeaggregatedLoss(double im);
    std::vector<LogNormalDist> LossesByFate(double im);
    void setRebuildCost(LogNormalDist cost);
    LogNormalDist getRebuildCost(void);
    void setDemolitionCost(LogNormalDist cost);
    LogNormalDist getDemolitionCost(void);
    LogNormalDist AnnualLoss(void);
    //std::unordered_map<EDP *, std::vector<CompGroup *>> ComponentsByEDP(void);
    //python::list ComponentsByFragility(void);
private:
    std::shared_ptr<SLAT::Structure> wrapper;
};

Structure *MakeStructure();
