#include <vector>
#include "functions.h"

void Init_Caching(void);
void IntegrationSettings(double tolerance, unsigned int max_evals);


enum FUNCTION_TYPE { NLH=0, PLC, LIN, LOGLOG };
enum LOGNORMAL_PARAM_TYPE { MEAN_X, MEDIAN_X, MEAN_LN_X, SD_X, SD_LN_X };


class DeterministicFn {
public:
    DeterministicFn();
    DeterministicFn(std::shared_ptr<SLAT::DeterministicFn> f);
    double ValueAt(double v);
    std::shared_ptr<SLAT::DeterministicFn> function;
};

DeterministicFn *factory(FUNCTION_TYPE t, std::vector<double>x, std::vector<double> y);
DeterministicFn *factory(FUNCTION_TYPE t, std::vector<double> params);

class ProbabilisticFn {
public:
    ProbabilisticFn();
    ProbabilisticFn(std::shared_ptr<SLAT::ProbabilisticFn> f);
    double P_exceedence(double x, double min_y);
    double X_at_exceedence(double x, double p);
    double Mean(double x);
    double MeanLn(double x);
    double Median(double x);
    double SD_ln(double x);
    double SD(double x);
private:
    std::shared_ptr<SLAT::ProbabilisticFn> function;
};

