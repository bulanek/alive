#include "data.h"

using namespace std;

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/

int Data::INSTANCE_COUNTER=0;

string LTYPE_ARRAY[]={"TL","OSL","IRSL","M-IR","M-VIS","TOL","TRPOSL","RIR","RBR","USER","POSL","SGOSL","RL","XRF"};
const vector<string>Data::LTYPE= vector<string>(LTYPE_ARRAY,LTYPE_ARRAY+sizeof(LTYPE_ARRAY)/sizeof(string));

string DTYPE_ARRAY[]={"Natural","N+dose","Bleach","Bleach+dose","Natural(Bleach)","N+dose(Bleach)","Dose","Background","Regenerative dose"};
const vector<string>Data::DTYPE=vector<string>(DTYPE_ARRAY,DTYPE_ARRAY+sizeof(DTYPE_ARRAY)/sizeof(string));

string LIGHT_SOURCE_ARRAY[]={"None","Lamp","IR diodes/IR laser","Calibration LED","Blue Diodes","White light","GreenLaser (single grain)","IR laser (single grain)"};
const vector<string>Data::LIGHT_SOURCE=vector<string>(LIGHT_SOURCE_ARRAY,LIGHT_SOURCE_ARRAY+sizeof(LIGHT_SOURCE_ARRAY)/sizeof(string));

string IRR_TYPE_ARRAY[]={"beta","alpha","gamma","X-ray","el.tmp.beta"};
const vector<string>Data::IRR_TYPE=vector<string>(IRR_TYPE_ARRAY,IRR_TYPE_ARRAY+sizeof(IRR_TYPE_ARRAY)/sizeof(string));

string IRR_UNIT_ARRAY[]={"Seconds","Gy","Rads","Minutes","Hours"};
const vector<string>Data::IRR_UNIT=vector<string>(IRR_UNIT_ARRAY,IRR_UNIT_ARRAY+sizeof(IRR_UNIT_ARRAY)/sizeof(string));

string BL_UNIT_ARRAY[]={"Seconds","Minutes","Hours","mJ","J"};
const vector<string>Data::BL_UNIT=vector<string>(BL_UNIT_ARRAY,BL_UNIT_ARRAY+sizeof(BL_UNIT_ARRAY)/sizeof(string));


string fad_formula[]={"[0]-[1]*log(x)","[0]*pow(x,[1])","[0]"};
const vector<string> Data::FAD_FORMULA=vector<string>(fad_formula,fad_formula+sizeof(fad_formula)/sizeof(string));

string fad_fit_functions[]={"Logaritmic","Exponential","Constant"};
const vector<string> Data::FAD_FUNCTIONS=vector<string>(fad_fit_functions,fad_fit_functions+sizeof(fad_fit_functions)/sizeof(string));
string var_names[]={
    "Version",
    "Length",
    "Previous",
    "NPoints",
    "LType",
    "Low",
    "High",
    "Rate",
    "Temperature",
    "XCoord",
    "YCoord",
    "Delay",
    "On",
    "Off",
    "Position",
    "Run",
    "Sequence",
    "User",
    "Dtype",
    "IRR_Time",
    "IRR_Type",
    "IRR_Unit",
    "Bl_Time",
    "Bl_Unit",
    "An_Temp",
    "An_Time",
    "Norm1",
    "Norm2",
    "Norm3",
    "BG",
    "Shift",
    "Sample",
    "Comment",
    "LightSource",
    "Set",
    "Tag",
    "Grain",
    "LightPower",
    "SystemID",

};

const vector<string> Data::VAR_NAMES=vector<string>(var_names,var_names+sizeof(var_names)/sizeof(string));

string user_var_names[]={
    "rangeSignal_0",
    "rangeSignal_1",
    "rangeBackground_0",
    "rangeBackground_1",
    "fad_fun",
    "fad_0",
    "fad_1",
    "eFad_0",
    "eFad_1",
    "fad_cov_0_1",
    "mDateTime",
    "IRR_DateTime"
};

const vector<string> Data::USER_VAR_NAMES=vector<string>(user_var_names,user_var_names+sizeof(user_var_names)/sizeof(string));


