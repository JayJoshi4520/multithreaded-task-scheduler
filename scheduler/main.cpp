#include "Scheduler.h"
#include <csignal>
#include <iostream>

using namespace std;

static SchedulerDaemon* g_sched = nullptr;

static void handle_sigint(int) {
    if (g_sched) {
        cerr << "\n[Scheduler] SIGINT received, shutting down...\n";
        g_sched->stop();
    }
}

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        cerr << "Usage: " << argv[0] << " <path/to/tasks.db> [num_workers]\n";
        return 1;
    }
    string db_path = argv[1];
    size_t workers = 4;
    if (argc == 3) workers = stoul(argv[2]);

    SchedulerDaemon sched(db_path, workers, 300);
    g_sched = &sched;
    signal(SIGINT, handle_sigint);

    cerr << "[Scheduler] Starting with " << workers << " workers, DB=" << db_path << "\n";
    sched.run();
    std::cerr << "[Scheduler] Stopped.\n";
    return 0;
}
