#ifndef DESK_H
#define DESK_H

#include <vector>
#include <deque>
#include <random>
#include <sstream>
#include "cell.h"
#include "../database/database.h"

namespace Game {
    using Row = std::vector<Cell>;
    using DeskField = std::vector<Row>;
    using Position = std::pair<int, int>;

    class Desk {
        bool active = true;

        void (Desk::*last_move)() = nullptr;

        unsigned int number_of_apples = 0;
        unsigned int move_delay = 500;
        int user_id;
        std::string name;
        Database &db;
        std::deque<Position> snake_position;

        [[nodiscard]] bool check_move(const Position &position) const;

        [[nodiscard]] bool in_bounds(const Position &position) const;

        static int kbhit();

        static int getch();

        static int waitKey(unsigned int delay);

        void move_generic(int dx, int dy, void (Desk::*move_func)());

    public:
        Desk(unsigned short width, unsigned short height, unsigned int move_delay, const std::string &name,
             Database &db);


        ~Desk();

        void spawn_apples(unsigned int count);

        void spawn_player(unsigned int x, unsigned int y);

        void move_right();

        void move_left();

        void move_up();

        void move_down();

        void run();

        friend std::ostream &operator<<(std::ostream &os, const Desk &desk) {
            std::ostringstream buffer;

            buffer << "╔";
            for (unsigned int i = 0; i < desk.width; i++) {
                buffer << "══";
            }
            buffer << "╗" << std::endl;

            for (const Row &row: desk.field) {
                buffer << "║";
                for (const Cell &cell: row) {
                    buffer << cell;
                }
                buffer << "║" << std::endl;
            }

            buffer << "╚";
            for (unsigned int i = 0; i < desk.width; i++) {
                buffer << "══";
            }
            buffer << "╝" << std::endl;

            buffer << "Score: " << desk.snake_position.size() << std::endl;

            os << buffer.str();
            return os;
        }

        unsigned short height = 1;
        unsigned short width = 1;
        unsigned int apples_to_spawn = width * height / 8 + 1;
        DeskField field;
    };
}


#endif //DESK_H
