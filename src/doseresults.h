#ifndef DOSERESULTS_H
#define DOSERESULTS_H

#include <QDialog>

#include	"datahandle.h"

using namespace std;
namespace Ui {
class DoseResults;
}

class DoseResults : public QDialog
{
    Q_OBJECT
    
public:
    explicit DoseResults(QWidget *parent = 0);
    ~DoseResults();
    static int WHICH;
   
private:
    Ui::DoseResults *ui;
    DataHandle* _dataHandle;
};

#endif // DOSERESULTS_H
