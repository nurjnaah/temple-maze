#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <cstdlib>
#include <thread>
#include <chrono>

using namespace std;

const int width = 20;
const int height = 10;

int playerX, playerY;
int targetX, targetY;
bool gameOver;
bool win;

char maze[height][width];
const char initialMaze[height][width] = {
    "###################",
    "#S#       #       #",
    "# # ####### ##### #",
    "# #       #   #   #",
    "# # ##### # # # # #",
    "#   #   #   # # # #",
    "### # ### ##### # #",
    "#   #   #   #     #",
    "### ##### #########",
    "###################"
};

bool obstacles[6] = {false, false, false, false, false, false};
int obstacleX[6], obstacleY[6];

bool points[5] = {false, false, false, false, false};
int pointX[5], pointY[5];

int score = 0;

void placeObstacles() {
    for (int i = 0; i < 6; i++) {
        int x, y;
        do {
            x = 1 + rand() % (width - 2);
            y = 1 + rand() % (height - 2);
        } while (maze[y][x] != ' ' || (x == playerX && y == playerY) || (x == targetX && y == targetY));

        maze[y][x] = '+';
        obstacles[i] = true;
        obstacleX[i] = x;
        obstacleY[i] = y;
    }
}

void removeObstacles() {
    for (int i = 0; i < 6; i++) {
        if (obstacles[i]) {
            maze[obstacleY[i]][obstacleX[i]] = ' ';
            obstacles[i] = false;
        }
    }
}

void placePoints() {
    for (int i = 0; i < 5; i++) {
        int x, y;
        do {
            x = 1 + rand() % (width - 2);
            y = 1 + rand() % (height - 2);
        } while (maze[y][x] != ' ' || (x == playerX && y == playerY) || (x == targetX && y == targetY) || maze[y][x] == '.');
        maze[y][x] = '.';
        points[i] = true;
        pointX[i] = x;
        pointY[i] = y;
    }
}

void removePoints() {
    for (int i = 0; i < 5; i++) {
        if (points[i]) {
            maze[pointY[i]][pointX[i]] = ' ';
            points[i] = false;
        }
    }
}

void drawMaze() {
    COORD cursorPosition;
    cursorPosition.X = 0;
    cursorPosition.Y = 0;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            cout << maze[i][j];
        }
        cout << endl;
    }

    cout << "Score: " << score << endl;
}

void getInput() {
    if (_kbhit()) {
        char key = _getch();
        int newX = playerX;
        int newY = playerY;

        switch (key) {
            case 'a':
                newX = playerX - 1;
                break;
            case 'd':
                newX = playerX + 1;
                break;
            case 'w':
                newY = playerY - 1;
                break;
            case 's':
                newY = playerY + 1;
                break;
            case 'q':
                gameOver = true;
                break;
        }

        if (maze[newY][newX] != '#') {
            maze[playerY][playerX] = ' ';
            playerX = newX;
            playerY = newY;

            for (int i = 0; i < 5; i++) {
                if (points[i] && playerX == pointX[i] && playerY == pointY[i]) {
                    score += 10;
                    points[i] = false;
                }
            }

            maze[playerY][playerX] = 'S';

            if (playerX == targetX && playerY == targetY) {
                win = true;
                score += 100;
            }
        }
    }
}

void updateGame() {
    for (int i = 0; i < 6; i++) {
        if (obstacles[i] && playerX == obstacleX[i] && playerY == obstacleY[i]) {
            gameOver = true;
        }
    }
}

void displayTitleScreen() {
    system("cls");
    cout << "Welcome to Temple Maze" << endl;
    cout << "Press 'N' for a New Game" << endl;
}

void obstacleThread() {
    while (!gameOver) {
        removeObstacles();
        placeObstacles();
        this_thread::sleep_for(chrono::seconds(3)); // Waktu hingga rintangan muncul kembali
    }
}

int main() {
    playerX = 1;
    playerY = 1;
    targetX = width - 2;
    targetY = height - 2;
    gameOver = false;
    win = false;
    score = 0;
    bool startGame = false; // Tambahkan variabel startGame

    // Sembunyikan kursor
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.bVisible = false;
    cursorInfo.dwSize = 1;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

    // Atur warna latar belakang konsol
    system("color 0A");

    // Salin peta awal ke peta saat ini
    memcpy(maze, initialMaze, sizeof(maze));

    // Membuat thread untuk rintangan
    thread obstacle(obstacleThread);

    char key = ' '; // Inisialisasi key dengan karakter apa saja

    while (true) {
        displayTitleScreen();
        key = _getch();
        if (key == 'n' || key == 'N') {
            system("cls"); // Bersihkan layar sebelum memulai permainan
            playerX = 1;
            playerY = 1;
            targetX = width - 2;
            targetY = height - 2;
            gameOver = false;
            win = false;
            score = 0;

            // Salin peta awal ke peta saat ini
            memcpy(maze, initialMaze, sizeof(maze));

            maze[playerY][playerX] = 'S';
            maze[targetY][targetX] = '>';
            placeObstacles();
            placePoints();
            
            startGame = true; // Setel startGame ke true saat tombol 'N' ditekan
        }

       while (startGame && !gameOver && !win) {
            drawMaze();
            getInput();
            updateGame();
        }

        if (gameOver) {
            COORD gameOverPosition;
            gameOverPosition.X = 5;
            gameOverPosition.Y = 5;
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), gameOverPosition);
            cout << "Game Over! Press 'Q' to return to the Main Menu.";
            key = _getch();
            if (key == 'q' || key == 'Q') {
                system("cls");
                gameOver = false;
                win = false;
            }
        }

        if (win) {
            COORD winPosition;
            winPosition.X = 5;
            winPosition.Y = 5;
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), winPosition);
            cout << "You Win! Press 'Q' to return to the Main Menu.";
            key = _getch();
            if (key == 'q' || key == 'Q') {
                system("cls");
                gameOver = false;
                win = false;
            }
        }
    }

    // Bergabung dengan thread rintangan
    obstacle.join();

    return 0;
} 
