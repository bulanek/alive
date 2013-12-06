#include "info.h"
#include "ui_info.h"


/*-----------------------------------------------------------------------------
 *  
 *-----------------------------------------------------------------------------*/
Info::Info(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Info)
{
    ui->setupUi(this);
    this->setWindowTitle("Measurement Info");
    _dataHandle=DataHandle::getInstance();
    _which=-1;
}



/*-----------------------------------------------------------------------------
 *  
 *-----------------------------------------------------------------------------*/
Info::~Info()
{
    delete ui;
}


/*-----------------------------------------------------------------------------
 *  
 *-----------------------------------------------------------------------------*/
void Info::accept(){
    this->storeData();
    this->hide();
}



/*-----------------------------------------------------------------------------
 *  
 *-----------------------------------------------------------------------------*/
void Info::setUIData(int which){
    _which=which;
    const Data data=_dataHandle->getData()[which];
    ui->lineEdit->setText(data.Sample.c_str());
    string Run=TOOLS::convertToString<int>(data.Run);
    ui->lineEdit_2->setText(Run.c_str());
    string Set=TOOLS::convertToString<int>(data.Set);
    ui->lineEdit_3->setText(Set.c_str());
    ui ->lineEdit_6->setText(data.Comment.c_str());
    string Position=TOOLS::convertToString<int>(data.Position);
    ui ->lineEdit_4->setText(Position.c_str());
    string Grain=TOOLS::convertToString<short>(data.Grain);
    ui->lineEdit_5->setText(Grain.c_str());


    QDateTime datetime;
    datetime.setTime_t(data.mDateTime);
    ui->dateTimeEdit->setDateTime(datetime);
    datetime.setTime_t(data.IRR_DateTime);
    ui->dateTimeEdit_2->setDateTime(datetime);
    string An_Temp=TOOLS::convertToString<double>(data.An_Temp);
    ui->lineEdit_9->setText(An_Temp.c_str());
    string Temperature=TOOLS::convertToString<double>(data.Temperature);
    ui->lineEdit_10->setText(Temperature.c_str());
    string BG=TOOLS::convertToString<int>(data.BG);
    ui->lineEdit_15->setText(BG.c_str());
    string  Low=TOOLS::convertToString<double>(data.Low);
    ui->lineEdit_11->setText(Low.c_str());
    string  High=TOOLS::convertToString<double>(data.High);
    ui->lineEdit_12->setText(High.c_str());
    string  Rate=TOOLS::convertToString<double>(data.Rate);
    ui->lineEdit_13->setText(Rate.c_str());
    string  XCoord=TOOLS::convertToString<int>(data.XCoord);
    ui->lineEdit_16->setText(XCoord.c_str());
    string  YCoord=TOOLS::convertToString<int>(data.YCoord);
    ui->lineEdit_27->setText(YCoord.c_str());
    string  LightPower=TOOLS::convertToString<double>(data.LightPower);
    ui->lineEdit_26->setText(LightPower.c_str());

//    string  EnableFlags=TOOLS::convertToString(data.EnableFlags);
//    ui->lineEdit_28->setText(EnableFlags.c_str());

    string  IRR_Time=TOOLS::convertToString<int>(data.IRR_Time);
    ui->lineEdit_17->setText(IRR_Time.c_str());

    string  Shift=TOOLS::convertToString<short>(data.Shift);
    ui->lineEdit_18->setText(Shift.c_str());

    string  On=TOOLS::convertToString<int>(data.On);
    ui->lineEdit_29->setText(On.c_str());
    string  Off=TOOLS::convertToString<short>(data.Off);
    ui->lineEdit_30->setText(Off.c_str());
    string  Delay=TOOLS::convertToString<short>(data.Delay);
    ui->lineEdit_31->setText(Delay.c_str());
//    string  OnTime=boost::lexical_cast<string>(data.OnTime);
//    ui->lineEdit_19->setText(OnTime.c_str());
//    string  OffTime=boost::lexical_cast<string>(data.OffTime);
//    ui->lineEdit_20->setText(OffTime.c_str());
//    string  OnGateDelay=boost::lexical_cast<string>(data.OnGateDelay);
//    ui->lineEdit_22->setText(OnGateDelay.c_str());
//    string  OffGateDelay=boost::lexical_cast<string>(data.OffGateDelay);
//    ui->lineEdit_21->setText(OffGateDelay.c_str());

    string  Norm1=TOOLS::convertToString<int>(data.Norm1);
    ui->lineEdit_32->setText(Norm1.c_str());
    string  Norm2=TOOLS::convertToString<int>(data.Norm2);
    ui->lineEdit_33->setText(Norm2.c_str());
    string  Norm3=TOOLS::convertToString<int>(data.Norm3);
    ui->lineEdit_34->setText(Norm3.c_str());

    string  An_Time=TOOLS::convertToString<double>(data.An_Time);
    ui->lineEdit_14->setText(An_Time.c_str());
    string  Bl_Time=TOOLS::convertToString<double>(data.Bl_Time);
    ui->lineEdit_8->setText(Bl_Time.c_str());

    cout<<endl;
    QComboBox* comboDType=ui->comboBox_4;
    for (unsigned i = 0; i < Data::DTYPE.size(); ++i) {
        comboDType->addItem(Data::DTYPE[i].c_str());
    }
    comboDType->setCurrentIndex(data.Dtype);

    QComboBox* comboLType=ui->comboBox_6;
    for (unsigned i = 0; i < Data::LTYPE.size(); ++i) {
        comboLType->addItem(Data::LTYPE[i].c_str());
    }
    comboLType->setCurrentIndex(data.LType);

    QComboBox* comboIrrType=ui->comboBox;
    for (unsigned i = 0; i < Data::IRR_TYPE.size(); ++i) {
        comboIrrType->addItem(Data::IRR_TYPE[i].c_str());
    }
    comboIrrType->setCurrentIndex(data.IRR_Type);

    QComboBox* comboIrrUnit=ui->comboBox_2;
    for (unsigned i = 0; i < Data::IRR_UNIT.size(); ++i) {
        comboIrrUnit->addItem(Data::IRR_UNIT[i].c_str());
    }
    comboIrrUnit->setCurrentIndex(data.IRR_Unit);

    QComboBox* comboBlUnit=ui->comboBox_3;
    for (unsigned i = 0; i < Data::BL_UNIT.size(); ++i) {
        comboBlUnit->addItem(Data::BL_UNIT[i].c_str());
    }
    comboBlUnit->setCurrentIndex(data.Bl_Unit);

    QComboBox* comboLightSource=ui->comboBox_5;
    for (unsigned i = 0; i < Data::LIGHT_SOURCE.size(); ++i) {
        comboLightSource->addItem(Data::LIGHT_SOURCE[i].c_str());
    }
    comboLightSource->setCurrentIndex(data.LightSource);
}


