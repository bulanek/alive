/**
  @author	Boris Bulanek ()
  @company National Radiation Protection Institute, Bartoskova 28, 140 00, Praha 4
  @email  boris.bulanek@suro.cz
  @tel.   226 518  279
  @date	01/30/13
  */
// =====================================================================================
#ifndef DATA_H
#define DATA_H


#include	<cstdio>
#include	<cstdlib>
#include	<cerrno>
#include	<cstring>
#include	<iostream>
#include	<string>
#include	<list>
#include 	<map>

#include	<vector>
#include	<ctime>

#include "boost/assign.hpp"
#include <gsl/gsl_matrix.h>


#include	"tools.h"

using namespace std;

enum FadFunction{
    Logaritmic,Exponential,Constant
};
enum EFadFunction{
    eLogaritmic,eExponential,eConstant
};



class Data{
public:
    Data();
    ~Data();
    Data& operator=(const Data& data);

    string getValue(const string aVar) const;

    short Version;
    short Length;
    short Previous;
    short NPoints;
    int LType;
    float Low;
    float High;
    float Rate;
    short Temperature;
    short XCoord;
    short YCoord;
    short Delay;
    short On;
    short Off;
    int Position;
    int Run;
    char Time[7];
    char Date[7];
    //    char Sequence[9];
    //    char User[9];
    string Sequence;
    string User;
    int Dtype;
    float IRR_Time;
    int IRR_Type;
    int IRR_Unit;
    float Bl_Time;
    int Bl_Unit;
    float An_Temp;
    float An_Time;
    int Norm1;
    int Norm2;
    int Norm3;
    int BG;
    short Shift;
    //    char Sample[21];
    //    char Comment[81];
    string Sample;
    string Comment;
    int LightSource;
    int Set;
    int Tag;
    short Grain;
    float LightPower;
    short SystemID;

    char RESERVED_3_1[36];
    float OnTime;
    float OffTime;
    int EnableFlags;
    int     OnGateDelay;
    int OffGateDelay;
    char RESERVED_3_2;

    char RESERVED_4_1[20];
    int CurveNo;
    int TimeTick;
    int StimPeriod;
    int GateEnabled;
    int     GateStart;
    int GateEnd;
    bool PTenabled;
    char RESERVED_4_2[10];

    vector<int> DPoints;
    int mDateTime;
    int IRR_DateTime;
    const static int TIMEDATE_SIZE=7;
    const static int USER_SEQUENCE_SIZE=9;
    const static int SAMPLE_SIZE=21;
    const static int COMMENT_SIZE=81;

    static const vector<string> LTYPE;
    static const vector<string> DTYPE;
    static const vector<string> LIGHT_SOURCE;
    static const vector<string>  IRR_TYPE;
    static const vector<string> IRR_UNIT;
    static const vector<string> BL_UNIT;
    struct tm DateTime;
    friend ostream& operator<<(ostream& stream ,const Data& aData);
    friend ostream& operator<<(ostream& stream ,const Data* aData);

    static const vector<string> FAD_FORMULA;
    static const vector<string> FAD_FUNCTIONS;
    double fadParameters[2];
    double eFadParameters[2];
    double fad_cov_0_1;
    gsl_matrix* fadCovariantMatrix;


    FadFunction fadFunction;
    double rangeSignal[2];
    double rangeBackground[2];

    const static vector<string> VAR_NAMES;
    const static vector<string> USER_VAR_NAMES;

    inline static const map<string,string>& getVarNamesHelp() {return VAR_NAMES_HELP;}
private:
    static map<string,string> VAR_NAMES_HELP;
    static int INSTANCE_COUNTER;
};


#endif // DATA_H
