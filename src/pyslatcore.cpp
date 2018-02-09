#include "caching.h"
#include "maq.h"
#include "pyslatcore.h"
#include "context.h"
#include "loss_functions.h"
#include <iostream>
#include <set>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/utility/setup/console.hpp>
using namespace std;
namespace logging = boost::log;

boost::shared_ptr<logging::sinks::sink> std_error_sink;
boost::shared_ptr<logging::sinks::sink> file_sink;

void Initialise(void)
{
    SLAT::Context::Initialise();
    std_error_sink = logging::add_console_log(std::cerr);
}

void SetLogFile(std::string path)
{
    if (file_sink != NULL) {
        logging::core::get()->remove_sink(file_sink);
    } 
    file_sink = logging::add_file_log(path);
}

void LogToStdErr(bool flag)
{
    if (flag) {
        if (std_error_sink == NULL) {
            boost::shared_ptr<logging::sinks::sink> k = logging::add_console_log(std::cerr);
        }
    } else {
        if (std_error_sink != NULL) {
            logging::core::get()->remove_sink(std_error_sink);
            std_error_sink = NULL;
        }
    }
}


void IntegrationSettings(double tolerance, unsigned int integration_eval_limit)
{
    SLAT::Integration::IntegrationSettings::Set_Tolerance(tolerance);
    SLAT::Integration::IntegrationSettings::Set_Integration_Eval_Limit(integration_eval_limit);
}

void Set_Integration_Tolerance(double tolerance)
{
    SLAT::Integration::IntegrationSettings::Set_Tolerance(tolerance);
}

void Set_Integration_Eval_Limit(unsigned int limit)
{
    SLAT::Integration::IntegrationSettings::Set_Integration_Eval_Limit(limit);
}

void Set_Integration_Search_Limit(unsigned int limit)
{
    SLAT::Integration::IntegrationSettings::Set_Integration_Search_Limit(limit);
}
void Set_Integration_Method(INTEGRATION_TYPE method)
{
    SLAT::Integration::IntegrationSettings::METHOD_TYPE m;
    switch (method) {
    case BINARY_SUBDIVISION:
        m = SLAT::Integration::IntegrationSettings::BINARY_SUBDIVISION;
        break;
    case REVERSE_BINARY_SUBDIVISION:
        m = SLAT::Integration::IntegrationSettings::REVERSE_BINARY_SUBDIVISION;
        break;
    case LOW_FIRST_REVERSE_BINARY_SUBDIVISION:
        m = SLAT::Integration::IntegrationSettings::LOW_FIRST_REVERSE_BINARY_SUBDIVISION;
        break;
    case SCATTERED:
        m = SLAT::Integration::IntegrationSettings::SCATTERED;
        break;
    case DIRECTED:
        m = SLAT::Integration::IntegrationSettings::DIRECTED;
        break;
    default:
        stringstream err;
        err << "Invalid integration method #" << (unsigned int)method << ".";
        throw std::invalid_argument(err.str());
    }
    SLAT::Integration::IntegrationSettings::Set_Integration_Method(m);
}

DeterministicFn::DeterministicFn() : function(NULL) {};
DeterministicFn::DeterministicFn(std::shared_ptr<SLAT::DeterministicFn> f)
{
    function = f;
}

double DeterministicFn::ValueAt(double v)
{
    return function->ValueAt(v);
}

DeterministicFn *factory3(FUNCTION_TYPE t, std::vector<double>x, std::vector<double> y)
{
    std::shared_ptr<SLAT::DeterministicFn> result;
    
    switch (t) {
    case PLC:
    case NLH:
        std::cout << "Must have exactly one list of data." << std::endl;
        break;

    case LIN:
    {
        if (x.size() < 2 || x.size() != y.size()) {
            std::cout << "Data lists must be same size, greater than 1." << std::endl;
            break;
        }
            
        result = std::make_shared<SLAT::LinearInterpolatedFn>(x.data(), y.data(), x.size());
        break;
    }
    case LOGLOG:
        if (x.size() < 2 || x.size() != y.size()) {
            std::cout << "Data lists must be same size, greater than 1." << std::endl;
            break;
        }
            
        result = std::make_shared<SLAT::LogLogInterpolatedFn>(x.data(), y.data(), x.size());
        break;
    default:
        std::cout << "Unrecognised function type" << std::endl;
    }
    return new DeterministicFn(result);
}

