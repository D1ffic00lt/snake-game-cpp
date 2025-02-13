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
        friend std::ostream& operator<<(std::ostream& os, const Cell& cell) {
            switch (cell.type) {
                case CellType::EMPTY:
                    os << " ";
                    break;
                case CellType::BODY:
                    os << "o";
                    break;
                case CellType::APPLE:
                    os << "*";
                    break;
                case CellType::HEAD:
                    os << "O";
                    break;
            }
            return os;
        }

        CellType type = CellType::EMPTY;
    };
}


#endif //CELL_H
