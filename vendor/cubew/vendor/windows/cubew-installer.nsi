;##*************************************************************************##
;##  CUBE        http://www.score-p.org/                                    ##
;##  SCALASCA    http://www.scalasca.org/                                   ##
;##*************************************************************************##
;##  Copyright (c) 1998-2015                                                ##
;##  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          ##
;##                                                                         ##
;##  Copyright (c) 2009-2015                                                ##
;##  German Research School for Simulation Sciences GmbH,                   ##
;##  Laboratory for Parallel Programming                                    ##
;##                                                                         ##
;##  This software may be modified and distributed under the terms of       ##
;##  a BSD-style license.  See the COPYING file in the package base         ##
;##  directory for details.                                                 ##
;##*************************************************************************##

; example2.nsi
;
; This script is based on example1.nsi, but it remember the directory, 
; has uninstall support and (optionally) installs start menu shortcuts.
;
; It will install example2.nsi into a directory that the user selects,
!include EnvVarUpdate.nsh
!include fileassoc.nsh

XPStyle on

Function .onInit

	# the plugins dir is automatically deleted when the installer exits
	InitPluginsDir
	File /oname=$PLUGINSDIR\splash.gif install\share\icons\cubelogo.gif
	#optional
	#File /oname=$PLUGINSDIR\splash.wav "C:\myprog\sound.wav"

	newadvsplash::show 2000 500 500 -2 $PLUGINSDIR\splash.gif

	Pop $0 ; $0 has '1' if the user closed the splash screen early,
			; '0' if everything closed normally, and '-1' if some error occurred.
FunctionEnd


;--------------------------------

; The name of the installer
Name "Cube C Writer (scalasca.org)"

; The file to write
OutFile "cubew-setup.exe"

; The default installation directory
InstallDir $PROGRAMFILES\CUBE


; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Scalasca\Cube\v4" "Install_Dir"

; Request application privileges for Windows Vista
;RequestExecutionLevel admin

;--------------------------------

LicenseText "Cube license agreement"
LicenseData "install\license.txt"


; Pages


Page license
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "Cube4 Installation"

  SectionIn RO


  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\bin
  ; Put file there
    File "install\bin\*.*"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\share\icons
  	File "install\share\icons\cubeicon.ico"
  ; Set output path to the installation directory.	
  SetOutPath $INSTDIR\lib
  	File "install\lib\*.*"
  SetOutPath $INSTDIR\include\cubew
  	File "install\include\cubew\*.*"


	

	
  ; Write the installation path into the registry
  WriteRegStr HKLM Software\Scalasca\Cube\v4 "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Scalasca\Cube\v4" "DisplayName" "Scalasca CUBE v4"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Scalasca\Cube\v4" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Scalasca\Cube\v4" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Scalasca\Cube\v4" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
  ${EnvVarUpdate} $0 "PATH" "P" "HKCU" "$INSTDIR\bin"
  
  
!insertmacro APP_ASSOCIATE "cubex" "Cube.Profile" "Performance profile" "$INSTDIR\share\icons\cubeicon.ico,0" \
     "Open with Cube(scalasca.org)" "$INSTDIR\bin\cube.exe $\"%1$\""
!insertmacro APP_ASSOCIATE "cube" "Cube.Profile" "Performance profile" "$INSTDIR\share\icons\cubeicon.ico,0" \
     "Open with Cube(scalasca.org)" "$INSTDIR\bin\cube.exe $\"%1$\""
!insertmacro APP_ASSOCIATE "cube.gz" "Cube.Profile" "Performance profile" "$INSTDIR\share\icons\cubeicon.ico,0" \
     "Open with Cube(scalasca.org)" "$INSTDIR\bin\cube.exe $\"%1$\""	 
  
  !insertmacro UPDATEFILEASSOC
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Scalasca\CubeW"
  CreateShortCut  "$SMPROGRAMS\Scalasca\Cube\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
	
  ; Remove files registration
   !insertmacro APP_UNASSOCIATE "cube" "Cube.Profile"
   !insertmacro APP_UNASSOCIATE "cube.gz" "Cube.Profile"
   !insertmacro APP_UNASSOCIATE "cubex" "Cube.Profile"   
   
  ; Adopt PATH variable
  ${un.EnvVarUpdate} $0 "PATH" "R" "HKCU" "$INSTDIR\bin"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Scalasca\Cube\v4"
  DeleteRegKey HKLM "Software\Scalasca\Cube\v4"

  ; Remove files and uninstaller
  
  Delete "$INSTDIR\cubeinstaller.nsi"
  Delete "$INSTDIR\uninstall.exe"
  Delete "$INSTDIR\bin\*.*"
  Delete "$INSTDIR\lib\*.*"
  Delete "$INSTDIR\include\cubew\*.*"
  Delete "$INSTDIR\include\*.*"
  Delete "$INSTDIR\share\doc\cubew\examples*.*"
  Delete "$INSTDIR\share\doc\cubew\*.*"
  Delete "$INSTDIR\share\icons\*.*"
  Delete "$INSTDIR\*.*"
  
  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Scalasca\Cube\*.*"
  Delete "$SMPROGRAMS\Scalasca\*.*"

  ; Remove directories used

  RMDir "$INSTDIR\bin"
  RMDir "$INSTDIR\lib"
  RMDir "$INSTDIR\include\cubew"
  RMDir "$INSTDIR\include"
  RMDir "$INSTDIR\share\doc\cubew\examples"
  RMDir "$INSTDIR\share\doc\cubew"
  RMDir "$INSTDIR\share\doc"
  RMDir "$INSTDIR\share\icons"
  RMDir "$INSTDIR\share"
  RMDir "$PROGRAMFILES\CUBE"
  RMDir "$INSTDIR"
  RMDir "$SMPROGRAMS\Scalasca\Cube"
  RMDir "$SMPROGRAMS\Scalasca"
SectionEnd