DeterministicFn *factory2(FUNCTION_TYPE t, std::vector<double> params)
{
    std::shared_ptr<SLAT::DeterministicFn> result;
    
    // std::cout << "> factory(" << t << "; ";
    // for (std::vector<double>::const_iterator i=params.begin(); i != params.end(); i++) {
    //     std::cout << *i << ", ";
    // }
    // std::cout << ")" << std::endl;
    switch (t) {
    case PLC:
    {
        double a = params[0];
        double b = params[1];
        result = std::make_shared<SLAT::PowerLawParametricCurve>(a, b);
        break;
    }
    case NLH:
    {
        // std::cout << "[NLH]" << std::endl;
        double alpha = params[0];
        double IM_asy = params[1];
        double v_asy = params[2];
        result = std::make_shared<SLAT::NonLinearHyperbolicLaw>(v_asy, IM_asy, alpha);
        break;
    }
    case LIN:
        std::cout << "Must have exactly two lists of data." << std::endl;
        break;
    case LOGLOG:
        std::cout << "Must have exactly two lists of data." << std::endl;
        break;
    default:
        std::cout << "Unrecognised function type" << std::endl;
    }
    return new DeterministicFn(result);
}

ProbabilisticFn::ProbabilisticFn() : function(NULL) {};
ProbabilisticFn::ProbabilisticFn(std::shared_ptr<SLAT::ProbabilisticFn> f)
{
    function = f;
}

double ProbabilisticFn::P_exceedence(double x, double min_y) {
    return function->P_exceedence(x, min_y);
};

double ProbabilisticFn::X_at_exceedence(double x, double p) {
    return function->X_at_exceedence(x, p);
};

double ProbabilisticFn::Mean(double x) {
    return function->Mean(x);
};

double ProbabilisticFn::MeanLn(double x) {
    return function->MeanLn(x);
};
  
double ProbabilisticFn::Median(double x) {
    return function->Median(x);
};

double ProbabilisticFn::SD_ln(double x) {
    return function->SD_ln(x);
};

double ProbabilisticFn::SD(double x) {
    return function->SD(x);
};

ProbabilisticFn *MakeLogNormalProbabilisticFn(const DeterministicFn &mu_function,
                                              LOGNORMAL_MU_TYPE mu_type,
                                              const DeterministicFn &sigma_function,
                                              LOGNORMAL_SIGMA_TYPE sigma_type)
{
    SLAT::LogNormalFn::M_TYPE m;
    SLAT::LogNormalFn::S_TYPE s;

    switch (mu_type) {
    case LOGNORMAL_MU_TYPE::MEAN_X:
        m = SLAT::LogNormalFn::M_TYPE::MEAN_X;
        break;
    case LOGNORMAL_MU_TYPE::MEDIAN_X:
        m = SLAT::LogNormalFn::M_TYPE::MEDIAN_X;
        break;
    case LOGNORMAL_MU_TYPE::MEAN_LN_X:
        m = SLAT::LogNormalFn::M_TYPE::MEAN_LN_X;
        break;
    default:
        m = SLAT::LogNormalFn::M_TYPE::MEAN_INVALID;
    };
    
    switch (sigma_type) {
    case LOGNORMAL_SIGMA_TYPE::SD_X:
        s = SLAT::LogNormalFn::S_TYPE::SIGMA_X;
        break;
    case LOGNORMAL_SIGMA_TYPE::SD_LN_X:
        s = SLAT::LogNormalFn::S_TYPE::SIGMA_LN_X;
        break;
    default:
        s = SLAT::LogNormalFn::S_TYPE::SIGMA_INVALID;
    };
    
    std::shared_ptr<SLAT::ProbabilisticFn> function = 
        std::make_shared<SLAT::LogNormalFn>(mu_function.function, m, sigma_function.function, s);
    return new ProbabilisticFn(function);
};


