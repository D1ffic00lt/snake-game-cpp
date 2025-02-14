#include "desk.h"
#include <random>
#include <iostream>
#include <thread>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

Game::Desk::Desk(const unsigned int width, const unsigned int height) {
    this->width = width;
    this->height = height;
    field.resize(height);
    for (Row &row: field) {
        row.resize(width);
    }
    last_move = &Desk::move_right;
}

Game::Desk::Desk(const unsigned int width, const unsigned int height, const unsigned int move_delay) {
    this->width = width;
    this->height = height;
    this->move_delay = move_delay;
    field.resize(height);
    for (Row &row: field) {
        row.resize(width);
    }
    last_move = &Desk::move_right;
}

Game::Desk::~Desk() = default;

void Game::Desk::spawn_apples(const unsigned int count) {
    std::vector<Position> apples(count);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution x_distr(0, static_cast<int>(width) - 1);
    std::uniform_int_distribution y_distr(0, static_cast<int>(height) - 1);
    for (auto &[x, y]: apples) {
        y = x_distr(gen);
        x = y_distr(gen);
    }
    for (const auto &[x, y]: apples) {
        if (field[y][x].type != CellType::EMPTY) {
            continue;
        }
        field[y][x].type = CellType::APPLE;
        ++number_of_apples;
    }
}

void Game::Desk::spawn_player(const unsigned int x, const unsigned int y) {
    if (!snake_position.empty()) {
        throw std::runtime_error("Player already spawned");
    }
    field[y][x].type = CellType::HEAD;
    snake_position.emplace_back(x, y);
}

void Game::Desk::move_down() {
    auto [x, y] = snake_position.back();
    if (y + 1 >= height) {
        active = false;
        return;
    }
    if (!check_move(std::make_pair(x, y + 1))) {
        active = false;
        return;
    }
    field[y][x].type = CellType::BODY;
    snake_position.emplace_back(x, y + 1);
    if (field[y + 1][x].type != CellType::APPLE) {
        auto [old_x, old_y] = snake_position.front();
        field[old_y][old_x].type = CellType::EMPTY;
        snake_position.pop_front();
    } else {
        field[y][x].type = CellType::BODY;
        --number_of_apples;
    }
    field[y + 1][x].type = CellType::HEAD;
    last_move = &Desk::move_down;
}

void Game::Desk::move_up() {
    auto [x, y] = snake_position.back();
    if (y - 1 < 0) {
        active = false;
        return;
    }
    if (!check_move(std::make_pair(x, y - 1))) {
        active = false;
        return;
    }
    field[y][x].type = CellType::BODY;
    snake_position.emplace_back(x, y - 1);
    if (field[y - 1][x].type != CellType::APPLE) {
        auto [old_x, old_y] = snake_position.front();
        field[old_y][old_x].type = CellType::EMPTY;
        snake_position.pop_front();
    } else {
        field[y][x].type = CellType::BODY;
        --number_of_apples;
    }
    field[y - 1][x].type = CellType::HEAD;
    last_move = &Desk::move_up;
}

void Game::Desk::move_left() {
    auto [x, y] = snake_position.back();
    if (x - 1 < 0) {
        active = false;
        return;
    }
    if (!check_move(std::make_pair(x - 1, y))) {
        active = false;
        return;
    }
    field[y][x].type = CellType::BODY;
    snake_position.emplace_back(x - 1, y);
    if (field[y][x - 1].type != CellType::APPLE) {
        auto [old_x, old_y] = snake_position.front();
        field[old_y][old_x].type = CellType::EMPTY;
        snake_position.pop_front();
    }
    else {
        field[y][x].type = CellType::BODY;
        --number_of_apples;
    }
    field[y][x - 1].type = CellType::HEAD;
    last_move = &Desk::move_left;
}

void Game::Desk::move_right() {
    auto [x, y] = snake_position.back();
    if (x + 1 >= width) {
        active = false;
        return;
    }
    if (!check_move(std::make_pair(x + 1, y))) {
        active = false;
        return;
    }
    field[y][x].type = CellType::BODY;
    snake_position.emplace_back(x + 1, y);
    if (field[y][x + 1].type != CellType::APPLE) {
        auto [old_x, old_y] = snake_position.front();
        field[old_y][old_x].type = CellType::EMPTY;
        snake_position.pop_front();
    }
    else {
        field[y][x].type = CellType::BODY;
        --number_of_apples;
    }
    field[y][x + 1].type = CellType::HEAD;
    last_move = &Desk::move_right;
}
int Game::Desk::kbhit() {
    termios oldt{}, newt{};

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    const int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    const int ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

int Game::Desk::getch() {
    termios oldt{}, newt{};
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    const int ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

int Game::Desk::waitKey(const unsigned int delay = 0) {
    const auto start = std::chrono::steady_clock::now();
    int keyPressed = -1;

    while (true) {
        if (kbhit()) {
            keyPressed = getch();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if (std::chrono::steady_clock::now() - start > std::chrono::milliseconds(delay)) {
            break;
        }
    }
    return keyPressed;
}


void Game::Desk::run() {
    while (this->active) {
        system("clear");
        std::cout << *this;
        if (number_of_apples < apples_to_spawn) {
            spawn_apples(1);
        }
        switch (waitKey(move_delay)) {
            case 'w':
                move_up();
                break;
            case 's':
                move_down();
                break;
            case 'a':
                move_left();
                break;
            case 'd':
                move_right();
                break;
            case 'q':
                exit(0);
            default:
                if (last_move != nullptr) {
                    (this->*last_move)();
                }
        }
    }
    std::cout << "GAME OVER" << std::endl;
}

bool Game::Desk::check_move(const Position &position) const {
    return field[position.second][position.first].type != CellType::BODY &&
           field[position.second][position.first].type != CellType::HEAD;
}
