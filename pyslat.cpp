/**
 * @file   pyslat.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Fri Dec  4 10:45:30 2015
 * 
 * @brief Python interface to SLAT C++ code.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */
#include <memory>
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <string>
#include "functions.h"
#include "relationships.h"
#include "lognormaldist.h"
#include "fragility.h"
#include "loss_functions.h"
#include "comp_group.h"
#include <iostream>
using namespace SLAT;

namespace python = boost::python;

namespace SLAT {
    class CompGroupWrapper;
    class FragilityFnWrapper;
    class LossFnWrapper;
    
    class DeterministicFnWrapper {
    public:
        DeterministicFnWrapper() : function(NULL) {};
        DeterministicFnWrapper(std::shared_ptr<DeterministicFn> f)
        {
            function = f;
        }
        double ValueAt(double v) {
            return function->ValueAt(v);
        }
        std::shared_ptr<DeterministicFn> function;
    private:
    };

    enum FUNCTION_TYPE { NLH, PLC, LIN, LOGLOG };
    enum LOGNORMAL_PARAM_TYPE { MEAN_X, MEDIAN_X, MEAN_LN_X, SD_X, SD_LN_X };
    
    DeterministicFnWrapper *factory(FUNCTION_TYPE t, python::list params)
    {
        DeterministicFn *result = NULL;
    
        switch (t) {
        case PLC:
        {
            double b = python::extract<double>(params.pop());
            double a = python::extract<double>(params.pop());
            result = new PowerLawParametricCurve(a, b);
            break;
        }
        case NLH:
        {
            double alpha = python::extract<double>(params.pop());
            double IM_asy = python::extract<double>(params.pop());
            double v_asy = python::extract<double>(params.pop());
            result = new NonLinearHyperbolicLaw(v_asy, IM_asy, alpha);
            break;
        }
        case LIN:
        {
            double *x_data, *y_data;
            int size = len(params);
            x_data = new double[size];
            y_data = new double[size];
        
            for (int i=0; i < size; i++) {
                python::list o = python::extract<python::list>(params.pop());
                double y = python::extract<double>(o.pop());
                double x = python::extract<double>(o.pop());

                x_data[size - i - 1] = x;
                y_data[size - i - 1] = y;
            }
            
            result = new LinearInterpolatedFn(x_data, y_data, size);
            break;
        }
        case LOGLOG:
        {
            double *x_data, *y_data;
            int size = len(params);
            x_data = new double[size];
            y_data = new double[size];
        
            for (int i=0; i < size; i++) {
                python::list o = python::extract<python::list>(params.pop());
                double y = python::extract<double>(o.pop());
                double x = python::extract<double>(o.pop());

                x_data[size - i - 1] = x;
                y_data[size - i - 1] = y;
            }
            
            result = new LogLogInterpolatedFn(x_data, y_data, size);
            break;
        }
        default:
            std::cout << "Unrecognised function type" << std::endl;
        }
        std::shared_ptr<DeterministicFn> ptr(result);
        return new DeterministicFnWrapper(ptr);
    }

    class ProbabilisticFnWrapper {
    public:
        ProbabilisticFnWrapper() : function(NULL) {};
        ProbabilisticFnWrapper(std::shared_ptr<ProbabilisticFn> f)
        {
            function = f;
        }
        double P_exceedence(double x, double min_y) {
            return function->P_exceedence(x, min_y);
        };
        double X_at_exceedence(double x, double p) {
            return function->X_at_exceedence(x, p);
        };

        double Mean(double x) {
            return function->Mean(x);
        };
        std::shared_ptr<ProbabilisticFn> function;
    private:
    };

