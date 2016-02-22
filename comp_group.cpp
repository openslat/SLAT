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
    {
        this->edp = edp;
        this->frag_fn = frag_fn;
        this->loss_fn = loss_fn;
        this->count = count;
    };

    double CompGroup::E_loss_EDP(double edp)
    {
        LogNormalDist ln_fn = LogNormalDist::AddWeightedDistributions(
            loss_fn->LossFns(), frag_fn->pHighest(edp));
        return ln_fn.get_mean_X();
    }

    double CompGroup::SD_ln_loss_EDP(double edp)
    {
        LogNormalDist ln_fn = LogNormalDist::AddWeightedDistributions(
            loss_fn->LossFns(), frag_fn->pHighest(edp));
        return ln_fn.get_sigma_lnX();
    }

    double CompGroup::SD_loss_EDP(double edp)
    {
        LogNormalDist ln_fn = LogNormalDist::AddWeightedDistributions(
            loss_fn->LossFns(), frag_fn->pHighest(edp));
        return ln_fn.get_sigma_X();
    }

    static double wrapper(double x,  std::function<double (double)> *f)
    {
        return (*f)(x);
    }

    double CompGroup::E_loss_IM(double im)
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
            return NAN;
        };
    }

    double CompGroup::SD_ln_loss_IM(double im)
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
            return NAN;
        };
    };
}
