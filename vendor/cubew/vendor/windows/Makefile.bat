:: 
:: This file is part of the CUBE software (http:\\www.scalasca.org\cube%
::
:: Copyright (c) 2009-2020, 
::    Forschungszentrum Juelich GmbH, Germany
::    German Research School for Simulation Sciences GmbH, Juelich\Aachen, Germany
::
:: Copyright (c) 2009-2014, 
::    RWTH Aachen University, Germany
::    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
::    Technische Universitaet Dresden, Germany
::    University of Oregon, Eugene, USA
::    Forschungszentrum Juelich GmbH, Germany
::    German Research School for Simulation Sciences GmbH, Juelich\Aachen, Germany
::    Technische Universitaet Muenchen, Germany
::
:: See the COPYING file in the package base directory for details.
::
:: maintainer Pavel Saviankou <p.saviankou@fz-juelich.de>
set MINGW=C:\QtSDK\5.13.1\mingw73_64
set NSIS="C:\Program Files (x86)\NSIS"
set srcdir=..\..\
set PWD=%~dp0
set PREFIX=%1
set TARGET=C:\Scalasca-Soft\Install\Cube2.0\%PREFIX%\install


mkdir  %TARGET%\bin
mkdir  %TARGET%\lib
mkdir  %TARGET%\include\cubew
mkdir  %TARGET%\share\icons
mkdir  %TARGET%\share\doc\cubew\examples

for %%f in ( 
cubew
) do (
cd  %%f
qmake 
mingw32-make clean
mingw32-make

if errorlevel 1 (
   echo Failure during make : %errorlevel%
   exit /b %errorlevel%
)
cd %PWD%
echo "Copy %%f..."
copy %%f\release\*.dll %TARGET%\bin
copy %%f\release\*.a   %TARGET%\lib
)


::GOTO:EOF
echo "Copy system libraries..."
copy %MINGW%\bin\libgcc_s_*.dll %TARGET%\bin
copy %MINGW%\bin\libstd*.dll %TARGET%\bin
echo "Copy c-writer header files..."
copy %srcdir%\src\cubew\*.h  %TARGET%\include\cubew\.
echo "Copy generated header files..."
copy %srcdir%\vpath\src\*.h %TARGET%\include\cubew\.
@echo "Copy Cube license..."
copy  %srcdir%\COPYING %TARGET%\license.txt
echo "NOW COMPILE NSIS script..." 
if errorlevel 1 (
   echo Failure during copying : %errorlevel%
   exit /b %errorlevel%
)
exit 0 
