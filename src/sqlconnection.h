/**
  @author	Boris Bulanek ()
  @company National Radiation Protection Institute, Bartoskova 28, 140 00, Praha 4
  @email  boris.bulanek@suro.cz
  @tel.   226 518  279
  @date	01/30/13
  */
// =====================================================================================
#ifndef SQLCONNECTION_H
#define SQLCONNECTION_H


#include <QDialog>
#include <QTreeView>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlDriver>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlError>
#include <QMessageBox>
#include <QFileDialog>
#include "ui_sqlconnection.h"

#include "datahandle.h"
#include "data.h"
#include "sqlhandle.h"
#include "tools.h"

namespace Ui {
class SqlConnection;
}

class SqlConnection : public QDialog
{
    Q_OBJECT

public:
    explicit SqlConnection(QWidget *parent = 0);
    inline bool isOpenDb(){return _db.isOpen();}
    ~SqlConnection();
    virtual void accept();
    const QTableView* getInfo1Table() const{return ui->tableView;}
    int getSqlEntry(const int ID);
    void insertRow();
    void deleteRow();
    void updateActions();
    void showDbTable();
    void onlyForSave();
    void onlyForOpen();
    static bool IS_SAVE;
public slots:
    void updateTableFromCommand();
    void showTable(const QString &t);
    void on_actionFetchDb_triggered();


    void on_actionInsertRow_triggered();

    void on_actionDeleteRow_triggered();


    void currentChanged() { updateActions();};

signals:
    void statusMessage(const QString &message);

private slots:

    void on_actionShowDbTable_triggered();

    void on_pushButton_clicked();


    void on_pushButton_2_clicked();

private:
    Ui::SqlConnection *ui;

    DataHandle* _dataHandle;
    SqlHandle* _sqlHandle;
    QSqlDatabase _db;
    int _currentDbLevel;
    int _currentDbMeasurement;
};



#endif // MYSQLCONNECTION_H
