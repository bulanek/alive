
#include	<fstream>
#include "sqlconnection.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "plateau.h"
#include "ui_plateau.h"
#include "calibration.h"
#include "ui_calibration.h"
#include "dbsave.h"
#include "info.h"
#include "ui_info.h"

#include <QtGui>
#include <iostream>
#include <vector>
#include <string>

//#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string.hpp"

#include "sqlhandle.h"
#include "tools.h"


using namespace std;


struct MainWindowData;


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
QString MainWindow::TITLE="TL/OSL data storage and analysis";

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(TITLE);
    _dataHandle=DataHandle::getInstance();


    /* graph interactions*/
    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                QCP::iSelectLegend | QCP::iSelectPlottables);
    ui->widget->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    ui->widget->legend->setFont(legendFont);
    ui->widget->legend->setSelectedFont(legendFont);
    ui->widget->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items


    ui->widget->addAction(ui->actionSavePlot);
    connect(ui->widget, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));
    // connect slot that ties some axis selections together (especially opposite axes):
    connect(ui->widget, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    connect(ui->widget, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));


    checkedTableVarMap.insert("Run");
    checkedTableVarMap.insert("Dtype");
    checkedTableVarMap.insert("IRR_Time");
    checkedTableVarMap.insert("Position");
    this->computeCurrentIrrPower_clicked();

}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
MainWindow::~MainWindow()
{
    checkedTableVarMap.clear();
    delete ui;
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::on_actionOpen_file_triggered()
{
    QString fname;
    try{
        fname=QFileDialog::getOpenFileName(this,"Open file",
                                           "/home/boris/dokumenty/SURO/EURADOS_school/intercomp/data");
        if (fname=="") return;
        this->setWindowTitle(TITLE+"; File: "+fname);
        cout.flush();
        if(_dataHandle->createData(fname.toStdString())==1){
            throw 1;
        }


        DataHandle::IS_NEW=1;
        cout.flush();

        this->showPlot();
        //ui->pushButton->setEnabled(1);
        ui->pushButton_3->setEnabled(1);
        ui->pushButton_4->setEnabled(1);
        ui->pushButton_7->setEnabled(1);
        ui->lineEdit_6->setEnabled(1);
        ui->lineEdit_7->setEnabled(1);
        ui->lineEdit_8->setEnabled(1);
        ui->lineEdit_9->setEnabled(1);

        this->getRangeSignalBackground();

        cout.flush();
        this->showTable();
        cout.flush();
        this->on_actionStoreData_triggered();
        cout.flush();
        this->showUIData();
        cout.flush();
        this->showUIData();
        cout.flush();
    }catch(...){
        string text= string("Cannot open binary file ")+fname.toStdString()+"!";
        cerr<<text<<endl;
        QMessageBox box;
        box.setText(text.c_str());
        box.exec();
    }
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::getRangeSignalBackground(){
    int NPoints;
    int rangeSignal[2];
    int rangeBackground[2];
    const vector<Data> data=_dataHandle->getData();

    for(set<int>::const_iterator it=_dataHandle->getUsedMeasurements().begin();it!=_dataHandle->getUsedMeasurements().end();++it){
        for(int i=0;i<2;++i){
            rangeSignal[i]=data[*it].rangeSignal[i];
            rangeBackground[i]=data[*it].rangeBackground[i];
        }
        NPoints=data[*it].NPoints;
        break;
    }
    ui->lineEdit_6->setText(TOOLS::convertToString(rangeSignal[0]).c_str());
    ui->lineEdit_7->setText(TOOLS::convertToString(rangeSignal[1]).c_str());
    ui->lineEdit_8->setText(TOOLS::convertToString(rangeBackground[0]).c_str());
    ui->lineEdit_9->setText(TOOLS::convertToString(rangeBackground[1]).c_str());
    ui->lineEdit_10->setText(TOOLS::convertToString(NPoints).c_str());
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::instance()->quit();
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::on_actionDoseParameters_triggered()
{
    this->on_actionStoreData_triggered();
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::on_actionOpen_db_triggered()
{
    _dataHandle=DataHandle::getInstance();
    DataHandle::IS_NEW=0;
    QString fname=QFileDialog::getOpenFileName(this,tr("Directory"),_dataHandle->getDatabasePath().c_str());
    _dataHandle->createDb(fname);
    if (!_dataHandle->getDb().isOpen()) return;

    SqlConnection sql;
    sql.onlyForOpen();
    if (sql.exec()==QDialog::Accepted){
        //ui->pushButton->setEnabled(1);
        ui->pushButton_3->setEnabled(1);
        ui->pushButton_4->setEnabled(1);
        ui->pushButton_7->setEnabled(1);
        ui->lineEdit_6->setEnabled(1);
        ui->lineEdit_7->setEnabled(1);
        ui->lineEdit_8->setEnabled(1);
        ui->lineEdit_9->setEnabled(1);

        const MainWindowData mainData=_dataHandle->getMainWindowData();
        this->setWindowTitle(TITLE+"; Database entry name: "+QString(mainData.dbName.c_str()));
        this->showPlot();
        this->showTable();
        //this->on_actionStoreData_triggered();
        this->showUIData();
        this->showUIData();
    }
    this->getRangeSignalBackground();
}



/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::on_actionSave_db_triggered()
{
    this->on_actionStoreData_triggered();
    QSqlDatabase db=_dataHandle->getDb();

    SqlHandle* sqlHandle=SqlHandle::getInstance();

    SqlConnection::IS_SAVE=1;
    SqlConnection mySqlConnection;
    mySqlConnection.onlyForSave();
    if(mySqlConnection.exec()==QDialog::Rejected){
        SqlConnection::IS_SAVE=0;
        return;
    }
    SqlConnection::IS_SAVE=0;

    QSqlQuery query;
    string execStr;

    int ID=0;
    query.exec("select id from INFO_1");
    while(query.next()){
        ID=query.value(0).toInt();
    }
    ID+=1;

    MainWindowData mainWindowData=_dataHandle->getMainWindowData();
    try{
        sqlHandle->createMainTables();
        cout<<"Save information into table INFO_1"<<endl;
        sqlHandle->insertIntoMainInfoTable(mainWindowData);
        cout<<"Save information into table INFO_2_<ID>"<<endl;
        sqlHandle->createSecondTable(ID);

    }catch(...){
        cerr<<"Warning: throw boost::bad_lexical_cast in MainWindow::on_actionSave_db_triggered"<<endl;
    }


    DbSave saveDbProgress(this,ID);
    //saveDbProgress.show();
    //saveDbProgress.setVisible(1);

}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::on_actionShowPlateau_triggered()
{
    this->on_actionStoreData_triggered();

    Plateau pl;
    pl.setModal(0);

    pl.show();
    pl.exec();
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::on_actionStoreData_triggered()
{
    MainWindowData data;
    data.name=ui->lineEdit->text().toStdString();
    data.IRR_Power=ui->lineEdit_4->text().toDouble();
    _dataHandle->setMainWindowData(data);
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::on_actionShowCalibration_triggered()
{
    this->on_actionStoreData_triggered();
    Calibration calibration;

    calibration.setModal(0);
    calibration.show();
    calibration.exec();
}




/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::showUIData(){

    const vector<Data> data=_dataHandle->getData();
    ui->lineEdit->setText(data[0].Sequence.c_str());
    ui->lineEdit_5->setText(data[0].User.c_str());
}



/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::showTable(){
    const vector<Data> data=_dataHandle->getData();
    const set<int> usedMeasurements=_dataHandle->getUsedMeasurements();
    unsigned numMeasurements=usedMeasurements.size();
    QTableWidget* tableWidget=ui->tableWidget;
    tableWidget->setRowCount(numMeasurements);
    tableWidget->setColumnCount(checkedTableVarMap.size()+1);
    QStringList horizontalList;
    horizontalList<<"Num_meas";
    for(set<string>::const_iterator it=checkedTableVarMap.begin();it!=checkedTableVarMap.end();++it) {
        horizontalList<<(*it).c_str();
    }
    tableWidget->setHorizontalHeaderLabels(horizontalList);
    int counter=0;
    int counterVar=0;
    QTableWidgetItem* itemDetail;
    int i=0;
    for(set<int>::const_iterator it=usedMeasurements.begin(); it!=usedMeasurements.end();++it){
        i=*it;

        itemDetail=new QTableWidgetItem(TOOLS::convertToString<int>(i+1).c_str());
        itemDetail->setFlags(Qt::ItemIsSelectable);
        tableWidget->setItem(counter,0,(QTableWidgetItem*)itemDetail);

        counterVar=0;
        for(set<string>::const_iterator it2=checkedTableVarMap.begin();it2!=checkedTableVarMap.end();++it2){
            itemDetail=new QTableWidgetItem(TOOLS::convertToString<string>(data[i].getValue(*it2)).c_str());
            itemDetail->setFlags(Qt::ItemIsSelectable);
            tableWidget->setItem(counter,counterVar+1,(QTableWidgetItem*)itemDetail);
            ++counterVar;
        }


        ++counter;
    }

}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::showPlot(){
    QCustomPlot* plotWidget=ui->widget;
    plotWidget->clearGraphs();
    const vector<Data> data=_dataHandle->getData();
    const set<int> usedMeasurements=_dataHandle->getUsedMeasurements();
    cout<<"Used measuremets size: "<<usedMeasurements.size()<<endl;
    cout<<"data size: "<<data.size()<<endl;
    plotWidget->xAxis->setLabel("time [channels]");
    plotWidget->yAxis->setLabel("signal");


    int counter=0;

    int k=0;
    for (set<int>::const_iterator it=usedMeasurements.begin();it!=usedMeasurements.end();++it) {
        k=*it;
        plotWidget->addGraph();
        string name=Data::LTYPE[data[k].LType]+"_"+TOOLS::convertToString<int>(k+1);
        plotWidget->graph(counter)->setName(name.c_str());
        QPen pen;
        pen.setColor(QColor((Qt::GlobalColor)(7+counter)));
        plotWidget->graph(counter)->setPen(pen);
        QVector<double> x(data[k].NPoints);
        QVector<double> y(data[k].NPoints);
        for (int i = 0; i < x.size(); ++i) {
            y[i]=data[k].DPoints[i];
            x[i]=i;
        }
        plotWidget->graph(counter)->setData(x,y);
        ++counter;
    }
    plotWidget->rescaleAxes();

    QCPAxisRect axis(plotWidget);
    axis.setRangeDrag(Qt::Horizontal | Qt::Vertical);
    axis.setRangeZoom(Qt::Horizontal | Qt::Vertical);

    plotWidget->replot();
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::on_actionEditMeasurement_triggered()
{
    string whichStr=ui->lineEdit_2->text().toStdString();
    int which =0;
    QMessageBox box;
    string text;
    try{
        which =atoi(whichStr.c_str());
        if(which<=0 || which>(int)_dataHandle->getData().size()) throw 20;
    }catch(...){
        cerr<<"throwing boost::bad_lexical_cast in MainWindow::on_actionEditMeasurement_triggered"<<endl;
        if(_dataHandle->getData().size()>1){
            text="wrong number of measurements (1-"+TOOLS::convertToString<unsigned>(_dataHandle->getData().size())+")";
        }else{
            text="wrong number of measurements (1)";
        }
        box.setText(text.c_str());
        box.exec();
        return;
    }

    Info info;
    info.setUIData(which-1);
    info.exec();
    this->showTable();
    this->showPlot();
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::about()
{
    QMessageBox::about(this, tr("About"), tr("The OSL simple analysis software"
                                             " for a dose reconstruction."
                                             "You can find a detail information in doc\\html\\index.html "
                                             "or doc\\latex\\refman.pdf or send me a "
                                             "<a href=mailto:boris.bulanek@suro.cz>mail</a>."));
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::on_pushButton_5_clicked()
{
    vector<string> whatInVec;
    set<int> measurements;
    string whatIn=ui->lineEdit_3->text().toStdString();
    boost::algorithm::split(whatInVec,whatIn,boost::algorithm::is_any_of(","));
    try{
        for (unsigned i = 0; i < whatInVec.size(); ++i) {
            size_t pos=whatInVec[i].find("-");
            if (pos!=string::npos){
                char buffer[4];
                for (int j = 0; j < 4; ++j) { buffer[j]='\0'; }
                whatInVec[i].copy(buffer,pos,0);
                string copy1(buffer);
                for (int j = 0; j < 4; ++j) { buffer[j]='\0'; }
                whatInVec[i].copy(buffer,whatInVec[i].size()-pos-1,pos+1);
                string copy2(buffer);
                int from=atoi(copy1.c_str())-1;
                int to=atoi(copy2.c_str())-1;
                for (int j = from; j <= to; ++j) {
                    measurements.insert(j);
                }
            }else{
                measurements.insert(atoi(whatInVec[i].c_str())-1);
            }
        }
        for(set<int>::iterator it=measurements.begin();it!=measurements.end();++it) {
            if(*it<0 || *it>=(int)_dataHandle->getData().size()) throw 1;
        }
        _dataHandle->setUsedMeasurements(measurements);
        this->showPlot();
        this->showTable();
    }catch(...){
        cerr<<"Bad lexical cast in MainWindow::on_pushButton_5_clicked "
           <<"or number not in range of measurements (1-"<<TOOLS::convertToString<unsigned>(_dataHandle->getData().size())<<")"<<endl;
        QMessageBox box;
        box.setText("bad format (e.q. 1-3,4,5) or bad number scale");
        box.exec();
        return;
    }
    this->getRangeSignalBackground();
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::savePlot(){
    QCustomPlot* plotWidget=ui->widget;
    QString fname;
    try{
        fname=QFileDialog::getSaveFileName(this,"Save file", "/home/boris/dokumenty/SURO/EURADOS_school/intercomp/data");
        plotWidget->savePdf(fname,1);
    }catch(...){
    }
}




/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::on_actionSavePlot_triggered()
{
    this->savePlot();
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::on_pushButton_clicked()
{
    set<int> measurements;
    for (unsigned i = 0; i < _dataHandle->getData().size(); ++i) {
        measurements.insert(i);
    }
    _dataHandle->setUsedMeasurements(measurements);
    ui->lineEdit_3->clear();
    ui->lineEdit->clearFocus();
    this->showPlot();
    this->showTable();
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::on_actionDump_txt_triggered()
{
    QString fname;
    try{
        fname=QFileDialog::getSaveFileName(this,"Save file",
                                           "/home/boris/dokumenty/SURO/EURADOS_school/intercomp/data");
        if (fname=="") return;
        ofstream outFile(fname.toStdString().c_str());
        outFile<<_dataHandle<<endl;
        outFile.close();
    }catch(...){
        string text= string("Cannot save txt file ")+fname.toStdString()+"!";
        cerr<<text<<endl;
        QMessageBox box;
        box.setText(text.c_str());
        box.exec();
        return;
    }
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::on_pushButton_6_clicked()
{
    const vector<Data> data=_dataHandle->getData();
    QDialog dialog;
    dialog.setWindowTitle("Variables");

    QGroupBox* aBoxBin=new QGroupBox("Variables from binary file",&dialog);
    QGroupBox* aBoxUser=new QGroupBox("Additional variables",&dialog);
    QHBoxLayout* mainLayout=new QHBoxLayout(&dialog);
    QHBoxLayout* binHorizontal=new QHBoxLayout(aBoxBin);
    QVBoxLayout* aBinLayout1=new QVBoxLayout(aBoxBin);
    QVBoxLayout* aBinLayout2=new QVBoxLayout(aBoxBin);
    binHorizontal->addLayout(aBinLayout1);
    binHorizontal->addLayout(aBinLayout2);

    QVBoxLayout* aUserLayout=new QVBoxLayout(aBoxUser);
    QVBoxLayout* aVertRightLayout=new QVBoxLayout();


    map<string,QCheckBox*> checkMap;
    const map<string,string> var_names_help=Data::getVarNamesHelp();
    map<string,string>::const_iterator it;

    int firstLine=int(Data::VAR_NAMES.size()/2.+0.5);
    for (int i = 0; i < firstLine; ++i) {
        QCheckBox* aBox=new QCheckBox(tr(TOOLS::convertToString<string>(Data::VAR_NAMES[i]).c_str()),aBoxBin);
        if(checkedTableVarMap.find(Data::VAR_NAMES[i])!=checkedTableVarMap.end()) aBox->setChecked(1);
        it=var_names_help.find(Data::VAR_NAMES[i]);
        if(it!=var_names_help.end()) aBox->setToolTip(it->second.c_str());
        aBinLayout1->addWidget(aBox);
        checkMap[Data::VAR_NAMES[i]]=aBox;
    }
    for (unsigned i= firstLine; i < Data::VAR_NAMES.size(); ++i) {
        QCheckBox* aBox=new QCheckBox(tr(TOOLS::convertToString<string>(Data::VAR_NAMES[i]).c_str()),aBoxBin);
        if(checkedTableVarMap.find(Data::VAR_NAMES[i])!=checkedTableVarMap.end()) aBox->setChecked(1);
        it=var_names_help.find(Data::VAR_NAMES[i]);
        if(it!=var_names_help.end()) aBox->setToolTip(it->second.c_str());
        aBinLayout2->addWidget(aBox);
        checkMap[Data::VAR_NAMES[i]]=aBox;
    }

    for (unsigned i = 0; i < Data::USER_VAR_NAMES.size(); ++i) {
        QCheckBox* aBox=new QCheckBox(tr(TOOLS::convertToString<string>(Data::USER_VAR_NAMES[i]).c_str()),aBoxUser);
        if(checkedTableVarMap.find(Data::USER_VAR_NAMES[i])!=checkedTableVarMap.end()) aBox->setChecked(1);
        it=var_names_help.find(Data::USER_VAR_NAMES[i]);
        if(it!=var_names_help.end()) aBox->setToolTip(it->second.c_str());
        aUserLayout->addWidget(aBox);
        checkMap[Data::USER_VAR_NAMES[i]]=aBox;
    }

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,Qt::Horizontal,&dialog);
    connect(buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    mainLayout->addWidget(aBoxBin);
    aVertRightLayout->addWidget(aBoxUser);
    aVertRightLayout->addStretch(1);
    aVertRightLayout->addWidget(buttonBox);

    mainLayout->addLayout(aVertRightLayout);

    if(dialog.exec()){
        checkedTableVarMap.clear();

        for(map<string,QCheckBox*>::iterator it=checkMap.begin();it!=checkMap.end(); ++it) {
            if(it->second->isChecked()){
                checkedTableVarMap.insert(it->first);
            }
        }
        this->showTable();
    }
}

/*-----------------------------------------------------------------------------
 *  
 *-----------------------------------------------------------------------------*/
void MainWindow::on_pushButton_7_clicked()
{

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
void MainWindow::on_actionSetSignalBackgroundRange_triggered()
{

    MainWindowData mainWindowData=_dataHandle->getMainWindowData();
    int signalRange[2];
    int backgroundRange[2];
    signalRange[0]=ui->lineEdit_6->text().toInt();
    signalRange[1]=ui->lineEdit_7->text().toInt();
    backgroundRange[0]=ui->lineEdit_8->text().toInt();
    backgroundRange[1]=ui->lineEdit_9->text().toInt();
    const set<int> usedMeasurements=_dataHandle->getUsedMeasurements();
    int NPoints= _dataHandle->getData()[*usedMeasurements.begin()].NPoints;
    if(backgroundRange[1]>=NPoints){
        string text= string("Background range outside of # points!");
        cerr<<text<<endl;
        QMessageBox box;
        box.setText(text.c_str());
        box.exec();
        ui->lineEdit_9->setText(TOOLS::convertToString(mainWindowData.rangeBackground[1]).c_str());
        return;
    }
    for(int i=0;i<2;++i){
        mainWindowData.rangeSignal[i]=signalRange[i];
        mainWindowData.rangeBackground[i]=backgroundRange[i];
    }
    _dataHandle->setMainWindowData(mainWindowData);
    for(set<int>::const_iterator it=usedMeasurements.begin();it!=usedMeasurements.end();++it){
        _dataHandle->setRangeSignalBackground(signalRange,backgroundRange,*it);
    }
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void MainWindow::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
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
    }
  }
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/

void MainWindow::selectionChanged()
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
void MainWindow::mousePress()
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
void MainWindow::on_pushButton_8_clicked()
{
    set<int> measurements;
    int position=ui->lineEdit_11->text().toDouble();
    const vector<Data> dataVec=_dataHandle->getData();

    for (unsigned i = 0; i < dataVec.size(); ++i) {
        if(dataVec[i].Position==position) measurements.insert(i);
    }
    _dataHandle->setUsedMeasurements(measurements);
    this->showPlot();
    this->showTable();
    this->getRangeSignalBackground();
}

void MainWindow::computeCurrentIrrPower_clicked()
{
    double halfLife=ui->lineEdit_13->text().toDouble()*365*24*60*60; /* half life from years to seconds */
    double referencePower=ui->lineEdit_14->text().toDouble();
    int referenceDate=ui->dd->dateTime().toTime_t();
    int currentDate=ui->dateEdit_2->dateTime().toTime_t();

    double currentPower=referencePower*exp(-(currentDate-referenceDate)*log(2)/halfLife);
    ui->lineEdit_4->setText(QString(TOOLS::convertToString(currentPower).c_str()));
    MainWindowData data;
    data.IRR_Power=ui->lineEdit_4->text().toDouble();
    _dataHandle->setMainWindowData(data);
}
