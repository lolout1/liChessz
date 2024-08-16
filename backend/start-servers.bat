@echo off



set URL=%1
set USER=%2

REM Start ChessCmakeGUI.exe
start "" "ChessCmakeGUI.exe"

REM Start JSChess.js with Node.js, passing the URL as an argument
start "" node "JSChess.js" "%URL%" "%USER%" //match-url and optional username if you do not want to build executable with https://github.com/lolout1/liChessz/tree/main/fullCompile

endlocal
