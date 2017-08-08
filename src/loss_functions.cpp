/**
 * @file   loss_functions.cpp
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Mon Nov 16 15:29:29 2015
 * 
 * @brief  Classes representing loss functions.
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */
#include "loss_functions.h"
#include <vector>
#include <stdexcept>

namespace SLAT {

    LogNormalDist LossFn::CalculateLoss(std::vector<double> probabilities, int count)
    {
        throw std::invalid_argument("LossFn::CalculateLoss()");
        return LogNormalDist();
    }

    std::ostream& operator<<(std::ostream& out, LossFn& o)
    {
        out << "LossFn: ";
        out << std::endl;
        return out;
    }

    SimpleLossFn::SimpleLossFn(std::vector<LogNormalDist> distributions)
    {
        if (distributions.size() == 0) {
            throw std::invalid_argument("distributions");
        } else {
            loss_functions = distributions;
        }
    }

    std::size_t SimpleLossFn::n_states(void)
    {
        return loss_functions.size();
    }

    LogNormalDist SimpleLossFn::CalculateLoss(std::vector<double> probabilities, int count)
    {
        LogNormalDist result = LogNormalDist::AddWeightedDistributions(loss_functions,
                                                                       probabilities);
        return result.ScaleDistributionByNumComponents(count);
    }

    std::ostream& operator<<(std::ostream& out, SimpleLossFn& o)
    {
        out << "SimpleLossFn: ";
        for (size_t i=0; i < o.n_states(); i++) {
            out << o.loss_functions[i] << "; ";
        }
        out << std::endl;
        return out;
    }

    std::ostream& operator<<(std::ostream& out, BiLevelLoss& o)
    {
        out << "BiLevelLoss: " << o.cost_at_min << " @ " << o.count_min
            << "; " << o.cost_at_max << " @ " << o.count_max
            << "; dispersion=" << o.dispersion;
        return out;
    }

    BiLevelLossFn::BiLevelLossFn(std::vector<BiLevelLoss> distributions)
    {
        if (distributions.size() == 0) {
            throw std::invalid_argument("distributions");
        } else {
            this->distributions = distributions;
        }
    }

    LogNormalDist BiLevelLoss::LossFunctionsForCount_calc(int count)
    {
        double mean;
        if (count <= count_min) {
            mean = cost_at_min;
        } else if (count >= count_max) {
            mean = cost_at_max;
        } else {
            mean = cost_at_min + float(count - count_min)/(count_max - count_min) * (cost_at_max - cost_at_min);
        }
        return LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(mean, dispersion);
    }

        
    std::size_t BiLevelLossFn::n_states(void)
    {
        return distributions.size();
    }

    LogNormalDist BiLevelLossFn::CalculateLoss(std::vector<double> probabilities, int count)
    {
        std::vector<LogNormalDist> dists_for_count(distributions.size());
        for (unsigned int i=0; i < distributions.size(); i++) {
            dists_for_count[i] = distributions[i].LossFunctionsForCount(count);
        }
        LogNormalDist result = LogNormalDist::AddWeightedDistributions(dists_for_count,
                                                                       probabilities);
        return result.ScaleDistributionByNumComponents(count);
    }

    std::ostream& operator<<(std::ostream& out, BiLevelLossFn& o)
    {
        out << "BiLevelLossFn: ";
        for (size_t i=0; i < o.n_states(); i++) {
            out << "<" << o.distributions[i] << ">, ";
        }
        out << std::endl;
        return out;
    }
}