map<string,string> Data::VAR_NAMES_HELP=boost::assign::map_list_of
//    ("IRR_Type",helpTxt_IRR_TYPE)
//    ("IRR_Unit",helpTxt_IRR_UNIT)
//    ("BL_UNIT",helpTxt_BL_UNIT)
("Version","Data format version number")
("Length","Length of this record [bytes]")
("Previous","Length of previous record [bytes]")
("NPoints","Number of data points")
//    ("LType","Luminiscence type")
("Low","Low (temperature, time, wavelength)")
("High","High (temprature, time, wavelength)")
("Rate","Rage (heating rate, scan rate)")
("Temperature","Sample temperature")
("XCoord","X position of a single grain")
("YCoord","Y position of a single grain")
("Delay","TOL \"delay\" channels")
("On","TOL \"on\" channels")
("Off","TOL \"off\" channels")
("Position","Carousel position")
("Run","Run number")
("Sequence","Sequence name")
("User","User name")
//    ("Dtype","Data type")
("mDateTime","Measurement date and time")
("IRR_Time","Calibration irradiation duration [s]")
("IRR_DateTime","Date and time of irradiation ")
("An_Temp","Annealing temperature")
("An_Time","Annealing time")
("Norm1","Normalisation factor (1)")
("Norm2","Normalisation factor (2)")
("Norm3","Normalisation factor (3)")
("BG","Background level")
("Shift","Number of channels to shift data")
("Sample","Sample name")
("Comment","Comment")
//    ("LightSource","Light Source")
("Set","Set number")
("Tag","Tag")
("Grain","Grain number")
("LightPower","Optical Stimulation Power")
("SystemID","System ID")
("Bl_Time","Bleaching time")
("rangeSignal_0","Low limit of signal interval in channels")
("rangeSignal_1","Upper limit of signal interval in channels")
("rangeBackground_0","Low limit of background interval in channels")
("rangeBackground_1","Upper limit of background interval in channels")
//    ("fad_fun","")
("fad_0","Fading function parameter 0")
("fad_1","Fading function parameter 1")
("eFad_0","Error of fading parameter 0")
("eFad_1","Error of fading parameter 1")
("fad_cov_0_1","Covariance of fading parameters 0 and 1");


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
Data::Data(){

    if(INSTANCE_COUNTER==0){
        string helpTxt_IRR_TYPE="Irradiation type [";
        for ( unsigned i=0;i<Data::IRR_TYPE.size()-1 ;++i ) {
            helpTxt_IRR_TYPE+=TOOLS::convertToString<int>(i)+"-"+Data::IRR_TYPE[i]+",";
        }
        helpTxt_IRR_TYPE+=TOOLS::convertToString<int>(Data::IRR_TYPE.size()-1)+"-"+Data::IRR_TYPE[Data::IRR_TYPE.size()-1]+"]";

        //    var_names_help_IRR_TYPE["IRR_Type"]=helpTxt;

        string helpTxt_IRR_UNIT="Irradiation unit [";
        for ( unsigned i=0;i<Data::IRR_UNIT.size()-1 ;++i ) {
            helpTxt_IRR_UNIT+=TOOLS::convertToString<int>(i)+"-"+Data::IRR_UNIT[i]+",";
        }
        helpTxt_IRR_UNIT+=TOOLS::convertToString<int>(Data::IRR_UNIT.size()-1)+"-"+Data::IRR_UNIT[Data::IRR_UNIT.size()-1]+"]";
        //    var_names_help["IRR_Unit"]=helpTxt_IRR_UNIT;



        string helpTxt_BL_UNIT="Bleaching unit [";
        for ( unsigned i=0;i<Data::BL_UNIT.size()-1 ;++i ) {
            helpTxt_BL_UNIT+=TOOLS::convertToString<int>(i)+"-"+Data::BL_UNIT[i]+",";
        }
        helpTxt_BL_UNIT+=TOOLS::convertToString<int>(Data::BL_UNIT.size()-1)+"-"+Data::BL_UNIT[Data::BL_UNIT.size()-1]+"]";

        string helpTxt_LTYPE="Luminiscence type [";
        for ( unsigned i=0;i<Data::LTYPE.size()-1 ;++i ) {
            helpTxt_LTYPE+=TOOLS::convertToString<int>(i)+"-"+Data::LTYPE[i]+",";
        }
        helpTxt_LTYPE+=TOOLS::convertToString<int>(Data::LTYPE.size()-1)+"-"+Data::LTYPE[Data::LTYPE.size()-1]+"]";

        string helpTxt_DTYPE="Data type [";
        for ( unsigned i=0;i<Data::DTYPE.size()-1 ;++i ) {
            helpTxt_DTYPE+=TOOLS::convertToString<int>(i)+"-"+Data::DTYPE[i]+",";
        }
        helpTxt_DTYPE+=TOOLS::convertToString<int>(Data::DTYPE.size()-1)+"-"+Data::DTYPE[Data::DTYPE.size()-1]+"]";

        string helpTxt_LIGHT_SOURCE="Light source[";
        for ( unsigned i=0;i<Data::LIGHT_SOURCE.size()-1 ;++i ) {
            helpTxt_LIGHT_SOURCE+=TOOLS::convertToString<int>(i)+"-"+Data::LIGHT_SOURCE[i]+",";
        }
        helpTxt_LIGHT_SOURCE+=TOOLS::convertToString<int>(Data::LIGHT_SOURCE.size()-1)+"-"+Data::LIGHT_SOURCE[Data::LIGHT_SOURCE.size()-1]+"]";

        string helpTxt_FAD_FUNCTIONS="Fading functions [";
        for ( unsigned i=0;i<Data::FAD_FUNCTIONS.size()-1 ;++i ) {
            helpTxt_FAD_FUNCTIONS+=TOOLS::convertToString<int>(i)+"-"+Data::FAD_FUNCTIONS[i]+"("+Data::FAD_FORMULA[i] +")"+",";
        }
        helpTxt_FAD_FUNCTIONS+=TOOLS::convertToString<int>(Data::FAD_FUNCTIONS.size()-1)+"-"+Data::FAD_FUNCTIONS[Data::FAD_FUNCTIONS.size()-1]+"("+Data::FAD_FORMULA[Data::FAD_FORMULA.size()-1]+")"+"]";


        VAR_NAMES_HELP["IRR_Type"]=helpTxt_IRR_TYPE;
        VAR_NAMES_HELP["IRR_Unit"]=helpTxt_IRR_UNIT;
        VAR_NAMES_HELP["Bl_Unit"]=helpTxt_BL_UNIT;
        VAR_NAMES_HELP["LType"]=helpTxt_LTYPE;
        VAR_NAMES_HELP["Dtype"]=helpTxt_DTYPE;
        VAR_NAMES_HELP["LightSource"]=helpTxt_LIGHT_SOURCE;
        VAR_NAMES_HELP["fad_fun"]=helpTxt_FAD_FUNCTIONS;

        fadCovariantMatrix=gsl_matrix_alloc(2,2);
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                gsl_matrix_set(fadCovariantMatrix,i,j,0);
            }
        }
    }
    ++INSTANCE_COUNTER;
}


