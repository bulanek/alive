#include "dbsave.h"
#include "ui_dbsave.h"

DbSave::DbSave(QWidget *parent, const int ID) :
    QDialog(parent),
    ui(new Ui::DbSave)
{
    ui->setupUi(this);
    _ID=ID;

    _dataHandle=DataHandle::getInstance();
    const set<int> usedMeasurements=_dataHandle->getUsedMeasurements();
    _it=usedMeasurements.begin();
    _numEntries=usedMeasurements.size();
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(_numEntries);
    _myThread=new MyThread(ID,this);

    connect(_myThread,SIGNAL(dbSaved(int)),this,SLOT(savedDbEntry(int)));
    _myThread->start();
    this->exec();

}

DbSave::~DbSave()
{
    delete ui;
}

void DbSave::savedDbEntry(int i){
    ui->progressBar->setValue(i+1);
//    const set<int>::const_iterator it=_dataHandle->getUsedMeasurements().begin();
//    string text="Saving measurement number "+boost::lexical_cast<string>(*(it+i));
//    ui->label->setText(text.c_str());
    if(i+1==_numEntries){
        this->accept();
    }
}
