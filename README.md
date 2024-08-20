# liChessBot 

https://www.youtube.com/watch?v=S0QR3cFFFlM

Chess Bot which takes an ongoing Lichess.org online match url as input upon startr up. Automously makes mouse movements to play the best move calculated @20 moves ahead (# is configurable) until the game is won/no longer ongoing.


This program will prompt you for a match url and then leverage JS webscraping which communicates with a C++ Open CV script via TCP ports to calculate the current board pos and the fen translation. The fen translation of the current chess board state enables us to quickly and efficiently communicate with the stockfish C++ api who sends back the best move by calculating up to 10 moves ahead. Calculation for moves further than 10 moves is configurable in the ChessMakeGUI.exe source directory. 

Data is transfered via sockets and TCP Ports to a C++ program where open CV and the Stockfish API is used to determine the current board state and the best move. The best move is transfered back to the node JS program via TCP socket and then Puppeteer is used to move the mouse to the location of the board relative to the X/Y coordinates in your monitor. It is important to note that I have hard coded the dimensions/position of the chess board relative to my 1920x1080 monitor with my web-browser hosting Lichess on fullscreen. If you have different monitor dimensions or wish to play on a halfscreen pupeteer does allow you to quickly capture ur mouse pos in x,y coordinates enabling you to set your own mouse movements relative to your screen. 


You will need to run both .JS and .Cpp files asynchronously so I have made a batch file with the url and username(optional) of the match taken as command line arguments. Downloading the .cpp and .Js files and the .bat file will be enough to run the code via command prompt/terminal. Simply type 'start-server.bat URL USER' with a space in between each string. (URL) must be a currently ongoing match on Lichess.com. User can be left blank aslong as the url is valid. 


However, I have provided scripts to compile all the files required to compile everything into one .exe via Electron which you will have to install seperately onto your computer. I have provided a html/JS frontend to prompt you for the Username, URL, stop the program, and renderer/pre_processing scripts. 

