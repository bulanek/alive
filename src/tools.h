#ifndef TOOLS_H
#define TOOLS_H

/**
  @brief	

  Detailed description follows here.

  @author	Boris Bulanek (), 
  @date	04/07/13
  */
// =====================================================================================


#include	<sstream>
#include	<cmath>
#include	<string>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>

using namespace std;
namespace TOOLS{

    template <class T>
        string convertToString(const T value){
            stringstream strStream;
            strStream<<value;
            return strStream.str();
        }
    struct DataFit{
        double* x;
        double* y;
        double* xErr;
        double* yErr;
        int n;
    };

    typedef  int(*funcResidual)(const gsl_vector*,void*,gsl_vector*);
    typedef  int(*funcDer)(const gsl_vector* ,void* ,gsl_matrix* );
    typedef  int(*func_fdf)(const gsl_vector * , void *, gsl_vector * , gsl_matrix * );
    typedef  double(*func)(const double& ,const double* );

    typedef  double(*fadFunc)(const double&, const double* );
    typedef  double(*eFadFunc)(const double&, const double*, const double* );

    double line(const double& x,const double* parameters );
    double quadratic(const double& x,const double* parameters );
    double cubic(const double& x,const double* parameters );
    double myExp(const double& x,const double* parameters );
    double expLine(const double& x,const double* parameters );
    int lineResidual(const gsl_vector* xGSL,void* params,gsl_vector* f);
    int lineDer(const gsl_vector* params,void* data,gsl_matrix* J);
    int line_fdf(const gsl_vector * params, void *data, gsl_vector * f, gsl_matrix * J);
    int quadraticResidual(const gsl_vector* xGSL,void* params,gsl_vector* f);
    int quadraticDer(const gsl_vector* params,void* data,gsl_matrix* J);
    int quadratic_fdf(const gsl_vector * params, void *data, gsl_vector * f, gsl_matrix * J);
    int cubicResidual(const gsl_vector* xGSL,void* params,gsl_vector* f);
    int cubicDer(const gsl_vector* params,void* data,gsl_matrix* J);
    int cubic_fdf(const gsl_vector * params, void *data, gsl_vector * f, gsl_matrix * J) ;
    int expResidual(const gsl_vector* xGSL,void* params,gsl_vector* f);
    int expDer(const gsl_vector* params,void* data,gsl_matrix* J);
    int exp_fdf(const gsl_vector * params, void *data, gsl_vector * f, gsl_matrix * J);
    int expLineResidual(const gsl_vector* xGSL,void* params,gsl_vector* f);
    int expLineDer(const gsl_vector* params,void* data,gsl_matrix* J);
    int expLine_fdf(const gsl_vector * params, void *data, gsl_vector * f, gsl_matrix * J);
    double fadLogaritmic(const double& x,const double* parameters );
    double fadExponential(const double& x,const double* parameters );
    double fadConstant(const double& x,const double* parameters );
    double eFadConstant(const double& x,const double* par,const double* ePar );
    double eFadLogaritmic(const double& x,const double* par,const double* ePar );
    double eFadExponential(const double& x,const double* par,const double* ePar);
};
#endif
