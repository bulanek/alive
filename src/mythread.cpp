#include <QtCore>
#include "mythread.h"

MyThread::MyThread(const int id,QObject *parent) :
    QThread(parent)
{
    _sqlHandle=SqlHandle::getInstance();
    _dataHandle=DataHandle::getInstance();
    _id=id;
}

void MyThread::run(){
    const set<int> usedMeasurements=_dataHandle->getUsedMeasurements();
    QMutex mutex;
    int counter=0;
    for (set<int>::const_iterator it=usedMeasurements.begin();it!=usedMeasurements.end();++it){
        mutex.lock();
        _sqlHandle->insertIntoSecondTable(_id,*it);
        _sqlHandle->insertIntoThirdTable(_id,*it);
        emit dbSaved(counter);
        ++counter;
        mutex.unlock();
    }
}
