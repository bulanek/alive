#ifndef SQLHANDLE_H
#define SQLHANDLE_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <iostream>

#include <QString>
#include <QVariant>
#include <QSqlDatabase>
//#include <QObject>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlRecord>


#include "datahandle.h"
#include	"tools.h"

struct MainWindowData;
struct OSLWindowData;
struct Data;
class SqlHandle
{
public:
    static SqlHandle* getInstance();
    void createMainTables();
    void createSecondTable(const int ID);
    static void insertIntoSecondTable(const int ID,const int measurement);
    void insertIntoMainInfoTable(const MainWindowData& mainWindowData);
    static void insertIntoThirdTable(const int ID,const int measurement);
    void deleteMeasurement(const int ID);
    const vector<Data> getSqlData(const int ID);
    Data getDataFromQuery(const QSqlQuery& query,const QSqlRecord& record);
signals:
    /* number of measurement already saved from 0 */
//	void valueChanged(int newValue);


private:
    SqlHandle(){};
    static SqlHandle* instance;
    QSqlDatabase _db;
    static DataHandle* _dataHandle;
    int _id;
};



#endif // SQLHANDLE_H
