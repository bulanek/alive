
/**
  @brief

  Detailed description follows here.

  @author	Boris Bulanek ()
  @company National Radiation Protection Institute, Bartoskova 28, 140 00, Praha 4
  @email  boris.bulanek@suro.cz
  @tel.   226 518  279
  @date	01/09/13
  */
// =====================================================================================

#include	"sys/stat.h"

#include	<cstdio>
#include	<cstdlib>
#include	<cerrno>
#include	<cstring>
#include	<iostream>
#include	<string>
#include	<list>

#include	<vector>
#include	<ctime>

#include "data.h"

#ifndef BINREADER_HH 
#define BINREADER_HH



using namespace std;


struct Data;




/*-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------*/
class BinReader{
public:
    BinReader(const char* fileName);
    BinReader(){}
    ~BinReader();
    vector<Data> setData(const char* fileName);
    const struct tm& getDateTime(const unsigned which) ;
    string getPascalString(FILE* inFile,const int aSize) const;
    inline const vector<Data>& getData() const{return _data;}
    inline const int getNumberOfBytes() const {return _numOfBytes;}
    const static int LENGTH_OF_HEADER=272;

private:
    struct stat _results;
    int _numOfBytes;
    vector<Data> _data;
    friend ostream& operator<<(ostream& stream ,const BinReader& baseClass);
    friend ostream& operator<<(ostream& stream ,const BinReader* baseClass);
};

#endif
