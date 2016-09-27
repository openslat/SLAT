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


//     class IMWrapper {
//     public:
//         IMWrapper() : relationship(NULL) {};
//         IMWrapper(std::shared_ptr<IM> r)
//         {
//             relationship = r;
//         }
//         double lambda(double x) 
//         {
//             return relationship->lambda(x);
//         }
//         python::list lambda_l(python::list l) 
//         {
//             return l;
//         }
//         void SetCollapse(LogNormalDistWrapper c)
//         {
//            relationship->SetCollapse(c.dist);
//         }
//         double pCollapse(double im)
//         {
//             return relationship->pCollapse(im);
//         }
//         double CollapseRate(void)
//         {
//             return relationship->CollapseRate();
//         }
//         void SetDemolition(LogNormalDistWrapper d)
//         {
//            relationship->SetDemolition(d.dist);
//         }
//         double pDemolition(double im)
//         {
//             return relationship->pDemolition(im);
//         }
//         double DemolitionRate(void)
//         {
//             return relationship->DemolitionRate();
//         }
//         double pRepair(double im)
//         {
//             return relationship->pRepair(im);
//         }
//     public:
//         std::shared_ptr<IM> relationship;
        
//     };
    
//     class EDPWrapper {
//     public:
//         EDPWrapper() : relationship(NULL) {};
//         EDPWrapper(std::shared_ptr<EDP> r)
//         {
//             relationship = r;
//         }
//         double lambda(double x) 
//         {
//             return relationship->lambda(x);
//         }
//         double P_exceedence(double x, double y) 
//         {
//             return relationship->P_exceedence(x, y);
//         }
//         python::list lambda_l(python::list l) 
//         {
//             return l;
//         }
//         double Mean(double x)
//         {
//             return relationship->Mean(x);
//         }
//         double MeanLn(double x)
//         {
//             return relationship->MeanLn(x);
//         }
//         double Median(double x)
//         {
//             return relationship->Median(x);
//         }
//         double SD_ln(double x)
//         {
//             return relationship->SD_ln(x);
//         }
//         double SD(double x)
//         {
//             return relationship->SD(x);
//         }
//         std::string get_Name(void)
//         {
//             return relationship->get_Name();
//         }
//         bool AreSame(const EDPWrapper &other)
//         {
//             return this->relationship == other.relationship;
//         }
//     public:
//         std::shared_ptr<EDP> relationship;
//         friend CompGroupWrapper *MakeCompGroup(EDPWrapper edp, FragilityFnWrapper frag_fn,
//                                                 LossFnWrapper loss_fn, int count);
//     };
    
//     IMWrapper *MakeIM(DeterministicFn f)
//     {
//         std::shared_ptr<IM> relationship(new IM(f.function));
//         return new IMWrapper(relationship);
//     }

//     EDPWrapper *MakeEDP(
//         IMWrapper base_rate,
//         ProbabilisticFn dependent_rate, 
//         std::string name)
//     {
//         std::shared_ptr<EDP> relationship =
// 	  std::make_shared<EDP>(base_rate.relationship,
//                             dependent_rate.function,
//                             name);
//         EDPWrapper *result = new EDPWrapper(relationship);
//         return result;
//     }


//     class FragilityFnWrapper;
//     class LossFnWrapper;
    
//     LogNormalDistWrapper *MakeLogNormalDist(python::dict parameters)
//     {
//         LogNormalFn::M_TYPE m_type = LogNormalFn::M_TYPE::MEAN_INVALID;
//         LogNormalFn::S_TYPE s_type = LogNormalFn::S_TYPE::SIGMA_INVALID;
//         double mu_param=NAN, sigma_param=NAN;
//         {
//             python::stl_input_iterator<int> iter(parameters.keys());
//             while (iter != python::stl_input_iterator<int>()) {
//                 python::object key(*iter);
//                 python::object value = parameters.get(key);

