# liChessBot

Lichess.org Chess Bot

This program will prompt you for a match url and then use node JS webscraping to get pictures of the code, use image recognition to determine the best move and the positions of each piece, and use Pupeeteer for mouse movements. 

Data is transfered via sockets and TCP Ports to a C++ program where open CV and the Stockfish API is used to determine the current board state and the best move.

The best move is transfered back to the node JS program by socket and then we use Puppeteer to move the mouse  to  the location of the board relative to the X/Y coordinates in your monitor. 

You will need to run both .JS and .Cpp files asynchronously so I have made a batch file with the url and username(optional) of the match taken as command line arguments. 

Downloading the .cpp and .Js files and the .bat file will be enough to run the code via command prompt/terminal. Simply type 'start-server.bat URL USER' with a space in between each string. 





However, I have provided scripts to compile all the code into one .exe via Electron which you will have to install seperately onto your computer. I have provided a html/JS frontend to prompt you for the Username, URL, stop the program, and a .bat file which will compile the program for you.

