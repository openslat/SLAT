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
        if (&o) {
            out << o.to_string();
        } else {
            out << "<NULL POINTER TO LossFn>";
        }
        return out;
    }

    std::string LossFn::to_string(void) 
    {
        std::stringstream out;
        out << "LossFn: ";
        out << std::endl;
        return out.str();
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

    std::string SimpleLossFn::to_string()
    {
        std::stringstream out;
        out << "SimpleLossFn: ";
        for (size_t i=0; i < this->n_states(); i++) {
            out << this->loss_functions[i] << "; ";
        }
        out << std::endl;
        return out.str();
    }

    BiLevelLoss::BiLevelLoss(int count_min, int count_max, double cost_at_min, double cost_at_max, double dispersion)
            : LossFunctionsForCount([this] (int count) {
                    return this->LossFunctionsForCount_calc(count);
                }, std::string("BiLevelLoss::LossFunctionsForCount"))
    {
        this->count_min = count_min;
        this->count_max = count_max;
        this->cost_at_min = cost_at_min;
        this->cost_at_max = cost_at_max;
        this->dispersion = dispersion;
    };

    std::string BiLevelLoss::to_string()
    {
        std::stringstream out;
        out << "BiLevelLoss: " << this->cost_at_min << " @ " << this->count_min
            << "; " << this->cost_at_max << " @ " << this->count_max
            << "; dispersion=" << this->dispersion;
        return out.str();
    }

    BiLevelLossFn::BiLevelLossFn(std::vector<std::shared_ptr<BiLevelLoss>> distributions) :
        distributions(distributions)
    {
        if (distributions.size() == 0) {
            throw std::invalid_argument("distributions");
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
            mean = cost_at_min + double(count - count_min)/(count_max - count_min) * (cost_at_max - cost_at_min);
        }
        return LogNormalDist::LogNormalDist_from_mean_X_and_sigma_lnX(mean, dispersion);
    }

        
    std::size_t BiLevelLossFn::n_states(void)
    {
        return distributions.size();
    }

    LogNormalDist BiLevelLossFn::CalculateLoss(std::vector<double> probabilities, int count)
    {
        if (probabilities.size() != distributions.size()) {
            throw std::invalid_argument("BiLevelLossFn::CalculateLoss()");
        }

        std::vector<LogNormalDist> dists_for_count(distributions.size());
        for (unsigned int i=0; i < distributions.size(); i++) {
            dists_for_count[i] = distributions[i]->LossFunctionsForCount(count);
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