//                 if (!value.is_none()) {
//                     double parameter = python::extract<double>(value);
//                     switch (*iter) {
//                     case LOGNORMAL_PARAM_TYPE::MEAN_X:
//                         if (m_type != LogNormalFn::M_TYPE::MEAN_INVALID) {
//                             throw std::invalid_argument("MEAN_X");
//                         } 
//                         m_type = LogNormalFn::M_TYPE::MEAN_X;
//                         mu_param = parameter;
//                         break;
//                     case LOGNORMAL_PARAM_TYPE::MEDIAN_X:
//                         if (m_type != LogNormalFn::M_TYPE::MEAN_INVALID) {
//                             throw std::invalid_argument("MEDIAN_X");
//                         } 
//                         m_type = LogNormalFn::M_TYPE::MEDIAN_X;
//                         mu_param = parameter;
//                         break;
//                     case LOGNORMAL_PARAM_TYPE::MEAN_LN_X:
//                         if (m_type != LogNormalFn::M_TYPE::MEAN_INVALID) {
//                             throw std::invalid_argument("MEAN_LN_X");
//                         } 
//                         m_type = LogNormalFn::M_TYPE::MEAN_LN_X;
//                         mu_param = parameter;
//                         break;
//                     case LOGNORMAL_PARAM_TYPE::SD_X:
//                         if (s_type != LogNormalFn::S_TYPE::SIGMA_INVALID) {
//                             throw std::invalid_argument("SIGMA_X");
//                         } 
//                         s_type = LogNormalFn::S_TYPE::SIGMA_X;
//                         sigma_param = parameter;
//                         break;
//                     case LOGNORMAL_PARAM_TYPE::SD_LN_X:
//                         if (s_type != LogNormalFn::S_TYPE::SIGMA_INVALID) {
//                             throw std::invalid_argument("SIGMA_LN_X");
//                         } 
//                         s_type = LogNormalFn::S_TYPE::SIGMA_LN_X;
//                         sigma_param = parameter;
//                         break;
//                     default:
//                         throw std::invalid_argument("unknown");
//                     };
//                         }
//                 iter++;
//             }
//         };
//         std::shared_ptr<LogNormalDist> dist;
            
//         if (m_type == LogNormalFn::M_TYPE::MEAN_LN_X &&
//             s_type == LogNormalFn::S_TYPE::SIGMA_LN_X)
//         {
//             dist = std::make_shared<LogNormalDist>(LogNormalDist::LogNormalDist_from_mu_lnX_and_sigma_lnX(mu_param, sigma_param));
//         } else if (m_type == LogNormalFn::M_TYPE::MEAN_X &&
//             s_type == LogNormalFn::S_TYPE::SIGMA_LN_X)
//         {
//             dist = std::make_shared<LogNormalDist>(LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(mu_param, sigma_param));
//         } else if (m_type == LogNormalFn::M_TYPE::MEDIAN_X &&
//             s_type == LogNormalFn::S_TYPE::SIGMA_LN_X)
//         {
//             dist = std::make_shared<LogNormalDist>(LogNormalDist::LogNormalDist_from_median_X_and_sigma_lnX(mu_param, sigma_param));
//         } else if (m_type == LogNormalFn::M_TYPE::MEAN_X &&
//             s_type == LogNormalFn::S_TYPE::SIGMA_X)
//         {
//             dist = std::make_shared<LogNormalDist>(LogNormalDist::LogNormalDist_from_mean_X_and_sigma_X(mu_param, sigma_param));
//         } else {
//             throw std::invalid_argument("mu, sigma combination not supported");
//         }

//         return new LogNormalDistWrapper(dist);
//     };


//     class FragilityFnWrapper {
//     public:
//         FragilityFnWrapper(std::shared_ptr<FragilityFn> function) { fragility = function; }
//         python::list pExceeded(double edp);
//         python::list pHighest(double edp);
//         int n_states() { return fragility->n_states(); };
//         bool AreSame(const FragilityFnWrapper &other)
//         {
//             return this->fragility == other.fragility;
//         }
//     private:
//         std::shared_ptr<FragilityFn> fragility;
//         friend CompGroupWrapper *MakeCompGroup(EDPWrapper edp, FragilityFnWrapper frag_fn,
//                                                 LossFnWrapper loss_fn, int count);
//     };

//     python::list FragilityFnWrapper::pExceeded(double edp)
//     {
//         std::vector<double> probabilities = fragility->pExceeded(edp);
//         python::list result;
//         for (std::vector<double>::iterator i = probabilities.begin(); i != probabilities.end(); i++) {
//             result.append(*i);
//         }
//         return result;
//     }

