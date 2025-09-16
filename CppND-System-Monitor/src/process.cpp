#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return m_pid; }

float Process::CpuUtilization() {
  long totalTime = LinuxParser::ActiveJiffies(m_pid) / sysconf(_SC_CLK_TCK);
  long elapsedTime = LinuxParser::UpTime() - LinuxParser::UpTime(m_pid);
  m_cpu_usage = totalTime * 1.0 / elapsedTime;
  return m_cpu_usage;
}

string Process::Command() {
  return LinuxParser::Command(m_pid);
}

string Process::Ram() {
  string res = LinuxParser::Ram(m_pid);
  m_ram = stoi(res);
  return res;
}

string Process::User() {
  return LinuxParser::User(m_pid);
}

long int Process::UpTime() {
  return LinuxParser::UpTime() - LinuxParser::UpTime(m_pid);
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator > (Process const& other) const {
  return m_cpu_usage > other.m_cpu_usage;
}
