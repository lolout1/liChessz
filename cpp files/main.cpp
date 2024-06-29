#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#endif

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include "Chess.h"
#include <cstdlib>
#include <vector>
#include <ostream>
#include <iostream>
#include <string>
#include <cstring>
#include "json.hpp"  // Include the downloaded json.hpp file


const int PORT = 1337;

using json = nlohmann::json;

using namespace cv;

#ifdef _WIN32
#define CLOSE_SOCKET closesocket
#else
#define CLOSE_SOCKET close
#endif

    char getTurnFromFEN(const std::string& fen) {
        // The second part of the FEN string indicates the active color
        // It starts right after the piece placement part, so we find the first space
        auto spaceIndex = fen.find(' ');

        if (spaceIndex != std::string::npos) {
            char activeColor = fen[spaceIndex + 1];
            if (activeColor == 'w') {
                return 'w';
            }
            else if (activeColor == 'b') {
                return 'b';
            }
            else {
                return '0';
            }
        }
        else {
            return '0';
        }
    }

int main(int argc, char** argv) {
    std::string game;
    Chess Chess;
    std::string fen = "";
    Chess.StartStockfish();
    Chess.StartNewGame();
    Chess.SendStockfishCommand("d");
    std::vector<std::string> uci_response = Chess.ReadStockfishOutput(250);
    for (int i = 0; i < uci_response.size(); i++) {
        printf("uci_response %d=%s\n", i, uci_response[i].c_str());
    }
    std::cout << std::endl;
    std::string k = "";
    int i = 0;
    /*
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);
    Mat ogimage;
    ogimage = imread("C:\\Users\\14698\\source\\repos\\ChessCmakeGUI\\ChessCmakeGUI\\chessBoard.png");

    if (!ogimage.data) {
        printf("No image data \n");
        return -1;
    }

    Mat newimage;
    newimage = imread("C:\\Users\\14698\\source\\repos\\ChessCmakeGUI\\ChessCmakeGUI\\chessboard1.png");

    if (!newimage.data) {
        printf("No image data \n");
        return -1;
    }

    int down_width = 684;
    int down_height = 684;
    Mat resize_down;

    // resize down
    resize(newimage, resize_down, Size(down_width, down_height), INTER_LINEAR);

    cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE | cv::WINDOW_GUI_EXPANDED);
    cv::imshow("Display Image", ogimage);

    cv::namedWindow("Display Image2", cv::WINDOW_AUTOSIZE | cv::WINDOW_GUI_EXPANDED);
    cv::imshow("Display Image2", newimage);

    waitKey(2);
    */
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }
#endif

    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    const int BUFFER_SIZE = 4096; // Example size, adjust as needed

    char* buffer = new char[BUFFER_SIZE];

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
#ifdef _WIN32
        WSACleanup();
#endif
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket created successfully" << std::endl;

    // Prepare server address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to address
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        CLOSE_SOCKET(server_fd);
#ifdef _WIN32
        WSACleanup();
#endif
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket bound to address" << std::endl;

    // Start listening
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        CLOSE_SOCKET(server_fd);
#ifdef _WIN32
        WSACleanup();
#endif
        exit(EXIT_FAILURE);
    }
    int new_socket;

    std::cout << "Server is listening on port " << PORT << std::endl;

        // Accept connection
    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            CLOSE_SOCKET(server_fd);
#ifdef _WIN32
            WSACleanup();
