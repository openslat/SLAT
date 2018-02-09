%module pyslatcore
%begin %{
#define SWIG_FILE_WITH_INIT
#include <cmath>
#include "pyslatcore.h"
%}
%include <std_string.i>
%include <std_vector.i>
%include <std_list.i>
%include <std_map.i>
namespace std{
    %template(VectorDouble) std::vector<double>;
    %template(VectorLogNormal_p) std::vector<LogNormalDist *>;
    %template(VectorLogNormal) std::vector<LogNormalDist>;
    %template(ListCompGroup) std::list<CompGroup *>;
    %template(ListCompGroups) std::list<std::list<CompGroup *>>;
    %template(VectorBiLevelLoss) std::vector<BiLevelLoss *>;
 }


void IntegrationSettings(double tolerance, unsigned int max_evals);
void Set_Integration_Tolerance(double tolerance);
void Set_Integration_Eval_Limit(unsigned int limit);
void Set_Integration_Search_Limit(unsigned int limit);
void Set_Integration_Method(INTEGRATION_TYPE type);
void reset_statistics(void);
std::string format_statistics(void);

void Initialise(void);
void SetLogFile(std::string path);
void LogToStdErr(bool flag);

enum FUNCTION_TYPE { NLH, PLC, LIN, LOGLOG };
enum LOGNORMAL_MU_TYPE { MEAN_X, MEDIAN_X, MEAN_LN_X };
enum LOGNORMAL_SIGMA_TYPE { SD_X, SD_LN_X };
enum INTEGRATION_TYPE { BINARY_SUBDIVISION,
                        REVERSE_BINARY_SUBDIVISION,
                        LOW_FIRST_REVERSE_BINARY_SUBDIVISION,
                        SCATTERED,
                        DIRECTED };


class DeterministicFn {
public:
    double ValueAt(double v);
};

DeterministicFn *factory3(FUNCTION_TYPE t, std::vector<double>x, std::vector<double> y);
DeterministicFn *factory2(FUNCTION_TYPE t, std::vector<double> params);

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

LogNormalDist AddDistributions(std::vector<LogNormalDist> dists);
LogNormalDist * MakeLogNormalDist(double mu, LOGNORMAL_MU_TYPE mu_type,
                                  double sigma, LOGNORMAL_SIGMA_TYPE sigma_type);
LogNormalDist * MakeLogNormalDist(void);

class IM {
public:
    IM();
    double get_lambda(double x);
    std::vector<double> get_lambda(std::vector<double> x);
    void SetCollapse(LogNormalDist c);
    double pCollapse(double im);
    std::vector<double> pCollapse(std::vector<double> im);
    double CollapseRate(void);
    void SetDemolition(LogNormalDist d);
    double pDemolition(double im);
    std::vector<double> pDemolition(std::vector<double> im);
    double DemolitionRate(void);
    double pRepair(double im);
    std::vector<double> pRepair(std::vector<double> im);
    void replace(IM *im);
};

IM *MakeIM(DeterministicFn f);

class EDP {
public:
    EDP();
    double get_lambda(double x);
    std::vector<double> get_lambda(std::vector<double> x);
    double P_exceedence(double x, double y);
    double Mean(double x);
    double MeanLn(double x);
    double Median(double x);
    double SD_ln(double x);
    double SD(double x);
    double X_at_exceedence(double im, double p);
    std::vector<double> bulk_Mean(std::vector<double> x);
    std::vector<double> bulk_MeanLn(std::vector<double> x);
    std::vector<double> bulk_Median(std::vector<double> x);
    std::vector<double> bulk_SD_ln(std::vector<double> x);
    std::vector<double> bulk_SD(std::vector<double> x);
    std::string get_Name(void);
    bool AreSame(const EDP &other);
    void replace(EDP *edp);
};

EDP *MakeEDP(IM base_wrate, ProbabilisticFn dependent_rate, std::string name);

class FragilityFn {
public:
    FragilityFn(std::shared_ptr<SLAT::FragilityFn> function);
    std::vector<double> pExceeded(double edp);
    std::vector<double> pHighest(double edp);
    int n_states();
    bool AreSame(const FragilityFn &other);
    void replace(FragilityFn *fragfn);
};

FragilityFn *MakeFragilityFn(std::vector<LogNormalDist *> distributions);

class LossFn {
public:
    LossFn(std::shared_ptr<SLAT::LossFn> function);
    int n_states();
    void replace(LossFn *lossfn);
};

class BiLevelLoss {
public:
    BiLevelLoss(int count_min, int count_max, double cost_at_min, double cost_at_max, double dispersion);
};
BiLevelLoss * MakeBiLevelLoss(int lower_limit,
                              int upper_limit,
                              double cost_at_min,
                              double cost_at_max, 
                              double dispersion);

LossFn *MakeSimpleLossFn(std::vector<LogNormalDist *> distributions);
LossFn *MakeBiLevelLossFn(std::vector<BiLevelLoss *> distributions);

class CompGroup {
public:
    CompGroup(std::shared_ptr<SLAT::CompGroup> group);
    std::string get_Name();
    double E_Cost_EDP(double edp);
    double SD_ln_cost_EDP(double edp);
    double E_Cost_IM(double edp);
    double SD_ln_Cost_IM(double edp);
    double E_annual_cost(void);
    double E_Delay_EDP(double edp);
    double SD_ln_Delay_EDP(double edp);
    double E_Delay_IM(double edp);
    double SD_ln_Delay_IM(double edp);
    std::vector<double> pDS_IM(double im);
    std::vector<double> Rate(void);
    double lambda_cost(double cost);
    bool AreSame(const CompGroup &other);
    EDP *get_EDP(void);
    void replace(CompGroup *cg);
private:
    std::shared_ptr<SLAT::CompGroup> wrapper;
        
    friend class Structure;
};

CompGroup *MakeCompGroup(EDP edp, FragilityFn frag_fn, LossFn cost_fn, LossFn delay_fn, int count, std::string name);

class Structure {
public:
    Structure(std::shared_ptr<SLAT::Structure> structure);
    void AddCompGroup(CompGroup cg);
    void RemoveCompGroup(CompGroup cg);
    LogNormalDist Cost(double im, bool consider_collapse);
    LogNormalDist TotalCost(double im);
    std::vector<LogNormalDist> DeaggregatedCost(double im);
    std::vector<LogNormalDist> CostsByFate(double im);
    double E_cost(int years, double discount_rate);
    void setRebuildCost(LogNormalDist cost);
    LogNormalDist getRebuildCost(void);
    void setDemolitionCost(LogNormalDist cost);
    LogNormalDist getDemolitionCost(void);
    LogNormalDist AnnualCost(void);
    std::list<std::list<CompGroup *>> ComponentsByEDP(void);
    std::list<std::list<CompGroup *>> ComponentsByFragility(void);
    double pdf(double im);
private:
    std::shared_ptr<SLAT::Structure> wrapper;
};

Structure *MakeStructure();

