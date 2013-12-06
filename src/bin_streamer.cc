/**
  @author	Boris Bulanek ()
  @company National Radiation Protection Institute, Bartoskova 28, 140 00, Praha 4
  @email  boris.bulanek@suro.cz
  @tel.   226 518  279
  @date	01/08/13
  */
// =====================================================================================

#include	<string>


#include	"BinReader.hh"


#include	<cstdlib>

int main ( int argc, char *argv[] )
{

    BinReader* binReader=new BinReader(argv[1]);
    cout<<binReader<<endl;
    delete binReader;

    return EXIT_SUCCESS;
}				// ----------  end of function main  ----------

