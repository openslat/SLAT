#include <iostream>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_deriv.h>
#include <functional>

class test_class {
public:
  double multiplier;
  std::function<double (double)> local_func;
  
  test_class(double x) {
    this->multiplier = x;
    this->local_func = [this] (double x) {
      //std::cout << "pre-function" << std::endl;
      //std::cerr << x << " * " << this->multiplier << " = " << (x * this->multiplier) << std::endl;
      return x * this->multiplier;
    };
  }
};

double wrapper(double x,  void *o)
{
  //  std::cout << "wrapper(" << x << ", " << f << ")" << std::endl;
  //std::cout << "";
  return ((test_class *)o)->local_func(x);
  //return (*f)(x);
}

double wrapper2(double x,  std::function<double (double)> *f)
{
  //std::cout << "";
  return (*f)(x);
}

double multiplier(double x, void *)
{
  return 1.5 * x;
}

int main(int argc, char **argv) 
{
  std::cout << "> main()" << std::endl;
  std::cout << "gsl test: " << std::endl;
  std::cout << gsl_cdf_lognormal_Q(0.1, 0, 1);
  std::cout << "end gsl test" << std::endl;

  {
    gsl_function F;
    F.function = multiplier;
    F.params = NULL;

    for (int i=0; i < 1; i++) {
      double result, abserror;
      gsl_deriv_forward(&F, i, 1E-8, &result, &abserror);
      std::cout << i << " --> " << result << std::endl;
    }    
  }

  {
    gsl_function F;
    F.function = multiplier;
    F.params = NULL;

    for (int i=0; i < 1; i++) {
      double result, abserror;
      gsl_deriv_forward(&F, i, 1E-8, &result, &abserror);
      std::cout << i << " --> " << result << std::endl;
    }    
  }

  std::cout << "--- interval ---" << std::endl;
  {
    test_class tc(1.5);
    gsl_function F;
    F.function = (double (*)(double, void *))wrapper;
    F.params = &tc;
    
    std::cout << "addr: " << &tc.local_func << ", "
	      << &F << ", " << (long)wrapper << std::endl;

    for (int i=0; i < 1; i++) {
      double result, abserror;
      gsl_deriv_forward(&F, i, 1E-8, &result, &abserror);
      std::cout << i << " --> " << result << std::endl;
    }    
  }

  std::cout << "--- interval ---" << std::endl;
  {
    test_class tc(1.5);
    gsl_function F;
    F.function = (double (*)(double, void *))wrapper2;
    F.params = &tc;
    
    std::cout << "addr: " << &tc.local_func << ", "
	      << &F << ", " << (long)wrapper << std::endl;

    for (int i=0; i < 1; i++) {
      double result, abserror;
      gsl_deriv_forward(&F, i, 1E-8, &result, &abserror);
      std::cout << i << " --> " << result << std::endl;
    }    
  }

  std::cout << "< main()" << std::endl;
  return 0;
}
