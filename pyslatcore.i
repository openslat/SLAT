%module pyslatcore
%{
#include "pyslatcore.h"
%}
%include <std_vector.i>
%include <std_map.i>
namespace std{
    %template(VectorDouble) std::vector<double>;
}


void Init_Caching(void);
void IntegrationSettings(double tolerance, unsigned int max_evals);

enum FUNCTION_TYPE { NLH, PLC, LIN, LOGLOG };
enum LOGNORMAL_PARAM_TYPE { MEAN_X, MEDIAN_X, MEAN_LN_X, SD_X, SD_LN_X };

class DeterministicFn {
public:
    double ValueAt(double v);
};

DeterministicFn *factory(FUNCTION_TYPE t, std::vector<double>x, std::vector<double> y);
DeterministicFn *factory(FUNCTION_TYPE t, std::vector<double> params);