LogNormalDist::LogNormalDist(std::shared_ptr<SLAT::LogNormalDist> dist)
{
    this->dist = dist;
};


double LogNormalDist::p_at_least(double x) const
{
    return dist->p_at_least(x); 
}

double LogNormalDist::p_at_most(double x) const
{
    return dist->p_at_most(x); 
}

double LogNormalDist::x_at_p(double p) const
{
    return dist->x_at_p(p); 
};

double LogNormalDist::get_mu_lnX(void) const
{
    return dist->get_mu_lnX();
}


double LogNormalDist::get_median_X(void) const
{
    return dist->get_median_X(); 
};

double LogNormalDist::get_mean_X(void) const
{
    return dist->get_mean_X(); 
}

double LogNormalDist::get_sigma_lnX(void) const
{
    return dist->get_sigma_lnX();
}

double LogNormalDist::get_sigma_X(void) const
{
    return dist->get_sigma_X(); 
}

LogNormalDist AddDistributions(std::vector<LogNormalDist> dists) 
{
    std::vector<SLAT::LogNormalDist> slat_dists(dists.size());
    for (size_t i=0; i < dists.size(); i++) {
        slat_dists[i] = *(dists[i].dist);
    }
    return LogNormalDist(std::make_shared<SLAT::LogNormalDist>(SLAT::LogNormalDist::AddDistributions(slat_dists)));
};
    
IM::IM() {
};

IM::IM(std::shared_ptr<SLAT::IM> r)
{
    relationship = r;
}


double IM::get_lambda(double x) 
{
    return relationship->lambda(x);
}

std:: vector<double> IM::get_lambda(std::vector<double> x) 
{
    return relationship->lambda(x);
}


void IM::SetCollapse(LogNormalDist c)
{
    relationship->SetCollapse(c.dist);
}

double IM::pCollapse(double im)
{
    return relationship->pCollapse(im);
}

std::vector<double> IM::pCollapse(std::vector<double> im)
{
    return relationship->pCollapse(im);
}

double IM::CollapseRate(void)
{
    return relationship->CollapseRate();
}

void IM::SetDemolition(LogNormalDist d)
{
    relationship->SetDemolition(d.dist);
}

double IM::pDemolition(double im)
{
    return relationship->pDemolition(im);
}

std::vector<double> IM::pDemolition(std::vector<double> im)
{
    return relationship->pDemolition(im);
}

double IM::DemolitionRate(void)
{
    return relationship->DemolitionRate();
}

double IM::pRepair(double im)
{
    return relationship->pRepair(im);
}

std::vector<double> IM::pRepair(std::vector<double> im)
{
    return relationship->pRepair(im);
}

void IM::replace(IM *im)
{
    relationship->replace(im->relationship);
}

EDP::EDP() : relationship(NULL)
{
}

EDP::EDP(std::shared_ptr<SLAT::EDP> r)
{
    relationship = r;
}

double EDP::get_lambda(double x) 
{
    return relationship->lambda(x);
}

std::vector<double> EDP::get_lambda(std::vector<double> x) 
{
    std::vector<double> result(x.size());
#pragma omp parallel for
    for (size_t i=0; i < x.size(); i++) {
        result[i] = get_lambda(x[i]);
    }
    return result;
}

double EDP::P_exceedence(double x, double y) 
{
    return relationship->P_exceedence(x, y);
}

double EDP::Mean(double x)
{
    return relationship->Mean(x);
}

double EDP::MeanLn(double x)
{
    return relationship->MeanLn(x);
}

