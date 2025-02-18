#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <sqlite3.h>


namespace Game {
    class Database {
        std::string path;
        sqlite3 *db{};

    public:
        explicit Database(std::string path);

        ~Database();
        void create_tables() const;
        void insert_user(const std::string &name) const;
        void insert_score(int user_id, int speed, int field_area, int score, int total_score) const;

        [[nodiscard]] int get_id_by_name(const std::string &name) const;
        [[nodiscard]] int get_best_score(const std::string &name) const;
        [[nodiscard]] std::string get_top(int n = 10) const;
    };
}


#endif //DATABASE_H
