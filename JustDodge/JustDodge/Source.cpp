#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <cassert>
#include <queue>
#include <atomic>
#include <stack>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <conio.h>
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

const int field_width = 19;
const int field_height = 3;

using namespace std;

bool Menu() {
    int choice;
    do {
        cout << "=== Menu ===" << endl;
        cout << "1. Start" << endl;
        cout << "2. Info" << endl;
        cout << "3. Leaderboard" << endl;
        cout << "4. Exit" << endl;
        cout << "Choose option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            this_thread::sleep_for(chrono::milliseconds(500));
            cout << "Press Enter to start...";
            cin.get();
            break;
        case 2:
            system("cls");
            cout << "Settings" << endl;
            cout << "Sut igru eto prosto dolshe vushuvat na igrovom pole poka na tebya letat vragy?. Upravlenye strelochkamy i lucshe ne zazhumat klavishi. Vot vse vragi: O - Tochki dvigayutsya prosto vpered ; B - boomerangi posle dostijeniya 2/3 kartu vozvrashayutsa ; L - molniya otprugivayet ot graniz." << endl;
            break;
        case 3:
            system("cls");
            cout << "Leaderboard" << endl;
            {
                ifstream file("leaderboard.txt");
                if (file.is_open()) {
                    string line;
                    while (getline(file, line)) {
                        cout << line << endl;
                    }
                    file.close();
                }
                else {
                    cout << ":/" << endl;
                }
            }
            break;
        case 4:
            system("cls");
            cout << "Exit" << endl;
            return false;
        default:
            cout << "Nepravylnui vvod" << endl;
            break;
        }
    } while (choice != 1);
    return true;
}

bool End_Menu(int elapsedSeconds) {
    std::cout << "Enter your name: ";
    std::string playerName;
    std::cin >> playerName;

    std::ofstream leaderboard("leaderboard.txt", std::ios::app);
    if (leaderboard.is_open()) {
        leaderboard << playerName << ": " << elapsedSeconds / 60 << "m " << elapsedSeconds % 60 << "s" << std::endl;
        leaderboard.close();
        std::cout << "Result saved!" << std::endl;
    }
    else {
        std::cout << "Error saving result :/" << std::endl;
    }

    int choice;
    std::cout << "1. Main Menu" << std::endl;
    std::cout << "2. Exit" << std::endl;
    std::cout << "Choose option: ";
    std::cin >> choice;

    return choice == 1;
}

//лютые звуки

void PlayShortSound(const char* soundFile) {
    wchar_t wideSoundFile[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, soundFile, -1, wideSoundFile, MAX_PATH);

    PlaySound(wideSoundFile, NULL, SND_ASYNC);
}

