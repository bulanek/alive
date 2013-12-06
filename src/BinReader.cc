/**
  @author	Boris Bulanek ()
  @company National Radiation Protection Institute, Bartoskova 28, 140 00, Praha 4
  @email  boris.bulanek@suro.cz
  @tel.   226 518  279
  @date	01/09/13
  */
// =====================================================================================

#include	"BinReader.hh"

using namespace std;

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
BinReader::BinReader(const char* fileName){
    this->setData(fileName);
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
BinReader::~BinReader(){
    _data.clear();
}


/*-----------------------------------------------------------------------------
 *  
 *-----------------------------------------------------------------------------*/
vector<Data> BinReader::setData(const char* fileName){

    /*  */
    for ( unsigned i=0;i<_data.size() ;++i ) {
        _data[i].DPoints.clear();
    }
    _data.clear();


    FILE	*inFile;										/* input-file pointer */
    inFile	= fopen( fileName, "rb" );
    if ( inFile == NULL ) {
        fprintf ( stderr, "couldn't open file '%s'; %s\n",
                fileName, strerror(errno) );
        exit (EXIT_FAILURE);
    }

    if(stat(fileName,&_results)==0){
        _numOfBytes=int(_results.st_size);
    }else{
        printf ( "Can't find the number of file bytes \n" );
        exit(EXIT_FAILURE);
    }
    rewind(inFile);

    /*  main program */

    int currentNumberOfBytes=0;

    while(!feof(inFile)&&(_numOfBytes-currentNumberOfBytes)>=LENGTH_OF_HEADER){
        Data* aData=new Data();
        fread(&aData->Version,1,sizeof(short),inFile);
        fread(&aData->Length,1,sizeof(short),inFile);
        fread(&aData->Previous,1,sizeof(short),inFile);
        fread(&aData->NPoints,1,sizeof(short),inFile);

        char LType;
        fread(&LType,1,sizeof(bool),inFile);
        aData->LType=int(LType);

        fread(&aData->Low,1,sizeof(int),inFile);
        fread(&aData->High,1,sizeof(int),inFile);
        fread(&aData->Rate,1,sizeof(int),inFile);
        fread(&aData->Temperature,1,sizeof(short),inFile);
        fread(&aData->XCoord,1,sizeof(short),inFile);
        fread(&aData->YCoord,1,sizeof(short),inFile);
        fread(&aData->Delay,1,sizeof(short),inFile);
        fread(&aData->On,1,sizeof(short),inFile);
        fread(&aData->Off,1,sizeof(short),inFile);

        char Position;
        fread(&Position,1,sizeof(bool),inFile);
        aData->Position=int(Position);

        char Run;
        fread(&Run,1,sizeof(bool),inFile);
        aData->Run=int(Run);

        char aTime[7];
        fread(aTime,1,7,inFile);
        for ( int i=1;i<7 ;++i ) aData->Time[i-1]=aTime[i];
        aData->Time[6]='\0';

        char Date[7];
        fread(Date,1,7,inFile);
        for ( int i=1;i<7 ;++i ) aData->Date[i-1]=Date[i];
        aData->Date[6]='\0';


        aData->Sequence=this->getPascalString(inFile,Data::USER_SEQUENCE_SIZE);
        aData->User=this->getPascalString(inFile,Data::USER_SEQUENCE_SIZE);


        char Dtype;
        fread(&Dtype,1,sizeof(char),inFile);
        aData->Dtype=int(Dtype);

        fread(&aData->IRR_Time,1,sizeof(float),inFile);

        char IRR_Type;
        fread(&IRR_Type,1,sizeof(char),inFile);
        aData->IRR_Type=int(IRR_Type);

        char IRR_Unit;
        fread(&IRR_Unit,1,sizeof(char),inFile);
        aData->IRR_Unit=int(IRR_Unit);

        fread(&aData->Bl_Time,1,sizeof(int),inFile);

        char Bl_Unit;
        fread(&Bl_Unit,1,sizeof(char),inFile);
        aData->Bl_Unit=int(Bl_Unit);


        fread(&aData->An_Temp,1,sizeof(int),inFile);
        fread(&aData->An_Time,1,sizeof(int),inFile);
        fread(&aData->Norm1,1,sizeof(int),inFile);
        fread(&aData->Norm2,1,sizeof(int),inFile);
        fread(&aData->Norm3,1,sizeof(int),inFile);
        fread(&aData->BG,1,sizeof(int),inFile);
        fread(&aData->Shift,1,sizeof(short),inFile);

        aData->Sample=this->getPascalString(inFile,Data::SAMPLE_SIZE);
        aData->Comment=this->getPascalString(inFile,Data::COMMENT_SIZE);

        char LightSource;
        fread(&LightSource,1,sizeof(bool),inFile);
        aData->LightSource=int(LightSource);

        char Set;
        fread(&Set,1,sizeof(bool),inFile);
        aData->Set=int(Set);

        char Tag    ;
        fread(&Tag,1,sizeof(bool),inFile);
        aData->Tag=int(Tag);

        fread(&aData->Grain,1,sizeof(short),inFile);
        fread(&aData->LightPower,1,sizeof(int),inFile);
        fread(&aData->SystemID,1,sizeof(short),inFile);
        if(aData->Version==3){
            fread(&aData->RESERVED_3_1,1,36,inFile);
            fread(&aData->OnTime,1,sizeof(int),inFile);
            fread(&aData->OffTime,1,sizeof(int),inFile);

            char EnableFlags;
            fread(&EnableFlags,1,sizeof(bool),inFile);
            aData->EnableFlags=int(EnableFlags);

            fread(&aData->OnGateDelay,1,sizeof(int),inFile);
            fread(&aData->OffGateDelay,1,sizeof(int),inFile);
            fread(&aData->RESERVED_3_2,1,1,inFile);
        }else if(aData->Version==4){
            fread(&aData->RESERVED_4_1,1,20,inFile);
            fread(&aData->CurveNo,1,sizeof(short),inFile);
            fread(&aData->TimeTick,1,sizeof(int),inFile);
            fread(&aData->OnTime,1,sizeof(int),inFile);
            fread(&aData->StimPeriod,1,sizeof(int),inFile);

            char GateEnabled;
            fread(&GateEnabled,1,sizeof(bool),inFile);
            aData->GateEnabled=int(GateEnabled);

            fread(&aData->GateStart,1,sizeof(int),inFile);
            fread(&aData->GateEnd,1,sizeof(int),inFile);

            fread(&aData->PTenabled,1,sizeof(bool),inFile);

            fread(&aData->RESERVED_4_2,1,10,inFile);
        }

        int tmpdata;
        for ( int i=0;i<aData->NPoints ;++i ) {
            fread(&tmpdata,1,sizeof(int),inFile);
            aData->DPoints.push_back(tmpdata);
        }
        _data.push_back(*aData);
        currentNumberOfBytes+=LENGTH_OF_HEADER+4*aData->NPoints;
    }

    for ( unsigned i=0;i<_data.size() ;++i ) {
        /*  set datetime */
        this->getDateTime(i);
        /* set default fading parameters as constant fad function and no fading */
        _data[i].fadFunction=Constant;
        for (int j = 0; j < 2; ++j) {
            _data[i].fadParameters[j]=1;
            _data[i].eFadParameters[j]=0;
        }
        _data[i].fadCovariantMatrix=gsl_matrix_alloc(2,2);
        for (int k = 0; k < 2; ++k) {
            for (int l = 0; l < 2; ++l) {
                gsl_matrix_set(_data[i].fadCovariantMatrix,k,l,0);
            }
        }
        /* set default interval range for signal and background */
        _data[i].rangeSignal[0]=0;
        _data[i].rangeSignal[1]=19;
        _data[i].rangeBackground[0]=20;
        _data[i].rangeBackground[1]=39;
    }





    /*  Close file */
    if( fclose(inFile) == EOF ) {			/* close input file   */
        fprintf ( stderr, "couldn't close file '%s'; %s\n",
                fileName, strerror(errno) );
        exit (EXIT_FAILURE);
    }
    return _data;
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 *  
 *-----------------------------------------------------------------------------*/
const struct tm& BinReader::getDateTime(const unsigned which) {
    if (which>=_data.size()){
        fprintf(stderr,"Cannot get datetime from %i measurement",which+1);
        exit(EXIT_FAILURE);
    }

    char what[3];
    memcpy(what,_data[which].Date,2);
    what[2]='\0';
    _data[which].DateTime.tm_mday=atoi(what);
    memcpy(what,_data[which].Date+2,2);
    what[2]='\0';
    _data[which].DateTime.tm_mon=atoi(what)-1;
    memcpy(what,_data[which].Date+4,2);
    what[2]='\0';
    string year="20"+string(what);
    _data[which].DateTime.tm_year=atoi(year.c_str())-1900;

    memcpy(what,_data[which].Time,2);
    what[2]='\0';
    _data[which].DateTime.tm_hour=atoi(what);
    memcpy(what,_data[which].Time+2,2);
    what[2]='\0';
    _data[which].DateTime.tm_min=atoi(what);
    memcpy(what,_data[which].Time+4,2);
    what[2]='\0';
    _data[which].DateTime.tm_sec=atoi(what);

    _data[which].mDateTime=mktime(&_data[which].DateTime);

    return _data[which].DateTime;

}

/*-----------------------------------------------------------------------------
 *  
 *-----------------------------------------------------------------------------*/
string BinReader::getPascalString(FILE* inFile,const int aSize) const{
    int length;
    char aChar=0;
    fread(&aChar,1,sizeof(char),inFile);
    length=int(aChar);
    string outString;
    for (int i = 0; i < aSize-1; ++i) {
        fread(&aChar,1,sizeof(char),inFile);
        if(i<length){
            outString.push_back(aChar);
        }
    }
    return outString;
}

/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
ostream& operator<<(ostream& stream,const BinReader* binReader){
    stream<<"Sequence editor output info: "<<endl
        <<"Number of measurements: "<<binReader->_data.size()<<endl;
    if(binReader->_data.size()!=0){
        stream<<"Version: "<<binReader->_data[0].Version<<endl;
    }
    string aTime;
    string aDate;


    for ( unsigned i=0;i<binReader->_data.size() ;++i ) {
        stream<<"Data for particular measurement:"<<endl;
        stream<<"\tMeasurement "<<i+1<<endl
            <<"\t\t NPoints: "<<binReader->_data[i].NPoints<<endl
            <<"\t\t First 5 points:"<<endl;
        for (int j=0 ;j<5 ;++j ) {
            stream<<"\t\t\t"<<binReader->_data[i].DPoints[j]<<endl;
        }

        stream<<binReader->_data[i]<<endl;
    }

    return stream;
}
/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
ostream& operator<<(ostream& stream,const BinReader& binReader){
    stream<<&binReader;
    return stream;
}
