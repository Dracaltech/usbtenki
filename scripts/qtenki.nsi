; example2.nsi
;
; This script is based on example1.nsi, but it remember the directory, 
; has uninstall support and (optionally) installs start menu shortcuts.
;
; It will install example2.nsi into a directory that the user selects,

;--------------------------------

; The name of the installer
Name "QTenki"

; The file to write
OutFile "qtenki-install-2.0.3.exe"

; The default installation directory
InstallDir $PROGRAMFILES\QTenki

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\QTenki" "Install_Dir"

LicenseData license.txt

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages

Page license
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "QTenki (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "libgcc_s_dw2-1.dll"
  File "libstdc++-6.dll"
  File "libusb0.dll"
  File "license.txt"
  File "mingwm10.dll"
  File "QtCore4.dll"
  File "qtenki.exe"
  File "QtGui4.dll"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\QTenki "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QTenki" "DisplayName" "QTenki"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QTenki" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QTenki" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QTenki" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

Section "QTenki libusb based driver"
	SectionIn RO

	SetOutPath $INSTDIR\usb_driver

	File /r "usb_driver\*"

	ExecWait "cmd"
SectionEnd

Section "QTenki source code"
  CreateDirectory $INSTDIR\sources
  SetOutPath $INSTDIR\sources
  File "sources\usbtenki-2.0.3.tar.gz"
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\QTenki"
  CreateShortCut "$SMPROGRAMS\QTenki\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\QTenki\QTenki.lnk" "$INSTDIR\qtenki.exe" "" "$INSTDIR\qtenki.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QTenki"
  DeleteRegKey HKLM SOFTWARE\QTenki

  ; Remove files and uninstaller
  Delete $INSTDIR\qtenki.exe
  Delete $INSTDIR\uninstall.exe

  Delete $INSTDIR\libgcc_s_dw2-1.dll
  Delete $INSTDIR\libstdc++-6.dll
  Delete $INSTDIR\libusb0.dll
  Delete $INSTDIR\license.txt
  Delete $INSTDIR\mingwm10.dll
  Delete $INSTDIR\QtCore4.dll
  Delete $INSTDIR\qtenki.exe
  Delete $INSTDIR\QtGui4.dll

  ; Remove sources, if installed
  Delete "$INSTDIR\sources\*.*"
  RMDir "$INSTDIR\sources"
  RMDIR /r "$INSTDIR\usb_driver"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\QTenki\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\QTenki"
  RMDir "$INSTDIR"

SectionEnd
