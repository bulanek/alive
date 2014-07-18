/**
  @author	Boris Bulanek
  @company National Radiation Protection Institute, Bartoskova 28, 140 00, Praha 4
  @email  boris.bulanek@suro.cz
  @tel   00420 226 518  279
  @date	01/30/13
  */
// =====================================================================================
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <set>

#include <QMainWindow>
#include <QThread>
#include <QModelIndex>
#include <QTableWidgetItem>
#include <QCheckBox>
#include "datahandle.h"
#include	"tools.h"
#include "qcustomplot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static QString TITLE;

    void showUIData();
    void showTable();
    void showPlot();
    void savePlot();
    void getRangeSignalBackground();

signals:
    void savedDbMeasurement(int ID);    

private slots:
    void about();
    void on_actionOpen_file_triggered();

    void on_actionExit_triggered();

    void on_actionDoseParameters_triggered();

    void on_actionOpen_db_triggered();

    void on_actionSave_db_triggered();

    void on_actionShowPlateau_triggered();

    void on_actionStoreData_triggered();


    void on_actionShowCalibration_triggered();


    void on_actionEditMeasurement_triggered();

    void on_pushButton_5_clicked();


    void on_actionSavePlot_triggered();

    void on_pushButton_clicked();

    void on_actionDump_txt_triggered();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_actionSetSignalBackgroundRange_triggered();

    void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    void selectionChanged();
    void mousePress();


    void on_pushButton_8_clicked();
    void computeCurrentIrrPower();

    void on_pushButton_9_clicked();

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    DataHandle* _dataHandle;
    std::set<string>  checkedTableVarMap;
};

#endif // MAINWINDOW_H
