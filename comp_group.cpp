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

namespace SLAT {
    CompGroup::CompGroup(std::shared_ptr<CompoundRateRelationship> edp,
                         std::shared_ptr<FragilityFn> frag_fn, 
                         std::shared_ptr<LossFn> loss_fn,
                         int count)
        : loss_EDP_dist([this] (double edp) {
                return LogNormalDist::AddWeightedDistributions(this->loss_fn->LossFns(), 
                                                               this->frag_fn->pHighest(edp)); 
            }, true)//,
          // E_loss_IM([this] (double im) {
          //         return this->E_loss_IM_calc(im);
          //     }, true)//,
          // SD_ln_loss_IM([this] (double im) {
          //         return this->SD_ln_loss_IM_calc(im);
          //     }, true),
          // E_annual_loss([this] (void) {
          //         return this->E_annual_loss();
          //     }),
          // lambda_loss([this] (double loss) {
          //         return this->lambda_loss_calc(loss);
          //     }, true)
    {
        this->edp = edp;
        this->frag_fn = frag_fn;
        this->loss_fn = loss_fn;
        this->count = count;

        E_loss_IM.func = [this] (double im) {
            return this->E_loss_IM_calc(im);
        };
        E_loss_IM.cache_active = true;
        
        SD_ln_loss_IM.func = [this] (double im) {
            return this->SD_ln_loss_IM_calc(im);
        };
        SD_ln_loss_IM.cache_active = true;

        E_annual_loss.func = [this] (void) {
            return this->E_annual_loss_calc();
        };

        lambda_loss.func = [this] (double loss) {
            return this->lambda_loss_calc(loss);
        };
        lambda_loss.cache_active = true;
    };

    double CompGroup::E_loss_EDP(double edp)
    {
        return loss_EDP_dist(edp).get_mean_X();
    }

    double CompGroup::SD_ln_loss_EDP(double edp)
    {
        return loss_EDP_dist(edp).get_sigma_lnX();
    }

    double CompGroup::SD_loss_EDP(double edp)
    {
        return loss_EDP_dist(edp).get_sigma_X();
    }

    static double wrapper(double x,  std::function<double (double)> *f)
    {
        return (*f)(x);
    }

    double CompGroup::E_loss_IM_calc(double im)
    {
        Integration::MAQ_RESULT result;
        result =  Integration::MAQ(
            [this, im] (double edp) -> double {
                double result;
                if (edp == 0) {
                    result = 0;
                } else {
                    std::function<double (double, void*)> local_lambda = [this, im] (double x, void *) {
                        double result = this->edp->P_exceedence(im, x);
                        return result;
                    };
                    gsl_function F;
                    F.function = (double (*)(double, void *))wrapper;
                    F.params = &local_lambda;
                    double deriv, abserror;
                    gsl_deriv_central(&F, edp, 1E-8, &deriv, &abserror);
                    if (isnan(deriv)) gsl_deriv_forward(&F, edp, 1E-8, &deriv, &abserror);
                    if (isnan(deriv)) gsl_deriv_backward(&F, edp, 1E-8, &deriv, &abserror);

                    double d = deriv;
                    //double d = this->edp->P_exceedence(im, edp);
                    double p = this->E_loss_EDP(edp);
                    result = p * std::abs(d);
                }
                return result;
            }, local_settings); 
        if (result.successful) {
            return result.integral;
        } else {
            return 0; //NAN;;
        };
    }

    double CompGroup::SD_ln_loss_IM_calc(double im)
    {
        Integration::MAQ_RESULT result;
        result =  Integration::MAQ(
            [this, im] (double edp) -> double {
                double result;
                if (edp == 0) {
                    result = 0;
                } else {
                    std::function<double (double, void*)> local_lambda = [this, im] (double x, void *) {
                        double result = this->edp->P_exceedence(im, x);
                        return result;
                    };
                    gsl_function F;
                    F.function = (double (*)(double, void *))wrapper;
                    F.params = &local_lambda;
                    double deriv, abserror;
                    gsl_deriv_central(&F, edp, 1E-8, &deriv, &abserror);
                    if (isnan(deriv)) gsl_deriv_forward(&F, edp, 1E-8, &deriv, &abserror);
                    if (isnan(deriv)) gsl_deriv_backward(&F, edp, 1E-8, &deriv, &abserror);

                    double d = deriv;
                    //double d = this->edp->P_exceedence(im, edp);
                    double e = this->E_loss_EDP(edp);
                    double sd = this->SD_loss_EDP(edp);
                    result = (e * e + sd * sd) * std::abs(d);
                }
                return result;
            }, local_settings); 
        if (result.successful) {
            double mean_x = E_loss_IM(im);
            double sigma_x = sqrt(result.integral  - mean_x * mean_x);
            double sigma_lnx = sqrt(log(1.0 + (sigma_x * sigma_x) / (mean_x * mean_x)));
            return sigma_lnx;
        } else {
            return 0; //NAN;;
        };
    };

    double CompGroup::E_annual_loss_calc(void)
    {
        Integration::MAQ_RESULT result;
        result = Integration::MAQ(
            [this] (double im) -> double {
                double expected_loss = E_loss_IM(im);
                double deriv = std::abs(edp->Base_Rate()->DerivativeAt(im));
                return expected_loss * deriv;
            }, local_settings);
        if (result.successful) {
            return result.integral;
        } else {
            return 0; //NAN;;
        }
    }
    double CompGroup::E_loss(int years, double discount_rate)
    {
        return ((1.0 - exp(-discount_rate * years)) / discount_rate) * E_annual_loss();
    }
    

    double CompGroup::lambda_loss_calc(double loss) 
    {
        Integration::MAQ_RESULT result;
        result = Integration::MAQ(
            [this, loss] (double im) -> double {
                double mean_x = E_loss_IM(im);
                double sd_ln_x = SD_ln_loss_IM(im);
                LogNormalDist ln_fn = LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(mean_x, sd_ln_x);
                
                return ln_fn.p_at_least(loss) * std::abs(edp->Base_Rate()->DerivativeAt(im));
            }, local_settings);
        if (result.successful) {
            return result.integral;
        } else {
            return 0; //NAN;
        }
    }
}
