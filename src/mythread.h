#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include "sqlhandle.h"
#include "datahandle.h"

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(const int id,QObject *parent = 0);
    virtual void run();
    
signals:
    void dbSaved(int);
    
public slots:

    private:
    SqlHandle* _sqlHandle;
    DataHandle* _dataHandle;
    int _id;
    
};

#endif // MYTHREAD_H