double EDP::Median(double x)
{
    return relationship->Median(x);
}

double EDP::SD_ln(double x)
{
    return relationship->SD_ln(x);
}

double EDP::SD(double x)
{
    return relationship->SD(x);
}

double EDP::X_at_exceedence(double im, double p)
{
    return relationship->X_at_exceedence(im, p);
}

std::vector<double> EDP::bulk_Mean(std::vector<double> x)
{
    std::vector<double> result(x.size());
#pragma omp parallel for
    for (size_t i=0; i < x.size(); i++) {
        result[i] = Mean(x[i]);
    }
    return result;
}

std::vector<double> EDP::bulk_MeanLn(std::vector<double> x)
{
    std::vector<double> result(x.size());
#pragma omp parallel for
    for (size_t i=0; i < x.size(); i++) {
        result[i] = MeanLn(x[i]);
    }
    return result;
}

std::vector<double> EDP::bulk_Median(std::vector<double> x)
{
    std::vector<double> result(x.size());
#pragma omp parallel for
    for (size_t i=0; i < x.size(); i++) {
        result[i] = Median(x[i]);
    }
    return result;
}

std::vector<double> EDP::bulk_SD_ln(std::vector<double> x)
{
    std::vector<double> result(x.size());
#pragma omp parallel for
    for (size_t i=0; i < x.size(); i++) {
        result[i] = SD_ln(x[i]);
    }
    return result;
}

std::vector<double> EDP::bulk_SD(std::vector<double> x)
{
    std::vector<double> result(x.size());
#pragma omp parallel for
    for (size_t i=0; i < x.size(); i++) {
        result[i] = SD(x[i]);
    }
    return result;
}

std::string EDP::get_Name(void)
{
    return relationship->get_Name();
}

bool EDP::AreSame(const EDP &other)
{
    return this->relationship == other.relationship;
}

void EDP::replace(EDP *edp)
{
    relationship->replace(edp->relationship);
}

IM *MakeIM(DeterministicFn f)
{
    std::shared_ptr<SLAT::IM> im = std::make_shared<SLAT::IM>(f.function);
    return new IM(im);
}

EDP *MakeEDP(IM base_rate, ProbabilisticFn dependent_rate, std::string name)
{
    std::shared_ptr<SLAT::EDP> relationship =
        std::make_shared<SLAT::EDP>(base_rate.relationship,
                                    dependent_rate.function,
                                    name);
    EDP *result = new EDP(relationship);
    return result;
}


//     class FragilityFnWrapper;
//     class LossFnWrapper;
    
LogNormalDist *MakeLogNormalDist(double mu, LOGNORMAL_MU_TYPE mu_type,
                                 double sigma, LOGNORMAL_SIGMA_TYPE sigma_type)
{
    SLAT::LogNormalDist dist;
    if (mu_type == MEAN_LN_X && sigma_type == SD_LN_X) {
        dist = SLAT::LogNormalDist::LogNormalDist_from_mu_lnX_and_sigma_lnX(mu, sigma);
    } else if (mu_type == MEAN_X && sigma_type == SD_LN_X) {
        dist = SLAT::LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(mu, sigma);
    } else if (mu_type == MEDIAN_X && sigma_type == SD_LN_X) {
        dist = SLAT::LogNormalDist::LogNormalDist_from_median_X_and_sigma_lnX(mu, sigma);
    } else if (mu_type == MEAN_X && sigma_type == SD_X) {
        dist = SLAT::LogNormalDist::LogNormalDist_from_mean_X_and_sigma_X(mu, sigma);
    } else {
        std::cout << "Unsupported combination of mu and sigma." << std::endl;
    }

    std::shared_ptr<SLAT::LogNormalDist> temp = std::make_shared<SLAT::LogNormalDist>(dist);
    return new LogNormalDist(temp);
}