//     python::list FragilityFnWrapper::pHighest(double edp)
//     {
//         std::vector<double> probabilities = fragility->pHighest(edp);
//         python::list result;
//         for (std::vector<double>::iterator i = probabilities.begin(); i != probabilities.end(); i++) {
//             result.append(*i);
//         }
//         return result;
//     }

//     FragilityFnWrapper *MakeFragilityFn(python::list parameters)
//     {
//         std::vector<LogNormalDist> distributions;

//         python::stl_input_iterator<python::dict> iter(parameters), end;
//         while (iter != end) {
//             distributions.push_back(*MakeLogNormalDist(*iter)->dist);
//             iter++;
//         }
//         return new FragilityFnWrapper(std::make_shared<FragilityFn>(distributions));
//     }

//     class LossFnWrapper {
//     public:
//         LossFnWrapper(std::shared_ptr<LossFn> function) { loss = function; }
//         int n_states() { return loss->n_states(); };
//     private:
//         std::shared_ptr<LossFn> loss;
//         friend CompGroupWrapper *MakeCompGroup(EDPWrapper edp, FragilityFnWrapper frag_fn,
//                                                 LossFnWrapper loss_fn, int count);
//     };

//     LossFnWrapper *MakeLossFn(python::list parameters)
//     {
//         std::vector<LogNormalDist> distributions;

//         python::stl_input_iterator<python::dict> iter(parameters), end;
//         while (iter != end) {
//             distributions.push_back(*MakeLogNormalDist(*iter)->dist);
//             iter++;
//         }
//         return new LossFnWrapper(std::make_shared<LossFn>(distributions));
//     }

//     class CompGroupWrapper {
//     public:
//         CompGroupWrapper(std::shared_ptr<CompGroup> group) { wrapper = group; };
//         double E_Loss_EDP(double edp) { return wrapper->E_loss_EDP(edp); };
//         double SD_ln_loss_EDP(double edp) { return wrapper->SD_ln_loss_EDP(edp); };
//         double E_Loss_IM(double edp) { return wrapper->E_loss_IM(edp); };
//         double SD_ln_loss_IM(double edp) { return wrapper->SD_ln_loss_IM(edp); };
//         double E_annual_loss(void) { return wrapper->E_annual_loss(); };
//         double E_loss(int years, double discount_rate) { return wrapper->E_loss(years, discount_rate); };
//         python::list pDS_IM(double im);
//         python::list Rate(void);
//         double lambda_loss(double loss) { return wrapper->lambda_loss(loss); };
//         bool AreSame(const CompGroupWrapper &other)
//         {
//             return this->wrapper == other.wrapper;
//         }
//     private:
//         std::shared_ptr<CompGroup> wrapper;
        
//         friend class StructureWrapper;
//     };
    
//     CompGroupWrapper *MakeCompGroup(EDPWrapper edp, FragilityFnWrapper frag_fn, LossFnWrapper loss_fn, int count)
//     {
//         return new CompGroupWrapper(std::make_shared<CompGroup>( 
//                                         edp.relationship, 
//                                         frag_fn.fragility,
//                                         loss_fn.loss, 
//                                         count));
//     }
    
//     python::list CompGroupWrapper::pDS_IM(double im)
//     {
//         std::vector<double> probabilities = wrapper->pDS_IM(im);
//         python::list result;
//         for (std::vector<double>::iterator i = probabilities.begin(); i != probabilities.end(); i++) {
//             result.append(*i);
//         }
//         return result;
//     }

//     python::list CompGroupWrapper::Rate(void)
//     {
//         std::vector<double> probabilities = wrapper->Rate();
//         python::list result;
//         for (std::vector<double>::iterator i = probabilities.begin(); i != probabilities.end(); i++) {
//             result.append(*i);
//         }
//         return result;
//     }

//     class StructureWrapper {
//     public:
//         StructureWrapper(std::shared_ptr<Structure> structure) { wrapper = structure; };
//         void AddCompGroup(CompGroupWrapper cg) {
//             wrapper->AddCompGroup(cg.wrapper);
//         };
//         LogNormalDistWrapper Loss(double im, bool consider_collapse) {
//             return LogNormalDistWrapper(std::shared_ptr<LogNormalDist>(new LogNormalDist(wrapper->Loss(im, consider_collapse))));
//         };
        
