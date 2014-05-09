#include "datahandle.h"
#include <iostream>
#include <istream>
#include <fstream>
#include <vector>
#include <string>
#include <utility>
#include <climits>

using namespace std;

DataHandle* DataHandle::instance=0;
bool DataHandle::IS_NEW=1;

string fit_functions[]={"Linear","Quadratic","Cubic","Exp","ExpLine"};
const vector<string> DataHandle::FIT_FUNCTIONS=vector<string>(fit_functions,fit_functions+sizeof(fit_functions)/sizeof(string));

string fit_formula[]={"[0]+[1]*x","[0]+[1]*x+[2]*x^2","[0]+[1]*x+[2]*x^2+[3]*x^3","[0]*(1-exp(-(x-[1])/abs([2])))","exp_fun+[3]*x"};
const vector<string> DataHandle::FIT_FORMULA=vector<string>(fit_formula,fit_formula+sizeof(fit_formula)/sizeof(string));




/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
DataHandle* DataHandle::getInstance()
{
    if(instance==0){
        instance=new DataHandle();
        instance->_binReader=new BinReader();
        double initialParameters[4]={1,1,1,1};
        instance->setInitialParameters(initialParameters);
    }

    return instance;
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void DataHandle::setData(const vector<Data> data){
    _data=data;
    _usedMeasurements.clear();
    for (unsigned i = 0; i < _data.size(); ++i) {
        _usedMeasurements.insert(i);
    }
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
int DataHandle::getConfigurationData(const string& confDataName){
    QDomDocument doc("xml");
    QFile file(confDataName.c_str());

    if(!file.open(QFile::ReadOnly | QFile::Text)) return 0;

    if(!doc.setContent(&file)) {
        file.close();
        return 0;
    }
    file.close();
    _confDom=doc;
    QDomElement root=doc.documentElement();
    QDomNode n = root.firstChild();
    string databasePath,binPath;
    while( !n.isNull() )
    {
        QDomElement e = n.toElement();
        if( !e.isNull() )
        {
            string tagName=e.tagName().toStdString();
            string text=e.text().toStdString();
            if (tagName=="DatabasePath"){
                _databasePath=text;
            }else if(tagName=="DatabaseName"){
                _databaseName=text;
            }
        }
        n = n.nextSibling();
    }
    cout<<endl;
    cout<<"Obtained configuration file data from: "<<confDataName<<endl;
    cout<<"\t Database path: "<<_databasePath<<endl;
    //cout<<"\t Database name: "<<_databaseName<<endl;
    return 1;
}

void DataHandle::setRangeSignalBackground(const int* signal,const int* background,const int which){
    for (int i = 0; i < 2; ++i) {
        _data[which].rangeSignal[i]=signal[i];
        _data[which].rangeBackground[i]=background[i];
    }
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
int DataHandle::createData(string name){

    cout.flush();
    _data=_binReader->setData(name.c_str());
    const int numOfBytes=_binReader->getNumberOfBytes();
    int numberOfBytesNeeded=0;
    for (unsigned i = 0; i < _data.size(); ++i) {
        numberOfBytesNeeded+=BinReader::LENGTH_OF_HEADER+_data[i].NPoints*4;
    }
    if ((numberOfBytesNeeded!=numOfBytes)||numOfBytes==0){
        return 1;
    }

    cout.flush();
    string line;
    vector<string> lineArray;
    for(unsigned i=0;i<_values.size();++i){
        _values[i].clear();
    }
    _values.clear();
    _usedMeasurements.clear();

    vector<int> values;
    cout.flush();

    for (unsigned i = 0; i < _data.size(); ++i) {
        _usedMeasurements.insert(i);
        for (int j = 0; j < _data[i].NPoints; ++j) {
            values.push_back(_data[i].DPoints[j]);
        }
        _values.push_back(values);
        values.clear();
    }
    cout.flush();
    return 0;
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
const vector<Data>& DataHandle::getData(){
    return _data;
}
pair<double,double> DataHandle::getDoseCorrected(const pair<double,double>& doseUnc,const pair<double,double> fadCorrection){
    pair<double,double> usedFadCorrection=fadCorrection;
    if (fadCorrection.first==0) usedFadCorrection=_fadCorrection;
    double correctedDose=usedFadCorrection.first*doseUnc.first;
    double eCorDose=sqrt(pow(usedFadCorrection.first*doseUnc.second,2)+pow(doseUnc.first*usedFadCorrection.second,2));
    return pair<double,double>(correctedDose,eCorDose);
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
//const vector<vector<int> >& DataHandle::getDataPoints(){
//   if (_values.size()==0 ){
//        _values.clear();
//        vector<int> values;
//
//        for(unsigned i=0;i< _data.size();++i) {
//            for (int j = 0; j < _data[i].NPoints; ++j) {
//                values.push_back(_data[i].DPoints[j]);
//            }
//            _values.push_back(values);
//            values.clear();
//        }
//    }
//    return _values;
//}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void DataHandle::createDb(const QString dbName,const QString userName,const QString hostName,const QString password){

    if(!_db.open()){
        _db=QSqlDatabase::addDatabase("QSQLITE");
        //        _db.setHostName(hostName);
        //_db.setUserName(userName);
        //        _db.setPassword(password);
        _db.setDatabaseName(dbName);
    }else{
        //        _db.setHostName(hostName);
        //_db.setUserName(userName);
        _db.setDatabaseName(dbName);
    }
    bool ok=_db.open();
    cout<<"Connecting to db..."<<endl;

    if(ok){
        cout<<"In database: "<<dbName.toStdString()<<endl;
    }else{
        cout<<"Cannot open database "<<dbName.toStdString()<<endl;
    }
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
double DataHandle::getIntegral(const int which, const double* integralRange) const{

    double integral=0;

    for ( int i=integralRange[0];i<=integralRange[1] ;++i ) {
        integral+=_data[which].DPoints[i];
    }
    return  integral;
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
pair<double,double> DataHandle::getSignal(const int which,const double* signalRange,const double* backgroundRange){
    const double* usedSignalRange=signalRange;
    const double* usedBackgroundRange=backgroundRange;
    if (usedSignalRange==NULL){
        usedSignalRange=_data[which].rangeSignal;
        usedBackgroundRange=_data[which].rangeBackground;
    }


    double	signal=	this->getIntegral(which,usedSignalRange);
    double background=this->getIntegral(which,usedBackgroundRange);

    double returnSignal=signal-background;
    double returnSignalError=sqrt(double(abs(signal+background)));

    return pair<double,double>(returnSignal,returnSignalError);
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
const double* DataHandle::computeCalibration(const double* signalRange,const double* backgroundRange,Function function){
    const double* usedSignalRange=signalRange;
    const double* usedBackgroundRange=backgroundRange;

    vector<pair<pair<double,double>,pair<double,double> > > inputData;

    //    double dim=1;
    int i=0;
    for(set<int>::iterator it=_usedMeasurements.begin();it!=_usedMeasurements.end();++it) {
        i=*it;
        usedSignalRange=signalRange;
        usedBackgroundRange=backgroundRange;
        if (usedSignalRange==NULL){
            usedSignalRange=_data[i].rangeSignal;
            usedBackgroundRange=_data[i].rangeBackground;
        }

        if((Data::DTYPE[_data[i].Dtype].find("Dose")==string::npos && Data::DTYPE[_data[i].Dtype].find("dose")==string::npos) ||
                Data::DTYPE[_data[i].Dtype].find("Regenerative dose")!=string::npos) continue;
        //        pair<double,double> fadCorr=computeFadCorr(i,_data[i].fadFunction);
        //        dim=fadCorr.first;
        //    if (dim==0) dim=1;
        inputData.push_back(pair<pair<double,double>,pair<double,double> >(
                    pair<double,double>(_data[i].IRR_Time,1),this->getSignal(i,usedSignalRange,usedBackgroundRange)));
    }
    if(inputData.size()==1) inputData.push_back(pair<pair<double,double>,pair<double,double> >(
                pair<double,double>(0,1),pair<double,double>(0,1)));



    return this->chiSquareComputeGSL(inputData,function);
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
pair<double,double> DataHandle::computeFadCorr(const int which,FadFunction function) const{

    pair<double,double> fadCorr;
    int timeIrrM_t=_data[which].mDateTime-_data[which].IRR_DateTime;
    double aTime=timeIrrM_t;

    if(_timeUnit==Minutes){
        aTime/=60.;
    }else if(_timeUnit==Hours){
        aTime/=60*60.;
    }else if (_timeUnit==Days){
        aTime/=60*60*24.;
    }
    const double* fadPar=this->getFadParameters(which);
    const double* eFadPar=this->getEFadParameters(which);




    TOOLS::fadFunc usedFunction=this->getFadFunction(function);
    fadCorr.first=(*usedFunction)(aTime,fadPar);

    TOOLS::eFadFunc eUsedFunction=this->getEFadFunction((EFadFunction)(function));
    fadCorr.second=(*eUsedFunction)(aTime,fadPar,eFadPar);

    fadCorr.first=1/fadCorr.first;
    fadCorr.second=pow(fadCorr.first,2)*fadCorr.second;

    return fadCorr;
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/

void DataHandle::setFadParameters(const double* parameters,gsl_matrix* fadCovariantMatrix,const FadFunction fadFunction, const int which){
    for(int i=0;i<2;++i)  _data[which].fadParameters[i]=parameters[i];
    for (int i = 0; i < 2; ++i){
        for (int j = 0; j < 2; ++j){
            gsl_matrix_set(_data[which].fadCovariantMatrix,i,j,gsl_matrix_get(fadCovariantMatrix,i,j));
        }
        _data[which].eFadParameters[i]=sqrt(gsl_matrix_get(fadCovariantMatrix,i,i));
    }
    _data[which].fadFunction=fadFunction;
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
TOOLS::func DataHandle::getFunction(Function function){
    double(*usedFunction)(const double& ,const double* ) =NULL;
    if(function==Linear){
        usedFunction=&TOOLS::line;
    } else if (function==Quadratic){
        usedFunction=&TOOLS::quadratic;
    } else if (function==Cubic){
        usedFunction=&TOOLS::cubic;
    } else if (function==Exp){
        usedFunction=&TOOLS::myExp;
    } else if (function==ExpLine){
        usedFunction=&TOOLS::expLine;
    } else
    { cerr<<"Error in DataHandle::getFunction"<<endl;
        assert(0);
    }
    return usedFunction;
}



/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
TOOLS::fadFunc DataHandle::getFadFunction(FadFunction function){
    double(*usedFunction)(const double&,const double*)=NULL;
    if(function==Logaritmic){
        usedFunction=&TOOLS::fadLogaritmic;
    }else if (function==Exponential){
        usedFunction=&TOOLS::fadExponential;
    }else if(function==Constant){
        usedFunction=&TOOLS::fadConstant;
    }else{
        cerr<<"Error in DataHandle::getFadFunction"<<endl;
        assert(usedFunction);
    }
    return usedFunction;
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
TOOLS::eFadFunc DataHandle::getEFadFunction(EFadFunction function){
    double(*usedFunction)(const double&,const double* ,const double*)=NULL;
    if(function==eLogaritmic){
        usedFunction=&TOOLS::eFadLogaritmic;
    }else if (function==eExponential){
        usedFunction=&TOOLS::eFadExponential;
    }else if(function==eConstant){
        usedFunction=&TOOLS::eFadConstant;
    }else{
        cerr<<"Error in DataHandle::getEFadFunction"<<endl;
        assert(usedFunction);
    }
    return usedFunction;
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void print_state (size_t iter, gsl_multifit_fdfsolver* s)
{
    printf ("iter: %3u x = % 15.8f % 15.8f % 15.8f "
            "|f(x)| = %g\n",
            iter,
            gsl_vector_get (s->x, 0),
            gsl_vector_get (s->x, 1),
            double(gsl_blas_dnrm2 (s->f)));
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
const double* DataHandle::chiSquareComputeGSL(const vector<pair<pair<double,double>,pair<double,double> > >& inputData,Function function){

    const gsl_multifit_fdfsolver_type *T;
    gsl_multifit_fdfsolver *s;
    int status;
    size_t iter=0;

    int(*usedFuncResidual)(const gsl_vector*,void*,gsl_vector*) =NULL;
    int(*usedFuncDer)(const gsl_vector*,void*,gsl_matrix*) =NULL;
    int(*usedFunc_fdf)(const gsl_vector*,void*,gsl_vector*,gsl_matrix*) =NULL;
    size_t p=0;


    if(function==Linear){
        usedFuncResidual=&TOOLS::lineResidual;
        usedFuncDer=&TOOLS::lineDer;
        usedFunc_fdf=&TOOLS::line_fdf;
        p=2;
    } else if (function==Quadratic){
        usedFuncResidual=&TOOLS::quadraticResidual;
        usedFuncDer=&TOOLS::quadraticDer;
        usedFunc_fdf=&TOOLS::quadratic_fdf;
        p=3;
    } else if (function==Cubic){
        usedFuncResidual=&TOOLS::cubicResidual;
        usedFuncDer=&TOOLS::cubicDer;
        usedFunc_fdf=&TOOLS::cubic_fdf;
        p=4;

    } else if (function==Exp){
        usedFuncResidual=&TOOLS::expResidual;
        usedFuncDer=&TOOLS::expDer;
        usedFunc_fdf=&TOOLS::exp_fdf;
        p=3;
    } else if (function==ExpLine){
        usedFuncResidual=&TOOLS::expLineResidual;
        usedFuncDer=&TOOLS::expLineDer;
        usedFunc_fdf=&TOOLS::expLine_fdf;
        p=4;
    } else
    { cerr<<"Error in DataHandle::chiSquareCompute"<<endl;
        assert(0);
    }

    const size_t n=inputData.size();

    gsl_matrix* covar=gsl_matrix_alloc(p,p);
    gsl_multifit_function_fdf fun;

    gsl_vector_view x=gsl_vector_view_array(_initialParameters,p);

    TOOLS::DataFit data;
    data.n=inputData.size();
    data.x=new double[data.n];
    data.y=new double[data.n];
    data.yErr=new double[data.n];
    for (int i = 0; i < data.n; ++i) {
        data.x[i]=inputData[i].first.first;
        data.y[i]=inputData[i].second.first;
        data.yErr[i]=inputData[i].second.second;
    }

    fun.f=usedFuncResidual;
    fun.fdf=usedFunc_fdf;
    fun.df=usedFuncDer;
    fun.n=n;
    fun.p=p;
    fun.params=&data;

    if( n<p ){
        throw 1;
    }

    T=gsl_multifit_fdfsolver_lmder;
    s=gsl_multifit_fdfsolver_alloc(T,n,p);
    gsl_multifit_fdfsolver_set(s,&fun,&x.vector);

    print_state(iter,s);

    do
    {
        iter++;
        status = gsl_multifit_fdfsolver_iterate(s);
        print_state (iter, s);
        printf ("status = %s\n", gsl_strerror (status));

        print_state (iter, s);
        status = gsl_multifit_test_delta (s->dx, s->x,
                1e-4, 1e-4);

    }
    while (iter < 500&& status==GSL_CONTINUE);



    double chi = gsl_blas_dnrm2(s->f);


    cout<<"chi: "<<chi<<endl;


    gsl_multifit_covar (s->J, 0.0, covar);

    gsl_matrix_fprintf (stdout, covar, "%g");

#define FIT(i) gsl_vector_get(s->x, i)
#define ERR(i) sqrt(gsl_matrix_get(covar,i,i))

    printf ("status = %s\n", gsl_strerror (status));
    for (unsigned i=0;i<p;++i) {
        _calibrationParameters[i]=FIT(i);
    }
    _covariantMatrix=covar;
    gsl_multifit_fdfsolver_free(s);
    return _calibrationParameters;
}



/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
pair<double,double> DataHandle::computeGeneralDose(const pair<double,double>& signal,Function function,const double resolution){
    TOOLS::func usedFunction=this->getFunction(function);

    double x=0;
    double funOut=0;
    //    if (signal<0) {
    //        cout<<"Throwing int in DataHandle::computeGeneralDose"<<endl;
    //        throw 1;
    //    }
    int counter=0;
    int counterMax=int(1000/resolution);

    if(funOut<signal.first){
        while(funOut<signal.first &&counter<counterMax){
            x+=resolution;
            funOut=(*usedFunction)(x,_calibrationParameters);
            ++counter;
        }
    }else{
        while(funOut>signal.first &&counter<counterMax){
            x-=resolution;
            funOut=(*usedFunction)(x,_calibrationParameters);
            ++counter;
        }
    }
    double eps=resolution;

    double derivation=((*usedFunction)(x+eps,_calibrationParameters)-(*usedFunction)(x-eps,_calibrationParameters))/(2.*eps);
    pair<double,double> outputPair;
    if(derivation!=0) outputPair.second=abs(signal.second/derivation);
    outputPair.first=x;


    return outputPair;
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void DataHandle::setDataNotPoints(const int which ,const Data &data){
    _data[which].Run=data.Run;
    _data[which].Set=data.Set;
    for (unsigned i = 0; i < data.Comment.size(); ++i) {
        if(i>=(int)Data::COMMENT_SIZE) break;
        _data[which].Comment[i]=data.Comment[i];
    }
    for (unsigned i = 0; i <data.Sample.size(); ++i) {
        if(i>=(int)Data::SAMPLE_SIZE) break;
        _data[which].Sample[i]=data.Sample[i];
    }
    for (unsigned i = 0; i <data.User.size(); ++i) {
        if(i>=(int)Data::USER_SEQUENCE_SIZE) break;
        _data[which].User[i]=data.User[i];
    }
    for (unsigned i = 0; i <data.Sequence.size(); ++i) {
        if(i>=(int)Data::USER_SEQUENCE_SIZE) break;
        _data[which].Sequence[i]=data.Sequence[i];
    }
    _data[which].Position=data.Position;
    _data[which].Grain=data.Grain;
    _data[which].mDateTime=data.mDateTime;
    _data[which].IRR_DateTime=data.IRR_DateTime;
    _data[which].An_Temp=data.An_Temp;
    _data[which].Temperature=data.Temperature;
    _data[which].BG=data.BG;
    _data[which].Low=data.Low;
    _data[which].High=data.High;
    _data[which].Rate=data.Rate;
    _data[which].XCoord=data.XCoord;
    _data[which].YCoord=data.YCoord;
    _data[which].LightPower=data.LightPower;
    _data[which].EnableFlags=data.EnableFlags;
    _data[which].Shift=data.Shift;
    _data[which].On=data.On;
    _data[which].Off=data.Off;
    _data[which].Delay=data.Delay;
    _data[which].OnTime=data.OnTime;
    _data[which].OffTime=data.OffTime;
    _data[which].OnGateDelay=data.OnGateDelay;
    _data[which].OffGateDelay=data.OffGateDelay;
    _data[which].Norm1=data.Norm1;
    _data[which].Norm2=data.Norm2;
    _data[which].Norm3=data.Norm3;
    _data[which].An_Time=data.An_Time;
    _data[which].Bl_Time=data.Bl_Time;
    _data[which].Dtype=data.Dtype;
    _data[which].LType=data.LType;
    _data[which].IRR_Type=data.IRR_Type;
    _data[which].IRR_Unit=data.IRR_Unit;
    _data[which].Bl_Unit=data.Bl_Unit;
    _data[which].LightSource=data.LightSource;
    _data[which].IRR_Time=data.IRR_Time;
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
set<int> DataHandle::getWhichDType(const string& whichType){
    set<int> whichDType;
    for(set<int>::iterator it=_usedMeasurements.begin();it!=_usedMeasurements.end();++it) {
        if(Data::DTYPE[_data[*it].Dtype]==whichType){
            whichDType.insert(*it);
        }
    }
    return whichDType;
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
ostream& operator<<(ostream& stream,const DataHandle& dataHandle){
    const MainWindowData mainData=dataHandle._mainWindowData;
    stream<<"Main information: "<<endl;
    stream<<"\tName: "<<mainData.name<< endl;
    stream<<"\tNumber of measurements: "<<dataHandle._data.size()<<endl;
    stream<<"\tPower of calibration source: "<<mainData.IRR_Power<<" Gy/s"<<endl;
    stream<<"Informations of particular measurement: "<<endl;
    for (unsigned i = 0; i < dataHandle._data.size(); ++i) {
        stream<<" Information for measurement "<<i+1<<":"<<endl;
        stream<<dataHandle._data[i]<<endl;
    }
    stream<<"Data points of measurements: "<<endl;
    for (unsigned i= 0; i< dataHandle._data.size(); ++i) {
        for (int j = 0; j < dataHandle._data[i].NPoints;++j) {
            stream<<dataHandle._data[i].DPoints[j]<<" ";
        }
        stream<<endl;
    }
    return stream;
}
ostream& operator<<(ostream& stream,const DataHandle* dataHandle){
    stream<<*dataHandle<<endl;
    return stream;
}