    ProbabilisticFnWrapper *MakeLogNormalProbabilisticFn(python::dict parameters)
    {
        // Check for valid parameters:
        int num_placements = parameters.keys().count(LOGNORMAL_PARAM_TYPE::MEAN_X) + 
            parameters.keys().count(LOGNORMAL_PARAM_TYPE::MEDIAN_X) +
            parameters.keys().count(LOGNORMAL_PARAM_TYPE::MEAN_LN_X);
        int num_spreads = parameters.keys().count(LOGNORMAL_PARAM_TYPE::SD_X) + 
            parameters.keys().count(LOGNORMAL_PARAM_TYPE::SD_LN_X);

        
        if (num_placements == 0) {
            std::cerr << "Must have at least one placement parameter." << std::endl;
            return (ProbabilisticFnWrapper *)0;
        } else if (num_placements > 1) {
            std::cerr << "Must have only one placement parameter." << std::endl;
            return (ProbabilisticFnWrapper *)0;
        }

        if (num_spreads == 0) {
            std::cerr << "Must have at least one spread parameter." << std::endl;
            return (ProbabilisticFnWrapper *)0;
        } else if (num_spreads > 1) {
            std::cerr << "Must have only one spread parameter." << std::endl;
            return (ProbabilisticFnWrapper *)0;
        }
        
        LogNormalFn::M_TYPE m_type = LogNormalFn::M_TYPE::MEAN_INVALID;
        LogNormalFn::S_TYPE s_type = LogNormalFn::S_TYPE::SIGMA_INVALID;
        DeterministicFnWrapper mu_function, sigma_function;
        {
            python::stl_input_iterator<int> iter(parameters.keys());
            while (iter != python::stl_input_iterator<int>()) {
                python::object key(*iter);
                python::object value = parameters.get(key);

                if (!value.is_none()) {
                    DeterministicFnWrapper fn = python::extract<DeterministicFnWrapper>(value);
                    switch (*iter) {
                    case LOGNORMAL_PARAM_TYPE::MEAN_X:
                        if (m_type != LogNormalFn::M_TYPE::MEAN_INVALID) {
                            throw std::invalid_argument("MEAN_X");
                        } 
                        m_type = LogNormalFn::M_TYPE::MEAN_X;
                        mu_function = fn;
                        break;
                    case LOGNORMAL_PARAM_TYPE::MEDIAN_X:
                        if (m_type != LogNormalFn::M_TYPE::MEAN_INVALID) {
                            throw std::invalid_argument("MEAN_INVALID");
                        } 
                        m_type = LogNormalFn::M_TYPE::MEDIAN_X;
                        mu_function = fn;
                        break;
                    case LOGNORMAL_PARAM_TYPE::MEAN_LN_X:
                        if (m_type != LogNormalFn::M_TYPE::MEAN_INVALID) {
                            throw std::invalid_argument("MEAN_LN_X");
                        } 
                        m_type = LogNormalFn::M_TYPE::MEAN_LN_X;
                        mu_function = fn;
                        break;
                    case LOGNORMAL_PARAM_TYPE::SD_X:
                        if (s_type != LogNormalFn::S_TYPE::SIGMA_INVALID) {
                            throw std::invalid_argument("SIGMA_X");
                        } 
                        s_type = LogNormalFn::S_TYPE::SIGMA_X;
                        sigma_function = fn;
                        break;
                    case LOGNORMAL_PARAM_TYPE::SD_LN_X:
                        if (s_type != LogNormalFn::S_TYPE::SIGMA_INVALID) {
                            throw std::invalid_argument("SIGMA_LN_X");
                        } 
                        s_type = LogNormalFn::S_TYPE::SIGMA_LN_X;
                        sigma_function = fn;
                        break;
                    default:
                        throw std::invalid_argument("INVALID");
                    };
                }
                iter++;
            }
            std::shared_ptr<ProbabilisticFn> function(
                new LogNormalFn(
                    std::shared_ptr<DeterministicFn>(mu_function.function), m_type,
                    std::shared_ptr<DeterministicFn>(sigma_function.function), s_type));
            return new ProbabilisticFnWrapper(function);
        }
    };

    class RateRelationshipWrapper {
    public:
        RateRelationshipWrapper() : relationship(NULL) {};
        RateRelationshipWrapper(std::shared_ptr<RateRelationship> r)
        {
            relationship = r;
        }
        RateRelationshipWrapper(std::shared_ptr<CompoundRateRelationship> r)
        {
            relationship = r;
        }
        double lambda(double x) 
        {
            return relationship->lambda(x);
        }
        python::list lambda_l(python::list l) 
        {
            return l;
        }
    public:
        std::shared_ptr<RateRelationship> relationship;
    };
    
    class CompoundRateRelationshipWrapper {
    public:
        CompoundRateRelationshipWrapper() : relationship(NULL) {};
        CompoundRateRelationshipWrapper(std::shared_ptr<CompoundRateRelationship> r)
        {
            relationship = r;
        }
        double lambda(double x) 
        {
            return relationship->lambda(x);
        }
        python::list lambda_l(python::list l) 
        {
            return l;
        }
    public:
        std::shared_ptr<CompoundRateRelationship> relationship;
        friend CompGroupWrapper *MakeCompGroup(CompoundRateRelationshipWrapper edp, FragilityFnWrapper frag_fn,
                                                LossFnWrapper loss_fn, int count);
    };
    
    RateRelationshipWrapper *MakeSimpleRelationship(DeterministicFnWrapper f)
    {
        std::shared_ptr<SimpleRateRelationship> relationship(
            new SimpleRateRelationship(f.function));
        return new RateRelationshipWrapper(relationship);
    }