LogNormalDist *MakeLogNormalDist(void)
{
   SLAT::LogNormalDist dist;
    std::shared_ptr<SLAT::LogNormalDist> temp = std::make_shared<SLAT::LogNormalDist>(dist);
    return new LogNormalDist(temp);
}


FragilityFn::FragilityFn(std::shared_ptr<SLAT::FragilityFn> function)
{
    fragility = function;
}

int FragilityFn::n_states()
{
    return fragility->n_states(); 
};

bool FragilityFn::AreSame(const FragilityFn &other)
{
    return this->fragility == other.fragility;
}

std::vector<double> FragilityFn::pExceeded(double edp)
{
    return fragility->pExceeded(edp);
}

std::vector<double> FragilityFn::pHighest(double edp)
{
    return fragility->pHighest(edp);
}

void FragilityFn::replace(FragilityFn *frag)
{
    fragility->replace(frag->fragility);
}

FragilityFn *MakeFragilityFn(std::vector<LogNormalDist *> distributions)
{
    std::vector<SLAT::LogNormalDist> slat_distributions(distributions.size());
    for (size_t i=0; i < distributions.size(); i++) {
        slat_distributions[i] = *(distributions[i]->dist);
    };
    return new FragilityFn(std::make_shared<SLAT::FragilityFn>(slat_distributions));
}

LossFn::LossFn(std::shared_ptr<SLAT::LossFn> function) 
{
    loss = function; 
}

int LossFn::n_states() 
{
    return loss->n_states(); 
};


void LossFn::replace(LossFn *lossfn)
{
    loss->replace(lossfn->loss);
}

LossFn *MakeSimpleLossFn(std::vector<LogNormalDist *> distributions)
{
    std::vector<SLAT::LogNormalDist> slat_distributions(distributions.size());
    for (size_t i=0; i < distributions.size(); i++) {
        slat_distributions[i] = *(distributions[i]->dist);
    };
    return new LossFn(std::make_shared<SLAT::SimpleLossFn>(slat_distributions));
}

BiLevelLoss * MakeBiLevelLoss(int lower_limit, 
                              int upper_limit, 
                              double cost_at_min,
                              double cost_at_max, 
                              double dispersion)
{
    std::shared_ptr<SLAT::BiLevelLoss> temp = std::make_shared<SLAT::BiLevelLoss>(lower_limit, upper_limit, cost_at_min, cost_at_max, dispersion);
    return new BiLevelLoss(temp);
}

LossFn *MakeBiLevelLossFn(std::vector<BiLevelLoss *> losses)
{
    std::vector<std::shared_ptr<SLAT::BiLevelLoss>> slat_losses(losses.size());
    for (size_t i=0; i < losses.size(); i++) {
        slat_losses[i] = losses[i]->loss;
    };
    return new LossFn(std::make_shared<SLAT::BiLevelLossFn>(slat_losses));
}

CompGroup::CompGroup(std::shared_ptr<SLAT::CompGroup> group)
{
    wrapper = group;
};

void CompGroup::replace(CompGroup *cg)
{
    wrapper->replace(cg->wrapper);
}

std::string CompGroup::get_Name()
{
    return wrapper->get_Name();
}

EDP *CompGroup::get_EDP(void)
{
    return new EDP(wrapper->get_EDP());
}

double CompGroup::E_Cost_EDP(double edp)
{
    return wrapper->E_cost_EDP(edp); 
};


double CompGroup::SD_ln_cost_EDP(double edp) { 
    return wrapper->SD_ln_cost_EDP(edp); 
};

double CompGroup::E_Cost_IM(double edp)
{
    return wrapper->E_cost_IM(edp); 
};

double CompGroup::SD_ln_Cost_IM(double edp)
{
    return wrapper->SD_ln_cost_IM(edp); 
};

double CompGroup::E_annual_cost(void) 
{
    return wrapper->E_annual_cost();
};

double CompGroup::lambda_cost(double cost)
{
    return wrapper->lambda_cost(cost); 
};

