#include "mainwindow.h"
#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include "datahandle.h"
/**
  @author	Boris Bulanek 
  @company National Radiation Protection Institute, Bartoskova 28, 140 00, Praha 4
  @email  boris.bulanek@suro.cz
  @tel   00420 226 518  279
  @date	02/19/13
  */
// =====================================================================================

/*! \mainpage %Alive Documentation
  \section about About the program 

  Program Alive (<b>A</b>nalyst <b>li</b>ght <b>ve</b>rsion) is created in order to provide simple and fast OSL dose estimation.
  Program allow user to calculate a dose using at least one calibration and one natural signal. 
  Signal is corrected on fading. Definition of a signal value is 
  tested by \ref plateau_test "Plateau test".

  A binary file from the Sequence reader of  <a href="http://www.nutech.dtu.dk/Produkter/Dosimetri/NUK_instruments/TL_OSL_readers.aspx?sc_lang=en">Risø software</a> can be used as an input.
  Program allow user to obtain and store data using <a href="http://www.sqlite.org">Sqlite</a>  database.
  User can dump all data to txt file as well.


  The program uses some additional packages: 
    - UI (User Interface) framework <a href="http://qt-project.org/">Qt</a> 
    - a package for minimization  <a href="http://www.gnu.org/software/gsl/">GSL</a>
    - collection of <tt>C++</tt> tools <a href="http://www.boost.org/">Boost</a> 
    - database software called <a href="http://www.sqlite.org">Sqlite</a> 

 \section Installation

  A single executable file \b alive.exe  is created for Windows (7 or XP) users using <a href="http://pic.dhe.ibm.com/infocenter/aix/v7r1/index.jsp?topic=%2Fcom.ibm.aix.prftungd%2Fdoc%2Fprftungd%2Fwhen_dyn_linking_static_linking.htm">static linking</a>. Linux users can execute \c alive.exe using program <a href="http://www.winehq.org/">wine</a>.
  I haven't tried to install the program on Mac but I believe that the installation is similar to installation on Linux.
  \subsection Linux
  All of needed packages have to be in official repositories of your distribution as all of them are open source programs.  For example in case of <a href="https://www.archlinux.org/"> Arch Linux</a> distribution, you have only to write down command like:\n <tt>sudo pacman -S cmake boost boost-build boost-libs gsl qt sqlite3 sqliteman</tt>.\n 
  After installation you have to go to the src directory and  e.g. follow these steps:\n
  <tt>
  mkdir build\n
  cd build\n
  qmake-qt4 ..\n
  make\n
  ./alive\n
  </tt>
  This approach uses shared library linking where the size of the executable \c alive is considerably smaller and you can use all of additional packages mentioned in the beginning of the section.


  \subsection sql_win Sqlite database

  All the information from binary file and some additional parameters can be stored in a simple database called <a href="http://www.sqlite.org">Sqlite</a>.  
  There exist graphical tool for working with a Sqlite database called <a href="http://www.sqliteman.com">Sqliteman</a>. 


\section running_code Program user guide
\subsection algorithm Algorithm

The signal is defined as a sum of first \f$N\f$ channels signal value minus signal of  the next \f$N\f$ channels due to background substraction.
Error of signal in channels is given by Poisson distribution. 
In order to compute dose of the "Natural" signal, there is minimum one "Dose" signal with known irradiation time required. 
User can choose functions following calibration functions:
    - linear \f$a+bx\f$; used by default; in case of one calibration point only \f$a=0\f$
    - quadratic \f$a+bx+cx^2\f$
    - cubic \f$a+bx+cx^2+dx^3\f$
    - exponential \f$a*\exp((x-b)/|c|)\f$
    - line+exponential \f$a*\exp((x-b)/|c|)+d*x\f$

Parameters \f$a,\dots ,d\f$ are written in program as \f$[0],\dots ,[3]\f$.

The proper parameters of function are estimated using \f$\chi^2\f$ minimization. 
The error of the dose is computed using assumption of linear behaviour of function at the point of the signal in error range: \f$\delta(dose)=\delta(signal)/\frac{\partial f(x)}{\partial x}_{|f(x)=signal}\f$.  

The fading function value is defined as a value which you have to multiply by the hypothetical unfaded signal  value in order to get the measured (faded) value. \n
User can choose  one of three possible fading functions for every measurement:
    - constant \f$a\f$ used default with \f$a=1\f$ (no fading)
    - logarithmic \f$a-b\ln(t)\f$
    - exponential \f$a*t^{-b}\f$
, where \f$t\f$ is the time between irradiation and measurement.\n
Fading parameters \f$a,b\f$ are written in the program as \f$fad\_0,fad\_1\f$.
User can adjust as the fading parameters as the  time-unit. Dose \f$D\f$ is corrected by fading of signal itself and fading of the calibration signal using formula:\n
\f$D_{corrected}=\frac{F_{calibration}}{F_{natural}}\cdot D_{uncorrected}\f$, where \f$F\f$ is previously mentioned fading correction.  
Because of assumption of the same time between calibration irradiation and measurement of the signal for all the calibration points  there is no separate correction of calibration signal. It can be easily changed in the future.


\subsection running_program Running program


The program is executed by opening the binary file \c alive.exe.  The program is reading a configuration file \c alive.xml where e.g. the default path to the database is written. It has to be in the same directory as an executable file. 
After executing alive executable  and open file (database entry) through
<tt> File->Open File (Db) </tt> you will see \n
\image html mainwindow.png 
1. Graphs of all measurements in the binary file (database). \n
2. The table of single measurements information. You can select, which variable to show by clicking on the button 
\c  Variables. Then you will see the window with all the variables. They are divided in two groups.
First group are the variables, which are stored in the origin binary file created by <tt>Sequence Viewer</tt>. 
The second group are variables created for this program. 
While clicking and staying with the mouse on the name, you can see the help information. This information is provided by the manual for \c Analyst  by Risø. The same names of variables are used while saving information into \c  Sqlite  database. 

You can select a subset of measurements for analysis editing a line <tt> Edit Measurement </tt> in a main window.
You can select as a single measurement as a range of measurements (e.g. 1,2-4). 
In order to adjust informations as irradiation and measurement time due to fading correction or irradiation time and type of data for calibration and subsequent dose estimation, you have to edit the needed variables by selecting proper measurement number and clicking on <tt>Edit Measurement</tt> button. 
\image html info.png 
After adjusting of all parameters, you can see the signal-dose graph with calibration fit by clicking on
<tt>Dose estimation</tt> button. 
\image html dose.png 
You can see the table with variables needed to estimate uncorrected signal from data(signal and background range), 
    fading correction (fading function and parameters) and informations about data type and irradiation time. 
    You can select time unit of time in a fading function, show uncorrected signal data by clicking on <tt> Show signal data</tt>, change calibration function, show parameters of calibration function (you have to click on replot in order to change the parameters) by clicking on <tt> Fit result</tt> button. 
    By clicking on <tt> Compute Dose</tt> button and writting down the particular measurement, you will see the table
    of computed dose with(out) fading correction and fading correction itself. \n

    In order to save data for further processing you can save all the data to the txt file by clicking on 
    <tt> File->Save txt</tt> in a main window. Until now there is not a possibility to read such a txt file for further processing. But you have a possibility to store and reuse of all data by using the <tt> Sqlite </tt> database.
    In order to save all the information to a database, you have first to \ref setup_sql "create the database".
    You can save the database by clicking <tt> File->Save db</tt> in main window. 
\image html sql.png 
    You will see the window where you have to type the name of the database and in case of saving data the name of database entry.
    After typing the database name and clicking on the <tt>Connect to database</tt> button you will see the table of all already saved binary files. Right clicking on the particular line, you will see the possibility of deletion of \c Sqlite data from particular binary file with deletion of all subtables or looking deeper on measurements for particular saved binary file.  
     After right clicking on the particular measurement, you can look on the measurement data. All the tables are editable and by editing them you are changing directly the information in the \c Sqlite database. 
     More about organization of database tables is written in section \ref db_tables "Database tables". 
  

    \subsection plateau_test Plateau test
    Clicking on the <tt>"Show Plateau"</tt> button in the main window, you can see the graph of the dependence of the calculated uncorrected dose on the range of a signal and background interval.
    The range value \c VALUE on the x-axis means that the signal for the dose calculation is evaluated as integral (0,\c VALUE) minus background integral (\c VALUE+1,2*\c VALUE+1).
    \image html plateau.png 


    \subsection db_tables Database tables
    All of the tables are generated automaticaly while saving the database. 
    Tables in the \c Sqlite database are divided into three levels. First-level table called INFO_1 is a table of saved binary files. Every line is connected to informations of particular binary file. This table included only information like ID(identification number, automaticaly incrementaly generated from 1), number of measurements, time of saving data. Second-level of tables called INFO_2_<ID> are tables of information for particular file where ID is mentioned identification number. Every line is connected to information of one single measurement of original binary file.  This tables include information like data type of measurement, luminiscence type, irradiation time etc. 
    The third level of tables called INFO_3_<ID>_<num_measurement> are tables of data of particular measurement where num_measurement is a number of measurement of particular database entry from binary file with identification number ID.\n

    E.g. if i've saved two binary files into \c Sqlite database and want to see data of the fourth measurement of the second binary file, have a look into \c INFO_1 table and look on the ID of the second measurement (have to be 2), and show the \c INFO_3_2_4. If you are using the \c Sqlite database window table of the program (<tt>File->Open db</tt> or <tt>File->Save db</tt>) you can show this table by clicking on the line with the correct ID in INFO_1 table and then on the line of the right measurement in the second-level table. 

    There is a possibility to dump all the information from run to the txt  file.

    \section devel Developement of the program
    The code is developed using C++ programming language and files developed in <tt>C++</tt> are all file with .cc (source) and .h (header) sufix.
    Files with .ui sufix are created by using Qt designer tools for UI (User interface) developement. Compilation generate files with ui and moc prefixes (more <a href="http://qt-project.org/doc/qt-4.8/qmake-manual.html">here</a>).

    The documentation of the code is created by <a href="http://www.stack.nl/~dimitri/doxygen/">doxygen</a>. This document page is created by it as well. Clicking on the <tt> "Data Structures -> Class Hierarchy" </tt> at the top of this page you can see the objects (classes) of this program. Following is the brief description of all classes. 

    \subsection brief_dev Program structure overview
    Obtain data from binary file is provided by BinReader class. Storage of data for one single measurement is provided by Data class. 
    Computing of needed observables asi the dose and fading correction is provided by DataHandle class.  
    Reading and writting the \c Sqlite database is provided by SqlHandle class. 

    Classes which provide the user interface are classes inherited by QMainWindow and QDialog. 
    There are also .ui files with the same name providing the design of windows only.
    - MainWindow is the class of the window, which you can see after executing the program executable.
    - Info is connected to the window, which you can see after clicking on Edit Measurement button in MainWindow.
    - Calibration is connected to the window, which you can see after clicking on Dose estimation button in MainWindow
    - Plateau is connected to the windo, which you can see after clicking on Plateau button in MainWindow
    - SqlConnection is connected to the window, which you can see while opening or saving \c Sqlite database
    - DbSave is connected to the window with progress bar, which appeares while saving the database entry
    - MyThread is a class withou the .ui file which provide separate thread for saving the database entry

    In order to develope program you have to develope an user interface as well. For that purpose you can use <tt>QtDesigner</tt>  or the developement tool
     <a href="http://qt.digia.com/Product/Developer-Tools/">QtCreator</a>.
    
    The code itself has not been documented until now. Everybody is welcome to help with the code developement.
    The developement of the code and documentation can and be managed using <a href="http://git-scm.com/">git</a> version system and all the code with possible developement branches stored in https://github.com/bulanek/alive. In order to commit your code and documentation changes to the web page which will be the storage of the newest code with all the history changes and possible branches you have to create (free) <a href="https://github.com/">GitHub</a> account and send me an email with your user name. I will then put yourself to the list of contributors. 

    If you have any question, please send me a mail to: boris.bulanek@suro.cz

    */



    /*-----------------------------------------------------------------------------
     *  
     *-----------------------------------------------------------------------------*/
const string confDataName="alive.xml";
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    DataHandle* _dataHandle=DataHandle::getInstance();
    _dataHandle->getConfigurationData(confDataName);
    QMenu *helpMenu = w.menuBar()->addMenu(QObject::tr("&About"));
    helpMenu->addAction(QObject::tr("About program"), &w, SLOT(about()));
    helpMenu->addAction(QObject::tr("About Qt"), qApp, SLOT(aboutQt()));
    w.show();
    return a.exec();
}

