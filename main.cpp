#include <iostream>
#include "desk/desk.h"

int main() {
    system("clear");
    unsigned int width, height, apples, move_delay;
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

    Game::Desk desk(width, height, move_delay);
    desk.apples_to_spawn = apples;
    desk.spawn_player(width / 2, height / 2);
    desk.spawn_apples(apples);
    std::cout << desk;
    desk.run();
    return 0;
}