void PlayBackgroundMusic() {
    PlaySound(L"UndertaleOST_053_Stronger Monsters.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

//основная игра типа



struct Point {
    int position;
    int line;
    int direction;
};

struct Boomerang {
    int position;
    int line;
    int direction;
    int travelDistance;
};

struct Lightning {
    int position;
    int line;
    int verticalDirection;
    int horizontalDirection;
};

void drawScene(int playerPosition, int currentLine, const Point& point, const Point& point2, const Point& point3, const Boomerang& boomerang, const Boomerang& boomerang2, const Lightning& lightning, int elapsedSeconds) {
    system("cls");

    cout << " =================" << endl;
    for (int i = 0; i < field_height; ++i) {
        cout << " ";
        for (int j = 1; j < field_width - 1; ++j) {
            if (i == currentLine && j == playerPosition) {
                cout << "P";
            }
            else if (i == boomerang.line && j == boomerang.position) {
                cout << "B";
            }
            else if (i == boomerang2.line && j == boomerang2.position) {
                cout << "B";
            }
            else if (i == lightning.line && j == lightning.position) {
                cout << "L";
            }
            else if (i == point.line && j == point.position) {
                cout << "O";
            }
            else if (i == point2.line && j == point2.position) {
                cout << "O";
            }
            else if (i == point3.line && j == point3.position) {
                cout << "O";
            }
            else if (j == 1 || j == field_width - 2) {
                cout << "|";
            }
            else {
                cout << "-";
            }
        }
        cout << " " << endl;
    }

    cout << " =================" << endl;
    cout << "Time Lived: " << elapsedSeconds / 60 << "m " << elapsedSeconds % 60 << "s" << endl;
}

void dSdebounce(bool& flag) {
    flag = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(125));
    flag = true;
}

Point createPoint(int availibleID) {
    Point point;
    point.direction = -1;
    point.position = availibleID;
    point.line = -1;
    return point;
}

Boomerang createBoomerang(int availibleID, bool side) {
    Boomerang boomerang;
    boomerang.position = availibleID;
    boomerang.line = -1;
    boomerang.travelDistance = 0;
    return boomerang;
}

Lightning createLightning(int availibleID) {
    Lightning lightning;
    lightning.position = availibleID;
    lightning.line = -1;
    lightning.verticalDirection = -1;
    lightning.horizontalDirection = 1;
    return lightning;
}

bool isPointOnLine(int line, const Point& point) {
    return line == point.line && point.position != -1;
}

void movePoint(Point& point, const Point& point2, const Point& point3) {
    if (point.position != -1 && point.position >= -900) {
        point.position += point.direction;
        if (point.position <= 2 || isPointOnLine(point.line, point2) || isPointOnLine(point.line, point3)) {
            point.position = field_width - 3;
            point.line = rand() % 3;
            while (point.line == point2.line || point.line == point3.line) {
                point.line = rand() % 3;
            }
            point.direction = -1;
        }
    }
}

void moveBoomerang(Boomerang& boomerang) {
    if (boomerang.position != -1 && boomerang.position >= -900) {
        boomerang.position += boomerang.direction;
        boomerang.travelDistance++;
        if (boomerang.travelDistance == 9) {
            boomerang.direction *= -1;
            boomerang.travelDistance = 0;
        }
        if (boomerang.position >= field_width - 2 || boomerang.position <= 2) {
            boomerang.position = (boomerang.position >= field_width - 2) ? field_width - 2 : 2;
            boomerang.direction *= -1;
            boomerang.line = rand() % 3;
        }
    }
}

void moveBoomerang2(Boomerang& boomerang2) {
    if (boomerang2.position != -1 && boomerang2.position >= -900) {
        boomerang2.position += boomerang2.direction;
        boomerang2.travelDistance++;
        if (boomerang2.travelDistance == 9) {
            boomerang2.direction *= -1;
            boomerang2.travelDistance = 0;
        }
        if (boomerang2.position >= field_width - 2 || boomerang2.position <= 2) {
            boomerang2.position = (boomerang2.position >= field_width - 2) ? field_width - 2 : 2;
            boomerang2.direction *= -1;
            boomerang2.line = rand() % 3;
        }
    }
}

void moveLightning(Lightning& lightning) {
    if (lightning.position != -1 && lightning.position >= -900) {
        if (lightning.line + lightning.verticalDirection < 0 || lightning.line + lightning.verticalDirection >= field_height) {
            lightning.verticalDirection *= -1;
        }

        if (lightning.position + lightning.horizontalDirection < 2 || lightning.position + lightning.horizontalDirection >= field_width - 2) {
            lightning.horizontalDirection *= -1;
        }

        lightning.position += lightning.horizontalDirection;
        lightning.line += lightning.verticalDirection;
    }
}

int main() {
    srand(time(NULL));

    PlayBackgroundMusic();

    bool canDrawScene = true;
    int playerPosition = 10;
    int currentLine = 2;
    int propIDList = -999;
    int debounce = 500; //вот эта вот темка отвечает за скорость движения преград/врагов?, просто если надо можете поставить больше что бы замедлить
    Point point = createPoint(propIDList);
    propIDList++;

    Point point2 = createPoint(propIDList);
    propIDList++;

    Point point3 = createPoint(propIDList);
    propIDList++;

    Boomerang boomerang = createBoomerang(propIDList, false);
    propIDList++;

    Boomerang boomerang2 = createBoomerang(propIDList, true);
    propIDList++;

    Lightning lightning = createLightning(propIDList);

    atomic<bool> gameRunning(true);

    gameRunning = Menu();

    auto startTime = std::chrono::steady_clock::now();

    int elapsedSeconds = 0;

    thread pointThread([&]() {
        while (gameRunning) {
            this_thread::sleep_for(chrono::milliseconds(debounce));

            movePoint(point, point2, point3);
            movePoint(point2, point, point3);
            movePoint(point3, point, point2);

            moveBoomerang(boomerang);
            moveBoomerang2(boomerang2);

            moveLightning(lightning);

            auto currentTime = std::chrono::steady_clock::now();
            elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();

            if (elapsedSeconds >= 5 && point.position <= -900) {
                point.position = field_width - 3;
                point.line = rand() % 3;
            }

            if (elapsedSeconds >= 10 && point2.position <= -900 && point3.position <= -900) {
                point2.position = field_width - 3;

                point2.line = rand() % 3;
                while (point2.line == point.line || point2.line == point3.line) {
                    point2.line = rand() % 3;
                }
                point3.position = field_width - 3;

                point3.line = rand() % 3;
                while (point3.line == point.line || point3.line == point2.line) {
                    point3.line = rand() % 3;
                }
            }

            if (elapsedSeconds == 25) {
                boomerang.position = field_width - 3;
                boomerang.line = rand() % 3;
                boomerang.direction = -1;
                boomerang.travelDistance = 0;
            }

            if (elapsedSeconds == 45) {
                boomerang2.position = 2;
                boomerang2.line = rand() % 3;
                boomerang2.direction = 1;
                boomerang2.travelDistance = 0;
            }

            if (elapsedSeconds == 59) {
                lightning.position = field_width - 3;
                lightning.line = rand() % 3;
            }

            if (elapsedSeconds == 120) {
                debounce = debounce * 0.65;
            }

            if (canDrawScene == true) {
                drawScene(playerPosition, currentLine, point, point2, point3, boomerang, boomerang2, lightning, elapsedSeconds);
            }

            if ((currentLine == point.line && playerPosition == point.position) ||
                (currentLine == point2.line && playerPosition == point2.position) ||
                (currentLine == point3.line && playerPosition == point3.position)) {
                gameRunning = false;
            }

            if ((currentLine == boomerang.line && playerPosition == boomerang.position) ||
                (currentLine == boomerang2.line && playerPosition == boomerang2.position)) {
                gameRunning = false;
            }

            if (currentLine == lightning.line && playerPosition == lightning.position) {
                gameRunning = false;
            }
        }
        });

    while (gameRunning) {
        if (_kbhit()) {
            char input = _getch();
            switch (input) {
            case 72:
                currentLine = max(currentLine - 1, 0);
                dSdebounce(canDrawScene);
                drawScene(playerPosition, currentLine, point, point2, point3, boomerang, boomerang2, lightning, elapsedSeconds);
                PlayShortSound("Move.wav");
                break;
            case 80:
                currentLine = min(currentLine + 1, field_height - 1);
                dSdebounce(canDrawScene);
                drawScene(playerPosition, currentLine, point, point2, point3, boomerang, boomerang2, lightning, elapsedSeconds);
                PlayShortSound("Move.wav");
                break;
            case 75:
                playerPosition = max(playerPosition - 1, 2);
                dSdebounce(canDrawScene);
                drawScene(playerPosition, currentLine, point, point2, point3, boomerang, boomerang2, lightning, elapsedSeconds);
                PlayShortSound("Move.wav");
                break;
            case 77:
                playerPosition = min(playerPosition + 1, field_width - 3);
                dSdebounce(canDrawScene);
                drawScene(playerPosition, currentLine, point, point2, point3, boomerang, boomerang2, lightning, elapsedSeconds);
                PlayShortSound("Move.wav");
                break;
            default:
                break;
            }
        }
    }

    PlayShortSound("Death.wav");
    gameRunning = false;
    pointThread.join();

    bool backToMenu = End_Menu(elapsedSeconds);

    if (backToMenu) {
        main();
    }

    return 0;
}