double CompGroup::E_Delay_EDP(double edp)
{
    return wrapper->E_delay_EDP(edp); 
};

double CompGroup::SD_ln_Delay_EDP(double edp) { 
    return wrapper->SD_ln_delay_EDP(edp); 
};

double CompGroup::E_Delay_IM(double edp)
{
    return wrapper->E_delay_IM(edp); 
};

double CompGroup::SD_ln_Delay_IM(double edp)
{
    return wrapper->SD_ln_delay_IM(edp); 
};

bool CompGroup::AreSame(const CompGroup &other)
{
    return this->wrapper == other.wrapper;
}

CompGroup *MakeCompGroup(EDP edp, FragilityFn frag_fn, LossFn cost_fn, LossFn delay_fn, int count, std::string name)
{
    return new CompGroup(std::make_shared<SLAT::CompGroup>( 
                             edp.relationship, 
                             frag_fn.fragility,
                             cost_fn.loss,
                             delay_fn.loss, 
                             count, name));
}
    
std::vector<double> CompGroup::pDS_IM(double im)
{
    return  wrapper->pDS_IM(im);
}

std::vector<double> CompGroup::Rate(void)
{
    return wrapper->Rate();
}

Structure::Structure(std::shared_ptr<SLAT::Structure> structure)
{
    wrapper = structure;
};

void Structure::AddCompGroup(CompGroup cg)
{
    wrapper->AddCompGroup(cg.wrapper);
};

void Structure::RemoveCompGroup(CompGroup cg)
{
    wrapper->RemoveCompGroup(cg.wrapper);
};

LogNormalDist Structure::Cost(double im, bool consider_collapse) {
    return LogNormalDist(std::make_shared<SLAT::LogNormalDist>(wrapper->Cost(im, consider_collapse)));
};
        
LogNormalDist Structure::TotalCost(double im) 
{
    return LogNormalDist(std::make_shared<SLAT::LogNormalDist>(wrapper->TotalCost(im)));
};

std::vector<LogNormalDist> Structure::DeaggregatedCost(double im)
{
    std::vector<LogNormalDist> results(2);
    std::pair<SLAT::LogNormalDist, SLAT::LogNormalDist> losses = wrapper->DeaggregatedCost(im);
    results[0] = LogNormalDist(std::make_shared<SLAT::LogNormalDist>(losses.first));
    results[1] = LogNormalDist(std::make_shared<SLAT::LogNormalDist>(losses.second));
    return results;
}

std::vector<LogNormalDist> Structure::CostsByFate(double im) 
{
    std::vector<LogNormalDist> results(3);
    SLAT::Structure::COSTS losses = wrapper->CostsByFate(im);
    results[0] = LogNormalDist(std::make_shared<SLAT::LogNormalDist>(losses.repair));
    results[1] = LogNormalDist(std::make_shared<SLAT::LogNormalDist>(losses.demolition));
    results[2] = LogNormalDist(std::make_shared<SLAT::LogNormalDist>(losses.collapse));
    return results;
}

double Structure::E_cost(int years, double discount_rate) 
{
    return wrapper->E_cost(years, discount_rate); 
};

void Structure::setRebuildCost(LogNormalDist cost) 
{
    wrapper->setRebuildCost(*cost.dist);
};

LogNormalDist Structure::getRebuildCost(void) 
{
    return LogNormalDist(std::make_shared<SLAT::LogNormalDist>(wrapper->getRebuildCost()));
};

void Structure::setDemolitionCost(LogNormalDist cost) 
{
    wrapper->setDemolitionCost(*cost.dist);
};

LogNormalDist Structure::getDemolitionCost(void) 
{
    return LogNormalDist(std::make_shared<SLAT::LogNormalDist>(wrapper->getDemolitionCost()));
};

LogNormalDist Structure::AnnualCost(void) 
{
    return LogNormalDist(std::make_shared<SLAT::LogNormalDist>(wrapper->AnnualCost()));
};

