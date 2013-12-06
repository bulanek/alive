/**
  @author	Boris Bulanek ()
  @company National Radiation Protection Institute, Bartoskova 28, 140 00, Praha 4
  @email  boris.bulanek@suro.cz
  @tel.   226 518  279
  @date	01/30/13
  */
// =====================================================================================
#ifndef INFO_H
#define INFO_H

#include <QDialog>
#include "datahandle.h"
#include	"tools.h"

namespace Ui {
class Info;
}

class Info : public QDialog
{
    Q_OBJECT
    
public:
    explicit Info(QWidget *parent = 0);
    ~Info();
    void setUIData( int which);
    void storeData();
public slots:
    virtual void accept();;
    
private:
    Ui::Info *ui;
DataHandle* _dataHandle;
int _which;
};

#endif // INFO_H