/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
Data::~Data(){
    DPoints.clear();
//    gsl_matrix_free(fadCovariantMatrix);
}


/*-----------------------------------------------------------------------------
 *  
 *-----------------------------------------------------------------------------*/
string Data::getValue(const string aVar) const{
    if(aVar=="Version"){
        return  TOOLS::convertToString<int>(Version);
    }else if(aVar=="Length"){
        return  TOOLS::convertToString<int>(Length);
    }else if(aVar=="Previous"){
        return TOOLS::convertToString<int>(Previous);
    }else if(aVar=="NPoints"){
        return TOOLS::convertToString<int>(NPoints);
    }else if(aVar=="LType"){
        return TOOLS::convertToString<int>(LType);
    }else if(aVar=="Low"){
        return TOOLS::convertToString<double>(Low);
    }else if(aVar=="High"){
        return TOOLS::convertToString<double>(High);
    }else if(aVar=="Rate"){
        return TOOLS::convertToString<double>(Rate);
    }else if(aVar=="Temperature"){
        return TOOLS::convertToString<double>(Temperature);
    }else if(aVar=="XCoord"){
        return TOOLS::convertToString<short>(XCoord);
    }else if(aVar=="YCoord"){
        return TOOLS::convertToString<short>(YCoord);
    }else if(aVar=="Delay"){
        return TOOLS::convertToString<short>(Delay);
    }else if(aVar=="On"){
        return TOOLS::convertToString<short>(On);
    }else if(aVar=="Off"){
        return TOOLS::convertToString<short>(Off);
    }else if(aVar=="Position"){
        return TOOLS::convertToString<int>(Position);
    }else if(aVar=="Run"){
        return TOOLS::convertToString<int>(Run);
    }else if(aVar=="Sequence"){
        return Sequence;
    }else if(aVar=="User"){
        return User;
    }else if(aVar=="Dtype"){
        return TOOLS::convertToString<int>(Dtype);
    }else if(aVar=="mDateTime"){
        time_t aTime=(time_t)mDateTime;
        return string(asctime(localtime(&aTime)));
    }else if(aVar=="IRR_DateTime"){
        time_t aTime=(time_t)IRR_DateTime;
        return string(asctime(localtime(&aTime)));
    }else if(aVar=="IRR_Time"){
        return TOOLS::convertToString<int>(IRR_Time);
    }else if(aVar=="IRR_Type"){
        return TOOLS::convertToString<int>(IRR_Type);
    }else if(aVar=="IRR_Unit"){
        return TOOLS::convertToString<int>(IRR_Unit);
    }else if(aVar=="Bl_Time"){
        return TOOLS::convertToString<double>(Bl_Time);
    }else if(aVar=="Bl_Unit"){
        return TOOLS::convertToString<int>(Bl_Unit);
    }else if(aVar=="An_Temp"){
        return TOOLS::convertToString<double>(An_Temp);
    }else if(aVar=="An_Time"){
        return TOOLS::convertToString<double>(An_Time);
    }else if(aVar=="Norm1"){
        return TOOLS::convertToString<int>(Norm1);
    }else if(aVar=="Norm2"){
        return TOOLS::convertToString<int>(Norm2);
    }else if(aVar=="Norm3"){
        return TOOLS::convertToString<int>(Norm3);
    }else if(aVar=="BG"){
        return TOOLS::convertToString<int>(BG);
    }else if(aVar=="Shift"){
        return TOOLS::convertToString<int>(Shift);
    }else if(aVar=="Sample"){
        return Sample;
    }else if(aVar=="Comment"){
        return Comment;
    }else if(aVar=="LightSource"){
        return TOOLS::convertToString<int>(LightSource);
    }else if(aVar=="Set"){
        return TOOLS::convertToString<int>(Set);
    }else if(aVar=="Tag"){
        return TOOLS::convertToString<int>(Tag);
    }else if(aVar=="Grain"){
        return TOOLS::convertToString<short>(Grain);
    }else if(aVar=="LightPower"){
        return TOOLS::convertToString<double>(LightPower);
    }else if(aVar=="SystemID"){
        return TOOLS::convertToString<short>(SystemID);
    }
    else if(aVar=="rangeSignal_0"){
        return TOOLS::convertToString<double>(rangeSignal[0]);
    }
    else if(aVar=="rangeSignal_1"){
        return TOOLS::convertToString<double>(rangeSignal[1]);
    }
    else if(aVar=="rangeBackground_0"){
        return TOOLS::convertToString<double>(rangeBackground[0]);
    }
    else if(aVar=="rangeBackground_1"){
        return TOOLS::convertToString<double>(rangeBackground[1]);
    }
    else if(aVar=="fad_fun"){
        return TOOLS::convertToString<double>(int(fadFunction));
    }
    else if(aVar=="fad_0"){
        return TOOLS::convertToString<double>(fadParameters[0]);
    }
    else if(aVar=="fad_1"){
        return TOOLS::convertToString<double>(fadParameters[1]);
    }
    else if(aVar=="eFad_0"){
        return TOOLS::convertToString<double>(sqrt(gsl_matrix_get(fadCovariantMatrix,0,0)));
    }
    else if(aVar=="eFad_1"){
        return TOOLS::convertToString<double>(sqrt(gsl_matrix_get(fadCovariantMatrix,1,1)));
    }
    else if(aVar=="fad_cov_0_1"){
        return TOOLS::convertToString<double>(gsl_matrix_get(fadCovariantMatrix,0,1));
    }
}




