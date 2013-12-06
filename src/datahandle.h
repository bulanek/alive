#ifndef DATAHANDLE_H
#define DATAHANDLE_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <string>

#include	<sstream>
#include <cmath>


#include <QString>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFile>
#include <QtXml>


#include "tools.h"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>

#include "BinReader.hh"
//#include "optimization.h"


using namespace std;



struct MainWindowData{
    string name;
    string dbName;
    string user;
    int rangeSignal[2];
    int rangeBackground[2];
    double IRR_Power;
};




enum Function{
    Linear,Quadratic,Cubic,Exp,ExpLine
};
enum TimeUnit{
    Seconds,Minutes,Hours,Days
};
enum FadFunction;
enum EFadFunction;

struct Data;



class DataHandle
{
public:
    static DataHandle* getInstance();
    //const vector<vector<int> >& getDataPoints();
    const vector<Data>& getData();
    int getConfigurationData(const string& confDataName);

    inline const double* getFadParameters(const int which) const{return _data[which].fadParameters;}
    inline const double* getEFadParameters(const int which) const{return _data[which].eFadParameters;
                                                                 }
    void setFadParameters(const double* parameters,gsl_matrix* fadCovariantMatrix,const FadFunction fadFunction, const int which);
    void setData(const vector<Data> data);
    void setDataNotPoints(const int which ,const Data &data);
    void setRangeSignalBackground(const int* signal,const int* background,const int which);
    inline void setDType(const int dtype,const int which){_data[which].Dtype=dtype;}
    inline void setIRRTime(const int irrTime,const int which){_data[which].IRR_Time=irrTime;}
    set<int> getWhichDType(const string& whichType);

    inline BinReader* getBinReader(){return _binReader;}
    int createData(string name);
    void createDb(const QString dbName,const QString userName="root",const QString hostName="localhost",const QString password="");

//    pair<double,double> computeOSLDose(const int which=-1,const double* signalRange=NULL,const double* backgroundRange=NULL,Function function=Linear);
    pair<double,double> computeGeneralDose(const pair<double,double>& signal,Function function,const double resolution=1e-4);
    pair<double,double> computeFadCorr (const int which,const FadFunction fadFunc ) const;
    //                pair<double,double> computeFadCorr (const int which,const bool isPreheat, TimeUnit unit=Hours) const;

    double getIntegral(const int which,const double* integralRange) const;
    std::pair<double,double> getSignal(const int which,const double* signalRange=NULL,const double* backgroundRange=NULL);

    inline void setMainWindowData(MainWindowData& data){_mainWindowData=data;}
    const double* computeCalibration(const double* signalRange=NULL,const double* backgroundRange=NULL,Function function=Linear);
    const double* chiSquareComputeGSL(const vector<pair<pair<double,double>,pair<double,double> > >& inputData,Function function);
    inline const double* getCalParameters() const{return _calibrationParameters;}
    inline const gsl_matrix* getCovariantMatrix() const{return _covariantMatrix;}
    inline QSqlDatabase& getDb() {return _db;}
    inline const MainWindowData& getMainWindowData() {return _mainWindowData;}
    inline double getDoseUnc(const double signal,const double* calibrationParameters) const{
        if (calibrationParameters[1]==0) return 0;
        return  (signal-calibrationParameters[0])/calibrationParameters[1];
    }
    inline const double* getInitialParameters()const{return _initialParameters;}
    inline void setInitialParameters(const double* initialParameters){
        for(int i=0;i<4;++i) _initialParameters[i]=initialParameters[i];
    }
    inline const vector<vector<int> >& getValues() const{return _values;}
    inline void setValues(vector<vector<int> >& values){ _values=values;}
    inline void setUsedMeasurements(const set<int> measurements){_usedMeasurements=measurements;}
    inline const set<int>& getUsedMeasurements(){ return _usedMeasurements;}
    inline void setFadCorrection(const pair<double,double>& fadCorrection){ _fadCorrection=fadCorrection;}
    inline const pair<double,double>& getFadCorrection() const{return _fadCorrection;}
    inline void setTimeUnit(const TimeUnit timeUnit){ _timeUnit=timeUnit;}
    inline TimeUnit getTimeUnit() const{return _timeUnit;}
    inline void setFunction(const Function function){ _function=function;}
    inline Function getFunction() const{return _function;}
    inline void setDatabasePath(const string& databasePath){_databasePath=databasePath;}
    inline const string& getDatabasePath() const{return _databasePath;}
    inline void setDatabaseName(const string& databaseName){_databaseName=databaseName;}
    inline const string& getDatabaseName() const{return _databaseName;}


    pair<double,double> getDoseCorrected(const pair<double,double>& doseUnc,const pair<double,double> fadCorrection=pair<double,double>(0,0));



    static const vector<string> FIT_FUNCTIONS;
    static const vector<string> FIT_FORMULA;
    static TOOLS::func getFunction(Function function);
    static TOOLS::fadFunc getFadFunction(FadFunction function);
    static TOOLS::eFadFunc getEFadFunction(EFadFunction function);

    static bool IS_NEW;

    friend ostream& operator<<(ostream& stream,const DataHandle& dataHandle);
    friend ostream& operator<<(ostream& stream,const DataHandle* dataHandle);
private:
    DataHandle(){};
    static DataHandle* instance;
    vector<vector<int> > _values;
    QSqlDatabase _db;
    QDomDocument _confDom;
    string _databaseName;
    string _databasePath;
    set<int> _calibrationSet;
    double _calibrationParameters[4];
    double _initialParameters[4];
    gsl_matrix* _covariantMatrix;
    BinReader* _binReader;
    vector<Data> _data;
    vector<Data> _usedData;
    set<int> _usedMeasurements;

    MainWindowData _mainWindowData;
    vector<pair<double,double> > _fadingParameters;
    pair<double,double> _fadCorrection;
    TimeUnit _timeUnit;
    Function _function;

}   ;

#endif // DATAHANDLE_H
