/**
 * @file   comp_group.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Wed Feb 03 10:04:50 NZDT 2016
 *
 * @brief  Classes representing component groups.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */

#include "comp_group.h"
#include <vector>
#include <gsl/gsl_deriv.h>
#include <gsl/gsl_integration.h>

using namespace std;

namespace SLAT {
    Integration::IntegrationSettings CompGroup::class_settings(
        Integration::IntegrationSettings::Get_Global_Settings());

    Integration::IntegrationSettings &CompGroup::Get_Class_Integration_Settings(void)
    {
        return class_settings;
    }

    Integration::IntegrationSettings &CompGroup::Get_Integration_Settings(void)
    {
        return local_settings;
    }
    
    /*
     * Constructor--initialise all members, including CachedFunctions and
     * CachedValues, then install callbacks for EDP changes.
     */
    CompGroup::CompGroup(std::shared_ptr<EDP> edp,
                         std::shared_ptr<FragilityFn> frag_fn, 
                         std::shared_ptr<LossFn> cost_fn,
                         std::shared_ptr<LossFn> delay_fn,
                         int count,
                         std::string name)
        :local_settings(&class_settings),
        E_cost_IM([this] (double im) {
                return this->E_cost_IM_calc(im);
            }, name + std::string("::E_cost_IM")),
         SD_ln_cost_IM([this] (double im) {
                 return this->SD_ln_cost_IM_calc(im);
             }, name + std::string("::SD_ln_cost_IM")),
         E_delay_IM([this] (double im) {
                return this->E_delay_IM_calc(im);
            }, name + std::string("::E_cost_IM")),
         SD_ln_delay_IM([this] (double im) {
                 return this->SD_ln_delay_IM_calc(im);
             }, name + std::string("::SD_ln_cost_IM")),
         E_annual_cost([this] (void) {

                 return this->E_annual_cost_calc();
             }, name + std::string("::E_annual_cost")),
         lambda_cost([this] (double cost) {
                 return this->lambda_cost_calc(cost);
             }, name + std::string("::lambda_cost")),
         cost_EDP_dist([this] (double edp) {
                 if (this->cost_fn  == NULL) {
                     return LogNormalDist();
                 } else {
                     return LogNormalDist::AddWeightedDistributions(this->cost_fn->LossFns(), 
                                                                    this->frag_fn->pHighest(edp)); 
                 }
             }, name + std::string("::cost_EDP_dist")),
         delay_EDP_dist([this] (double edp) {
                 if (this->delay_fn  == NULL) {
                     return LogNormalDist();
                 } else {
                     return LogNormalDist::AddWeightedDistributions(this->delay_fn->LossFns(), 
                                                                    this->frag_fn->pHighest(edp)); 
                 }
             }, name + std::string("::delay_EDP_dist")),
         Rate([this] (void) {
                 return this->calc_Rate();
             }, name + std::string("::Rate")),
         name(name),
         edp(edp),
         frag_fn(frag_fn),
         cost_fn(cost_fn),
         delay_fn(delay_fn),
         count(count)
    {
        /*
         * When the EDP changes or is replaced, clear the cached values.
         */
        edp_callback_id = edp->add_callbacks(
            [this] (void) {
                this->E_cost_IM.ClearCache();
                this->SD_ln_cost_IM.ClearCache();
                this->E_delay_IM.ClearCache();
                this->SD_ln_delay_IM.ClearCache();
                this->E_annual_cost.ClearCache();
                this->lambda_cost.ClearCache();
                this->cost_EDP_dist.ClearCache();
                this->delay_EDP_dist.ClearCache();
                this->Rate.ClearCache();
            },
            [this] (std::shared_ptr<EDP> new_edp) {
                this->E_cost_IM.ClearCache();
                this->SD_ln_cost_IM.ClearCache();
                this->E_delay_IM.ClearCache();
                this->SD_ln_delay_IM.ClearCache();
                this->E_annual_cost.ClearCache();
                this->lambda_cost.ClearCache();
                this->cost_EDP_dist.ClearCache();
                this->delay_EDP_dist.ClearCache();
                this->Rate.ClearCache();
                this->edp = new_edp;
            });
    };

    double CompGroup::E_cost_EDP(double edp)
    {
        return this->count * cost_EDP_dist(edp).get_mean_X();
    }

    double CompGroup::mean_ln_cost_EDP(double edp)
    {
        return this->count * cost_EDP_dist(edp).get_mu_lnX();
    }

    double CompGroup::SD_ln_cost_EDP(double edp)
    {
        return cost_EDP_dist(edp).get_sigma_lnX();
    }

