#ifndef TOOLS_HH
#define TOOLS_HH

/**
  @brief

  Detailed description follows here.

  @author	Boris Bulanek (),
  @date	03/23/13
  */
// =====================================================================================




#include	"tools.h"

using namespace std;

namespace TOOLS{




/*-----------------------------------------------------------------------------
     *
     *-----------------------------------------------------------------------------*/
double line(const double& x,const double* parameters ){
    return    parameters[0]+x*parameters[1];

}

/*-----------------------------------------------------------------------------
     *
     *-----------------------------------------------------------------------------*/
double quadratic(const double& x,const double* parameters ){
    return    parameters[0]+x*parameters[1] +pow(x,2)*parameters[2];
}

/*-----------------------------------------------------------------------------
     *
     *-----------------------------------------------------------------------------*/
double cubic(const double& x,const double* parameters ){
    return    parameters[0]+x*parameters[1] +pow(x,2)*parameters[2]+pow(x,3)*parameters[3];
}

/*-----------------------------------------------------------------------------
     *
     *-----------------------------------------------------------------------------*/
double myExp(const double& x,const double* parameters ){
    if (parameters[2]==0) return 0;
    double argument=-(x-parameters[1])/abs(parameters[2]);
    if (argument>1000) return 0;
    return    parameters[0]*(1-exp(argument));
}

/*-----------------------------------------------------------------------------
     *
     *-----------------------------------------------------------------------------*/
double expLine(const double& x,const double* parameters ){
    if (parameters[2]==0) return 0;
    return    myExp(x,parameters)+parameters[3]*x;
}


///*-----------------------------------------------------------------------------
// *
// *-----------------------------------------------------------------------------*/
int lineResidual(const gsl_vector* xGSL,void* params,gsl_vector* f){
    double parameters[2]={gsl_vector_get(xGSL,0),gsl_vector_get(xGSL,1)};

    DataFit* dataFit=(DataFit*)params;
    double* x=dataFit->x;
    double* y=dataFit->y;
    double* yErr=dataFit->yErr;

    int n=dataFit->n;
    for (int i = 0; i < n; ++i) {
        gsl_vector_set(f,i,(line(x[i],parameters)-y[i])/yErr[i]);
    }
    return GSL_SUCCESS;
}
/*-----------------------------------------------------------------------------
     *
     *-----------------------------------------------------------------------------*/
int lineDer(const gsl_vector* params,void* data,gsl_matrix* J){

    DataFit* dataFit=(DataFit*)data;
    double* x=dataFit->x;
    double* yErr=dataFit->yErr;
    int n=dataFit->n;
    for (int i = 0; i < n; ++i) {
        gsl_matrix_set(J,i,0,1/yErr[i]);
        gsl_matrix_set(J,i,1,x[i]/yErr[i]);
    }
    return GSL_SUCCESS;
}


/*-----------------------------------------------------------------------------
     *
     *-----------------------------------------------------------------------------*/
int line_fdf(const gsl_vector * params, void *data,
             gsl_vector * f, gsl_matrix * J)
{
    lineResidual(params, data, f);
    lineDer(params, data, J);

    return GSL_SUCCESS;
}
///*-----------------------------------------------------------------------------
// *
// *-----------------------------------------------------------------------------*/
int quadraticResidual(const gsl_vector* xGSL,void* params,gsl_vector* f){
    double parameters[3]={gsl_vector_get(xGSL,0),gsl_vector_get(xGSL,1),gsl_vector_get(xGSL,2)};
    DataFit* dataFit=(DataFit*)params;
    double* x=dataFit->x;
    double* y=dataFit->y;
    double* yErr=dataFit->yErr;

    int n=dataFit->n;
    for (int i = 0; i < n; ++i) {
        gsl_vector_set(f,i,(quadratic(x[i],parameters)-y[i])/yErr[i]);
    }
    return GSL_SUCCESS;
}

/*-----------------------------------------------------------------------------
     *
     *-----------------------------------------------------------------------------*/
int quadraticDer(const gsl_vector* params,void* data,gsl_matrix* J){

    DataFit* dataFit=(DataFit*)data;
    double* x=dataFit->x;
    double* yErr=dataFit->yErr;

    int n=dataFit->n;
    for (int i = 0; i < n; ++i) {
        gsl_matrix_set(J,i,0,1/yErr[i]);
        gsl_matrix_set(J,i,1,x[i]/yErr[i]);
        gsl_matrix_set(J,i,2,pow(x[i],2)/yErr[i]);
    }
    return GSL_SUCCESS;
}


/*-----------------------------------------------------------------------------
     *
     *-----------------------------------------------------------------------------*/
int quadratic_fdf(const gsl_vector * params, void *data,
                  gsl_vector * f, gsl_matrix * J)
{
    quadraticResidual(params, data, f);
    quadraticDer(params, data, J);

    return GSL_SUCCESS;
}
///*-----------------------------------------------------------------------------
// *
// *-----------------------------------------------------------------------------*/
int cubicResidual(const gsl_vector* xGSL,void* params,gsl_vector* f){
    double parameters[4]={gsl_vector_get(xGSL,0),gsl_vector_get(xGSL,1),
                          gsl_vector_get(xGSL,2),gsl_vector_get(xGSL,3)};

    DataFit* dataFit=(DataFit*)params;
    double* x=dataFit->x;
    double* y=dataFit->y;
    double* yErr=dataFit->yErr;

    int n=dataFit->n;
    for (int i = 0; i < n; ++i) {
        gsl_vector_set(f,i,(cubic(x[i],parameters)-y[i])/yErr[i]);
    }
    return GSL_SUCCESS;
}

/*-----------------------------------------------------------------------------
*
*-----------------------------------------------------------------------------*/
int cubicDer(const gsl_vector* params,void* data,gsl_matrix* J){

    DataFit* dataFit=(DataFit*)data;
    double* x=dataFit->x;
    double* yErr=dataFit->yErr;

    int n=dataFit->n;
    for (int i = 0; i < n; ++i) {
        gsl_matrix_set(J,i,0,1/yErr[i]);
        gsl_matrix_set(J,i,1,x[i]/yErr[i]);
        gsl_matrix_set(J,i,2,pow(x[i],2)/yErr[i]);
        gsl_matrix_set(J,i,3,pow(x[i],3)/yErr[i]);
    }

    return GSL_SUCCESS;
}


/*-----------------------------------------------------------------------------
*
*-----------------------------------------------------------------------------*/
int cubic_fdf(const gsl_vector * params, void *data,
              gsl_vector * f, gsl_matrix * J)
{
    cubicResidual(params, data, f);
    cubicDer(params, data, J);

    return GSL_SUCCESS;
}

///*-----------------------------------------------------------------------------
// *
// *-----------------------------------------------------------------------------*/
int expResidual(const gsl_vector* xGSL,void* params,gsl_vector* f){
    double parameters[3]={gsl_vector_get(xGSL,0),gsl_vector_get(xGSL,1), gsl_vector_get(xGSL,2)};

    DataFit* dataFit=(DataFit*)params;
    double* x=dataFit->x;
    double* y=dataFit->y;
    double* yErr=dataFit->yErr;

    int n=dataFit->n;
    for (int i = 0; i < n; ++i) {
        gsl_vector_set(f,i,(myExp(x[i],parameters)-y[i])/yErr[i]);
    }
    return GSL_SUCCESS;
}

/*-----------------------------------------------------------------------------
     *
     *-----------------------------------------------------------------------------*/
int expDer(const gsl_vector* params,void* data,gsl_matrix* J){
    double parameters[3]={gsl_vector_get(params,0),gsl_vector_get(params,1), gsl_vector_get(params,2)};

    DataFit* dataFit=(DataFit*)data;
    double* x=dataFit->x;
    double* yErr=dataFit->yErr;

    int n=dataFit->n;
    for (int i = 0; i < n; ++i) {
        double value=myExp(x[i],parameters);
        gsl_matrix_set(J,i,0,value/parameters[0]/yErr[i]);
        gsl_matrix_set(J,i,1,(value-parameters[0])/parameters[2]/yErr[i]);
        gsl_matrix_set(J,i,2, (value-parameters[0])*(x[i]-parameters[1])/pow(parameters[2],2)/yErr[i]);
    }

    return GSL_SUCCESS;
}


/*-----------------------------------------------------------------------------
     *
     *-----------------------------------------------------------------------------*/
int exp_fdf(const gsl_vector * params, void *data,
            gsl_vector * f, gsl_matrix * J)
{
    expResidual(params, data, f);
    expDer(params, data, J);

    return GSL_SUCCESS;
}
//
///*-----------------------------------------------------------------------------
// *
// *-----------------------------------------------------------------------------*/
int expLineResidual(const gsl_vector* xGSL,void* params,gsl_vector* f){
    double parameters[4]={gsl_vector_get(xGSL,0),gsl_vector_get(xGSL,1),
                          gsl_vector_get(xGSL,2),gsl_vector_get(xGSL,3)};

    DataFit* dataFit=(DataFit*)params;
    double* x=dataFit->x;
    double* y=dataFit->y;
    double* yErr=dataFit->yErr;

    int n=dataFit->n;
    for (int i = 0; i < n; ++i) {
        gsl_vector_set(f,i,(expLine(x[i],parameters)-y[i])/yErr[i]);
    }
    return GSL_SUCCESS;
}

/*-----------------------------------------------------------------------------
     *
     *-----------------------------------------------------------------------------*/
int expLineDer(const gsl_vector* params,void* data,gsl_matrix* J){
    double parameters[3]={gsl_vector_get(params,0),gsl_vector_get(params,1), gsl_vector_get(params,2)};

    DataFit* dataFit=(DataFit*)data;
    double* x=dataFit->x;
    double* yErr=dataFit->yErr;

    int n=dataFit->n;
    for (int i = 0; i < n; ++i) {
        double value=myExp(x[i],parameters);
        gsl_matrix_set(J,i,0,value/parameters[0]/yErr[i]);
        gsl_matrix_set(J,i,1,(value-parameters[0])/parameters[2]/yErr[i]);
        gsl_matrix_set(J,i,2, (value-parameters[0])*(x[i]-parameters[1])/pow(parameters[2],2)/yErr[i]);
        gsl_matrix_set(J,i,2, (value-parameters[0])*(x[i]-parameters[1])/pow(parameters[2],2)/yErr[i]);
        gsl_matrix_set(J,i,3,x[i]/yErr[i]);
    }
    return GSL_SUCCESS;
}


/*-----------------------------------------------------------------------------
     *
     *-----------------------------------------------------------------------------*/
int expLine_fdf(const gsl_vector * params, void *data,
                gsl_vector * f, gsl_matrix * J)
{
    expLineResidual(params, data, f);
    expLineDer(params, data, J);
    return GSL_SUCCESS;
}


/*-----------------------------------------------------------------------------
     *
     *-----------------------------------------------------------------------------*/
double fadLogaritmic(const double& x,const double* parameters ){
    return parameters[0]+parameters[1]*log(x);
}
double fadExponential(const double& x,const double* parameters ){
    return parameters[0]*pow(x,-parameters[1]);
}
double fadConstant(const double& x,const double* parameters ){return parameters[0];}
double eFadConstant(const double& x,const double* par,const double* ePar ){return ePar[0];}
double eFadLogaritmic(const double& x,const double* par,const double* ePar ){
    double retVal=0;
    retVal=pow(ePar[0],2)+pow(log(x)*ePar[1],2);//-2(log(x)*pow
    retVal=sqrt(retVal);
    return retVal;
}
double eFadExponential(const double& x,const double* par,const double* ePar){
    double retVal=0;
    retVal=pow(ePar[0]/par[0],2)+pow(log(x)*ePar[1],2);//-2*log(x)/par(0)*pow
    return sqrt(retVal)*fadExponential(x,par);
}
};

#endif
