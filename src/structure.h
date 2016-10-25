/**
 * @file   structure.h
 * @author Michael Gauland <michael.gauland@canterbury.ac.nz>
 * @date   Wed Feb 03 10:04:50 NZDT 2016
 *
 * @brief  Classes representing structures (composed on component groups).
 * 
 * This file part of SLAT (the Seismic Loss Assessment Tool).
 *
 * ©2015 Canterbury University
 */
#ifndef _STRUCTURE_H_
#define _STRUCTURE_H_

#include "comp_group.h"
#include "lognormaldist.h"
#include "maq.h"

namespace SLAT {
    class Structure : public Replaceable<Structure> {
    public:
        Structure(std::string name);
        ~Structure() {};

        void AddCompGroup(std::shared_ptr<CompGroup> cg);

        LogNormalDist Cost(double im, bool consider_collapse);
        LogNormalDist TotalCost(double im);
        typedef struct { LogNormalDist repair, demolition, collapse; } COSTS;
        COSTS CostsByFate(double im);
        Caching::CachedValue<LogNormalDist> AnnualCost;
        std::pair<LogNormalDist, LogNormalDist> DeaggregatedCost(double im);
        void setRebuildCost(LogNormalDist dist) { rebuild_cost = dist; };
        LogNormalDist getRebuildCost(void) { return rebuild_cost; };
        void setDemolitionCost(LogNormalDist dist) { demolition_cost = dist; };
        LogNormalDist getDemolitionCost(void) { return demolition_cost; };
        const std::vector<std::shared_ptr<CompGroup>> Components(void)
        {
            return components;
        };
    private:
        std::string name;
        LogNormalDist calc_AnnualCost(void);
        LogNormalDist CostNC(double im);
        std::vector<std::shared_ptr<CompGroup>> components;
        std::shared_ptr<IM> im;
        LogNormalDist rebuild_cost;
        LogNormalDist demolition_cost;
    };
}
#endif