    double CompGroup::SD_cost_EDP(double edp)
    {
        return cost_EDP_dist(edp).get_sigma_X();
    }

    double CompGroup::E_delay_EDP(double edp)
    {
        return this->count * delay_EDP_dist(edp).get_mean_X();
    }

    double CompGroup::SD_ln_delay_EDP(double edp)
    {
        return delay_EDP_dist(edp).get_sigma_lnX();
    }

    double CompGroup::SD_delay_EDP(double edp)
    {
        return delay_EDP_dist(edp).get_sigma_X();
    }

    /*
     * This function is used to wrap lambdas so they can be passed to gsl
     * functions.
     */
    static double wrapper(double x,  std::function<double (double)> *f)
    {
        return (*f)(x);
    }

    /*
     * Calculate the distribution of the cost given IM. The mean and standard
     * deviation can be computed in parallel.
     */
    LogNormalDist CompGroup::CostDist_IM(double im) {
        double E = NAN, sd_ln = NAN;
#pragma omp parallel sections
        {
#pragma omp section
            {
                E = E_cost_IM(im);
            }
#pragma omp section
            {
                sd_ln = SD_ln_cost_IM(im);
            }
        }

        return LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(E, sd_ln);
    }

    double CompGroup::E_cost_IM_calc(double im)
    {
        Integration::MAQ_RESULT result;

        Integration::IntegrationSettings temp_settings(&local_settings);
        {
            std::stringstream s;
            s << get_Name() << "::E_cost_IM_calc(" << im << ")";
            temp_settings.warning_label = s.str();
        }

        result =  Integration::MAQ(
            [this, im] (double edp) -> double {
                double result;
                if (edp == 0) {
                    result = 0;
                } else {
                    std::function<double (double)> local_lambda = [this, im] (double x) {
                        double result = this->edp->P_exceedence(im, x);
                        return result;
                    };
                    gsl_function F;
                    F.function = (double (*)(double, void *))wrapper;
                    F.params = &local_lambda;
                    double deriv, abserror;
                    gsl_deriv_central(&F, edp, 1E-8, &deriv, &abserror);
                    if (std::isnan(deriv)) gsl_deriv_forward(&F, edp, 1E-8, &deriv, &abserror);
                    if (std::isnan(deriv)) gsl_deriv_backward(&F, edp, 1E-8, &deriv, &abserror);
                    
                    double d = deriv;
                    //double d = this->edp->P_exceedence(im, edp);
                    double p = this->E_cost_EDP(edp);
                    result = p * std::abs(d);
                }
                return result;
            }, temp_settings); 
        if (result.successful) {
            return result.integral;
        } else {
            return NAN;
        };
    }

    double CompGroup::SD_ln_cost_IM_calc(double im)
    {
        Integration::MAQ_RESULT result;
        Integration::IntegrationSettings temp_settings(&local_settings);
        {
            std::stringstream s;
            s << get_Name() << "::SD_ln_cost_IM_calc(" << im << ")";
            temp_settings.warning_label = s.str();
        }
        result =  Integration::MAQ(
            [this, im] (double edp) -> double {
                double result;
                if (edp == 0) {
                    result = 0;
                } else {
                    std::function<double (double)> local_lambda = [this, im] (double x) {
                        if (false) {
                            double pExceedence = this->edp->P_exceedence(im, x);
                            double pRepair = this->edp->Base_Rate()->pRepair(im);
                            return pExceedence * pRepair + (1.0 - pRepair);
                        } else {
                            double result = this->edp->P_exceedence(im, x);
                            return result;
                        }
                    };
                    gsl_function F;
                    F.function = (double (*)(double, void *))wrapper;
                    F.params = &local_lambda;
                    double deriv, abserror;
                    gsl_deriv_central(&F, edp, 1E-8, &deriv, &abserror);
                    if (std::isnan(deriv)) gsl_deriv_forward(&F, edp, 1E-8, &deriv, &abserror);
                    if (std::isnan(deriv)) gsl_deriv_backward(&F, edp, 1E-8, &deriv, &abserror);

                    double d = deriv;
                    //double d = this->edp->P_exceedence(im, edp);
                    double e = this->E_cost_EDP(edp) / this->count;
                    double sd;
                    sd = this->SD_cost_EDP(edp);
                    
                    result = (e * e + sd * sd) * std::abs(d);
                }
                return result;
            }, temp_settings); 
        if (result.successful) {
            double mean_x = E_cost_IM(im) / this->count;

            double sigma_x = sqrt(result.integral  - mean_x * mean_x);
            ///@todo What happens if sigma_x is NAN?
            if (std::isnan(sigma_x)) {
                sigma_x = 0;
            }
            double sigma_lnx = sqrt(log(1.0 + (sigma_x * sigma_x) / (mean_x * mean_x)));
            if (mean_x == 0) sigma_lnx = 0; ///@todo What should sigma_lnx be if mean_x is zero?
            return sigma_lnx;
        } else {
            return NAN;;
        };
    }

