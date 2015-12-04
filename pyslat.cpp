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
#include <string>
#include "functions.h"
#include "relationships.h"
#include <iostream>
using namespace SLAT;

namespace python = boost::python;

namespace SLAT {

    class DeterministicFunctionWrapper {
    public:
        DeterministicFunctionWrapper() : function(NULL) {};
        DeterministicFunctionWrapper(std::shared_ptr<DeterministicFunction> f)
        {
            function = f;
        }
        double ValueAt(double v) {
            return function->ValueAt(v);
        }
        std::shared_ptr<DeterministicFunction> function;
    private:
    };

    enum FUNCTION_TYPE { NLH, PLC, LIN, LOGLOG };
    
    DeterministicFunctionWrapper *factory(FUNCTION_TYPE t, python::list params)
    {
        DeterministicFunction *result = NULL;
    
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
            
            result = new LinearInterpolatedFunction(x_data, y_data, size);
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
            
            result = new LogLogInterpolatedFunction(x_data, y_data, size);
            break;
        }
        default:
            std::cout << "Unrecognised function type" << std::endl;
        }
        std::shared_ptr<DeterministicFunction> ptr(result);
        return new DeterministicFunctionWrapper(ptr);
    }

    class ProbabilisticFunctionWrapper {
    public:
        ProbabilisticFunctionWrapper() : function(NULL) {};
        ProbabilisticFunctionWrapper(std::shared_ptr<ProbabilisticFunction> f)
        {
            function = f;
        }
        double P_exceedence(double x, double min_y) {
            return function->P_exceedence(x, min_y);
        };
        double X_at_exceedence(double x, double p) {
            return function->X_at_exceedence(x, p);
        };
        std::shared_ptr<ProbabilisticFunction> function;
    private:
    };

    ProbabilisticFunctionWrapper *MakeLogNormalProbabilisticFunction(
        DeterministicFunctionWrapper mu,
        DeterministicFunctionWrapper sigma)
    {
        std::shared_ptr<ProbabilisticFunction> function(
            new LogNormalFunction(
                std::shared_ptr<DeterministicFunction>(mu.function),
                std::shared_ptr<DeterministicFunction>(sigma.function)));
        return new ProbabilisticFunctionWrapper(function);
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
    
    RateRelationshipWrapper *MakeSimpleRelationship(DeterministicFunctionWrapper f)
    {
        std::shared_ptr<SimpleRateRelationship> relationship(
            new SimpleRateRelationship(f.function));
        return new RateRelationshipWrapper(relationship);
    }

    RateRelationshipWrapper *MakeCompoundRelationship(
        RateRelationshipWrapper base_rate,
        ProbabilisticFunctionWrapper dependent_rate)
    {
        std::shared_ptr<CompoundRateRelationship> relationship(
            new CompoundRateRelationship(base_rate.relationship,
                                         dependent_rate.function));
        RateRelationshipWrapper *result = new RateRelationshipWrapper(relationship);
        return result;
    }


// Python requires an exported function called init<module-name> in every
// extension module. This is where we build the module contents.
    BOOST_PYTHON_MODULE(pyslat)
    {
        python::def("factory", factory, python::return_value_policy<python::manage_new_object>());
        python::class_<DeterministicFunctionWrapper>("DeterministicFunction", 
                                                      python::no_init)
            .def("ValueAt", &DeterministicFunctionWrapper::ValueAt)
            ;

        python::def("MakeLogNormalProbabilisticFunction",
                    MakeLogNormalProbabilisticFunction,
                    python::return_value_policy<python::manage_new_object>());

        python::class_<ProbabilisticFunctionWrapper>("ProbabilisticFunction", python::no_init)
            .def("P_exceedence", &ProbabilisticFunctionWrapper::P_exceedence)
            .def("X_at_exceedence", &ProbabilisticFunctionWrapper::X_at_exceedence)
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

        python::enum_<FUNCTION_TYPE>("FUNCTION_TYPE")
            .value("NLH", NLH)
            .value("PLC", PLC)
            .value("LIN", LIN)
            .value("LOGLOG", LOGLOG)
            ;
    }
}
