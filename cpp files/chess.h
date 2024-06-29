#ifndef CHESS_H
#define CHESS_H

#include <iostream>
#include <string>

#include <Windows.h>
#include <vector>
#include <cstdint>

class Chess
{
public:
    const std::string START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string currentfen;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    HANDLE hStdinRead, hStdinWrite, hStdoutRead, hStdoutWrite, hStderrRead, hStderrWrite;
    SECURITY_ATTRIBUTES saAttr;

    void run(int argc, char** argv);

    void StartStockfish();
    void CloseStockfish();
    void SendStockfishCommand(std::string str);
    std::vector<std::string> ReadStockfishOutput(DWORD delayMillis);
    void GeneratePiecesFromFenString(std::string fenString);
    std::string currentFen = "";
    void GenerateLegalHighlights(std::vector<std::string> legal_moves);
    std::vector<std::string> GetLegalMoves(std::string squareCode);
    void StartNewGame();
    void SetDifficultyLevel(int diff);
    std::string GetBestMove();
    float cpuThinkTime = 1000.f;
    void PlayMove(std::string move);
    void GetFenFromStockfish();
    std::vector<std::string> MoveLog;
    float autoplayInterval = 1.f / 4;
    float autoplayTimer = 0.f;
    bool isCheckmate = false;
    bool isDraw = false;
    void StopStockfishAfterTime(DWORD millis);
    void CheckDrawConditions();
};

#endif // CHESS_H
