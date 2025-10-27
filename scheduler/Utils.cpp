#include "Utils.h"
#include "Scheduler.h"
#include <array>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <sys/wait.h>

using namespace std;

int exec_command_capture(const string& cmd, string& out, string& err) {
    // Redirect stderr to a pipe by wrapping the shell command
    string wrapped = "/bin/sh -c '" + cmd + " 2>&1'";
    FILE* pipe = popen(wrapped.c_str(), "r");
    if (!pipe) {
        err = "Failed to popen";
        return -1;
    }

    array<char, 4096> buffer{};
    out.clear();
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        out += buffer.data();
    }

    int rc = pclose(pipe);
    if (WIFEXITED(rc)) return WEXITSTATUS(rc);
    return -1;
}

int SchedulerDaemon::exec_capture(const string& cmd, string& out, string& err) {
    // We can reuse the same implementation
    return exec_command_capture(cmd, out, err);
}