    double CompGroup::E_annual_cost_calc(void)
    {
        TempContext context([this] (std::ostream &o) {
                o << "CompGroup::E_annual_cost_calc() [" << this->name << "]";
            });
        
        Integration::IntegrationSettings temp_settings(&local_settings);
        {
            std::stringstream s;
            s << get_Name() << "::E_annual_cost_calc()";
            temp_settings.warning_label = s.str();
        }
        
        Integration::MAQ_RESULT result;
        result = Integration::MAQ(
            [this] (double im) -> double {
                double expected_cost = E_cost_IM(im);
                double deriv = std::abs(edp->Base_Rate()->DerivativeAt(im));
                return expected_cost * deriv;
            }, temp_settings);
        if (result.successful) {
            return result.integral;
        } else {
            return NAN;;
        }
    }
    double CompGroup::E_cost(int years, double discount_rate)
    {
        return ((1.0 - exp(-discount_rate * years)) / discount_rate) * E_annual_cost();
    }
    

    double CompGroup::lambda_cost_calc(double cost) 
    {
        Integration::IntegrationSettings temp_settings(&local_settings);
        {
            std::stringstream s;
            s << get_Name() << "::lambda_cost_calc(" << cost << ")";
            temp_settings.warning_label = s.str();
        }
        Integration::MAQ_RESULT result;
        result = Integration::MAQ(
            [this, cost] (double im) -> double {
                double mean_x = E_cost_IM(im);
                double sd_ln_x;
                sd_ln_x = SD_ln_cost_IM(im);

                LogNormalDist ln_fn = LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(mean_x, sd_ln_x);
                
                return ln_fn.p_at_least(cost) * std::abs(edp->Base_Rate()->DerivativeAt(im));
            }, temp_settings);
        if (result.successful) {
            return result.integral;
        } else {
            return NAN;
        }
    }

    double CompGroup::E_delay_IM_calc(double im)
    {
        Integration::IntegrationSettings temp_settings(&local_settings);
        {
            std::stringstream s;
            s << get_Name() << "::E_delay_IM_calc(" << im << ")";
            temp_settings.warning_label = s.str();
        }
        Integration::MAQ_RESULT result;
        result =  Integration::MAQ(
            [this, im] (double edp) -> double {
                double result;
                if (edp == 0) {
                    result = 0;
                } else {
                    std::function<double (double)> local_lambda = [this, im] (double x) {
                        double result = this->edp->P_exceedence(im, x);
                        return result;
                    };
                    gsl_function F;
                    F.function = (double (*)(double, void *))wrapper;
                    F.params = &local_lambda;
                    double deriv, abserror;
                    gsl_deriv_central(&F, edp, 1E-8, &deriv, &abserror);
                    if (std::isnan(deriv)) gsl_deriv_forward(&F, edp, 1E-8, &deriv, &abserror);
                    if (std::isnan(deriv)) gsl_deriv_backward(&F, edp, 1E-8, &deriv, &abserror);
                    
                    double d = deriv;
                    //double d = this->edp->P_exceedence(im, edp);
                    double p = this->E_delay_EDP(edp);
                    result = p * std::abs(d);
                }
                return result;
            }, temp_settings); 
        if (result.successful) {
            return result.integral;
        } else {
            return NAN;;
        };
    }

