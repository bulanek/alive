/**
  @author	Boris Bulanek ()
  @company National Radiation Protection Institute, Bartoskova 28, 140 00, Praha 4
  @email  boris.bulanek@suro.cz
  @tel.   226 518  279
  @date	01/30/13
  */
// =====================================================================================

#ifndef PLATEAU_H
#define PLATEAU_H

#include <QDialog>
#include <QFileDialog>
#include <QTableWidget>
#include <QMessageBox>

#include "datahandle.h"
#include	"tools.h"

namespace Ui {
class Plateau;
}

class Plateau : public QDialog
{
    Q_OBJECT
    
public:
    explicit Plateau(QWidget *parent = 0);
    ~Plateau();
   inline const Ui::Plateau* getUi(){return ui;}
   void savePlateauPlot();
    
private slots:
void changeInitialParameters(QTableWidgetItem* item);
   void on_actionShowPlateau_triggered();

   void on_actionSavePlateauPlot_triggered();

   void on_pushButton_2_clicked();

private:
    Ui::Plateau *ui;
    DataHandle* _dataHandle;
};

#endif // PLATEAU_H