    CompoundRateRelationshipWrapper *MakeCompoundRelationship(
        RateRelationshipWrapper base_rate,
        ProbabilisticFnWrapper dependent_rate)
    {
        std::shared_ptr<CompoundRateRelationship> relationship(
            new CompoundRateRelationship(base_rate.relationship,
                                         dependent_rate.function));
        CompoundRateRelationshipWrapper *result = new CompoundRateRelationshipWrapper(relationship);
        return result;
    }


    class FragilityFnWrapper;
    class LossFnWrapper;
    
    class LogNormalDistWrapper {
    public:
        LogNormalDistWrapper(std::shared_ptr<LogNormalDist> dist) { this->dist = dist; };
        double p_at_least(double x) const { return dist->p_at_least(x); }
        double p_at_most(double x) const { return dist->p_at_most(x); }

        double x_at_p(double p) const { return dist->x_at_p(p); };

        double get_mu_lnX(void) const { return dist->get_mu_lnX(); }
        double get_median_X(void) const { return dist->get_median_X(); };
        double get_mean_X(void) const { return dist->get_mean_X(); }
        double get_sigma_lnX(void) const {return dist->get_sigma_lnX(); }
        double get_sigma_X(void) const {return dist->get_sigma_X(); }
    private:
        std::shared_ptr<LogNormalDist> dist;
        friend FragilityFnWrapper *MakeFragilityFn(python::list);
        friend LossFnWrapper *MakeLossFn(python::list);        
    };

    LogNormalDistWrapper *MakeLogNormalDist(python::dict parameters)
    {
        LogNormalFn::M_TYPE m_type = LogNormalFn::M_TYPE::MEAN_INVALID;
        LogNormalFn::S_TYPE s_type = LogNormalFn::S_TYPE::SIGMA_INVALID;
        double mu_param, sigma_param;
        {
            python::stl_input_iterator<int> iter(parameters.keys());
            while (iter != python::stl_input_iterator<int>()) {
                python::object key(*iter);
                python::object value = parameters.get(key);

                if (!value.is_none()) {
                    double parameter = python::extract<double>(value);
                    switch (*iter) {
                    case LOGNORMAL_PARAM_TYPE::MEAN_X:
                        if (m_type != LogNormalFn::M_TYPE::MEAN_INVALID) {
                            throw std::invalid_argument("MEAN_X");
                        } 
                        m_type = LogNormalFn::M_TYPE::MEAN_X;
                        mu_param = parameter;
                        break;
                    case LOGNORMAL_PARAM_TYPE::MEDIAN_X:
                        if (m_type != LogNormalFn::M_TYPE::MEAN_INVALID) {
                            throw std::invalid_argument("MEDIAN_X");
                        } 
                        m_type = LogNormalFn::M_TYPE::MEDIAN_X;
                        mu_param = parameter;
                        break;
                    case LOGNORMAL_PARAM_TYPE::MEAN_LN_X:
                        if (m_type != LogNormalFn::M_TYPE::MEAN_INVALID) {
                            throw std::invalid_argument("MEAN_LN_X");
                        } 
                        m_type = LogNormalFn::M_TYPE::MEAN_LN_X;
                        mu_param = parameter;
                        break;
                    case LOGNORMAL_PARAM_TYPE::SD_X:
                        if (s_type != LogNormalFn::S_TYPE::SIGMA_INVALID) {
                            throw std::invalid_argument("SIGMA_X");
                        } 
                        s_type = LogNormalFn::S_TYPE::SIGMA_X;
                        sigma_param = parameter;
                        break;
                    case LOGNORMAL_PARAM_TYPE::SD_LN_X:
                        if (s_type != LogNormalFn::S_TYPE::SIGMA_INVALID) {
                            throw std::invalid_argument("SIGMA_LN_X");
                        } 
                        s_type = LogNormalFn::S_TYPE::SIGMA_LN_X;
                        sigma_param = parameter;
                        break;
                    default:
                        throw std::invalid_argument("unknown");
                    };
                        }
                iter++;
            }
        };
        std::shared_ptr<LogNormalDist> dist;
            
        if (m_type == LogNormalFn::M_TYPE::MEAN_LN_X &&
            s_type == LogNormalFn::S_TYPE::SIGMA_LN_X)
        {
            dist = std::make_shared<LogNormalDist>(LogNormalDist::LogNormalDist_from_mu_lnX_and_sigma_lnX(mu_param, sigma_param));
        } else if (m_type == LogNormalFn::M_TYPE::MEAN_X &&
            s_type == LogNormalFn::S_TYPE::SIGMA_LN_X)
        {
            dist = std::make_shared<LogNormalDist>(LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(mu_param, sigma_param));
        } else if (m_type == LogNormalFn::M_TYPE::MEDIAN_X &&
            s_type == LogNormalFn::S_TYPE::SIGMA_LN_X)
        {
            dist = std::make_shared<LogNormalDist>(LogNormalDist::LogNormalDist_from_median_X_and_sigma_lnX(mu_param, sigma_param));
        } else if (m_type == LogNormalFn::M_TYPE::MEAN_X &&
            s_type == LogNormalFn::S_TYPE::SIGMA_X)
        {
            dist = std::make_shared<LogNormalDist>(LogNormalDist::LogNormalDist_from_mean_X_and_sigma_X(mu_param, sigma_param));
        } else {
            throw std::invalid_argument("mu, sigma combination not supported");
        }

        return new LogNormalDistWrapper(dist);
    };


