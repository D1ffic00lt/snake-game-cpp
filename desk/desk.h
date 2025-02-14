#ifndef DESK_H
#define DESK_H

#include <vector>
#include <deque>
#include <thread>
#include "cell.h"


namespace Game {
    using Row = std::vector<Cell>;
    using DeskField = std::vector<Row>;
    using Position = std::pair<int, int>;

    class Desk {
        bool active = true;

        void (Desk::*last_move)() = nullptr;

        unsigned int number_of_apples = 0;
        unsigned int move_delay = 500;
        std::deque<Position> snake_position;

        [[nodiscard]] bool check_move(const Position &position) const;

        static int kbhit();

        static int getch();

        static int waitKey(unsigned int delay);

    public:
        Desk(unsigned int width, unsigned int height);
        Desk(unsigned int width, unsigned int height, unsigned int move_delay);

        ~Desk();

        void spawn_apples(unsigned int count);

        void spawn_player(unsigned int x, unsigned int y);

        void move_right();

        void move_left();

        void move_up();

        void move_down();

        void run();

        friend std::ostream &operator<<(std::ostream &os, const Desk &desk) {
            for (const Row &row: desk.field) {
                for (const Cell &cell: row) {
                    os << cell;
                }
                os << std::endl;
            }
            os << "Score: " << desk.snake_position.size() << std::endl;
            return os;
        }


        unsigned int height = 1;
        unsigned int width = 1;
        unsigned int apples_to_spawn = width * height / 8 + 1;
        DeskField field;
    };
}


#endif //DESK_H
