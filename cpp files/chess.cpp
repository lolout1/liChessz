#include "chess.h"
#include <cstdio>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <Windows.h>



void Chess::run(int argc, char **argv) {
   
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    StartStockfish();
    StartNewGame();
    SendStockfishCommand("d");
    ReadStockfishOutput(200);

    }/*
    PlayMove("g2g3");
    Sleep(100);
    SendStockfishCommand("d");
     Sleep(100);
     ReadStockfishOutput(10);
     Sleep(100);
    SendStockfishCommand(GetBestMove());
    Sleep(100);
    SendStockfishCommand("d");
    Sleep(100);
    
    */
    // Add any cleanup or further logic here


void Chess::StartStockfish() {
    // Initialize process startup information
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES saAttr;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create pipes for standard input, output, and error
    if (!CreatePipe(&hStdinRead, &hStdinWrite, &saAttr, 0)) {
        printf("CreatePipe failed (%d).\n", GetLastError());
        return;
    }
    if (!CreatePipe(&hStdoutRead, &hStdoutWrite, &saAttr, 0)) {
        printf("CreatePipe failed (%d).\n", GetLastError());
        return;
    }
    if (!CreatePipe(&hStderrRead, &hStderrWrite, &saAttr, 0)) {
        printf("CreatePipe failed (%d).\n", GetLastError());
        return;
    }

    // Setup the process to use the pipes for standard handles
    si.hStdError = hStderrWrite;
    si.hStdOutput = hStdoutWrite;
    si.hStdInput = hStdinRead;
    si.dwFlags |= STARTF_USESTDHANDLES;

    // Path to Stockfish executable
    const wchar_t* stockfishPath = L"C:\\Users\\14698\\source\\repos\\ChessCmakeGUI\\ChessCmakeGUI\\stockfish_x86-64-avx2.exe";
    wprintf(L"Attempting to start Stockfish from path: %s\n", stockfishPath);

    // Start the Stockfish process using CreateProcessW for wide-character support
    if (!CreateProcessW(
        stockfishPath,
        NULL,                  // Command line
        NULL,                  // Process handle not inheritable
        NULL,                  // Thread handle not inheritable
        TRUE,                  // Set handle inheritance to TRUE
        0,                     // No creation flags
        NULL,                  // Use parent's environment block
        NULL,                  // Use parent's starting directory
        &si,                   // Pointer to STARTUPINFOW structure
        &pi)                   // Pointer to PROCESS_INFORMATION structure
        ) {
        printf("CreateProcess failed (%d).\n", GetLastError());
       
    }
}

void Chess::CloseStockfish() {
    // Properly close handles and clean up
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hStdinRead);
    CloseHandle(hStdinWrite);
    CloseHandle(hStdoutRead);
    CloseHandle(hStdoutWrite);
    CloseHandle(hStderrRead);
    CloseHandle(hStderrWrite);
}

void Chess::SendStockfishCommand(std::string str) {
    
        DWORD bytesWritten = 0;
        str += '\n';
        if (WriteFile(hStdinWrite, str.c_str(), str.length(), &bytesWritten, NULL))
        {
            printf("Sent command:%s  . BytesWritten=%ld\n", str.c_str(), bytesWritten);
        }
     
      


}

std::vector<std::string> Chess::ReadStockfishOutput(DWORD delayMillis) {
    Sleep(delayMillis);
    printf("****Read called\n");
    std::vector<std::string>lines;

    DWORD bytesRead = 0;
    char buffer[1024 * 8];

   
    while (true) {


        DWORD bytesAvailable = 0;
        // Check if there is data available to read from the pipe
        if (!PeekNamedPipe(hStdoutRead, NULL, 0, NULL, &bytesAvailable, NULL)) {
            DWORD errorCode = GetLastError();
            printf("Error occurred while peeking the pipe: %d\n", errorCode);
            break;
        }


        // Read data from the pipe
        if (!ReadFile(hStdoutRead, buffer, sizeof(buffer), &bytesRead, NULL)) {
            printf("FAIL bytes READ = %ld\n", bytesRead);
            DWORD errorCode = GetLastError();
            if (errorCode == ERROR_BROKEN_PIPE) {
                printf("Pipe was closed\n");
                break;
            }
            else {
                printf("Error occurred while reading from the pipe: %d\n", errorCode);
            }
            break;
        }

        std::stringstream bufferStream(std::string(buffer, bytesRead));
        std::string line;
        while (std::getline(bufferStream, line)) {
            printf("%s\n", line.c_str());
            lines.push_back(line);
        }

       
        fflush(stdout);
        fflush(stderr);
      
        break;
    }

    
    return lines;
}