    class FragilityFnWrapper {
    public:
        FragilityFnWrapper(std::shared_ptr<FragilityFn> function) { fragility = function; }
        python::list pExceeded(double edp);
        python::list pHighest(double edp);
        int n_states() { return fragility->n_states(); };
    private:
        std::shared_ptr<FragilityFn> fragility;
        friend CompGroupWrapper *MakeCompGroup(CompoundRateRelationshipWrapper edp, FragilityFnWrapper frag_fn,
                                                LossFnWrapper loss_fn, int count);
    };

    python::list FragilityFnWrapper::pExceeded(double edp)
    {
        std::vector<double> probabilities = fragility->pExceeded(edp);
        python::list result;
        for (std::vector<double>::iterator i = probabilities.begin(); i != probabilities.end(); i++) {
            result.append(*i);
        }
        return result;
    }

    python::list FragilityFnWrapper::pHighest(double edp)
    {
        std::vector<double> probabilities = fragility->pHighest(edp);
        python::list result;
        for (std::vector<double>::iterator i = probabilities.begin(); i != probabilities.end(); i++) {
            result.append(*i);
        }
        return result;
    }

    FragilityFnWrapper *MakeFragilityFn(python::list parameters)
    {
        std::vector<LogNormalDist> distributions;

        python::stl_input_iterator<python::dict> iter(parameters), end;
        while (iter != end) {
            distributions.push_back(*MakeLogNormalDist(*iter)->dist);
            iter++;
        }
        return new FragilityFnWrapper(std::make_shared<FragilityFn>(distributions));
    }

    class LossFnWrapper {
    public:
        LossFnWrapper(std::shared_ptr<LossFn> function) { loss = function; }
        int n_states() { return loss->n_states(); };
    private:
        std::shared_ptr<LossFn> loss;
        friend CompGroupWrapper *MakeCompGroup(CompoundRateRelationshipWrapper edp, FragilityFnWrapper frag_fn,
                                                LossFnWrapper loss_fn, int count);
    };

    LossFnWrapper *MakeLossFn(python::list parameters)
    {
        std::vector<LogNormalDist> distributions;

        python::stl_input_iterator<python::dict> iter(parameters), end;
        while (iter != end) {
            distributions.push_back(*MakeLogNormalDist(*iter)->dist);
            iter++;
        }
        return new LossFnWrapper(std::make_shared<LossFn>(distributions));
    }

    class CompGroupWrapper {
    public:
        CompGroupWrapper(std::shared_ptr<CompGroup> group) { wrapper = group; };
        double E_Loss_EDP(double edp) { return wrapper->E_loss_EDP(edp); };
        double SD_ln_loss_EDP(double edp) { return wrapper->SD_ln_loss_EDP(edp); };
        double E_Loss_IM(double edp) { return wrapper->E_loss_IM(edp); };
        double SD_ln_loss_IM(double edp) { return wrapper->SD_ln_loss_IM(edp); };
        double E_annual_loss(void) { return wrapper->E_annual_loss(); };
        double E_loss(int years, double discount_rate) { return wrapper->E_loss(years, discount_rate); };
        double lambda_loss(double loss) { return wrapper->lambda_loss(loss); };
    private:
        std::shared_ptr<CompGroup> wrapper;
    };
    