//         LogNormalDistWrapper TotalLoss(double im) {
//             return LogNormalDistWrapper(std::shared_ptr<LogNormalDist>(new LogNormalDist(wrapper->TotalLoss(im))));
//         };
        
//         python::list DeaggregatedLoss(double im) {
//             std::pair<LogNormalDist, LogNormalDist> values = wrapper->DeaggregatedLoss(im);
            
//             python::list result;
//             {
//                 std::shared_ptr<LogNormalDist> temp = std::make_shared<LogNormalDist>(values.first);
//                 result.append(LogNormalDistWrapper(temp));
//             }

//             {
//                 std::shared_ptr<LogNormalDist> temp = std::make_shared<LogNormalDist>(values.second);
//                 result.append(LogNormalDistWrapper(temp));
//             }
//             return result;
//         }

//         python::list LossesByFate(double im) {
//             Structure::LOSSES losses = wrapper->LossesByFate(im);
//             python::list result;
//             {
//                 std::shared_ptr<LogNormalDist> temp = std::make_shared<LogNormalDist>(losses.repair);
//                 result.append(LogNormalDistWrapper(temp));
//             }
//             {
//                 std::shared_ptr<LogNormalDist> temp = std::make_shared<LogNormalDist>(losses.demolition);
//                 result.append(LogNormalDistWrapper(temp));
//             }
//             {
//                 std::shared_ptr<LogNormalDist> temp = std::make_shared<LogNormalDist>(losses.collapse);
//                 result.append(LogNormalDistWrapper(temp));
//             }
//             return result;
//         }

//         void setRebuildCost(LogNormalDistWrapper cost) {
//             wrapper->setRebuildCost(*cost.dist);
//         };
//         LogNormalDistWrapper getRebuildCost(void) {
//             return LogNormalDistWrapper(std::shared_ptr<LogNormalDist>(new LogNormalDist(wrapper->getRebuildCost())));
//         };
//         void setDemolitionCost(LogNormalDistWrapper cost) {
//             wrapper->setDemolitionCost(*cost.dist);
//         };
//         LogNormalDistWrapper getDemolitionCost(void) {
//             return LogNormalDistWrapper(std::shared_ptr<LogNormalDist>(new LogNormalDist(wrapper->getDemolitionCost())));
//         };
//         LogNormalDistWrapper AnnualLoss(void) {
//             return LogNormalDistWrapper(std::shared_ptr<LogNormalDist>(new LogNormalDist(wrapper->AnnualLoss())));
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
//                 mapping.append(EDPWrapper(*key));

//                 for (size_t j=0; j < edp_cg_mapping[*key].size(); j++) {
//                     mapping.append(CompGroupWrapper(edp_cg_mapping[*key][j]));
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
//                 mapping.append(FragilityFnWrapper(*key));

//                 for (size_t j=0; j < frag_cg_mapping[*key].size(); j++) {
//                     //std::cout << "   " << frag_cg_mapping[*key][j] << std::endl;
//                     mapping.append(CompGroupWrapper(frag_cg_mapping[*key][j]));
//                 }
//                 result.append(mapping);
//             }
//             //std::cout << "< ComponentsByFragility()" << std::endl;
//             return result;
//         };
//     private:
//         std::shared_ptr<Structure> wrapper;
//     };

//     StructureWrapper *MakeStructure() 
//     {
//         return new StructureWrapper(std::make_shared<Structure>("anonymous structure"));
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

//         python::class_<IMWrapper>("IM", python::no_init)
//             .def("getlambda", &IMWrapper::lambda)
//             .def("SetCollapse", &IMWrapper::SetCollapse)
//             .def("CollapseRate", &IMWrapper::CollapseRate)
//             .def("pCollapse", &IMWrapper::pCollapse)
//             .def("SetDemolition", &IMWrapper::SetDemolition)
//             .def("DemolitionRate", &IMWrapper::DemolitionRate)
//             .def("pDemolition", &IMWrapper::pDemolition)
//             .def("pRepair", &IMWrapper::pRepair)
//             ;

