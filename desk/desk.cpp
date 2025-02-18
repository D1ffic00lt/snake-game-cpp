#include "desk.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

Game::Desk::Desk(const unsigned short width, const unsigned short height, const unsigned int move_delay,
                 const std::string &name, Database &db): db(db) {
    this->width = width;
    this->height = height;
    this->move_delay = move_delay;
    this->name = name;
    db.insert_user(this->name);
    field.resize(height);
    for (Row &row: field) {
        row.resize(width);
    }
    last_move = &Desk::move_right;
    user_id = db.get_id_by_name(name);
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

void Game::Desk::move_generic(const int dx, const int dy, void (Desk::*move_func)()) {
    auto [x, y] = snake_position.back();
    if (snake_position.size() > 1) {
        if (auto [prev_x, prev_y] = snake_position[snake_position.size() - 2];
            dx == -(x - prev_x) && dy == -(y - prev_y)) {
            (this->*last_move)();
            return;
        }
    }

    int new_x = x + dx;
    int new_y = y + dy;
    if (!in_bounds({new_x, new_y}) || !check_move({new_x, new_y})) {
        active = false;
        return;
    }

    field[y][x].type = CellType::BODY;
    snake_position.emplace_back(new_x, new_y);
    if (field[new_y][new_x].type != CellType::APPLE) {
        auto [old_x, old_y] = snake_position.front();
        field[old_y][old_x].type = CellType::EMPTY;
        snake_position.pop_front();
    } else {
        --number_of_apples;
    }
    field[new_y][new_x].type = CellType::HEAD;
    last_move = move_func;
}

void Game::Desk::move_up() { move_generic(0, -1, &Desk::move_up); }
void Game::Desk::move_down() { move_generic(0, 1, &Desk::move_down); }
void Game::Desk::move_left() { move_generic(-1, 0, &Desk::move_left); }
void Game::Desk::move_right() { move_generic(1, 0, &Desk::move_right); }

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
    const int score = static_cast<int>(snake_position.size());
    const int speed = static_cast<int>(move_delay);
    db.insert_score(user_id, speed, width * height, score, score * (speed / 100));
    const std::string top = db.get_top();
    std::cout << "Top 10 players:" << std::endl;
    std::cout << top << std::endl;
}

bool Game::Desk::check_move(const Position &position) const {
    return field[position.second][position.first].type != CellType::BODY &&
           field[position.second][position.first].type != CellType::HEAD;
}

bool Game::Desk::in_bounds(const Position &position) const {
    return (position.first >= 0 && position.first < width &&
            position.second >= 0 && position.second < height);
}
