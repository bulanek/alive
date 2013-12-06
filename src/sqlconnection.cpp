#include "sqlconnection.h"
#include "ui_sqlconnection.h"
#include <iostream>



/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
bool SqlConnection::IS_SAVE=0;


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
SqlConnection::SqlConnection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SqlConnection)
{
    ui->setupUi(this);

    //ui->tableView->addAction(ui->actionInsertRow);
    ui->tableView->addAction(ui->actionDeleteRow);
    ui->tableView->addAction(ui->actionShowDbTable);
    this->setWindowTitle("SQL data connection");
    _dataHandle=DataHandle::getInstance();
    ui->dbEntry->setText(_dataHandle->getMainWindowData().name.c_str());
    _currentDbLevel=1;
    _currentDbMeasurement=1;
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
SqlConnection::~SqlConnection()
{
    delete ui;
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void SqlConnection::accept(){
    const MainWindowData mainData=_dataHandle->getMainWindowData();
    MainWindowData mainDataCopy=mainData;
    mainDataCopy.name=ui->dbEntry->text().toStdString();
    _dataHandle->setMainWindowData(mainDataCopy);

//    QDir directory(QString(_dataHandle->getDatabasePath().c_str()));
//    QString path=directory.filePath(ui->dbName->text());
//    _dataHandle->createDb(path);
    int ID=ui->ID->text().toInt();

    if(IS_SAVE==0){
        int size=this->getSqlEntry(ID);

        if(size==0){
            cerr<<"Warning: SqlConnection::accept with zero size of data!"<<endl;
            return;
        }
    }
    if(!_dataHandle->getDb().isOpen()) return;
    QSqlQuery query;
    query.exec("select id,name from INFO_1");
    while(query.next()){
        if(query.value(0).toInt()==ID){
            const MainWindowData mainData=_dataHandle->getMainWindowData();
            MainWindowData mainDataCopy=mainData;
            mainDataCopy.dbName=query.value(1).toString().toStdString();
            _dataHandle->setMainWindowData(mainDataCopy);
            break;
        }
    }
    this->done(1);
    this->hide();
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void SqlConnection::onlyForSave(){
    //    ui->horizontalLayout->setEnabled(0);
    //    ui->label_4->setEnabled(0);
    ui->label_4->hide();
    ui->ID->hide();
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void SqlConnection::onlyForOpen(){
    //    ui->label_7->setEnabled(0);
    //    ui->dbEntry->setEnabled(0);
    ui->label_7->hide();
    ui->dbEntry->hide();
    this->showTable("INFO_1");
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void SqlConnection::on_actionFetchDb_triggered()

{
    _dataHandle=DataHandle::getInstance();
    //QDir directory(QString(_dataHandle->getDatabasePath().c_str()));
    //QString path=directory.filePath(ui->dbName->text());
    //_dataHandle->createDb(path);
    this->showTable("INFO_1");
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void SqlConnection::showTable(const QString &t)
{
    QTableView* table=ui->tableView;

    QSqlTableModel *tableModel= new QSqlTableModel(table,_dataHandle->getDb());
    tableModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    tableModel->setTable(_dataHandle->getDb().driver()->escapeIdentifier(t, QSqlDriver::TableName));
    tableModel->select();
    //    for (int i = 0; i < tableModel->rowCount(); ++i) {
    //        QModelIndex	index=tableModel->index(i,1);
    //        tableModel->setData(index,Data::LTYPE[tableModel->data(index).toInt()].c_str());
    //    }
    table->setModel(tableModel);
    table->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);

    connect(table->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(currentChanged()));
    this->updateActions();
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
int SqlConnection::getSqlEntry(const int ID){

    SqlHandle* sqlHandle=SqlHandle::getInstance();
    const vector<Data> data=sqlHandle->getSqlData(ID);


    int size=data.size();

    if (size==0) {
        cerr<<"Warning: in SqlConnection::getSqlEntry zero size of data!"<<endl;
        return 0;
    }
    _dataHandle->setData(data);

    return size;
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void SqlConnection::insertRow()
{
    QTableView* table=ui->tableView;

    QSqlTableModel *model = qobject_cast<QSqlTableModel *>(table->model());
    if (!model)
        return;

    QModelIndex insertIndex = table->currentIndex();
    int row = insertIndex.row() == -1 ? 0 : insertIndex.row();
    model->insertRow(row);
    insertIndex = model->index(row, 0);
    table->setCurrentIndex(insertIndex);
    table->edit(insertIndex);
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void SqlConnection::deleteRow()
{

    QTableView* table=ui->tableView;
    QSqlTableModel *model = qobject_cast<QSqlTableModel *>(table->model());
    if (!model)
        return;

    //model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    QModelIndexList currentSelection = table->selectionModel()->selectedIndexes();
    int ID=0;
    for (int i = 0; i < currentSelection.count(); ++i) {
        if(i!=0) continue;
        QModelIndex index=currentSelection.at(i);
        ID=index.data().toInt();
        model->removeRow(currentSelection.at(i).row());
    }

    model->submitAll();
    //model->setEditStrategy(QSqlTableModel::OnRowChange);

    SqlHandle* sqlHandle=SqlHandle::getInstance();
    sqlHandle->deleteMeasurement(ID);


    updateActions();
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void SqlConnection::showDbTable(){
    QTableView* table=ui->tableView;
    QSqlTableModel *model = qobject_cast<QSqlTableModel *>(table->model());
    if (!model)
        return;

    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    QModelIndexList currentSelection = table->selectionModel()->selectedIndexes();
    string tableShow="";
    for (int i = 0; i < currentSelection.size(); ++i) {

        if(i!=0) continue;
        QModelIndex index=currentSelection.at(i);
        switch(_currentDbLevel){
        case 1:
            tableShow="INFO_2_"+index.data().toString().toStdString();
            _currentDbMeasurement=index.data().toString().toInt();
            _currentDbLevel=2;
            break;
        case 2:
            tableShow="INFO_3_"+TOOLS::convertToString<int>(_currentDbMeasurement)+"_"+
                    index.data().toString().toStdString();
            _currentDbLevel=3;
            break;
        case 3:

            tableShow="INFO_1";
            break;
        }
    }
    //    model->submitAll();
    //    model->setEditStrategy(QSqlTableModel::OnRowChange);
    cout<<"Show table "<<tableShow<<endl;
    this->showTable(tableShow.c_str());

    updateActions();
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void SqlConnection::updateTableFromCommand(){
    //    QTableView* table=ui->tableView;
    //    QSqlQueryModel *model = new QSqlQueryModel(table);
    //    string queryStr=ui->sqlEdit->toPlainText().toStdString();
    //    if (queryStr.find("INFO_1")!=string::npos){
    //        this->showTable("INFO_1");
    //        return;
    //    }

    //    model->setQuery(QSqlQuery(ui->sqlEdit->toPlainText(),_dataHandle->getDb()));
    //    table->setModel(model);

    //    if (model->lastError().type() != QSqlError::NoError)
    //        emit statusMessage(model->lastError().text());
    //    else if (model->query().isSelect())
    //        emit statusMessage(tr("Query OK."));
    //    else
    //        emit statusMessage(tr("Query OK, number of affected rows: %1").arg(
    //                    model->query().numRowsAffected()));

    //    updateActions();
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void SqlConnection::updateActions()
{
    QTableView* table=ui->tableView;
    bool enableIns = qobject_cast<QSqlTableModel *>(table->model());
    bool enableDel = enableIns && table->currentIndex().isValid() && _currentDbLevel==1;

    ui->actionInsertRow->setEnabled(enableIns);
    ui->actionDeleteRow->setEnabled(enableDel);
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void SqlConnection::on_actionInsertRow_triggered()
{
    this->insertRow();
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void SqlConnection::on_actionDeleteRow_triggered()
{
    QMessageBox box;
    box.setText("Do you want to delete this measurement?");
    box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    if(box.exec()==QMessageBox::Ok) this->deleteRow();
}


void SqlConnection::on_actionShowDbTable_triggered()
{
    this->showDbTable();
}

void SqlConnection::on_pushButton_clicked()
{
    if (_currentDbLevel!=1) --_currentDbLevel;
    string tableShow;
    switch(_currentDbLevel){
    case 1:
        this->showTable("INFO_1");
        break;
    case 2:
        tableShow="INFO_2_"+TOOLS::convertToString<int>(_currentDbMeasurement);
        this->showTable(tableShow.c_str());
        break;
    }
}

void SqlConnection::on_pushButton_2_clicked()
{
    _dataHandle=DataHandle::getInstance();
    QString fname=QFileDialog::getOpenFileName(this,tr("Directory"),_dataHandle->getDatabasePath().c_str());
    _dataHandle->createDb(fname);//fname.toStdString());
    this->on_actionFetchDb_triggered();
    this->showDbTable();
    this->showTable("INFO_1");
}
