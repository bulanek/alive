/**
  @author	Boris Bulanek ()
  @company National Radiation Protection Institute, Bartoskova 28, 140 00, Praha 4
  @email  boris.bulanek@suro.cz
  @tel.   226 518  279
  @date	01/30/13
  */
// =====================================================================================
#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QDialog>
#include <QFileDialog>
#include <QTableWidget>
#include	<QInputDialog>
#include <QMessageBox>
#include "qcustomplot.h"

#include "datahandle.h"
#include	"tools.h"


namespace Ui {
class Calibration;
}

class Calibration : public QDialog
{
    Q_OBJECT
    
public:
    explicit Calibration(QWidget *parent = 0);
    ~Calibration();
    Ui::Calibration* getUi();
    void showData();
    void showResidualData();
    void savePlot();
    void getCalibrationParameters();
    void showTable();
    void saveTableData();
    virtual void accept();
private slots:
    void tableChanged(QTableWidgetItem*);
    void changeInitialParameters(QTableWidgetItem*);

    void on_actionComputeDose_triggered();

    void on_actionUpdateGraph_triggered();

    void on_actionShowTable_triggered();

    void on_actionSavePlot_triggered();

    void on_commandLinkButton_4_clicked();

    void on_pushButton_clicked();

//    void on_commandLinkButton_5_clicked();



    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_comboBox_2_currentIndexChanged(int index);

    /* graph interaction slots*/
    void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    void selectionChanged();
    void mousePress();


    void on_checkBox_stateChanged(int arg1);

private:
    Ui::Calibration *ui;
    DataHandle* _dataHandle;
};

#endif // CALIBRATION_H
