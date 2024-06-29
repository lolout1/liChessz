@echo off



set URL=%1
set USER=%2

REM Start ChessCmakeGUI.exe
start "" "z\elec\ChessCmakeGUI.exe"

REM Start JSChess.js with Node.js, passing the URL as an argument
start "" node "z\elec\JSChess.js" "%URL%" "%USER%"

endlocal