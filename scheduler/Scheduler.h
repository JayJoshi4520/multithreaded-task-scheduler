#pragma once
#include "DB.hpp"
#include "ThreadPool.h"
#include <chrono>
#include <thread>
#include <atomic>
using namespace std;
class SchedulerDaemon {
public:
    SchedulerDaemon(const string& db_path, size_t workers, int poll_ms = 500)
        : db_(db_path), pool_(workers), poll_ms_(poll_ms), stop_(false) {}

    void run() {
        while (!stop_) {
            auto batch = db_.fetch_pending_batch();
            for (auto& row : batch) {
                // optimistic claim
                if (!db_.try_mark_running(row.id)) continue;

                pool_.enqueue([this, id=row.id, cmd=row.command]{
                    // Note: we honor cancel only before running (simple demo)
                    if (db_.is_cancel_requested(id)) {
                        db_.complete_err(id, "Canceled before start");
                        return;
                    }
                    string out, err;
                    int rc = exec_capture(cmd, out, err);
                    if (rc == 0) db_.complete_ok(id, out);
                    else db_.complete_err(id, "Exit code " + to_string(rc) + (err.empty() ? "" : (": " + err)), out);
                });
            }
            this_thread::sleep_for(chrono::milliseconds(poll_ms_));
        }
    }

    void stop() { stop_ = true; }

private:
    // Implemented in Utils.cpp
    int exec_capture(const string& cmd, string& out, string& err);

    DB db_;
    ThreadPool pool_;
    int poll_ms_;
    atomic<bool> stop_;
};
