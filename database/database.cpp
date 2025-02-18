#include "database.h"

Game::Database::Database(std::string path) {
    this->path = std::move(path);
    if (sqlite3_open(this->path.c_str(), &db) != SQLITE_OK) {
        throw std::runtime_error("Can't open database");
    }
}

Game::Database::~Database() {
    sqlite3_close(db);
}

void Game::Database::create_tables() const {
    char *err_msg = nullptr;
    const char *sql = "CREATE TABLE IF NOT EXISTS user ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL"
            ");";
    if (sqlite3_exec(db, sql, nullptr, nullptr, &err_msg) != SQLITE_OK) {
        throw std::runtime_error("Can't create table: " + std::string(err_msg));
    }
    sql = "CREATE TABLE IF NOT EXISTS score ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "user_id INTEGER NOT NULL,"
            "speed INTEGER NOT NULL,"
            "field_area INTEGER NOT NULL,"
            "score INTEGER NOT NULL,"
            "total_score INTEGER NOT NULL,"
            "FOREIGN KEY (user_id) REFERENCES user(id)"
            ");";
    if (sqlite3_exec(db, sql, nullptr, nullptr, &err_msg) != SQLITE_OK) {
        throw std::runtime_error("Can't create table: " + std::string(err_msg));
    }
}

void Game::Database::insert_user(const std::string &name) const {
    sqlite3_stmt *stmt = nullptr;
    if (const char *sql = "INSERT INTO `user` (name) SELECT ? WHERE NOT EXISTS (SELECT 1 FROM `user` WHERE name = ?);";
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Can't prepare statement: " + std::string(sqlite3_errmsg(db)));
    }
    if (sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        throw std::runtime_error("Can't bind text: " + std::string(sqlite3_errmsg(db)));
    }
    if (sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        throw std::runtime_error("Can't bind text: " + std::string(sqlite3_errmsg(db)));
    }
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        throw std::runtime_error("Can't insert user: " + std::string(sqlite3_errmsg(db)));
    }
    sqlite3_finalize(stmt);
}

void Game::Database::insert_score(const int user_id, const int speed, const int field_area, const int score,
                                  const int total_score) const {
    sqlite3_stmt *stmt = nullptr;
    if (const char *sql =
                "INSERT INTO `score` (`user_id`, `speed`, `field_area`, `score`, `total_score`) VALUES (?, ?, ?, ?, ?);"
        ;
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Can't prepare statement: " + std::string(sqlite3_errmsg(db)));
    }
    if (sqlite3_bind_int(stmt, 1, user_id) != SQLITE_OK) {
        throw std::runtime_error("Can't bind int: " + std::string(sqlite3_errmsg(db)));
    }
    if (sqlite3_bind_int(stmt, 2, speed) != SQLITE_OK) {
        throw std::runtime_error("Can't bind int: " + std::string(sqlite3_errmsg(db)));
    }
    if (sqlite3_bind_int(stmt, 3, field_area) != SQLITE_OK) {
        throw std::runtime_error("Can't bind int: " + std::string(sqlite3_errmsg(db)));
    }
    if (sqlite3_bind_int(stmt, 4, score) != SQLITE_OK) {
        throw std::runtime_error("Can't bind int: " + std::string(sqlite3_errmsg(db)));
    }
    if (sqlite3_bind_int(stmt, 5, total_score) != SQLITE_OK) {
        throw std::runtime_error("Can't bind int: " + std::string(sqlite3_errmsg(db)));
    }
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        throw std::runtime_error("Can't insert score: " + std::string(sqlite3_errmsg(db)));
    }
    sqlite3_finalize(stmt);
}

int Game::Database::get_id_by_name(const std::string &name) const {
    sqlite3_stmt *stmt = nullptr;
    if (const char *sql = "SELECT id FROM `user` WHERE name = ?;";
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Can't prepare statement: " + std::string(sqlite3_errmsg(db)));
    }
    if (sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        throw std::runtime_error("Can't bind text: " + std::string(sqlite3_errmsg(db)));
    }
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        throw std::runtime_error("Can't get id: " + std::string(sqlite3_errmsg(db)));
    }
    const int id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return id;

}

int Game::Database::get_best_score(const std::string &name) const {
    sqlite3_stmt *stmt = nullptr;
    if (const char *sql = "SELECT MAX(score) FROM `score` WHERE user_id = (SELECT id FROM `user` WHERE name = ?);";
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Can't prepare statement: " + std::string(sqlite3_errmsg(db)));
    }
    if (sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        throw std::runtime_error("Can't bind text: " + std::string(sqlite3_errmsg(db)));
    }
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        throw std::runtime_error("Can't get best score: " + std::string(sqlite3_errmsg(db)));
    }
    const int best_score = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return best_score;
}

std::string Game::Database::get_top(const int n) const {
    sqlite3_stmt *stmt = nullptr;
    if (const char *sql = "SELECT name, MAX(score) FROM `user` JOIN `score` ON user.id = score.user_id GROUP BY name ORDER BY MAX(score) DESC LIMIT ?;";
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Can't prepare statement: " + std::string(sqlite3_errmsg(db)));
    }
    if (sqlite3_bind_int(stmt, 1, n) != SQLITE_OK) {
        throw std::runtime_error("Can't bind int: " + std::string(sqlite3_errmsg(db)));
    }
    std::string top;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        top += std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0))) + " " +
               std::to_string(sqlite3_column_int(stmt, 1)) + "\n";
    }
    sqlite3_finalize(stmt);
    return top;
}