//         python::class_<EDPWrapper>("EDP", python::no_init)
//             .def("getlambda", &EDPWrapper::lambda)
//             .def("P_exceedence", &EDPWrapper::P_exceedence)
//             .def("Mean", &EDPWrapper::Mean)
//             .def("MeanLn", &EDPWrapper::MeanLn)
//             .def("Median", &EDPWrapper::Median)
//             .def("SD_ln", &EDPWrapper::SD_ln)
//             .def("SD", &EDPWrapper::SD)
//             .def("get_Name", &EDPWrapper::get_Name)
//             .def("AreSame", &EDPWrapper::AreSame)
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
        
//         python::class_<LogNormalDistWrapper>("LogNormalDist", python::no_init)
//             .def("p_at_least", &LogNormalDistWrapper::p_at_least)
//             .def("p_at_most", &LogNormalDistWrapper::p_at_most)
//             .def("x_at_p", &LogNormalDistWrapper::x_at_p)
//             .def("get_mu_lnX", &LogNormalDistWrapper::get_mu_lnX)
//             .def("get_median_X", &LogNormalDistWrapper::get_median_X)
//             .def("get_mean_X", &LogNormalDistWrapper::get_mean_X)
//             .def("get_sigma_lnX", &LogNormalDistWrapper::get_sigma_lnX)
//             .def("get_sigma_X", &LogNormalDistWrapper::get_sigma_X)
//             ;

//         python::def("AddDistributions", 
//                     AddDistributions);

        
//         python::class_<FragilityFnWrapper>("FragilityFn", python::no_init)
//             .def("pExceeded", &FragilityFnWrapper::pExceeded)
//             .def("pHighest", &FragilityFnWrapper::pHighest)
//             .def("n_states", &FragilityFnWrapper::n_states)
//             .def("AreSame", &FragilityFnWrapper::AreSame)
//             ;

//         python::def("MakeFragilityFn", 
//                     MakeFragilityFn,
//                     python::return_value_policy<python::manage_new_object>());
        
//         python::class_<LossFnWrapper>("LossFn", python::no_init)
//             .def("n_states", &LossFnWrapper::n_states)
//             ;

//         python::def("MakeLossFn", 
//                     MakeLossFn,
//                     python::return_value_policy<python::manage_new_object>());
        
//         python::class_<CompGroupWrapper>("CompGroup", python::no_init)
//             .def("E_Loss_EDP", &CompGroupWrapper::E_Loss_EDP)
//             .def("SD_ln_loss_EDP", &CompGroupWrapper::SD_ln_loss_EDP)
//             .def("E_Loss_IM", &CompGroupWrapper::E_Loss_IM)
//             .def("SD_ln_loss_IM", &CompGroupWrapper::SD_ln_loss_IM)
//             .def("E_annual_loss", &CompGroupWrapper::E_annual_loss)
//             .def("E_loss", &CompGroupWrapper::E_loss)
//             .def("lambda_loss", &CompGroupWrapper::lambda_loss)
//             .def("AreSame", &CompGroupWrapper::AreSame)
//             .def("pDS_IM", &CompGroupWrapper::pDS_IM)
//             .def("Rate", &CompGroupWrapper::Rate)
//             ;

//         python::def("MakeCompGroup", 
//                     MakeCompGroup,
//                     python::return_value_policy<python::manage_new_object>());

//         python::class_<StructureWrapper>("Structure", python::no_init)
//             .def("AddCompGroup", &StructureWrapper::AddCompGroup)
//             .def("Loss", &StructureWrapper::Loss)
//             .def("DeaggregatedLoss", &StructureWrapper::DeaggregatedLoss)
//             .def("setRebuildCost", &StructureWrapper::setRebuildCost)
//             .def("getRebuildCost", &StructureWrapper::getRebuildCost)
//             .def("setDemolitionCost", &StructureWrapper::setDemolitionCost)
//             .def("getDemolitionCost", &StructureWrapper::getDemolitionCost)
//             .def("AnnualLoss", &StructureWrapper::AnnualLoss)
//             .def("LossesByFate", &StructureWrapper::LossesByFate)
//             .def("ComponentsByEDP", &StructureWrapper::ComponentsByEDP)
//             .def("ComponentsByFragility", &StructureWrapper::ComponentsByFragility)
//             .def("TotalLoss", &StructureWrapper::TotalLoss)
//             ;

        
//         python::def("MakeStructure", 
//                     MakeStructure,
//                     python::return_value_policy<python::manage_new_object>());
        
//     }
// }
