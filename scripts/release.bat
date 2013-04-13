@echo off
setlocal

SET PATH=%path%;"C:\Program Files (x86)\NSIS"
SET AC="c:\certs\godaddy.crt"
SET TS_SERVER="http://tsa.starfieldtech.com"
SET CERTNAME="Dracal"

SET VERSION="2.0.3"

REM Sign the qtenki executable
SignTool sign /ac %AC% /n "%CERTNAME%" /t %TS_SERVER% qtenki.exe

REM Generate the installer qtenki-install.exe
makensis /DVERSION=%VERSION% qtenki.nsi

rename "qtenki-install.exe" "qtenki-install-%VERSION%.exe"

REM Sign the installer
SignTool sign /ac %AC% /n %CERTNAME% /t %TS_SERVER% "qtenki-install-%VERSION%.exe"

move "qtenki-install-%VERSION%.exe" releases


