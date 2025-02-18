#include <iostream>
#include "desk/desk.h"
#include "database/database.h"
#ifdef _WIN32
    #include <windows.h>
#else
#include <cstdlib>
#endif

int main() {
    system("clear");
    Game::Database db(".database.db");
    db.create_tables();
    unsigned short width, height, apples, move_delay;
    std::string name;
    std::cout << "Enter your name: ";
    std::cin >> name;
    std::cout << "Enter width and height of the desk: ";
    std::cin >> width >> height;
    std::cout << "Enter number of apples (start): ";
    std::cin >> apples;
    if (apples > width * height) {
        std::cerr << "Number of apples to spawn is greater than the number of cells in the desk" << std::endl;
        return 1;
    }
    std::cout << "Enter move delay: ";
    std::cin >> move_delay;

    system("clear");
#ifdef _WIN32
        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hStdout == INVALID_HANDLE_VALUE) {
            return 1;
        }

        COORD bufferSize = { static_cast<SHORT>(width * 2 + 2), static_cast<SHORT>(height + 5) };
        if (!SetConsoleScreenBufferSize(hStdout, bufferSize)) {
            return 1
        }

        SMALL_RECT windowSize = { 0, 0, static_cast<SHORT>(width * 2 + 2 - 1), static_cast<SHORT>(height + 5 - 1) };
        if (!SetConsoleWindowInfo(hStdout, TRUE, &windowSize)) {
            return 1
        }
#else
    std::cout << "\033[8;" << height + 5 << ";" << width * 2 + 2 << "t";
#endif

    Game::Desk desk(width, height, move_delay, name, db);
    desk.apples_to_spawn = apples;
    desk.spawn_player(width / 2, height / 2);
    desk.spawn_apples(apples);
    std::cout << desk;
    desk.run();
    return 0;
}
