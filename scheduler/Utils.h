#pragma once
#include <string>

int exec_command_capture(const std::string& cmd, std::string& out, std::string& err);