#endif
            delete[] buffer;
            exit(EXIT_FAILURE);
        }
        std::cout << "Connection accepted" << std::endl;
  

            int valread = recv(new_socket, buffer, BUFFER_SIZE - 1, 0);

            if (valread <= 0) {
                std::cerr << "recv failed or connection closed" << std::endl;
                CLOSE_SOCKET(new_socket);
                continue;
            }


            Chess.SendStockfishCommand("d");
            std::vector<std::string> uci_response = Chess.ReadStockfishOutput(100);
            for (auto i = 0; i < uci_response.size(); i++) {
                printf("uci_response %d=%s\n", i, uci_response[i].c_str());
            }
            bool now = false;
            char turn;
            std::string bestMove = "";
            
            bool first = true;
            
            char color = '0';
            std::vector<std::string> moves;
            auto count = 1;
            bool moved = false;
            std::string data(buffer, valread);
            json jsonData = json::parse(data);
            std::string m = "";

            if (jsonData.contains("error")) {
                std::cerr << "Error: " << jsonData["error"] << std::endl;
                continue;
            }


            if (jsonData.contains("pName") && jsonData.contains("moves")) {
                std::string color = jsonData["color"];
               
              
                if (color == "w") {
                    color = 'w';
                }
                if (color == "b") {
                    color = 'b';
                }
               
                std::cout << color << std::endl;

            
            }
            else {
                std::cerr << "JSON data does not contain required fields (color and/or moves)." << std::endl;
            }

            if (jsonData.contains("ready") ) {
                auto read = jsonData["ready"];
                std::cout << "ready:" << read << std::endl;
                if (read==true) {
                    if (jsonData.contains("fenPart"))
                        if (jsonData["fenPart"] != fen) {
                            try {

                                fen = jsonData["fenPart"];
                                turn = getTurnFromFEN(fen);
                                if (turn == color) {
                                    now = true;
                                    std::cout << "Move this turn\n";
                                }
                                if (turn != color) {
                                    now = false;
                                    std::cout << "Do not move this turn\n";
                                }
                                std::cout << "Sending fen.." << fen << std::flush;
                                Chess.SendStockfishCommand("position fen " + fen);
                                Sleep(100);
                                Chess.SendStockfishCommand("d");
                                first = false;
                                std::vector<std::string> uci_response = Chess.ReadStockfishOutput(200);
                                for (auto i = 0; i < uci_response.size(); i++) {
                                    printf("uci_response %d=%s\n", i, uci_response[i].c_str());
                                }

/*
                                if (jsonData["Stats"].contains("width") && jsonData["Stats"].contains("height")) {
                                    int widt = jsonData["Stats"]["width"];
                                    int heigh = jsonData["Stats"]["height"];
                                    std::cout << "Board Width: " << widt << ", Height: " << heigh << std::endl;

                                    // Chess.convertCoords(widt, heigh);
                                }

                                if (jsonData["moves"].is_array()) {
                                    auto movesArray = jsonData["moves"];
                                    for (const auto& move : movesArray) {
                                        if (move.is_string()) {
                                            std::string modifiedMove = move;

                                            std::cout << modifiedMove << " ";

                                            for (auto i = 0; i < modifiedMove.length() - 1; i++) {
                                                if (modifiedMove[i] == 'x') {
                                                    modifiedMove[i] = modifiedMove[i + 1];
                                                }
                                            }
                                            moves.push_back(modifiedMove);
                                        }
                                        else {
                                            std::cerr << "Invalid move data in JSON array." << std::endl;
                                        }
                                    }
                                }
                                else {
                                    std::cerr << "Moves data is not an array." << std::endl;
                                }

                                std::cout << std::endl;
                                std::cout << "Data: " << color << std::endl;
                                std::cout << "Moves: ";
                                for (const auto& move : moves) {
                                    std::cout << move << " " << std::endl;
                                    //k = k + move + " ";
                                    //std::cout << "k" << k << std::endl;

                                    //Chess.SendStockfishCommand(move);
                                }
                                */
                                std::cout << std::endl;
                                std::cout << "Color: " << color << std::endl;

                                if (read == true) {
                                    bestMove = Chess.GetBestMove();
                                  // Chess.SendStockfishCommand("position fen " + fen + " moves " + bestMove);
                                    Sleep(100);
                                    if (bestMove.length() > 1) {
                                       
                                        // Prepare response JSON
                                        json responseJson = {
                                            {"bestMove", bestMove}
                                        };
                                        // Convert JSON to string
                                        std::string response = responseJson.dump();
                                        // Send response back to client
                                        send(new_socket, response.c_str(), response.length(), 0);
                                        std::cout << "Sent response to client: " << response << std::endl;
                                     
                                        }
                                        
                                    }


                                    // std::cout << "Play move: " << k << std::endl << std::flush;
                                     //Chess.SendStockfishCommand("go movetime 10");
                                    Chess.SendStockfishCommand("d");
                                   
                                    uci_response = Chess.ReadStockfishOutput(300);
                                    for (auto i = 0; i < uci_response.size(); i++) {
                                        printf("uci_response %d=%s\n", i, uci_response[i].c_str());
                                    }
                                    Sleep(10);
                               
                               
                            }
                            catch (json::parse_error& e) {
                                std::cerr << "JSON parse error: " << e.what() << std::endl;
                                continue;
                            }

                        }
                }
            }

       
    }
    
    CLOSE_SOCKET(new_socket);
    CLOSE_SOCKET(server_fd);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;

}