std::string Chess::GetBestMove()
{
    std::string best;

    SendStockfishCommand("go depth 10");
    std::vector<std::string> response = ReadStockfishOutput(1000);

    std::cout << "Stockfish response:" << std::endl;
   

    for (int i = 0; i < response.size(); i++)
    {
        printf("go-depth output: %d=%s\n", i, response[i].c_str());
        if (response[i].compare(0, 9, "bestmove ") == 0)
        {
            best = response[i].substr(9);
            std::size_t found = best.find(" ");
            best = best.substr(0, found);
        }
        if (response[i].find("mate 0") != std::string::npos)
        {
            isCheckmate = true;

            printf("\n\n\n\n\n");
            printf("           88                                88                                                         \n");
            printf("           88                                88                                        ,d               \n");
            printf("           88                                88                                        88               \n");
            printf(" ,adPPYba, 88,dPPYba,   ,adPPYba,  ,adPPYba, 88   ,d8  88,dPYba,,adPYba,  ,adPPYYba, MM88MMM ,adPPYba,  \n");
            printf("a8       * 88P      8a !8P     88 a8      ** 88 ,a8    88P     88      8a        `Y8   88   !8P     88  \n");
            printf("8b         88       88 8PP******* 8b         8888[     88      88      88 ,adPPPPP88   88   8PP*******  \n");
            printf("'8a,   ,.# 88       88 !8b,       *8a,   ,.# 88~*Yba,  88      88      88 88,    ,88   88,  !8b,        \n");
            printf("  *Ybbd8*' 88       88   ~Ybbd8*'  ~*Ybbd8*' 88   ~Y8a 88      88      88 ~*8bbdP*Y8   *Y888  ~Ybbd8*'  \n");
            printf("\n\nCHECKMATE\n\n\n");
        }

    }
    return best;
}


void Chess::PlayMove(std::string move)
{
    
    //std::vector<std::string> response1 = ReadStockfishOutput((DWORD)100);
  


    std::string cmd = "position fen ";
    cmd += currentFen;
    cmd += " moves ";
    cmd += move;
    std::cout << cmd << std::endl;
    SendStockfishCommand(cmd);

    Sleep(100);

    GetFenFromStockfish();

    
   // GeneratePiecesFromFenString(currentFen);
}

