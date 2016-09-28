#include "caching.h"
#include "maq.h"
#include "pyslatcore.h"

void Init_Caching(void)
{
    SLAT::Caching::Init_Caching();
}


void IntegrationSettings(double tolerance, unsigned int max_evals)
{
    SLAT::Integration::IntegrationSettings::Set_Tolerance(tolerance);
    SLAT::Integration::IntegrationSettings::Set_Max_Evals(max_evals);
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

DeterministicFn *factory(FUNCTION_TYPE t, std::vector<double>x, std::vector<double> y)
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

DeterministicFn *factory(FUNCTION_TYPE t, std::vector<double> params)
{
    std::shared_ptr<SLAT::DeterministicFn> result;
    
    std::cout << "> factory(" << t << "; ";
    for (std::vector<double>::const_iterator i=params.begin(); i != params.end(); i++) {
        std::cout << *i << ", ";
    }
    std::cout << ")" << std::endl;
    switch (t) {
    case PLC:
    {
        double b = params[0];
        double a = params[1];
        result = std::make_shared<SLAT::PowerLawParametricCurve>(a, b);
        break;
    }
    case NLH:
    {
        std::cout << "[NLH]" << std::endl;
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
    

// /**
//  * @file   pyslatcore.cpp
//  * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
//  * @date   Fri Dec  4 10:45:30 2015
//  * 
//  * @brief Python interface to SLAT C++ code.
//  * 
//  * This file part of SLAT (the Seismic Loss Assessment Tool).
//  *
//  * ©2015 Canterbury University
//  */
// //#include <time.h>
// //#include <w32api/windows.h>
// #include <limits.h>
// #include <boost/math/special_functions.hpp>
// #include <boost/python.hpp>
// #include <boost/python/stl_iterator.hpp>
// #include <math.h>
// #include <memory>
// #include <string>
// #include <set>
// #include "functions.h"
// #include "relationships.h"
// #include "lognormaldist.h"
// #include "fragility.h"
// #include "loss_functions.h"
// #include "comp_group.h"
// #include "maq.h"
// #include "structure.h"
// #include <iostream>
// using namespace SLAT;

// namespace python = boost::python;

// namespace SLAT {
//     class CompGroupWrapper;
//     class FragilityFnWrapper;
//     class LossFnWrapper;
    

//     class DeterministicFn {
//     public:
//         DeterministicFn() : function(NULL) {};
//         DeterministicFn(std::shared_ptr<DeterministicFn> f)
//         {
//             function = f;
//         }
//         double ValueAt(double v) {
//             return function->ValueAt(v);
//         }
//         std::shared_ptr<DeterministicFn> function;
//     private:
//     };


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


void IM::SetCollapse(LogNormalDist c)
{
    relationship->SetCollapse(c.dist);
}

double IM::pCollapse(double im)
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

double IM::DemolitionRate(void)
{
    return relationship->DemolitionRate();
}

double IM::pRepair(double im)
{
    return relationship->pRepair(im);
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

std::string EDP::get_Name(void)
{
    return relationship->get_Name();
}

bool EDP::AreSame(const EDP &other)
{
    return this->relationship == other.relationship;
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


LossFn *MakeLossFn(std::vector<LogNormalDist *> distributions)
{
    std::vector<SLAT::LogNormalDist> slat_distributions(distributions.size());
    for (size_t i=0; i < distributions.size(); i++) {
        slat_distributions[i] = *(distributions[i]->dist);
    };
    return new LossFn(std::make_shared<SLAT::LossFn>(slat_distributions));
}


CompGroup::CompGroup(std::shared_ptr<SLAT::CompGroup> group)
{
    wrapper = group;
};

double CompGroup::E_Loss_EDP(double edp)
{
    return wrapper->E_loss_EDP(edp); 
};


double CompGroup::SD_ln_loss_EDP(double edp) { 
    return wrapper->SD_ln_loss_EDP(edp); 
};

double CompGroup::E_Loss_IM(double edp)
{
    return wrapper->E_loss_IM(edp); 
};

double CompGroup::SD_ln_loss_IM(double edp)
{
    return wrapper->SD_ln_loss_IM(edp); 
};

double CompGroup::E_annual_loss(void) 
{
    return wrapper->E_annual_loss();
};

double CompGroup::E_loss(int years, double discount_rate) 
{
    return wrapper->E_loss(years, discount_rate); 
};

double CompGroup::lambda_loss(double loss)
{
    return wrapper->lambda_loss(loss); 
};

bool CompGroup::AreSame(const CompGroup &other)
{
    return this->wrapper == other.wrapper;
}

CompGroup *MakeCompGroup(EDP edp, FragilityFn frag_fn, LossFn loss_fn, int count)
{
    return new CompGroup(std::make_shared<SLAT::CompGroup>( 
                                    edp.relationship, 
                                    frag_fn.fragility,
                                    loss_fn.loss, 
                                    count));
}
    
std::vector<double> CompGroup::pDS_IM(double im)
{
    return  wrapper->pDS_IM(im);
}

std::vector<double> CompGroup::Rate(void)
{
    return wrapper->Rate();
}

//     class Structure {
//     public:
//         Structure(std::shared_ptr<Structure> structure) { wrapper = structure; };
//         void AddCompGroup(CompGroup cg) {
//             wrapper->AddCompGroup(cg.wrapper);
//         };
//         LogNormalDist Loss(double im, bool consider_collapse) {
//             return LogNormalDist(std::shared_ptr<LogNormalDist>(new LogNormalDist(wrapper->Loss(im, consider_collapse))));
//         };
        
//         LogNormalDist TotalLoss(double im) {
//             return LogNormalDist(std::shared_ptr<LogNormalDist>(new LogNormalDist(wrapper->TotalLoss(im))));
//         };
        
//         python::list DeaggregatedLoss(double im) {
//             std::pair<LogNormalDist, LogNormalDist> values = wrapper->DeaggregatedLoss(im);
            
//             python::list result;
//             {
//                 std::shared_ptr<LogNormalDist> temp = std::make_shared<LogNormalDist>(values.first);
//                 result.append(LogNormalDist(temp));
//             }

//             {
//                 std::shared_ptr<LogNormalDist> temp = std::make_shared<LogNormalDist>(values.second);
//                 result.append(LogNormalDist(temp));
//             }
//             return result;
//         }

//         python::list LossesByFate(double im) {
//             Structure::LOSSES losses = wrapper->LossesByFate(im);
//             python::list result;
//             {
//                 std::shared_ptr<LogNormalDist> temp = std::make_shared<LogNormalDist>(losses.repair);
//                 result.append(LogNormalDist(temp));
//             }
//             {
//                 std::shared_ptr<LogNormalDist> temp = std::make_shared<LogNormalDist>(losses.demolition);
//                 result.append(LogNormalDist(temp));
//             }
//             {
//                 std::shared_ptr<LogNormalDist> temp = std::make_shared<LogNormalDist>(losses.collapse);
//                 result.append(LogNormalDist(temp));
//             }
//             return result;
//         }

//         void setRebuildCost(LogNormalDist cost) {
//             wrapper->setRebuildCost(*cost.dist);
//         };
//         LogNormalDist getRebuildCost(void) {
//             return LogNormalDist(std::shared_ptr<LogNormalDist>(new LogNormalDist(wrapper->getRebuildCost())));
//         };
//         void setDemolitionCost(LogNormalDist cost) {
//             wrapper->setDemolitionCost(*cost.dist);
//         };
//         LogNormalDist getDemolitionCost(void) {
//             return LogNormalDist(std::shared_ptr<LogNormalDist>(new LogNormalDist(wrapper->getDemolitionCost())));
//         };
//         LogNormalDist AnnualLoss(void) {
//             return LogNormalDist(std::shared_ptr<LogNormalDist>(new LogNormalDist(wrapper->AnnualLoss())));
//         };
//         python::list ComponentsByEDP(void) {
//             python::list result;
//             const std::vector<std::shared_ptr<CompGroup>> components = wrapper->Components();
//             std::map<std::shared_ptr<EDP>, std::vector<std::shared_ptr<CompGroup>>> edp_cg_mapping;
//             std::set<std::shared_ptr<EDP>> keys;

//             for (size_t i=0; i < components.size(); i++) {
//                 edp_cg_mapping[components[i]->get_EDP()].push_back(components[i]);
//                 keys.insert(components[i]->get_EDP());
//             }
            
//             for (std::set<std::shared_ptr<EDP>>::iterator key = keys.begin();
//                  key != keys.end();
//                  key++)
//             {
//                 python::list mapping;
//                 mapping.append(EDP(*key));

//                 for (size_t j=0; j < edp_cg_mapping[*key].size(); j++) {
//                     mapping.append(CompGroup(edp_cg_mapping[*key][j]));
//                 }
//                 result.append(mapping);
//             }
//             return result;
//         };
//         python::list ComponentsByFragility(void) {
// //            std::cout << "> ComponentsByFragility()" << std::endl;
//             python::list result;
//             const std::vector<std::shared_ptr<CompGroup>> components = wrapper->Components();
//             std::map<std::shared_ptr<FragilityFn>, std::vector<std::shared_ptr<CompGroup>>> frag_cg_mapping;
//             std::set<std::shared_ptr<FragilityFn>> keys;

//             for (size_t i=0; i < components.size(); i++) {
//                 frag_cg_mapping[components[i]->get_Fragility()].push_back(components[i]);
//                 keys.insert(components[i]->get_Fragility());
//             }
            
//             for (std::set<std::shared_ptr<FragilityFn>>::iterator key = keys.begin();
//                  key != keys.end();
//                  key++)
//             {
// //                std::cout << "KEY: " << *key << std::endl;
                
//                 python::list mapping;
//                 mapping.append(FragilityFn(*key));

//                 for (size_t j=0; j < frag_cg_mapping[*key].size(); j++) {
//                     //std::cout << "   " << frag_cg_mapping[*key][j] << std::endl;
//                     mapping.append(CompGroup(frag_cg_mapping[*key][j]));
//                 }
//                 result.append(mapping);
//             }
//             //std::cout << "< ComponentsByFragility()" << std::endl;
//             return result;
//         };
//     private:
//         std::shared_ptr<Structure> wrapper;
//     };

//     Structure *MakeStructure() 
//     {
//         return new Structure(std::make_shared<Structure>("anonymous structure"));
//     }

//     void IntegrationSettings(double tolerance, unsigned int max_evals)
//     {
//         Integration::IntegrationSettings::Set_Tolerance(tolerance);
//         Integration::IntegrationSettings::Set_Max_Evals(max_evals);
//     }

// // Python requires an exported function called init<module-name> in every
// // extension module. This is where we build the module contents.
//     BOOST_PYTHON_MODULE(pyslatcore)
//     {
//       SLAT::Caching::Init_Caching();
      
//         python::def("IntegrationSettings", IntegrationSettings);
        
//         python::def("factory", factory, python::return_value_policy<python::manage_new_object>());
//         python::class_<DeterministicFn>("DeterministicFn", 
//                                                       python::no_init)
//             .def("ValueAt", &DeterministicFn::ValueAt)
//             ;

//         python::def("MakeLogNormalProbabilisticFn",
//                     MakeLogNormalProbabilisticFn, 
//                     python::return_value_policy<python::manage_new_object>());

//         python::class_<ProbabilisticFn>("ProbabilisticFn", python::no_init)
//             .def("P_exceedence", &ProbabilisticFn::P_exceedence)
//             .def("X_at_exceedence", &ProbabilisticFn::X_at_exceedence)
//             .def("Mean", &ProbabilisticFn::Mean)
//             .def("MeanLn", &ProbabilisticFn::MeanLn)
//             .def("Median", &ProbabilisticFn::Median)
//             .def("SD_ln", &ProbabilisticFn::SD_ln)
//             .def("SD", &ProbabilisticFn::SD)
//             ;

//         python::def("MakeIM",
//                     MakeIM,
//                     python::return_value_policy<python::manage_new_object>());

//         python::def("MakeEDP",
//                     MakeEDP,
//                     python::return_value_policy<python::manage_new_object>());

//         python::class_<IM>("IM", python::no_init)
//             .def("getlambda", &IM::lambda)
//             .def("SetCollapse", &IM::SetCollapse)
//             .def("CollapseRate", &IM::CollapseRate)
//             .def("pCollapse", &IM::pCollapse)
//             .def("SetDemolition", &IM::SetDemolition)
//             .def("DemolitionRate", &IM::DemolitionRate)
//             .def("pDemolition", &IM::pDemolition)
//             .def("pRepair", &IM::pRepair)
//             ;

//         python::class_<EDP>("EDP", python::no_init)
//             .def("getlambda", &EDP::lambda)
//             .def("P_exceedence", &EDP::P_exceedence)
//             .def("Mean", &EDP::Mean)
//             .def("MeanLn", &EDP::MeanLn)
//             .def("Median", &EDP::Median)
//             .def("SD_ln", &EDP::SD_ln)
//             .def("SD", &EDP::SD)
//             .def("get_Name", &EDP::get_Name)
//             .def("AreSame", &EDP::AreSame)
//             ;

//         python::enum_<FUNCTION_TYPE>("FUNCTION_TYPE")
//             .value("NLH", NLH)
//             .value("PLC", PLC)
//             .value("LIN", LIN)
//             .value("LOGLOG", LOGLOG)
//             ;

//         python::enum_<LOGNORMAL_PARAM_TYPE>("LOGNORMAL_PARAM_TYPE")
//             .value("MEAN_X", MEAN_X)
//             .value("MEDIAN_X", MEDIAN_X)
//             .value("MEAN_LN_X", MEAN_LN_X)
//             .value("SD_X", SD_X)
//             .value("SD_LN_X", SD_LN_X)
//             ;

//         python::def("MakeLogNormalDist", 
//                     MakeLogNormalDist,
//                     python::return_value_policy<python::manage_new_object>());
        
//         python::class_<LogNormalDist>("LogNormalDist", python::no_init)
//             .def("p_at_least", &LogNormalDist::p_at_least)
//             .def("p_at_most", &LogNormalDist::p_at_most)
//             .def("x_at_p", &LogNormalDist::x_at_p)
//             .def("get_mu_lnX", &LogNormalDist::get_mu_lnX)
//             .def("get_median_X", &LogNormalDist::get_median_X)
//             .def("get_mean_X", &LogNormalDist::get_mean_X)
//             .def("get_sigma_lnX", &LogNormalDist::get_sigma_lnX)
//             .def("get_sigma_X", &LogNormalDist::get_sigma_X)
//             ;

//         python::def("AddDistributions", 
//                     AddDistributions);

        
//         python::class_<FragilityFn>("FragilityFn", python::no_init)
//             .def("pExceeded", &FragilityFn::pExceeded)
//             .def("pHighest", &FragilityFn::pHighest)
//             .def("n_states", &FragilityFn::n_states)
//             .def("AreSame", &FragilityFn::AreSame)
//             ;

//         python::def("MakeFragilityFn", 
//                     MakeFragilityFn,
//                     python::return_value_policy<python::manage_new_object>());
        
//         python::class_<LossFn>("LossFn", python::no_init)
//             .def("n_states", &LossFn::n_states)
//             ;

//         python::def("MakeLossFn", 
//                     MakeLossFn,
//                     python::return_value_policy<python::manage_new_object>());
        
//         python::class_<CompGroup>("CompGroup", python::no_init)
//             .def("E_Loss_EDP", &CompGroup::E_Loss_EDP)
//             .def("SD_ln_loss_EDP", &CompGroup::SD_ln_loss_EDP)
//             .def("E_Loss_IM", &CompGroup::E_Loss_IM)
//             .def("SD_ln_loss_IM", &CompGroup::SD_ln_loss_IM)
//             .def("E_annual_loss", &CompGroup::E_annual_loss)
//             .def("E_loss", &CompGroup::E_loss)
//             .def("lambda_loss", &CompGroup::lambda_loss)
//             .def("AreSame", &CompGroup::AreSame)
//             .def("pDS_IM", &CompGroup::pDS_IM)
//             .def("Rate", &CompGroup::Rate)
//             ;

//         python::def("MakeCompGroup", 
//                     MakeCompGroup,
//                     python::return_value_policy<python::manage_new_object>());

//         python::class_<Structure>("Structure", python::no_init)
//             .def("AddCompGroup", &Structure::AddCompGroup)
//             .def("Loss", &Structure::Loss)
//             .def("DeaggregatedLoss", &Structure::DeaggregatedLoss)
//             .def("setRebuildCost", &Structure::setRebuildCost)
//             .def("getRebuildCost", &Structure::getRebuildCost)
//             .def("setDemolitionCost", &Structure::setDemolitionCost)
//             .def("getDemolitionCost", &Structure::getDemolitionCost)
//             .def("AnnualLoss", &Structure::AnnualLoss)
//             .def("LossesByFate", &Structure::LossesByFate)
//             .def("ComponentsByEDP", &Structure::ComponentsByEDP)
//             .def("ComponentsByFragility", &Structure::ComponentsByFragility)
//             .def("TotalLoss", &Structure::TotalLoss)
//             ;

        
//         python::def("MakeStructure", 
//                     MakeStructure,
//                     python::return_value_policy<python::manage_new_object>());
        
//     }
// }