std::list<std::list<CompGroup *>> Structure::ComponentsByEDP(void)
{
    std::list<std::list<CompGroup *>> result;
    const std::vector<std::shared_ptr<SLAT::CompGroup>> components = wrapper->Components();
    std::map<std::shared_ptr<SLAT::EDP>, std::vector<std::shared_ptr<SLAT::CompGroup>>> edp_cg_mapping;
    std::set<std::shared_ptr<SLAT::EDP>> keys;

    for (size_t i=0; i < components.size(); i++) {
        edp_cg_mapping[components[i]->get_EDP()].push_back(components[i]);
        keys.insert(components[i]->get_EDP());
    }
            
    for (std::set<std::shared_ptr<SLAT::EDP>>::iterator key = keys.begin();
         key != keys.end();
         key++)
    {
        std::list<CompGroup *> components;
        for (size_t j=0; j < edp_cg_mapping[*key].size(); j++) {
            components.push_back(new CompGroup(edp_cg_mapping[*key][j]));
        }
        result.push_back(components);
    }
    return result;
};

std::list<std::list<CompGroup *>> Structure::ComponentsByFragility(void)
{
    std::list<std::list<CompGroup *>> result;
    const std::vector<std::shared_ptr<SLAT::CompGroup>> components = wrapper->Components();
    std::map<std::shared_ptr<SLAT::FragilityFn>, std::vector<std::shared_ptr<SLAT::CompGroup>>> frag_cg_mapping;
    std::set<std::shared_ptr<SLAT::FragilityFn>> keys;

    for (size_t i=0; i < components.size(); i++) {
        frag_cg_mapping[components[i]->get_Fragility()].push_back(components[i]);
        keys.insert(components[i]->get_Fragility());
    }
            
    for (std::set<std::shared_ptr<SLAT::FragilityFn>>::iterator key = keys.begin();
         key != keys.end();
         key++)
    {
        std::list<CompGroup *> components;
        for (size_t j=0; j < frag_cg_mapping[*key].size(); j++) {
            components.push_back(new CompGroup(frag_cg_mapping[*key][j]));
        }
        result.push_back(components);
    }
    return result;
};

double Structure::pdf(double im)
{
    return wrapper->pdf(im);
};

// python::list ComponentsByFragility(void) {
// //            std::cout << "> ComponentsByFragility()" << std::endl;
//     python::list result;
//     const std::vector<std::shared_ptr<CompGroup>> components = wrapper->Components();
//     std::map<std::shared_ptr<FragilityFn>, std::vector<std::shared_ptr<CompGroup>>> frag_cg_mapping;
//     std::set<std::shared_ptr<FragilityFn>> keys;

//     for (size_t i=0; i < components.size(); i++) {
//         frag_cg_mapping[components[i]->get_Fragility()].push_back(components[i]);
//         keys.insert(components[i]->get_Fragility());
//     }
            
//     for (std::set<std::shared_ptr<FragilityFn>>::iterator key = keys.begin();
//          key != keys.end();
//          key++)
//     {
// //                std::cout << "KEY: " << *key << std::endl;
                
//         python::list mapping;
//         mapping.append(FragilityFn(*key));

//         for (size_t j=0; j < frag_cg_mapping[*key].size(); j++) {
//             //std::cout << "   " << frag_cg_mapping[*key][j] << std::endl;
//             mapping.append(CompGroup(frag_cg_mapping[*key][j]));
//         }
//         result.append(mapping);
//     }
//     //std::cout << "< ComponentsByFragility()" << std::endl;
//     return result;
// };

Structure *MakeStructure() 
{
    return new Structure(std::make_shared<SLAT::Structure>("anonymous structure"));
}


void reset_statistics(void)
{
    SLAT::Integration::reset_statistics();
}

std::string format_statistics(void)
{
    std::stringstream str;
    SLAT::Integration::format_statistics(str);
    return str.str();
}
    
