#include "sqlhandle.h"
#include <QDateTime>

 using namespace std;


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
SqlHandle* SqlHandle::instance=0;

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
SqlHandle* SqlHandle::getInstance()
{
    if(instance==0){
        instance=new SqlHandle();
    }
    return instance;
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
DataHandle* SqlHandle::_dataHandle=DataHandle::getInstance();



/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void SqlHandle::createMainTables(){
    QSqlQuery query;
    bool queryReturn;
    queryReturn=query.exec("create table if not exists INFO_1("
                           "id INTEGER primary key autoincrement,"
                           "name VARCHAR(20),"
                           "IRR_Power DOUBLE,"
                           "num_m TINYINT,"
                           "save_time DATETIME default current_timestamp"
                           ");");
    if (!queryReturn){
        cout<<query.lastQuery().toStdString()<<endl;
        qDebug()<<"Cannot create table!";
    }

    _id=1;
    queryReturn=query.exec("select id from INFO_1;");
    if(!queryReturn){
        qDebug()<<"Error in ID selection!";
        qDebug()<<query.lastQuery();
    }else{
        while(query.next()) {
            _id= query.value(0).toInt()+1;
        }
    }
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void SqlHandle::insertIntoMainInfoTable(const MainWindowData& mainWindowData){

    _dataHandle=DataHandle::getInstance();
    string execStr=string("insert into INFO_1 (id,name,num_m,IRR_Power) values (")
                    +TOOLS::convertToString<int>(_id)+","
                    +"\""+mainWindowData.name+"\","
                    +TOOLS::convertToString<unsigned>(_dataHandle->getUsedMeasurements().size())+","
                    +TOOLS::convertToString<double>(mainWindowData.IRR_Power)
                    +");";
    QSqlQuery query;
    bool queryReturn=query.exec(execStr.c_str());
    if(!queryReturn){
        cout<<query.lastQuery().toStdString()<<endl;;
        qDebug()<<"Cannot write table!";
        qDebug()<<execStr.c_str();
    }
}





/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void SqlHandle::createSecondTable(const int ID){
    QSqlQuery query;
    string info_2_name="INFO_2_"+TOOLS::convertToString<int>(ID);
    string execStr="create table if not exists "+info_2_name+"("
                    "num_m INTEGER primary key autoincrement,"
                    "Version TINYINT,"
                    "Length SMALLINT,"
                    "Previous SMALLINT,"
                    "NPoints SMALLINT,"
                    "LType TINYINT,"
                    "Low FLOAT,"
                    "High FLOAT,"
                    "Rate FLOAT,"
                    "Temperature SMALLINT,"
                    "XCoord SMALLINT,"
                    "YCoord SMALLINT,"
                    "Delay SMALLINT,"
                    "OnOn SMALLINT,"
                    "Off SMALLINT,"
                    "Position TINYINT,"
                    "Run TINYINT,"
                    //                    "Time VARCHAR(7),"
                    //                    "Date VARCHAR(7),"
                    "mDateTime DATETIME,"
                    "IRR_DateTime DATETIME,"
                    "Sequence VARCHAR(9),"
                    "User VARCHAR(9),"
                    "Dtype TINYINT,"
                    "IRR_Time FLOAT,"
                    "IRR_Type TINYINT,"
                    "IRR_Unit TINYINT,"
                    "Bl_Time FLOAT,"
                    "Bl_Unit TINYINT,"
                    "An_Temp FLOAT,"
                    "An_Time FLOAT,"
                    "Norm1 INT,"
                    "Norm2 INT,"
                    "Norm3 INT,"
                    "BG SMALLINT,"
                    "Shift SMALLINT,"
                    "Sample VARCHAR(21),"
                    "Comment VARCHAR(81),"
                    "LightSource TINYINT,"
                    "SetSet TINYINT,"
                    "Tag TINYINT,"
                    "Grain TINYINT,"
                    "LightPower FLOAT,"
                    "SystemID SMALLINT,"
                    "OnTime FLOAT,"
                    "OffTime FLOAT,"
                    "EnableFlags SMALLINT,"
                    "OnGateDelay SMALLINT,"
                    "OffGateDelay SMALLINT,"
                    "CurveNo SMALLINT,"
                    "TimeTick SMALLINT,"
                    "StimPeriod SMALLINT,"
                    "GateEnabled SMALLINT,"
                    "GateStart SMALLINT,"
                    "GateEnd SMALLINT,"
                    "PTenabled SMALLINT,"
                    "rangeSignal_0 SMALLINT,"
                    "rangeSignal_1 SMALLINT,"
                    "rangeBackground_0 SMALLINT,"
                    "rangeBackground_1 SMALLINT,"
                    "fad_fun TINYINT,"
                    "fad_0 DOUBLE,"
                    "fad_1 DOUBLE,"
                    "eFad_0 DOUBLE,"
                    "eFad_1 DOUBLE,"
                    "fad_cov_0_1 DOUBLE)";

    bool queryReturn=query.exec(execStr.c_str());
    if(!queryReturn){
        qDebug()<<query.lastQuery();
        qDebug()<<"Cannot create INFO_2!";
    }
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void SqlHandle::insertIntoSecondTable(const int ID,const int measurement){

    const string tableName="INFO_2_"+TOOLS::convertToString<int>(ID);
    QSqlQuery query;
    bool queryReturn;
    string execStr;
    _dataHandle=DataHandle::getInstance();
    const set<int> usedMeasurements=_dataHandle->getUsedMeasurements();
    const vector<Data> data=_dataHandle->getData();

    int i=measurement;
    int counter=0;
    execStr="insert into "+tableName+"("
        "Version,"
        "Length,"
        "Previous,"
        "NPoints,"
        "LType,"
        "Low,"
        "High,"
        "Rate,"
        "Temperature,"
        "XCoord,"
        "YCoord,"
        "Delay,"
        "OnOn,"
        "Off,"
        "Position,"
        "Run,"
        //                        "Time,"
        //                        "Date,"
        "mDateTime,"
        "IRR_DateTime,"
        "Sequence,"
        "User,"
        "Dtype,"
        "IRR_Time,"
        "IRR_Type,"
        "IRR_Unit,"
        "Bl_Time,"
        "Bl_Unit,"
        "An_Temp,"
        "An_Time,"
        "Norm1,"
        "Norm2,"
        "Norm3,"
        "BG,"
        "Shift,"
        "Sample,"
        "Comment,"
        "LightSource,"
        "SetSet,"
        "Tag,"
        "Grain,"
        "LightPower,"
        "SystemID,"
        "OnTime,"
        "OffTime,"
        "EnableFlags,"
        "OnGateDelay,"
        "OffGateDelay,"
        "CurveNo,"
        "TimeTick,"
        "StimPeriod,"
        "GateEnabled,"
        "GateStart,"
        "GateEnd,"
        "PTenabled,"
        "rangeSignal_0,"
        "rangeSignal_1,"
        "rangeBackground_0,"
        "rangeBackground_1,"
        "fad_0,"
        "fad_1,"
        "eFad_0,"
        "eFad_1,"
        "fad_cov_0_1,"
        "fad_fun"
        ") VALUES (";
    execStr+=TOOLS::convertToString<int>(data[i].Version)+",";
    execStr+=TOOLS::convertToString<int>(data[i].Length)+",";
    execStr+=TOOLS::convertToString<int>(data[i].Previous)+",";
    execStr+=TOOLS::convertToString<int>(data[i].NPoints)+",";
    execStr+=TOOLS::convertToString<int>(data[i].LType)+",";
    execStr+=TOOLS::convertToString<double>(data[i].Low)+",";
    execStr+=TOOLS::convertToString<double>(data[i].High)+",";
    execStr+=TOOLS::convertToString<double>(data[i].Rate)+",";
    execStr+=TOOLS::convertToString<double>(data[i].Temperature)+",";
    execStr+=TOOLS::convertToString<int>(data[i].XCoord)+",";
    execStr+=TOOLS::convertToString<int>(data[i].YCoord)+",";
    execStr+=TOOLS::convertToString<int>(data[i].Delay)+",";
    execStr+=TOOLS::convertToString<int>(data[i].On)+",";
    execStr+=TOOLS::convertToString<int>(data[i].Off)+",";
    execStr+=TOOLS::convertToString<int>(data[i].Position)+",";
    execStr+=TOOLS::convertToString<int>(data[i].Run)+",";
    //        execStr+=TOOLS::convertToString<double>(data[i].Time)+",";
    //        execStr+=TOOLS::convertToString<double>(data[i].Date)+",";
    execStr+="datetime("+TOOLS::convertToString<int>(data[i].mDateTime)+",\'unixepoch\',\'localtime\'),";
    execStr+="datetime("+TOOLS::convertToString<int>(data[i].IRR_DateTime)+",\'unixepoch\',\'localtime\'),";
    execStr+="\""+data[i].Sequence+"\",";
    execStr+="\""+data[i].User+"\",";
    execStr+=TOOLS::convertToString<int>(data[i].Dtype)+",";
    execStr+=TOOLS::convertToString<int>(data[i].IRR_Time)+",";
    execStr+=TOOLS::convertToString<int>(data[i].IRR_Type)+",";
    execStr+=TOOLS::convertToString<int>(data[i].IRR_Unit)+",";
    execStr+=TOOLS::convertToString<double>(data[i].Bl_Time)+",";
    execStr+=TOOLS::convertToString<int>(data[i].Bl_Unit)+",";
    execStr+=TOOLS::convertToString<double>(data[i].An_Temp)+",";
    execStr+=TOOLS::convertToString<double>(data[i].An_Time)+",";
    execStr+=TOOLS::convertToString<int>(data[i].Norm1)+",";
    execStr+=TOOLS::convertToString<int>(data[i].Norm2)+",";
    execStr+=TOOLS::convertToString<int>(data[i].Norm3)+",";
    execStr+=TOOLS::convertToString<int>(data[i].BG)+",";
    execStr+=TOOLS::convertToString<int>(data[i].Shift)+",";
    execStr+="\""+data[i].Sample+"\""+",";
    execStr+="\""+data[i].Comment+"\""+",";
    execStr+=TOOLS::convertToString<int>(data[i].LightSource)+",";
    execStr+=TOOLS::convertToString<int>(data[i].Set)+",";
    execStr+=TOOLS::convertToString<int>(data[i].Tag)+",";
    execStr+=TOOLS::convertToString<int>(data[i].Grain)+",";
    execStr+=TOOLS::convertToString<double>(data[i].LightPower)+",";
    execStr+=TOOLS::convertToString<int>(data[i].SystemID)+",";
    execStr+=TOOLS::convertToString<double>(data[i].OnTime)+",";
    execStr+=TOOLS::convertToString<double>(data[i].OffTime)+",";
    execStr+=TOOLS::convertToString<int>(data[i].EnableFlags)+",";
    execStr+=TOOLS::convertToString<int>(data[i].OnGateDelay)+",";
    execStr+=TOOLS::convertToString<int>(data[i].OffGateDelay)+",";
    execStr+=TOOLS::convertToString<int>(data[i].CurveNo)+",";
    execStr+=TOOLS::convertToString<int>(data[i].TimeTick)+",";
    execStr+=TOOLS::convertToString<int>(data[i].StimPeriod)+",";
    execStr+=TOOLS::convertToString<int>(data[i].GateEnabled)+",";
    execStr+=TOOLS::convertToString<int>(data[i].GateStart)+",";
    execStr+=TOOLS::convertToString<int>(data[i].GateEnd)+",";
    execStr+=TOOLS::convertToString<int>(int(data[i].PTenabled))+",";
    execStr+=TOOLS::convertToString<double>(data[i].rangeSignal[0])+",";
    execStr+=TOOLS::convertToString<double>(data[i].rangeSignal[1])+",";
    execStr+=TOOLS::convertToString<double>(data[i].rangeBackground[0])+",";
    execStr+=TOOLS::convertToString<double>(data[i].rangeBackground[1])+",";
    for ( int j=0;j<2 ;++j ) {
        execStr+=TOOLS::convertToString<double>(double(data[i].fadParameters[j]))+",";
    }
    for (int j=0;j<2 ;++j ) {
        execStr+=TOOLS::convertToString<double>(double(sqrt(gsl_matrix_get(data[i].fadCovariantMatrix,j,j))))+",";
    }
    execStr+=TOOLS::convertToString<double>(double(gsl_matrix_get(data[i].fadCovariantMatrix,0,1)))+",";
    execStr+=TOOLS::convertToString<double>(int(data[i].fadFunction))+")";

    query.prepare(execStr.c_str());

    queryReturn=query.exec();

    if(!queryReturn){
        cout<<query.lastQuery().toStdString()<<endl;;
        qDebug()<<"Cannot insert into "<<tableName.c_str();
    }
    //        emit valueChanged(counter);

}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void SqlHandle::insertIntoThirdTable(const int ID,const int measurement){
    _dataHandle=DataHandle::getInstance();
    const vector<vector<int> > values=_dataHandle->getValues();
    string execStr;
    bool queryReturn;
    QSqlQuery query;
    const set<int> usedMeasurements= _dataHandle->getUsedMeasurements();
    int i=0;
    int counter=1;
    for(set<int>::const_iterator it=usedMeasurements.begin();it!=usedMeasurements.end();++it) {
        if(*it==measurement) break;
        ++counter;
    }

    query.exec("begin transaction;");
    i=measurement;
    string text="Saving "+TOOLS::convertToString<int>(i)+" measurement";
    string info_3_name="INFO_3_"+TOOLS::convertToString<int>(ID)+"_"+TOOLS::convertToString<int>(counter);
    execStr="create table if not exists "+info_3_name+"("
            "num_m INTEGER primary key autoincrement,"
            "value MEDIUMINT);";
    queryReturn=query.exec(execStr.c_str());
    if(!queryReturn){
        cout<<query.lastQuery().toStdString()<<endl;;
        qDebug()<<"Cannot create table INFO_3";
    }else{
        cout<<"Create table "+info_3_name<<endl;
    }
    /* Next line has to improve speed of inserting MySQL table. I've seen no effect*/
    string lockString="lock tables "+info_3_name+" write;";
  //  query.exec(lockString.c_str());
    for (unsigned j = 0; j < values[i].size(); ++j) {
        execStr="insert into "+info_3_name+" (value) values ("+TOOLS::convertToString<int>(values[i][j])+");";
        queryReturn=query.exec(execStr.c_str());
        if(!queryReturn){
            cout<<query.lastQuery().toStdString()<<endl;;
            qDebug()<<"Cannot insert into table INFO_3";
        }
    }
    query.exec("end transaction;");

    //query.exec("unlock tables;");
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
const vector<Data> SqlHandle::getSqlData(const int ID){
    QSqlQuery q;
    bool queryReturn;
    MainWindowData mainData;
    queryReturn=q.exec("select id,name,IRR_Power from INFO_1;");
    if (!queryReturn){
        qDebug()<<q.lastQuery();
        qDebug()<<"Cannot read table "<<"INFO_1!";
    }
    while (q.next()){
        if(q.value(0).toInt()==ID) break;
    }
    mainData.name=q.value(1).toString().toStdString();
    mainData.IRR_Power=q.value(2).toDouble();
    _dataHandle=DataHandle::getInstance();
    _dataHandle->setMainWindowData(mainData);


    vector<Data> dataVec;
    QSqlQuery q1;
    string infoName="INFO_2_"+TOOLS::convertToString<int>(ID);
    string queryStr="select "
                    "num_m,"
                    "Version,"
                    "Length,"
                    "Previous,"
                    "NPoints,"
                    "LType,"
                    "Low,"
                    "High,"
                    "Rate,"
                    "Temperature,"
                    "XCoord,"
                    "YCoord,"
                    "Delay,"
                    "OnOn,"
                    "Off,"
                    "Position,"
                    "Run,"
                    //                    "Time,"
                    //                    "Date,"
                    "mDateTime,"
                    "IRR_DateTime,"
                    "Sequence,"
                    "User,"
                    "Dtype,"
                    "IRR_Time,"
                    "IRR_Type,"
                    "IRR_Unit,"
                    "Bl_Time,"
                    "Bl_Unit,"
                    "An_Temp,"
                    "An_Time,"
                    "Norm1,"
                    "Norm2,"
                    "Norm3,"
                    "BG,"
                    "Shift,"
                    "Sample,"
                    "Comment,"
                    "LightSource,"
                    "SetSet,"
                    "Tag,"
                    "Grain,"
                    "LightPower,"
                    "SystemID,"
                    "OnTime,"
                    "OffTime,"
                    "EnableFlags,"
                    "OnGateDelay,"
                    "OffGateDelay,"
                    "CurveNo,"
                    "TimeTick,"
                    "StimPeriod,"
                    "GateEnabled,"
                    "GateStart,"
                    "GateEnd,"
                    "PTenabled,"
                    "rangeSignal_0,"
                    "rangeSignal_1,"
                    "rangeBackground_0,"
                    "rangeBackground_1,"
                    "fad_0,"
                    "fad_1,"
                    "eFad_0,"
                    "eFad_1,"
                    "fad_cov_0_1,"
                    "fad_fun"
                    " from "+infoName;
    queryReturn=q1.exec(queryStr.c_str());
    QSqlRecord record=q1.record();
    if (!queryReturn){
        qDebug()<<q1.lastQuery();
        qDebug()<<"Cannot read table "<<infoName.c_str()<<"!";
    }else{
        int counter=0;
        while (q1.next()){
            Data data;
            data=this->getDataFromQuery(q1,record);

            string query2Str="select*from INFO_3_"+TOOLS::convertToString<int>(ID)+"_"+TOOLS::convertToString<int>(counter+1);
            QSqlQuery q2;
            q2.exec(query2Str.c_str());
            while(q2.next()){
                data.DPoints.push_back(q2.value(1).toInt());
            }
            dataVec.push_back(data);

            ++counter;
        }

    }
    return dataVec;
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
Data SqlHandle::getDataFromQuery(const QSqlQuery& query,const QSqlRecord& record){
    Data data;
    data.Version=query.value(record.indexOf("Version")).toInt();
    data.Length=query.value(record.indexOf("Length")).toInt();
    data.Previous=query.value(record.indexOf("Previous")).toInt();
    data.NPoints=query.value(record.indexOf("NPoints")).toInt();
    data.LType=query.value(record.indexOf("LType")).toInt();
    data.Low=query.value(record.indexOf("Low")).toFloat();
    data.High=query.value(record.indexOf("High")).toFloat();
    data.Rate=query.value(record.indexOf("Rate")).toFloat();
    data.Temperature=query.value(record.indexOf("Temperature")).toInt();
    data.XCoord=query.value(record.indexOf("XCoord")).toInt();
    data.YCoord=query.value(record.indexOf("YCoord")).toInt();
    data.Delay=query.value(record.indexOf("Delay")).toInt();
    data.On=query.value(record.indexOf("OnOn")).toInt();
    data.Off=query.value(record.indexOf("Off")).toInt();
    data.Position=query.value(record.indexOf("Position")).toInt();
    data.Run=query.value(record.indexOf("Run")).toInt();
    //    data.Time=query.value(record.indexOf("Time")).toString().toStdString().c_str();
    //    data.Date=query.value(record.indexOf("Date")).toInt();
    data.mDateTime=query.value(record.indexOf("mDateTime")).toDateTime().toTime_t();
    data.IRR_DateTime=query.value(record.indexOf("IRR_DateTime")).toDateTime().toTime_t();
    data.Sequence=query.value(record.indexOf("Sequence")).toString().toStdString();
    data.User=query.value(record.indexOf("User")).toString().toStdString();
    data.Dtype=query.value(record.indexOf("Dtype")).toInt();
    data.IRR_Time=query.value(record.indexOf("IRR_Time")).toFloat();
    data.IRR_Type=query.value(record.indexOf("IRR_Type")).toInt();
    data.IRR_Unit=query.value(record.indexOf("IRR_Unit")).toInt();
    data.Bl_Time=query.value(record.indexOf("Bl_Time")).toFloat();
    data.Bl_Unit=query.value(record.indexOf("Bl_Unit")).toInt();
    data.An_Temp=query.value(record.indexOf("An_Temp")).toFloat();
    data.An_Time=query.value(record.indexOf("An_Time")).toFloat();
    data.Norm1=query.value(record.indexOf("Norm1")).toInt();
    data.Norm2=query.value(record.indexOf("Norm2")).toInt();
    data.Norm3=query.value(record.indexOf("Norm3")).toInt();
    data.BG=query.value(record.indexOf("BG")).toInt();
    data.Shift=query.value(record.indexOf("Shift")).toInt();
    data.Sample=query.value(record.indexOf("Sample")).toString().toStdString();
    data.Comment=query.value(record.indexOf("Comment")).toString().toStdString();
    data.LightSource=query.value(record.indexOf("LightSource")).toInt();
    data.Set=query.value(record.indexOf("SetSet")).toInt();
    data.Tag=query.value(record.indexOf("Tag")).toInt();
    data.Grain=query.value(record.indexOf("Grain")).toInt();
    data.LightPower=query.value(record.indexOf("LightPower")).toFloat();
    data.SystemID=query.value(record.indexOf("SystemID")).toInt();
    data.OnTime=query.value(record.indexOf("OnTime")).toFloat();
    data.OffTime=query.value(record.indexOf("OffTime")).toFloat();
    data.EnableFlags=query.value(record.indexOf("EnableFlags")).toInt();
    data.OnGateDelay=query.value(record.indexOf("OnGateDelay")).toInt();
    data.OffGateDelay=query.value(record.indexOf("OffGateDelay")).toInt();
    data.CurveNo=query.value(record.indexOf("CurveNo")).toInt();
    data.TimeTick=query.value(record.indexOf("TimeTick")).toInt();
    data.StimPeriod=query.value(record.indexOf("StimPeriod")).toInt();
    data.GateEnabled=query.value(record.indexOf("GateEnabled")).toInt();
    data.GateStart=query.value(record.indexOf("GateStart")).toInt();
    data.GateEnd=query.value(record.indexOf("GateEnd")).toInt();
    data.PTenabled=query.value(record.indexOf("PTenabled")).toInt();
    data.rangeSignal[0]=query.value(record.indexOf("rangeSignal_0")).toInt();
    data.rangeSignal[1]=query.value(record.indexOf("rangeSignal_1")).toInt();
    data.rangeBackground[0]=query.value(record.indexOf("rangeBackground_0")).toInt();
    data.rangeBackground[1]=query.value(record.indexOf("rangeBackground_1")).toInt();

    data.fadFunction=(FadFunction)(query.value(record.indexOf("fad_fun")).toInt());
    string aStr;
    for ( int i=0;i<2 ;++i ) {
        aStr="fad_"+TOOLS::convertToString<int>(i);
        data.fadParameters[i]=query.value(record.indexOf(aStr.c_str())).toDouble();
    }
    data.fadCovariantMatrix=gsl_matrix_alloc(2,2);
    for ( int i=0;i<2 ;++i ) {
        aStr="eFad_"+TOOLS::convertToString<int>(i);
        gsl_matrix_set(data.fadCovariantMatrix,i,i,pow(query.value(record.indexOf(aStr.c_str())).toDouble(),2));
        data.eFadParameters[i]=query.value(record.indexOf(aStr.c_str())).toDouble();
    }
    aStr="fad_cov_0_1";
    gsl_matrix_set(data.fadCovariantMatrix,0,1,query.value(record.indexOf(aStr.c_str())).toDouble());
    gsl_matrix_set(data.fadCovariantMatrix,1,0,query.value(record.indexOf(aStr.c_str())).toDouble());

    printf("In getDataFromQuery() eFad: %.2f %.2f",data.eFadParameters[0],data.eFadParameters[1]);

   return data;
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
void SqlHandle::deleteMeasurement(const int ID){
    QSqlQuery query;
    string queryStr="select num_m from INFO_2_"+TOOLS::convertToString<int>(ID);
    bool queryReturn;
    query.exec(queryStr.c_str());
    int num_m=0;
    while (query.next()) {
        num_m=query.value(0).toInt();
    }
    for (int i = 0; i < num_m; ++i) {
        queryStr="drop table if exists INFO_3_"+TOOLS::convertToString<int>(ID)+"_"+TOOLS::convertToString<int>(i+1);
        queryReturn=query.exec(queryStr.c_str());
        if(!queryReturn){
            qDebug()<<"Cannot drop table INFO_3!";
            qDebug()<<query.lastQuery();
        }
    }
    queryStr="drop table if exists INFO_2_"+TOOLS::convertToString<int>(ID);
    queryReturn=query.exec(queryStr.c_str());
    if(!queryReturn){
        qDebug()<<"Cannot drop table INFO_2!";
        qDebug()<<query.lastQuery();
    }
}
