#ifndef CELL_H
#define CELL_H
#include <ostream>

namespace Game {
    enum class CellType {
        EMPTY,
        BODY,
        APPLE,
        HEAD
    };


    class Cell {
    public:
        Cell() = default;

        ~Cell() = default;

        explicit Cell(CellType type);

        friend std::ostream &operator<<(std::ostream &os, const Cell &cell) {
            switch (cell.type) {
                case CellType::EMPTY:
                    os << "\033[40m  \033[0m";
                    break;
                case CellType::BODY:
                    os << "\033[42m  \033[0m";
                    break;
                case CellType::APPLE:
                    os << "\033[41m  \033[0m";
                    break;
                case CellType::HEAD:
                    os << "\033[102m  \033[0m";
                    break;
            }
            return os;
        }

        CellType type = CellType::EMPTY;
    };
}


#endif //CELL_H
