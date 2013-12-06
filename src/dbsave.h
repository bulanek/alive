#ifndef DBSAVE_H
#define DBSAVE_H

#include <set>

#include <QDialog>
#include	"sqlhandle.h"
#include	"datahandle.h"
#include "mythread.h"
#include "tools.h"


namespace Ui {
class DbSave;
}

class DbSave : public QDialog
{
    Q_OBJECT
    
public:
    explicit DbSave(QWidget *parent = 0,int ID=0);
    ~DbSave();
    MyThread* _myThread;

private:
    Ui::DbSave *ui;

    SqlHandle* _sqlHandle;
    DataHandle* _dataHandle;
    set<int>::const_iterator _it;
    int _ID;
    int _numEntries;

private slots:
    void savedDbEntry(int i);
};

#endif // DBSAVE_H