/*-----------------------------------------------------------------------------
 *  
 *-----------------------------------------------------------------------------*/
void Info::storeData(){
    Data data;

    data.Run=ui->lineEdit_2->text().toInt();
    data.Set=ui->lineEdit_3->text().toInt();
    data.Comment=ui ->lineEdit_6->text().toStdString();
    data.Position=ui ->lineEdit_4->text().toInt();
    data.Grain=ui->lineEdit_5->text().toShort();
    data.mDateTime=ui->dateTimeEdit->dateTime().toTime_t();
    data.IRR_DateTime=ui->dateTimeEdit_2->dateTime().toTime_t();
    string An_Temp=TOOLS::convertToString<short>(data.An_Temp);
    data.An_Temp=ui->lineEdit_9->text().toFloat();
    data.Temperature= ui->lineEdit_10->text().toShort();
    data.BG=ui->lineEdit_15->text().toInt();
    data.Low=ui->lineEdit_11->text().toFloat();
    data.High=ui->lineEdit_12->text().toFloat();
    data.Rate=ui->lineEdit_13->text().toFloat();
    data.XCoord=ui->lineEdit_16->text().toShort();
    data.YCoord=ui->lineEdit_27->text().toShort();
    data.LightPower=ui->lineEdit_26->text().toFloat();
//    data.EnableFlags=ui->lineEdit_28->text().toInt();
    data.Shift=ui->lineEdit_18->text().toShort();
    data.On=ui->lineEdit_29->text().toShort();
    data.Off=ui->lineEdit_30->text().toShort();
    data.Delay=ui->lineEdit_31->text().toShort();
//    data.OnTime=ui->lineEdit_19->text().toFloat();
//    data.OffTime=ui->lineEdit_20->text().toFloat();
//    data.OnGateDelay=ui->lineEdit_22->text().toInt();
//    data.OffGateDelay=ui->lineEdit_21->text().toInt();
    data.Norm1=ui->lineEdit_32->text().toInt();
    data.Norm2=ui->lineEdit_33->text().toInt();
    data.Norm3=ui->lineEdit_34->text().toInt();
    data.An_Time=ui->lineEdit_14->text().toFloat();
    data.Bl_Time=ui->lineEdit_8->text().toFloat();
    data.Dtype=ui->comboBox_4->currentIndex();
    data.LType=ui->comboBox_6->currentIndex();
    data.IRR_Type=ui->comboBox->currentIndex();
    data.IRR_Unit=ui->comboBox_2->currentIndex();
    data.Bl_Unit=ui->comboBox_3->currentIndex();
    data.LightSource=ui->comboBox_5->currentIndex();
    data.IRR_Time=ui->lineEdit_17->text().toFloat();
    data.Sample=ui->lineEdit->text().toStdString();

    _dataHandle->setDataNotPoints(_which,data);
}