    double CompGroup::SD_ln_delay_IM_calc(double im)
    {
        Integration::IntegrationSettings temp_settings(&local_settings);
        {
            std::stringstream s;
            s << get_Name() << "::SD_ln_delay_IM_calc(" << im << ")";
            temp_settings.warning_label = s.str();
        }
        Integration::MAQ_RESULT result;
        result =  Integration::MAQ(
            [this, im] (double edp) -> double {
                double result;
                if (edp == 0) {
                    result = 0;
                } else {
                    std::function<double (double)> local_lambda = [this, im] (double x) {
                        if (false) {
                            double pExceedence = this->edp->P_exceedence(im, x);
                            double pRepair = this->edp->Base_Rate()->pRepair(im);
                            return pExceedence * pRepair + (1.0 - pRepair);
                        } else {
                            double result = this->edp->P_exceedence(im, x);
                            return result;
                        }
                    };
                    gsl_function F;
                    F.function = (double (*)(double, void *))wrapper;
                    F.params = &local_lambda;
                    double deriv, abserror;
                    gsl_deriv_central(&F, edp, 1E-8, &deriv, &abserror);
                    if (std::isnan(deriv)) gsl_deriv_forward(&F, edp, 1E-8, &deriv, &abserror);
                    if (std::isnan(deriv)) gsl_deriv_backward(&F, edp, 1E-8, &deriv, &abserror);

                    double d = deriv;
                    //double d = this->edp->P_exceedence(im, edp);
                    double e = this->E_delay_EDP(edp) / this->count;
                    double sd;
                    sd = this->SD_delay_EDP(edp);
                    
                    result = (e * e + sd * sd) * std::abs(d);
                }
                return result;
            }, temp_settings); 
        if (result.successful) {
            double mean_x = E_delay_IM(im) / this->count;

            double sigma_x = sqrt(result.integral  - mean_x * mean_x);
            ///@todo What happens if sigma_x is NAN?
            if (std::isnan(sigma_x)) {
                sigma_x = 0;
            }
            double sigma_lnx = sqrt(log(1.0 + (sigma_x * sigma_x) / (mean_x * mean_x)));
            if (mean_x == 0) sigma_lnx = 0; //sigma_x;
            return sigma_lnx;
        } else {
            return NAN;;
        };
    }

    std::shared_ptr<FragilityFn> CompGroup::FragFn(void)
    {
        return frag_fn;
    }

    double CompGroup::pDS_IM_calc(std::pair<int, double> params)
    {
        int i = params.first;
        double im = params.second;
        LogNormalDist dist = FragFn()->DamageStates()[i];

        Integration::IntegrationSettings temp_settings(&local_settings);
        {
            std::stringstream s;
            s << get_Name() << "::pDS_IM_calc(" << im << ")";
            temp_settings.warning_label = s.str();
        }
        Integration::MAQ_RESULT result;
        result =  Integration::MAQ(
            [this, im, dist] (double edp) -> double {
                double result;
                if (edp == 0) {
                    result = 0;
                } else {
                    std::function<double (double)> local_lambda = [this, im] (double x) {
                        double result = this->edp->P_exceedence(im, x);
                        return result;
                    };
                    gsl_function F;
                    F.function = (double (*)(double, void *))wrapper;
                    F.params = &local_lambda;
                    double deriv, abserror;
                    gsl_deriv_central(&F, edp, 1E-8, &deriv, &abserror);
                    if (std::isnan(deriv)) gsl_deriv_forward(&F, edp, 1E-8, &deriv, &abserror);
                    if (std::isnan(deriv)) gsl_deriv_backward(&F, edp, 1E-8, &deriv, &abserror);

                    double p = dist.p_at_most(edp);
                    result = p * std::abs(deriv);
                }
                return result;
            }, temp_settings); 
        
        if (result.successful) {
            return result.integral;
        } else {
            return 0; // NAN
        };
    }

    vector<double> CompGroup::pDS_IM(double im)
    {
        vector<double> results(FragFn()->n_states());
        for (size_t i=0; i < results.size(); i++) {
            results[i] = pDS_IM_calc(std::pair<int, double>(i, im));
        };
        return results;
    };
    
    vector<double> CompGroup::calc_Rate(void)
    {
        vector<double> results(FragFn()->n_states());
        for (size_t i=0; i < results.size(); i++) {
            // Integrate p(DS|IM) * dlambdaIM/dIM * dIM
            Integration::IntegrationSettings temp_settings(&local_settings);
            {
                std::stringstream s;
                s << get_Name() << "::calc_Rate()";
                temp_settings.warning_label = s.str();
            }
            Integration::MAQ_RESULT result;
            result =  Integration::MAQ(
                [this, i] (double im) -> double {
                    double result;
                    if (im == 0) {
                        result = 0;
                    } else {
                        double deriv = this->edp->Base_Rate()->DerivativeAt(im);
                        double p = this->pDS_IM_calc(std::pair<int, double>(i, im));
                        result = p * std::abs(deriv);
                    }
                    return result;
                }, temp_settings); 
            if (result.successful) {
                results[i] = result.integral;
            } else {
                results[i] = NAN;;
            };
        }
        return results;
    }

    std::ostream& operator<<(std::ostream& out, const CompGroup& o)
    {
        out << "CompGroup " << o.get_Name() 
            << ": " << o.count 
            << ", " << o.edp
            << ", " << o.frag_fn
            << ", " << o.cost_fn
            << ", " << o.delay_fn
            << std::endl;
        return out;
    }

}
