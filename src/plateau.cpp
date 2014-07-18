#include "plateau.h"
#include "ui_plateau.h"


/*-----------------------------------------------------------------------------
 *  
 *-----------------------------------------------------------------------------*/
Plateau::Plateau(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Plateau)
{
    ui->setupUi(this);
    this->setWindowTitle("Plateau graph");
    _dataHandle=DataHandle::getInstance();

    for (unsigned i  = 0; i  < DataHandle::FIT_FUNCTIONS.size(); ++i ) {
        ui->comboBox->addItem(DataHandle::FIT_FUNCTIONS[i].c_str());
    }
    ui->comboBox->setCurrentIndex(0);
    this->on_actionShowPlateau_triggered();
    ui->widget->addAction(ui->actionSavePlateauPlot);
}


/*-----------------------------------------------------------------------------
 *  
 *-----------------------------------------------------------------------------*/
Plateau::~Plateau()
{
    delete ui;
}


/*-----------------------------------------------------------------------------
 *  
 *-----------------------------------------------------------------------------*/
void Plateau::on_actionShowPlateau_triggered()
{
    const MainWindowData mainData=_dataHandle->getMainWindowData();
    QCustomPlot* plotWidget=ui->widget;
    plotWidget->setInteraction(QCP::iSelectPlottables);
    QCPAxisRect axis(plotWidget);
    axis.setRangeDrag(Qt::Horizontal | Qt::Vertical);
    axis.setRangeZoom(Qt::Horizontal | Qt::Vertical);
    plotWidget->xAxis->setLabel("range [channels]");
    plotWidget->yAxis->setLabel("uncorrected dose [Gy]");
    plotWidget->clearGraphs();

    int size=_dataHandle->getData()[0].DPoints.size();

    int startRange=ui->lineEdit->text().toInt();
    int endRange=int(size/2.)-1;
    const int range[]={startRange,endRange};

    if(range[1]>size){
        string text="Range cannot be bigger then "+TOOLS::convertToString<int>(size)+"/2!";
        QMessageBox box;
        box.setText(text.c_str());
        box.exec();
        return;
    }else if(range[0]>range[1] || range[0]<0 || range[1]<=0 ||range[0]==range[1]){
        string text="Uncorrect range!";
        QMessageBox box;
        box.setText(text.c_str());
        box.exec();
        return;
    }

    QVector<double> x;
    QVector<double> y;
    QVector<double> yErr;
    vector<Data> data=_dataHandle->getData();
    int which=0;
    for (unsigned i = 0; i < data.size(); ++i) {
        if (Data::DTYPE[data[i].Dtype]=="Natural") break;
    }
    try{
        Function function=(Function)ui->comboBox->currentIndex();
        for (int i = startRange; i <= endRange; ++i) {
            int rangeSignal[]={0,i};
            int rangeBackground[]={i+1,2*i+1};
            _dataHandle->computeCalibration(rangeSignal,rangeBackground,function);
            pair<double,double> signal=_dataHandle->getSignal(which,rangeSignal,rangeBackground);
            pair<double,double> dose=_dataHandle->computeGeneralDose(signal,function);
            x.push_back(rangeSignal[1]);
            y.push_back(dose.first*mainData.IRR_Power);
            yErr.push_back(dose.second);
        }
    }catch(...){
        string text="Negative signal value emerged while subtracting background!";
        QMessageBox box;
        box.setText(text.c_str());
        box.exec();
        return;
    }

    plotWidget->addGraph()->setData(x,y);
    plotWidget->graph(0)->setName("plateau");
    plotWidget->rescaleAxes();
    plotWidget->replot();
}

/*-----------------------------------------------------------------------------
 *  
 *-----------------------------------------------------------------------------*/
void Plateau::savePlateauPlot(){
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
void Plateau::on_actionSavePlateauPlot_triggered()
{
    this->savePlateauPlot();
}


/*-----------------------------------------------------------------------------
 *  
 *-----------------------------------------------------------------------------*/
void Plateau::on_pushButton_2_clicked()
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
void Plateau::changeInitialParameters(QTableWidgetItem* item){
    int which=item->column();
    const double* initialParameters=_dataHandle->getInitialParameters();
    double copyInPar[4];
    for(int i=0;i<4;++i) copyInPar[i]=initialParameters[i];
    double data=item->data(0).toDouble();
    copyInPar[which]=data;
    _dataHandle->setInitialParameters(copyInPar);
}