/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
Data& Data::operator=(const Data& data){
    if (this==&data){
        return *this;
    }
    this->Version=data.Version;
    this->Length=data.Length;
    this->Previous=data.Previous;
    this->NPoints=data.NPoints;
    this->LType=data.LType;
    this->Low=data.Low;
    this->High=data.High;
    this->Rate=data.Rate;
    this->Temperature=data.Temperature;
    this->XCoord=data.XCoord;
    this->YCoord=data.YCoord;
    this->Delay=data.Delay;
    this->On=data.On;
    this->Off=data.Off;
    this->Position=data.Position;
    this->Run=data.Run;

    for ( int i=0;i<Data::TIMEDATE_SIZE;++i ) {
        this->Time[i]=data.Time[i];
        this->Date[i]=data.Date[i];
    }

    this->Sequence=data.Sequence;
    this->User=data.User;
    this->Dtype=data.Dtype;
    this->mDateTime=data.mDateTime;
    this->IRR_DateTime=data.IRR_DateTime;
    this->IRR_Time=data.IRR_Time;
    this->IRR_Type=data.IRR_Type;
    this->IRR_Unit=data.IRR_Unit;
    this->Bl_Time=data.Bl_Time;
    this->Bl_Unit=data.Bl_Unit;
    this->An_Temp=data.An_Temp;
    this->An_Time=data.An_Time;
    this->Norm1=data.Norm1;
    this->Norm2=data.Norm2;
    this->Norm3=data.Norm3;
    this->BG=data.BG;
    this->Shift=data.Shift;
    this->Sample=data.Sample;
    this->Comment=data.Comment;
    this->LightSource=data.LightSource;
    this->Set=data.Set;
    this->Tag=data.Tag;
    this->Grain=data.Grain;
    this->LightPower=data.LightPower;
    this->SystemID=data.SystemID;

    if (data.Version==3){
        this->OnTime=data.OnTime;
        this->OffTime=data.OffTime;
        this->EnableFlags=data.EnableFlags;
        this->OnGateDelay=data.OnGateDelay;
        this->OffGateDelay=data.OffGateDelay;
    }
    if(data.Version==4){
        this->CurveNo=data.CurveNo;
        this->TimeTick=data.TimeTick;
        this->StimPeriod=data.StimPeriod;
        this->GateEnabled=data.GateEnabled;
        this->GateStart=data.GateStart;
        this->GateEnd=data.GateEnd;
        this->PTenabled=data.PTenabled;
    }

    this->fadCovariantMatrix=gsl_matrix_alloc(2,2);
    for ( int i=0;i<2;++i ) {
        this->fadParameters[i]=data.fadParameters[i];
        for (int j=0;j<2;++j){
            gsl_matrix_set(this->fadCovariantMatrix,i,j,gsl_matrix_get(data.fadCovariantMatrix,i,j));
        }
    }
    this->fadFunction=data.fadFunction;


    this->DPoints.clear();
    for ( unsigned i=0;i<data.DPoints.size();++i ) {
        this->DPoints.push_back(data.DPoints[i]);
    }
    this->DateTime=data.DateTime;
    this->fadFunction=data.fadFunction;

    for (int i = 0; i < 2; ++i) {
        this->rangeSignal[i]=data.rangeSignal[i];
        this->rangeBackground[i]=data.rangeBackground[i];
    }
    return *this;
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
ostream& operator<<(ostream& stream,const Data* data){

    stream<<"\t\t Luminiscence type: "<<Data::LTYPE[data->LType]<<endl
        <<"\t\t Low (temperature,time,wavelength): "<<data->Low<<endl
        <<"\t\t High (temperature,time,wavelength): "<<data->High<<endl
        <<"\t\t Rate (heating rate, scan rate): "<<data->Rate<<endl
        <<"\t\t Sample temperature: "<<data->Temperature<<endl
        <<"\t\t X position of single grain: "<<data->XCoord<<endl
        <<"\t\t Y position of single grain: "<<data->YCoord<<endl
        <<"\t\t TOL delay channels: "<<data->Delay<<endl
        <<"\t\t TOL on channels: "<<data->On<<endl
        <<"\t\t TOL off channels: "<<data->Off<<endl
        <<"\t\t Carousel position: "<<int(data->Position)<<endl
        <<"\t\t Run Number: "<<int(data->Run)<<endl
        <<"\t\t Date and time of measurement(orig.): "<<data->Date<<" "<<data->Time<<endl
        <<"\t\t Date and time of measurement: "<<asctime(&data->DateTime)<<endl
        <<"\t\t Sequence name: "<<data->Sequence<<endl
        <<"\t\t User name: "<<data->User<<endl
        <<"\t\t Data type: "<<Data::DTYPE[data->Dtype]<<endl
        <<"\t\t Irradiation time: "<<data->IRR_Time<<endl
        <<"\t\t Irradiation type: "<<Data::IRR_TYPE[data->IRR_Type]<<endl
        <<"\t\t Irradiation unit: "<<Data::IRR_UNIT[data->IRR_Unit]<<endl
        <<"\t\t Bleaching time: "<<data->Bl_Time<<endl
        <<"\t\t Bleaching unit: "<<Data::BL_UNIT[data->Bl_Unit]<<endl
        <<"\t\t Annealing temperature: "<<data->An_Temp<<endl
        <<"\t\t Annealing time: "<<data->An_Time<<endl
        <<"\t\t Normalization factor (2): "<<data->Norm2<<endl
        <<"\t\t Normalization factor (3): "<<data->Norm3<<endl
        <<"\t\t Background level: "<<data->BG<<endl
        <<"\t\t Number of channels to shift data: "<<data->Shift<<endl
        <<"\t\t Sample name: "<<data->Sample<<endl
        <<"\t\t Comment: "<<data->Comment<<endl
        <<"\t\t Light source: "<<Data::LIGHT_SOURCE[data->LightSource]<<endl
        <<"\t\t Set number: "<<int(data->Set)<<endl
        <<"\t\t Tag: "<<int(data->Tag)<<endl
        <<"\t\t Grain number: "<<data->Grain<<endl
        <<"\t\t Optical stimulation power: "<<data->LightPower<<endl
        <<"\t\t System ID: "<<data->SystemID<<endl;
    if (data->Version==3){
        stream<<"\t\t On-time for pulsed stimulation [s]: "<<data->OnTime<<endl
            <<"\t\t Off-time for pulsed stimulation [s]: "<<data->OffTime<<endl
            <<"\t\t Enable flags (PMT Gating and Photon Timer enable): "<<data->EnableFlags<<endl
            <<"\t\t On-gate delay [s]: "<<data->OnGateDelay<<endl
            <<"\t\t Off-gate delay [s]: "<<data->OffGateDelay<<endl;
    }else if(data->Version==4){
        stream<<"\t\t Curve number (for multiple curve operation): "<<data->CurveNo<<endl
            <<"\t\t Time unit for pulse parameters: "<<data->StimPeriod<<endl
            <<"\t\t PMT signal gating enabled: "<<data->GateEnabled<<endl
            <<"\t\t Start of gating (in time ticks from start of on pulse): "<<data->GateStart<<endl
            <<"\t\t End of gating (in time ticks from start of on pulse): "<<data->GateEnd<<endl
            <<"\t\t Photon Timer enabled: "<<data->PTenabled<<endl;
    }
    stream<<"Fading function: "<<Data::FAD_FUNCTIONS[data->fadFunction]<<endl;
    stream<<"Fading Parameters: "<<endl;
    for (int i = 0; i < 2; ++i) {
        stream<<"\t"<<data->fadParameters[i]<<"+-"<<gsl_matrix_get(data->fadCovariantMatrix,i,i)<<endl;
    }
    stream<<"Range of signal: "<<data->rangeSignal[0]<<" - "<<data->rangeSignal[1]<<endl;
    stream<<"Range of background: "<<data->rangeBackground[0]<<" - "<<data->rangeBackground[1]<<endl;

    stream<<"First 5 points: "<<endl;
    for (int i = 0; i < data->NPoints; ++i) {
        if(i>=5) break;
        stream<<"\t"<<data->DPoints[i]<<endl;
    }
    return stream;
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
ostream& operator<<(ostream& stream,const Data& data){
    stream<<&data;
    return stream;
}
