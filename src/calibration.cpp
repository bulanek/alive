
#include "calibration.h"
#include "ui_calibration.h"
#include <sstream>


#include	"doseresults.h"
struct OSLWindowData;


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
Calibration::Calibration(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Calibration)
{
    ui->setupUi(this);
    this->setWindowTitle("Data graph");
    _dataHandle=DataHandle::getInstance();
    ui->widget->addAction(ui->actionSavePlot);
    for (unsigned i  = 0; i  < DataHandle::FIT_FUNCTIONS.size(); ++i ) {
        ui->comboBox->addItem(DataHandle::FIT_FUNCTIONS[i].c_str());
    }
    ui->comboBox->setCurrentIndex(0);
    ui->comboBox_2->setCurrentIndex(2);
    _dataHandle->setTimeUnit((TimeUnit)(2));


    this->getCalibrationParameters();
    this->showTable();

    this->on_actionUpdateGraph_triggered();
    connect(ui->tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(tableChanged(QTableWidgetItem*)));


    /* graph interactions*/
    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                QCP::iSelectLegend | QCP::iSelectPlottables);
    ui->widget->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    ui->widget->legend->setFont(legendFont);
    ui->widget->legend->setSelectedFont(legendFont);
    ui->widget->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items


    connect(ui->widget, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));
    // connect slot that ties some axis selections together (especially opposite axes):
    connect(ui->widget, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    connect(ui->widget, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));


}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
Calibration::~Calibration()
{
    delete ui;
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void Calibration::accept(){
    this->saveTableData();
    this->done(1);
    this->hide();
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
Ui::Calibration* Calibration::getUi(){
    return ui;
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void Calibration::showData(){

    this->saveTableData();

    QCustomPlot* plotWidget=ui->widget;
    plotWidget->clearGraphs();
//    plotWidget->legend->setPositionStyle((QCPLegend::PositionStyle)(1));
    plotWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                  QCP::iSelectLegend | QCP::iSelectPlottables);

    QCPAxisRect axis(plotWidget);
    axis.setRangeDrag(Qt::Horizontal | Qt::Vertical);
    axis.setRangeZoom(Qt::Horizontal | Qt::Vertical);

    plotWidget->xAxis->setLabel("Dose [s]");
    plotWidget->yAxis->setLabel("Signal");
    plotWidget->addGraph();
    plotWidget->graph(0)->setLineStyle(QCPGraph::lsNone);
    plotWidget->graph(0)->setScatterStyle(QCPScatterStyle::ssCrossCircle);


    QVector<double> x;
    QVector<double> y;
    QVector<double> yErr;
    QVector<pair<int,pair<double,double> > > measurementsV;

    const vector<Data> data=_dataHandle->getData();
    const MainWindowData mainData=_dataHandle->getMainWindowData();

    int counter=-1;
    const set<int> usedMeasurements=_dataHandle->getUsedMeasurements();
    int i=0;
    for(set<int>::const_iterator it=usedMeasurements.begin();it!=usedMeasurements.end();++it){
        i=*it;
        ++counter;
        if(Data::DTYPE[data[i].Dtype].find("Dose")==string::npos && Data::DTYPE[data[i].Dtype].find("dose")==string::npos){

            measurementsV.push_back(pair<int,pair<double,double> >(i,_dataHandle->getSignal(i)));

            y.push_back(_dataHandle->getSignal(i).first);
            yErr.push_back(_dataHandle->getSignal(i).second);
            pair<double,double> doseUnc=_dataHandle->computeGeneralDose(_dataHandle->getSignal(i),(Function)(ui->comboBox->currentIndex()));
            x.push_back(doseUnc.first);

            continue;
        }

        x.push_back(data[i].IRR_Time);
        y.push_back(_dataHandle->getSignal(i).first);
        yErr.push_back(_dataHandle->getSignal(i).second);
    }

    plotWidget->graph(0)->setName(QString("Data"));
    plotWidget->graph(0)->setErrorType(QCPGraph::etValue);
    plotWidget->graph(0)->setDataValueError(x,y,yErr);




    try{
        _dataHandle->computeCalibration(NULL,NULL,(Function)(ui->comboBox->currentIndex()));
    }catch(int i){
        if(i==1){
            string message="More number of fit parameters then number of data points!";
            QMessageBox box;
            box.setText(message.c_str());
            box.exec();
            return;
        }
    }

    QPen pen;
    pen.setStyle(Qt::DotLine);
    plotWidget->addGraph();
    plotWidget->graph(1)->setPen(pen);
    string functionName=DataHandle::FIT_FUNCTIONS[ui->comboBox->currentIndex()];
    plotWidget->graph(1)->setName(functionName.c_str());

    int lineMax=0;

    /* Show measurements  lines */
    plotWidget->rescaleAxes();
    lineMax=plotWidget->graph(0)->keyAxis()->range().upper;
    int NUM_POINTS=lineMax*10;
    QVector<double> doseLineX;
    QVector<double> doseLineY;
    for (int i = 0; i < measurementsV.size(); ++i) {
        doseLineX.clear();
        doseLineY.clear();
        pair<double,double> doseUnc=_dataHandle->computeGeneralDose(measurementsV[i].second,(Function)(ui->comboBox->currentIndex()));

        if(lineMax<doseUnc.first) lineMax=doseUnc.first;
        plotWidget->addGraph();
        string name="Meas_"+TOOLS::convertToString<int>(measurementsV[i].first+1);
        plotWidget->graph(2+i)->setName(name.c_str());
        QPen pen;
        pen.setColor(QColor((Qt::GlobalColor)(7+i)));
        pen.setStyle(Qt::DashLine);
        plotWidget->graph(2+i)->setPen(pen);
        for (int j = 0; j < NUM_POINTS; ++j) {
            doseLineX.push_back(j*doseUnc.first/NUM_POINTS);
            doseLineY.push_back(measurementsV[i].second.first);
        }
        for (int j = 0; j < NUM_POINTS; ++j) {
            doseLineX.push_back(doseUnc.first);
            doseLineY.push_back(j*measurementsV[i].second.first/NUM_POINTS);
        }
        plotWidget->graph(2+i)->setData(doseLineX,doseLineY);
    }

    QVector<double> lineX(lineMax);
    QVector<double> lineY(lineMax);

    for (int i = 0; i < lineMax; ++i) {
        lineX[i]=i;
        lineY[i]=(*DataHandle::getFunction((Function)(ui->comboBox->currentIndex())))(i,_dataHandle->getCalParameters());
    }
    plotWidget->graph(1)->setData(lineX,lineY);

    plotWidget->rescaleAxes();
    plotWidget->legend->setVisible(true);
//    plotWidget->legend->setPositionStyle((QCPLegend::PositionStyle)(1));

    plotWidget->replot();
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void Calibration::showResidualData(){
    this->saveTableData();

    QCustomPlot* resPlotWidget=ui->widget_2;

    resPlotWidget->clearGraphs();
    //    resPlotWidget->legend->setPositionStyle((QCPLegend::PositionStyle)(1));
    resPlotWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                   QCP::iSelectLegend | QCP::iSelectPlottables);

    QCPAxisRect axis(resPlotWidget);
    axis.setRangeDrag(Qt::Horizontal | Qt::Vertical);
    axis.setRangeZoom(Qt::Horizontal | Qt::Vertical);

    resPlotWidget->xAxis->setLabel("Dose [s]");
    resPlotWidget->yAxis->setLabel("Signal/Calibration fnc");
    resPlotWidget->addGraph();
    resPlotWidget->graph(0)->setLineStyle(QCPGraph::lsNone);
    resPlotWidget->graph(0)->setScatterStyle(QCPScatterStyle::ssCrossCircle);

    QVector<double> X(0);
    QVector<double> Y(0);
    QVector<double> YErr(0);

    const vector<Data> data=_dataHandle->getData();

    const set<int> usedMeasurements=_dataHandle->getUsedMeasurements();
    int i=0;

    double dose;
    pair<double,double> residual;
    double funcVal;
    pair<double,double> signal;
    double maxDose=0;
    for(set<int>::const_iterator it=usedMeasurements.begin();it!=usedMeasurements.end();++it){
        i=*it;
        if(Data::DTYPE[data[i].Dtype].find("Regenerative dose")!=string::npos){

            dose=double(data[i].IRR_Time);
            funcVal=double((*DataHandle::getFunction((Function)(ui->comboBox->currentIndex())))(dose,_dataHandle->getCalParameters()));
            signal=_dataHandle->getSignal(i);

            if(funcVal!=0){
                residual.first=signal.first/funcVal;
            }else{
                residual.first=0;
            }
            if(dose>maxDose) maxDose=dose;

            residual.second=sqrt(pow(residual.first,2)*pow(signal.second/signal.first,2));

            X.push_back(dose);
            Y.push_back(residual.first);
            YErr.push_back(residual.second);

            continue;
        }
    }


    resPlotWidget->graph(0)->setName(QString("Residual"));
    resPlotWidget->graph(0)->setErrorType(QCPGraph::etValue);
    resPlotWidget->graph(0)->setDataValueError(X,Y,YErr);

        /* draw 1 line */
    int NUM_POINTS=1000;
    QVector<double> lineX;
    QVector<double> lineY;

    for (int j = 0; j < NUM_POINTS; ++j) {
        lineX.push_back(j/float(NUM_POINTS)*(1.1*maxDose));
        lineY.push_back(1);
    }
    resPlotWidget->addGraph();
    resPlotWidget->graph(1)->setData(lineX,lineY);
    QPen pen;
    pen.setStyle(Qt::DotLine);
    resPlotWidget->graph(1)->setPen(pen);


    resPlotWidget->rescaleAxes();

//    resPlotWidget->legend->setVisible(true);



    resPlotWidget->replot();
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void Calibration::on_actionUpdateGraph_triggered()
{
    this->saveTableData();
    this->showData();
    this->showResidualData();
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void Calibration::on_actionShowTable_triggered()
{
    this->saveTableData();
    const vector<Data> data=_dataHandle->getData();
    const set<int> usedMeasurements=_dataHandle->getUsedMeasurements();
    QDialog dialog;
    dialog.setWindowTitle("Signal data table");
    QTableWidget tableWidget(&dialog);
    tableWidget.setColumnCount(7);
    tableWidget.setRowCount(usedMeasurements.size());
    QStringList horizontalList;
    horizontalList<<"Num_m"<<"Type"<<"IRR_Time"<<"Signal"<<"Background"<<"S-B"<<"Error(S-B)";
    tableWidget.setHorizontalHeaderLabels(horizontalList);
    int counter=0;
    int i=0;
    for (set<int>::const_iterator it=usedMeasurements.begin(); it!=usedMeasurements.end();++it) {
        i=*it;

        tableWidget.setItem(counter,0,new QTableWidgetItem(TOOLS::convertToString<int>(i+1).c_str()));
        tableWidget.setItem(counter,1,new QTableWidgetItem(Data::DTYPE[data[i].Dtype].c_str()));
        tableWidget.setItem(counter,2,new QTableWidgetItem(TOOLS::convertToString<int>(data[i].IRR_Time).c_str()));
        tableWidget.setItem(counter,3,new QTableWidgetItem(TOOLS::convertToString<double>(_dataHandle->getIntegral(i,data[i].rangeSignal)).c_str()));
        tableWidget.setItem(counter,4,new QTableWidgetItem(TOOLS::convertToString<double>(_dataHandle->getIntegral(i,data[i].rangeBackground)).c_str()));
        tableWidget.setItem(counter,5,new QTableWidgetItem(TOOLS::convertToString<double>(_dataHandle->getSignal(i).first).c_str()));
        tableWidget.setItem(counter,6,new QTableWidgetItem(TOOLS::convertToString<double>(_dataHandle->getSignal(i).second).c_str()));

        ++counter;
    }
    dialog.setMinimumWidth(tableWidget.horizontalHeader()->length()+60);
    tableWidget.setMinimumWidth(tableWidget.horizontalHeader()->length()+60);//tableWidget.verticalHeader()->length()+100);
    tableWidget.setMaximumHeight(800);
    tableWidget.setMinimumHeight(300);
    dialog.setMaximumHeight(500);
    dialog.setMinimumHeight(300);
    QGridLayout layout(&dialog);
    layout.addWidget(&tableWidget,2,1);
    dialog.exec();
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void Calibration::savePlot(){
    QCustomPlot* plotWidget=ui->widget;
    QString fname;
    try{
        fname=QFileDialog::getSaveFileName(this,"Save file", "/home/boris/dokumenty/SURO/EURADOS_school/intercomp/data");
        plotWidget->savePdf(fname);
    }catch(...){
    }
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void Calibration::on_actionSavePlot_triggered()
{
    this->savePlot();
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void Calibration::on_commandLinkButton_4_clicked()
{
    Function function=(Function)(ui->comboBox->currentIndex());
    int numberOfParameters=2;
    if(function==Quadratic || function==Exp) numberOfParameters=3;
    if(function==Cubic || function==ExpLine) numberOfParameters=4;

    QDialog dialog;
    string title=" Parameters for function "+DataHandle::FIT_FUNCTIONS[int(function)]+": "+DataHandle::FIT_FORMULA[int(function)];
    dialog.setWindowTitle(title.c_str());
    QTableWidget tableWidget(&dialog);
    tableWidget.setColumnCount(numberOfParameters+numberOfParameters+numberOfParameters*(numberOfParameters-1)/2.);
    tableWidget.setRowCount(1);
    const double* parameters=_dataHandle->getCalParameters();
    const gsl_matrix* covMatrix=_dataHandle->getCovariantMatrix();
    QStringList horizontalList;
    for (int i = 0; i < numberOfParameters; ++i) {
        horizontalList<<(string("par_")+TOOLS::convertToString<int>(i)).c_str();
    }
    for (int i = 0; i < numberOfParameters; ++i) {
        horizontalList<<(string("ePar_")+TOOLS::convertToString<int>(i)).c_str();
    }

    for (int i = 0; i < numberOfParameters; ++i) {
        for(int j=i+1;j<numberOfParameters;++j){
            horizontalList<<(string("cov_")+TOOLS::convertToString<int>(i).c_str()+"_"+TOOLS::convertToString<int>(j)).c_str();
        }
    }

    tableWidget.setHorizontalHeaderLabels(horizontalList);
    for (int i = 0; i < numberOfParameters; ++i) {
        tableWidget.setItem(0,i,new QTableWidgetItem(TOOLS::convertToString<double>(parameters[i]).c_str()));
        tableWidget.setItem(0,i+numberOfParameters,new QTableWidgetItem(TOOLS::convertToString<double>(sqrt(gsl_matrix_get(covMatrix,i,i))).c_str()));
    }

    int counter=0;
    for (int i = 0; i < numberOfParameters; ++i) {
        for(int j=i+1;j<numberOfParameters;++j){
            tableWidget.setItem(0,2*numberOfParameters+counter,new QTableWidgetItem(TOOLS::convertToString<double>(gsl_matrix_get(covMatrix,i,j)).c_str()));
            ++counter;
        }
    }

    dialog.setMinimumWidth(tableWidget.horizontalHeader()->length()+60);
    dialog.setMinimumWidth(tableWidget.horizontalHeader()->length()+60);
    tableWidget.setMinimumWidth(tableWidget.horizontalHeader()->length()+60);//tableWidget.verticalHeader()->length()+100);
    tableWidget.setMaximumHeight(800);
    tableWidget.setMinimumHeight(300);
    dialog.setMaximumHeight(500);
    dialog.setMinimumHeight(300);
    QGridLayout layout(&dialog);
    layout.addWidget(&tableWidget,2,1);
    dialog.exec();
}




/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void Calibration::on_actionComputeDose_triggered(){

    this->saveTableData();
    QTableWidget* tableWidget=ui->tableWidget;
    const set<int> usedMeasurements=_dataHandle->getUsedMeasurements();
    _dataHandle=DataHandle::getInstance();

    pair<double,double> fadCorrection;
    int whichLine=ui->lineEdit->text().toInt()-1;

    try{
        if(usedMeasurements.find(whichLine)==usedMeasurements.end()){
            QMessageBox box;
            box.setText("Wrong number of measurement to dose estimation");
            box.exec();
            return;
        }
        /* use experimental fading*/
        if(ui->checkBox->isChecked()){
            int signalMeas=ui->lineEdit_2->text().toInt();
            int fadedSignalMeas=ui->lineEdit_3->text().toInt();
            if (signalMeas>_dataHandle->getData().size()|| fadedSignalMeas>_dataHandle->getData().size()){
                QMessageBox box;
                string title="Experimental fading number of measurement out of range!";
                box.setText(title.c_str());
                box.exec();
                return;
            }
            pair<double,double> signal=_dataHandle->getSignal(signalMeas-1);
            pair<double,double> fadedSignal=_dataHandle->getSignal(fadedSignalMeas-1);
            fadCorrection.first=fadedSignal.first/signal.first;
            fadCorrection.second=sqrt(pow(fadCorrection.first,2)*(pow(signal.second/signal.first,2)+pow(fadedSignal.second/fadedSignal.first,2)));


            gsl_matrix* fadCovMatrix=gsl_matrix_alloc(2,2);
            gsl_matrix_set(fadCovMatrix,0,0,pow(fadCorrection.second,2));
            _dataHandle->setFadParameters(&fadCorrection.first,fadCovMatrix,Constant,whichLine);
            gsl_matrix_free(fadCovMatrix);
        }else{
        fadCorrection=_dataHandle->computeFadCorr(whichLine ,
                                                  (FadFunction)(((QComboBox*)tableWidget->cellWidget(whichLine,7))->currentIndex()));
        }
        _dataHandle->setFadCorrection(fadCorrection);
    }catch(...){
        QMessageBox box;
        string title="cannot be zero fading correction (log(0)=inf)";
        box.setText(title.c_str());
        box.exec();
        return;
    }

    _dataHandle->setTimeUnit((TimeUnit)(ui->comboBox_2->currentIndex()));
    _dataHandle->setFunction((Function)(ui->comboBox->currentIndex()));

    DoseResults::WHICH=whichLine;
    DoseResults doseResults;
    doseResults.exec();
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void Calibration::getCalibrationParameters(){
    try{
        _dataHandle->computeCalibration(NULL,NULL,(Function)(ui->comboBox->currentIndex()));

    }catch(int i){
        if(i==1){
            string message="More number of fit parameters then number of data points!";
            QMessageBox box;
            box.setText(message.c_str());
            box.exec();
            return;
        }
    }
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void Calibration::on_pushButton_clicked()
{
    Function function=(Function)(ui->comboBox->currentIndex());
    int numberOfParameters=2;
    if(function==Quadratic || function==Exp) numberOfParameters=3;
    if(function==Cubic || function==ExpLine) numberOfParameters=4;

    QDialog dialog;
    string title=" Initial parameters for function "+DataHandle::FIT_FUNCTIONS[int(function)]+": "+DataHandle::FIT_FORMULA[int(function)];
    dialog.setWindowTitle(title.c_str());
    QTableWidget tableWidget(&dialog);
    tableWidget.setColumnCount(numberOfParameters);
    tableWidget.setRowCount(1);
    const double* parameters=_dataHandle->getInitialParameters();
    QStringList horizontalList;
    for (int i = 0; i < numberOfParameters; ++i) {
        horizontalList<<TOOLS::convertToString<int>(i).c_str();
    }
    tableWidget.setHorizontalHeaderLabels(horizontalList);
    for (int i = 0; i < numberOfParameters; ++i) {
        tableWidget.setItem(0,i,new QTableWidgetItem(TOOLS::convertToString<double>(parameters[i]).c_str()));
    }
    dialog.setMinimumWidth(tableWidget.horizontalHeader()->length()+60);
    tableWidget.setMinimumWidth(tableWidget.horizontalHeader()->length()+60);//tableWidget.verticalHeader()->length()+100);
    tableWidget.setMaximumHeight(800);
    tableWidget.setMinimumHeight(300);
    dialog.setMaximumHeight(500);
    dialog.setMinimumHeight(300);
    QGridLayout layout(&dialog);
    layout.addWidget(&tableWidget,2,1);
    connect(&tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(changeInitialParameters(QTableWidgetItem*)));
    dialog.exec();
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void Calibration::changeInitialParameters(QTableWidgetItem* item){
    int which=item->column();
    const double* initialParameters=_dataHandle->getInitialParameters();
    double copyInitPar[4];
    for(int i=0;i<4;++i) copyInitPar[i]=initialParameters[i];
    double data=item->data(0).toDouble();
    copyInitPar[which]=data;
    _dataHandle->setInitialParameters(copyInitPar);
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void Calibration::showTable(){
    const set<int> usedMeasurements=_dataHandle->getUsedMeasurements();
    unsigned numMeasurements=usedMeasurements.size();
    QTableWidget* tableWidget=ui->tableWidget;
    tableWidget->setRowCount(numMeasurements);
    tableWidget->setColumnCount(3+2*2+1+2*2+1);
    QStringList horizontalList;
    horizontalList<<"Num_m"<<"DType"<<"IRR_Time"<<"signalRange_0"<<"signalRange_1"
                 <<"backgroundRangeL"<<"backgroundRangeR"
                <<"fad_func"<<"fad_0"<<"fad_1"<<"eFad_0"<<"eFad_1"<<"fad_cov_0_1";
    tableWidget->setHorizontalHeaderLabels(horizontalList);
    int counter=0;
    vector<Data> data=_dataHandle->getData();
    string cellString;
    int i=0;
    for(set<int>::const_iterator it=usedMeasurements.begin(); it!=usedMeasurements.end();++it) {
        i=*it;
        QComboBox* typeCombo=new QComboBox();
        for (unsigned j  = 0; j  < Data::DTYPE.size(); ++j ) {
            typeCombo->addItem(Data::DTYPE[j].c_str());
        }
        typeCombo->setCurrentIndex(data[i].Dtype);

        QComboBox* fadFunCombo=new QComboBox();
        for (unsigned j  = 0; j  < Data::FAD_FUNCTIONS.size(); ++j ) {
            fadFunCombo->addItem(Data::FAD_FUNCTIONS[j].c_str());
        }
        fadFunCombo->setCurrentIndex(int(data[i].fadFunction));
        tableWidget->setCellWidget(counter,1,typeCombo);
        tableWidget->setCellWidget(counter,7,fadFunCombo);
        tableWidget->setItem(counter,0,new QTableWidgetItem(TOOLS::convertToString<int>(i+1).c_str()));
        tableWidget->setItem(counter,2,new QTableWidgetItem(TOOLS::convertToString<int>(data[i].IRR_Time).c_str()));
        tableWidget->setItem(counter,3,new QTableWidgetItem(TOOLS::convertToString<double>(data[i].rangeSignal[0]).c_str()));
        tableWidget->setItem(counter,4,new QTableWidgetItem(TOOLS::convertToString<double>(data[i].rangeSignal[1]).c_str()));
        tableWidget->setItem(counter,5,new QTableWidgetItem(TOOLS::convertToString<double>(data[i].rangeBackground[0]).c_str()));
        tableWidget->setItem(counter,6,new QTableWidgetItem(TOOLS::convertToString<double>(data[i].rangeBackground[1]).c_str()));
        tableWidget->setItem(counter,8,new QTableWidgetItem(TOOLS::convertToString<double>(data[i].fadParameters[0]).c_str()));
        tableWidget->setItem(counter,9,new QTableWidgetItem(TOOLS::convertToString<double>(data[i].fadParameters[1]).c_str()));
        tableWidget->setItem(counter,10,new QTableWidgetItem(TOOLS::convertToString<double>(sqrt(gsl_matrix_get(data[i].fadCovariantMatrix,0,0))).c_str()));
        tableWidget->setItem(counter,11,new QTableWidgetItem(TOOLS::convertToString<double>(sqrt(gsl_matrix_get(data[i].fadCovariantMatrix,1,1))).c_str()));
        tableWidget->setItem(counter,12,new QTableWidgetItem(TOOLS::convertToString<double>(gsl_matrix_get(data[i].fadCovariantMatrix,0,1)).c_str()));

        ++counter;
    }
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void Calibration::saveTableData(){
    int rangeSignal[2];
    int rangeBackground[2];
    double fadParameters[2];
    double eFadParameters[2];
    int which;
    FadFunction fadFunction;
    float irr_time;
    int dtype;

    QTableWidget* tableWidget=ui->tableWidget;
    int rows=tableWidget->rowCount();
    gsl_matrix* fadCovariantMatrix=gsl_matrix_alloc(2,2);

    for (int i = 0; i < rows; ++i) {
        which=tableWidget->item(i,0)->text().toInt()-1;
        dtype=((QComboBox*)tableWidget->cellWidget(i,1))->currentIndex();
        irr_time=tableWidget->item(i,2)->text().toFloat();

        rangeSignal[0]=tableWidget->item(i,3)->text().toInt();
        rangeSignal[1]=tableWidget->item(i,4)->text().toInt();
        rangeBackground[0]=tableWidget->item(i,5)->text().toInt();
        rangeBackground[1]=tableWidget->item(i,6)->text().toInt();
        fadFunction=(FadFunction)(((QComboBox*)tableWidget->cellWidget(i,7))->currentIndex());
        fadParameters[0]=tableWidget->item(i,8)->text().toDouble();
        fadParameters[1]=tableWidget->item(i,9)->text().toDouble();

        gsl_matrix_set(fadCovariantMatrix,0,0,pow(tableWidget->item(i,10)->text().toDouble(),2));
        gsl_matrix_set(fadCovariantMatrix,1,1,pow(tableWidget->item(i,11)->text().toDouble(),2));
        gsl_matrix_set(fadCovariantMatrix,0,1,tableWidget->item(i,12)->text().toDouble());
        gsl_matrix_set(fadCovariantMatrix,1,0,tableWidget->item(i,12)->text().toDouble());

        _dataHandle->setFadParameters(fadParameters,fadCovariantMatrix,fadFunction,which);
        _dataHandle->setRangeSignalBackground(rangeSignal,rangeBackground,which);
        _dataHandle->setDType(dtype,which);
        _dataHandle->setIRRTime(irr_time,which);
    }
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void Calibration::tableChanged(QTableWidgetItem * item){
    this->saveTableData();
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void Calibration::on_pushButton_2_clicked()
{

    QTableWidget* tableWidget=ui->tableWidget;
    double fadParameters[2]={0,0};
    double eFadParameters[2]={0,0};

    bool isFirst=0;
    for (unsigned i = 0; i < _dataHandle->getUsedMeasurements().size(); ++i) {
        //        if(Data::DTYPE[((QComboBox*)tableWidget->cellWidget(i,1))->currentIndex()]!="Dose") continue;
        if(isFirst==0){
            isFirst=1;
            fadParameters[0]=tableWidget->item(i,8)->text().toDouble();
            fadParameters[1]=tableWidget->item(i,9)->text().toDouble();
            eFadParameters[0]=tableWidget->item(i,10)->text().toDouble();
            eFadParameters[1]=tableWidget->item(i,11)->text().toDouble();
            continue;
        }
        tableWidget->setItem(i,8,new QTableWidgetItem(TOOLS::convertToString<double>(fadParameters[0]).c_str()));
        tableWidget->setItem(i,9,new QTableWidgetItem(TOOLS::convertToString<double>(fadParameters[1]).c_str()));
        tableWidget->setItem(i,10,new QTableWidgetItem(TOOLS::convertToString<double>(eFadParameters[0]).c_str()));
        tableWidget->setItem(i,11,new QTableWidgetItem(TOOLS::convertToString<double>(eFadParameters[1]).c_str()));
    }

    this->saveTableData();
}


void Calibration::on_pushButton_3_clicked()
{

    QTableWidget* tableWidget=ui->tableWidget;
    double rangeSignal[2]={0,0};
    double rangeBackground[2]={0,0};
    for (unsigned i = 0; i < _dataHandle->getUsedMeasurements().size(); ++i) {
        if(i==0){
            rangeSignal[0]=tableWidget->item(i,3)->text().toDouble();
            rangeSignal[1]=tableWidget->item(i,4)->text().toDouble();
            rangeBackground[0]=tableWidget->item(i,5)->text().toDouble();
            rangeBackground[1]=tableWidget->item(i,6)->text().toDouble();
            continue;
        }
        tableWidget->setItem(i,3,new QTableWidgetItem(TOOLS::convertToString<double>(rangeSignal[0]).c_str()));
        tableWidget->setItem(i,4,new QTableWidgetItem(TOOLS::convertToString<double>(rangeSignal[1]).c_str()));
        tableWidget->setItem(i,5,new QTableWidgetItem(TOOLS::convertToString<double>(rangeBackground[0]).c_str()));
        tableWidget->setItem(i,6,new QTableWidgetItem(TOOLS::convertToString<double>(rangeBackground[1]).c_str()));

    }

    this->saveTableData();
}


void Calibration::on_comboBox_2_currentIndexChanged(int index)
{
    _dataHandle->setTimeUnit((TimeUnit)(index));
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void Calibration::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
  // Rename a graph by double clicking on its legend item
  Q_UNUSED(legend)
  if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
  {
    QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
    bool ok;
    QString newName = QInputDialog::getText(this, "QCustomPlot example", "New graph name:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
    if (ok)
    {
      plItem->plottable()->setName(newName);
      ui->widget->replot();
      ui->widget_2->replot();
    }
  }
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/

void Calibration::selectionChanged()
{
  /*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.

   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.

   Further, we want to synchronize the selection of the graphs with the selection state of the respective
   legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
   or on its legend item.
  */

  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->widget->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->widget->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->widget->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->widget->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->widget->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->widget->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->widget->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->widget->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->widget->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->widget->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->widget->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->widget->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }

  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i<ui->widget->graphCount(); ++i)
  {
    QCPGraph *graph = ui->widget->graph(i);
    QCPPlottableLegendItem *item = ui->widget->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      item->setSelected(true);
      graph->setSelected(true);
    }
  }
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/

void Calibration::mousePress()
{
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged

  if (ui->widget->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->widget->axisRect()->setRangeDrag(ui->widget->xAxis->orientation());
  else if (ui->widget->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->widget->axisRect()->setRangeDrag(ui->widget->yAxis->orientation());
  else
    ui->widget->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void Calibration::on_checkBox_stateChanged(int arg1)
{
    if(arg1!=0){
        ui->lineEdit_2->setEnabled(1);
        ui->lineEdit_3->setEnabled(1);
        ui->lineEdit_2->setText("");
        ui->lineEdit_3->setText("");

    }else{
        ui->lineEdit_2->setEnabled(0);
        ui->lineEdit_3->setEnabled(0);
    }
}
