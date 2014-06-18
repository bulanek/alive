#include "doseresults.h"
#include "ui_doseresults.h"
#include	<iomanip>

int DoseResults::WHICH=-1;

DoseResults::DoseResults(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DoseResults)
{
    ui->setupUi(this);
    this->setWindowTitle("Dose results");
    _dataHandle=DataHandle::getInstance();

    const vector<Data> data=_dataHandle->getData();
    //pair<double,double> doseUnc=_dataHandle->computeOSLDose(WHICH);
    pair<double,double> signal=_dataHandle->getSignal(WHICH);
    pair<double,double> doseUnc = _dataHandle->computeGeneralDose(signal,_dataHandle->getFunction());

    const pair<double,double> fadCorrectionNatural=_dataHandle->getFadCorrection();
    int whichCal=0;
    foreach(int i,_dataHandle->getUsedMeasurements()){
        if(Data::DTYPE[data[i].Dtype].find("Dose")!=string::npos ||Data::DTYPE[data[i].Dtype].find("dose")!=string::npos){
            whichCal=i;
            break;
        }
    }

    pair<double,double> fadCorrectionDose=_dataHandle->computeFadCorr(whichCal,data[whichCal].fadFunction);



    const MainWindowData mainData=_dataHandle->getMainWindowData();


    double corrDose=(1/fadCorrectionNatural.first/fadCorrectionDose.first)*doseUnc.first;
    double eCorDose=corrDose*sqrt(pow(doseUnc.second/doseUnc.first,2)+ pow(fadCorrectionNatural.second/fadCorrectionNatural.first,2)+
                                  pow(fadCorrectionDose.second/fadCorrectionDose.first,2));
    pair<double,double> correctedDose=pair<double,double>(corrDose,eCorDose);

    stringstream str;
    str<<setprecision(6);
    str<<fadCorrectionNatural.first;
    ui->fad_v->setText(QString(str.str().c_str()));
    str.str("");
    str<<fadCorrectionNatural.second;
    ui->fad_e->setText(QString(str.str().c_str()));
    str.str("");
    str<<fadCorrectionDose.first;
    ui->cal_fad_v->setText(QString(str.str().c_str()));
    str.str("");
    str<<fadCorrectionDose.second;
    ui->cal_fad_e->setText(QString(str.str().c_str()));
    str.str("");

    str<<correctedDose.first*mainData.IRR_Power;
    ui->dose_fad_v->setText(QString(str.str().c_str()));
    str.str("");
    str<<correctedDose.second*mainData.IRR_Power;
    ui->dose_fad_e->setText(QString(str.str().c_str()));
    str.str("");

    str<<doseUnc.first*mainData.IRR_Power;
    ui->dose_no_fad_v->setText(QString(str.str().c_str()));
    str.str("");
    str<<doseUnc.second*mainData.IRR_Power;
    ui->dose_no_fad_e->setText(QString(str.str().c_str()));

}

DoseResults::~DoseResults()
{
    delete ui;
}