void Chess::GetFenFromStockfish()
{
    SendStockfishCommand("d");
    Sleep(100);
    std::vector<std::string> response = ReadStockfishOutput(100);
    for (int i = 0; i < response.size(); i++)
    {
        printf("response %d=%s\n", i, response[i].c_str());
        //cache fen string, for use in rendering pieces
        if (response[i].compare(0, 5, "Fen: ") == 0)
        {
            std::string fenStr = currentFen;
            auto spaceIndex = fenStr.find(" ");
            if (spaceIndex != std::string::npos)
            {
                fenStr = fenStr.substr(0, spaceIndex);
            }
            MoveLog.push_back(fenStr);
            currentFen = response[i].substr(5);

            std::string str = currentFen;

            auto last_space_pos = str.find_last_of(' ');
            auto second_last_space_pos = str.find_last_of(' ', last_space_pos - 1);
            std::string half_moves = str.substr(second_last_space_pos + 1, last_space_pos - second_last_space_pos - 1);
            int halfmoves = std::stoi(half_moves);


            printf("HALFMOVES=%d\n", halfmoves);
            if (halfmoves >= 100)
            {
                isDraw = true;
                printf("!!!!!!!!!!!!!!!!  DRAW BY 50 MOVES RULE  !!!!!!!!!!!!!!!!\n");
            }

        }
    }
     
    //CheckDrawConditions();
}


        std::vector<std::string> Chess::GetLegalMoves(std::string squareCode)
        {
            std::vector<std::string> legals;

            // Construct the command for Stockfish to find legal moves from the given square
            std::string cmd = "go searchmoves ";
            cmd += squareCode;

            // Iterate over all possible destination squares on the chessboard
            for (int file = 0; file < 8; file++)
            {
                for (int rank = 0; rank < 8; rank++)
                {
                    // Convert file and rank indices to algebraic notation (e.g., "a1", "b2")
                    std::string destinationStr = "";
                    destinationStr += (char)(file + 'a');             // Convert file index to letter ('a' to 'h')
                    destinationStr += (char)(rank + '0' + 1);         // Convert rank index to number ('1' to '8')

                    // Construct the complete command to send to Stockfish
                    std::string fullCmd = cmd + " " + destinationStr;

                    // Send the command to Stockfish and immediately stop it to get quick response
                    SendStockfishCommand(fullCmd);
                    SendStockfishCommand("stop");

                    // Read Stockfish's response
                    std::vector<std::string> response = ReadStockfishOutput(0);
                    for (int i = 0; i < response.size(); i++)
                    {
                        printf("legal_response %d=%s\n", i, response[i].c_str());

                        // Check if the response contains a legal move
                        if (response[i].find("bestmove") != std::string::npos && response[i].find("(n") == std::string::npos)
                        {
                            // Extract the move from the response (assuming the move format is "bestmove e2e4" for example)
                            std::string legalMove = response[i].substr(9, 4); // Adjust based on actual move format from Stockfish
                            legals.push_back(legalMove);
                            printf("LEGAL MOVE ADDED TO LIST: %s\n\n", legalMove.c_str());
                        }
                    }
                }
            }

            // Debug info
            printf("Number of legal moves found: %d\n", (int)legals.size());
            for (int i = 0; i < legals.size(); i++)
            {
                printf("Legal Move: %s\n", legals[i].c_str());
            }

            return legals;
        }
     
  
        void Chess::CheckDrawConditions()
        {
            // check repetition...
            for (int i = 0; i < MoveLog.size(); i++)  //TODO: come up with a better way.
            {
                for (int j = i + 1; j < MoveLog.size(); j++)
                {
                    for (int k = j + 1; k < MoveLog.size(); k++)
                    {
                        if (MoveLog[i] == MoveLog[j] && MoveLog[j] == MoveLog[k])
                        {
                            isDraw = true;
                            printf("!!!!!!!!!!!!!!!!  DRAW BY REPETITION  !!!!!!!!!!!!!!!!\n%d, %d, and %d\n", i, j, k);
                        }
                    }
                }
            }

            // check 50 rule... (50 rule is checked in the GetFenFromStockfish function

            // check stalemate...
              //check current player-to-act's king has a legal move? If it has no legal move and the game is not checkmate, then its stalemate.
                    // use player_to_act to find currentplayer. Then use FEN string to find location of king on board. 
                    // with that square, use the GetLegalMoves function to see if the returned vector is empty. If its empty, simply check isCheckmate


        }
        
        void Chess::StartNewGame()
        {
            // Send a command to Stockfish
            SendStockfishCommand("uci");
            std::vector<std::string> uci_response = ReadStockfishOutput(200);
            for (int i = 0; i < uci_response.size(); i++)
            {
                printf("uci_response %d=%s\n", i, uci_response[i].c_str());
            }

            //SetDifficultyLevel(10); // set diff 0-20

            SendStockfishCommand("isready");
            std::vector<std::string> isready_response = ReadStockfishOutput(200);
            for (int i = 0; i < isready_response.size(); i++)
            {
                printf("isready_response %d=%s\n", i, isready_response[i].c_str());
            }



            currentFen = START_FEN;
            //GeneratePiecesFromFenString(START_FEN); // TODO: Could turn this into a parameter of this function, so we can start game from any position.
            SendStockfishCommand("d");
            auto read = ReadStockfishOutput( 200);
            
        
}