    CompGroupWrapper *MakeCompGroup(CompoundRateRelationshipWrapper edp, FragilityFnWrapper frag_fn, LossFnWrapper loss_fn, int count)
    {
        return new CompGroupWrapper(std::make_shared<CompGroup>( 
                                        edp.relationship, 
                                        frag_fn.fragility,
                                        loss_fn.loss, 
                                        count));
    }

// Python requires an exported function called init<module-name> in every
// extension module. This is where we build the module contents.
    BOOST_PYTHON_MODULE(pyslat)
    {
        python::def("factory", factory, python::return_value_policy<python::manage_new_object>());
        python::class_<DeterministicFnWrapper>("DeterministicFn", 
                                                      python::no_init)
            .def("ValueAt", &DeterministicFnWrapper::ValueAt)
            ;

        python::def("MakeLogNormalProbabilisticFn",
                    MakeLogNormalProbabilisticFn, 
                    python::return_value_policy<python::manage_new_object>());

        python::class_<ProbabilisticFnWrapper>("ProbabilisticFn", python::no_init)
            .def("P_exceedence", &ProbabilisticFnWrapper::P_exceedence)
            .def("X_at_exceedence", &ProbabilisticFnWrapper::X_at_exceedence)
            .def("Mean", &ProbabilisticFnWrapper::Mean)
            ;

        python::def("MakeSimpleRelationship",
                    MakeSimpleRelationship,
                    python::return_value_policy<python::manage_new_object>());

        python::def("MakeCompoundRelationship",
                    MakeCompoundRelationship,
                    python::return_value_policy<python::manage_new_object>());

        python::class_<RateRelationshipWrapper>("RateRelationship", python::no_init)
            .def("getlambda", &RateRelationshipWrapper::lambda)
            ;

        python::class_<CompoundRateRelationshipWrapper>("CompoundRateRelationship", python::no_init)
            .def("getlambda", &CompoundRateRelationshipWrapper::lambda)
            ;

        python::enum_<FUNCTION_TYPE>("FUNCTION_TYPE")
            .value("NLH", NLH)
            .value("PLC", PLC)
            .value("LIN", LIN)
            .value("LOGLOG", LOGLOG)
            ;

        python::enum_<LOGNORMAL_PARAM_TYPE>("LOGNORMAL_PARAM_TYPE")
            .value("MEAN_X", MEAN_X)
            .value("MEDIAN_X", MEDIAN_X)
            .value("MEAN_LN_X", MEAN_LN_X)
            .value("SD_X", SD_X)
            .value("SD_LN_X", SD_LN_X)
            ;

        python::def("MakeLogNormalDist", 
                    MakeLogNormalDist,
                    python::return_value_policy<python::manage_new_object>());
        
        python::class_<LogNormalDistWrapper>("LogNormalDist", python::no_init)
            .def("p_at_least", &LogNormalDistWrapper::p_at_least)
            .def("p_at_most", &LogNormalDistWrapper::p_at_most)
            .def("x_at_p", &LogNormalDistWrapper::x_at_p)
            .def("get_mu_lnX", &LogNormalDistWrapper::get_mu_lnX)
            .def("get_median_X", &LogNormalDistWrapper::get_median_X)
            .def("get_mean_X", &LogNormalDistWrapper::get_mean_X)
            .def("get_sigma_lnX", &LogNormalDistWrapper::get_sigma_lnX)
            .def("get_sigma_X", &LogNormalDistWrapper::get_sigma_X)
            ;

        python::class_<FragilityFnWrapper>("FragilityFn", python::no_init)
            .def("pExceeded", &FragilityFnWrapper::pExceeded)
            .def("pHighest", &FragilityFnWrapper::pHighest)
            .def("n_states", &FragilityFnWrapper::n_states)
            ;

        python::def("MakeFragilityFn", 
                    MakeFragilityFn,
                    python::return_value_policy<python::manage_new_object>());
        
        python::class_<LossFnWrapper>("LossFn", python::no_init)
            .def("n_states", &LossFnWrapper::n_states)
            ;

        python::def("MakeLossFn", 
                    MakeLossFn,
                    python::return_value_policy<python::manage_new_object>());
        
        python::class_<CompGroupWrapper>("CompGroup", python::no_init)
            .def("E_Loss_EDP", &CompGroupWrapper::E_Loss_EDP)
            .def("SD_ln_loss_EDP", &CompGroupWrapper::SD_ln_loss_EDP)
            .def("E_Loss_IM", &CompGroupWrapper::E_Loss_IM)
            .def("SD_ln_loss_IM", &CompGroupWrapper::SD_ln_loss_IM)
            .def("E_annual_loss", &CompGroupWrapper::E_annual_loss)
            .def("E_loss", &CompGroupWrapper::E_loss)
            .def("lambda_loss", &CompGroupWrapper::lambda_loss)
            ;

        python::def("MakeCompGroup", 
                    MakeCompGroup,
                    python::return_value_policy<python::manage_new_object>());
        
        
    }
}
