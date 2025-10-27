#pragma once
#include <string>
#include <stdexcept>
#include <sqlite3.h>
#include <optional>
#include <vector>
using namespace std;

struct TaskRow {
    int64_t id;
    string command;
    int priority;
    string status;
};

class DB {
public:
    explicit DB(const string& path) {
        if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK) {
            throw runtime_error("Failed to open DB: " + string(sqlite3_errmsg(db_)));
        }
        exec(R"SQL(
        PRAGMA journal_mode=WAL;
        CREATE TABLE IF NOT EXISTS tasks(
          id INTEGER PRIMARY KEY AUTOINCREMENT,
          command TEXT NOT NULL,
          priority INTEGER NOT NULL DEFAULT 0,
          status TEXT NOT NULL DEFAULT 'pending',
          cancel_requested INTEGER NOT NULL DEFAULT 0,
          created_at TEXT DEFAULT (datetime('now')),
          started_at TEXT,
          finished_at TEXT,
          output TEXT,
          error TEXT
        );
        CREATE INDEX IF NOT EXISTS idx_tasks_status ON tasks(status);
        )SQL");
    }

    ~DB() { if (db_) sqlite3_close(db_); }

    void exec(const string& sql) {
        char* errmsg = nullptr;
        if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errmsg) != SQLITE_OK) {
            string e = errmsg ? errmsg : "unknown";
            sqlite3_free(errmsg);
            throw runtime_error("SQL error: " + e);
        }
    }

    vector<TaskRow> fetch_pending_batch(size_t limit=32) {
        vector<TaskRow> rows;
        const char* sql =
            "SELECT id, command, priority, status "
            "FROM tasks "
            "WHERE status='pending' AND cancel_requested=0 "
            "ORDER BY priority DESC, created_at ASC "
            "LIMIT ?;";
        sqlite3_stmt* stmt=nullptr;
        sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, (int)limit);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            TaskRow r;
            r.id = sqlite3_column_int64(stmt, 0);
            r.command = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            r.priority = sqlite3_column_int(stmt, 2);
            r.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            rows.push_back(std::move(r));
        }
        sqlite3_finalize(stmt);
        return rows;
    }

    bool try_mark_running(int64_t id) {
        const char* sql = "UPDATE tasks SET status='running', started_at=datetime('now') "
                          "WHERE id=? AND status='pending' AND cancel_requested=0;";
        sqlite3_stmt* stmt=nullptr;
        sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        sqlite3_bind_int64(stmt, 1, id);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return (rc == SQLITE_DONE && sqlite3_changes(db_) == 1);
    }

    void complete_ok(int64_t id, const string& output) {
        const char* sql = "UPDATE tasks SET status='completed', finished_at=datetime('now'), output=? WHERE id=?;";
        sqlite3_stmt* stmt=nullptr;
        sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, output.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt, 2, id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    void complete_err(int64_t id, const string& error, const string& output="") {
        const char* sql = "UPDATE tasks SET status='failed', finished_at=datetime('now'), error=?, output=? WHERE id=?;";
        sqlite3_stmt* stmt=nullptr;
        sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, error.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, output.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt, 3, id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    bool is_cancel_requested(int64_t id) {
        const char* sql = "SELECT cancel_requested FROM tasks WHERE id=?;";
        sqlite3_stmt* stmt=nullptr;
        sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        sqlite3_bind_int64(stmt, 1, id);
        int rc = sqlite3_step(stmt);
        bool requested = false;
        if (rc == SQLITE_ROW) {
            requested = sqlite3_column_int(stmt, 0) != 0;
        }
        sqlite3_finalize(stmt);
        return requested;
    }

private:
    sqlite3* db_ = nullptr;
};
