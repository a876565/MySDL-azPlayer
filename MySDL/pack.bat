@echo off
setlocal enableextensions enabledelayedexpansion
set zpath=C:\Program Files\7-Zip\7z
set files=  
for %%i in (*.h,*.cpp) do set files=!files! %%i
echo %files%
"%zpath% a MySDL.zip %files%"
copy MySDL.zip J:\MySDL.zip
pause
