#include <vector>
#include <unordered_map>
#include "functions.h"
#include "relationships.h"
#include "fragility.h"
#include "loss_functions.h"
#include "comp_group.h"
#include "structure.h" 


enum FUNCTION_TYPE { NLH=0, PLC, LIN, LOGLOG };
enum LOGNORMAL_MU_TYPE { MEAN_X, MEDIAN_X, MEAN_LN_X };
enum LOGNORMAL_SIGMA_TYPE { SD_X, SD_LN_X };
enum INTEGRATION_TYPE { BINARY_SUBDIVISION,
                        REVERSE_BINARY_SUBDIVISION,
                        LOW_FIRST_REVERSE_BINARY_SUBDIVISION,
                        SCATTERED,
                        DIRECTED };

void IntegrationSettings(double tolerance, unsigned int integration_eval_limit);
void Set_Integration_Tolerance(double tolerance);
void Set_Integration_Eval_Limit(unsigned int limit);
void Set_Integration_Search_Limit(unsigned int limit);
void Set_Integration_Method(INTEGRATION_TYPE type);

class IM;
class EDP;
class FragilityFn;
class LossFn;
class CompGroup;

class DeterministicFn {
public:
    DeterministicFn();
    DeterministicFn(std::shared_ptr<SLAT::DeterministicFn> f);
    double ValueAt(double v);
    std::shared_ptr<SLAT::DeterministicFn> function;
};

DeterministicFn *factory3(FUNCTION_TYPE t, std::vector<double>x, std::vector<double> y);
DeterministicFn *factory2(FUNCTION_TYPE t, std::vector<double> params);

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


class BiLevelLoss {
public:
    BiLevelLoss(int count_min, int count_max, double cost_at_min, double cost_at_max, double dispersion)
    {
        loss = std::make_shared<SLAT::BiLevelLoss>(count_min, count_max, cost_at_min, cost_at_max, dispersion);
    }
    BiLevelLoss(std::shared_ptr<SLAT::BiLevelLoss> loss) {
        this->loss = loss;
    }
    friend LossFn *MakeBiLevelLossFn(std::vector<BiLevelLoss *> distributions);
private:
    std::shared_ptr<SLAT::BiLevelLoss> loss;
};
BiLevelLoss * MakeBiLevelLoss(int lower_limit,
                              int upper_limit,
                              double cost_at_min,
                              double cost_at_max, 
                              double dispersion);

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
    friend LossFn *MakeSimpleLossFn(std::vector<LogNormalDist *> distributions);

    friend class Structure;
    friend class IM;
    friend LogNormalDist AddDistributions(std::vector<LogNormalDist> dists);
};

LogNormalDist *MakeLogNormalDist(double mu, LOGNORMAL_MU_TYPE mu_type,
                                 double sigma, LOGNORMAL_SIGMA_TYPE sigma_type);
LogNormalDist *MakeLogNormalDist(void);
LossFn *MakeBiLevelLossFn(std::vector<BiLevelLoss *> distributions);



class IM {
public:
    IM();
    IM(std::shared_ptr<SLAT::IM> r);
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
public:
    std::shared_ptr<SLAT::IM> relationship;
        
};

IM *MakeIM(DeterministicFn f);

class EDP {
public:
    EDP();
    EDP(std::shared_ptr<SLAT::EDP> r);
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
    void replace(FragilityFn *frag);
private:
    std::shared_ptr<SLAT::FragilityFn> fragility;
    friend CompGroup *MakeCompGroup(EDP edp, FragilityFn frag_fn,
                                    LossFn cost_fn, LossFn delay_fn,
                                    int count, std::string name);
};

FragilityFn *MakeFragilityFn(std::vector<LogNormalDist *> distributions);

class LossFn {
public:
    LossFn(std::shared_ptr<SLAT::LossFn> function);
    int n_states();
    void replace(LossFn *lossfn);
private:
    std::shared_ptr<SLAT::LossFn> loss;
    friend CompGroup *MakeCompGroup(EDP edp, FragilityFn frag_fn,
                                    LossFn cost_fn, LossFn delay_fn, int count, std::string name);
};


class CompGroup {
public:
    CompGroup(std::shared_ptr<SLAT::CompGroup> group);
    std::string get_Name();
    double E_Cost_EDP(double edp);
    double E_Delay_EDP(double edp);
    double SD_ln_cost_EDP(double edp);
    double SD_ln_Delay_EDP(double edp);
    double E_Cost_IM(double edp);
    double SD_ln_Cost_IM(double edp);
    double E_annual_cost(void);
    double E_cost(int years, double discount_rate);
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
 
void Initialise(void);
void SetLogFile(std::string path);
void LogToStdErr(bool flag);
void reset_statistics(void);
std::string format_statistics(void);
