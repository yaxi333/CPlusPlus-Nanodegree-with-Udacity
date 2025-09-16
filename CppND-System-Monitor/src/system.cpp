#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
/*You need to complete the mentioned TODOs in order to satisfy the rubric criteria "The student will be able to extract and display basic data about the system."

You need to properly format the uptime. Refer to the comments mentioned in format. cpp for formatting the uptime.*/

Processor& System::Cpu() { return m_cpu; }

vector<Process>& System::Processes() {
  vector<int> pids = LinuxParser::Pids();
  for (int pid : pids) {
    Process p(pid);
    p.Ram();
    m_processes.emplace_back(p);
  }
  std::sort(m_processes.begin(), m_processes.end(), [](Process a, Process b)
  {
    return a > b;
  });
  return m_processes;
}

std::string System::Kernel() {
  m_kernal = LinuxParser::Kernel();  
  return m_kernal;
}

float System::MemoryUtilization() {
  m_memory_util = LinuxParser::MemoryUtilization();
  return m_memory_util;
}

std::string System::OperatingSystem() { 
  m_os = LinuxParser::OperatingSystem();
  return m_os;
}

int System::RunningProcesses() {
  m_running_proc = LinuxParser::RunningProcesses();
  return m_running_proc;
}

int System::TotalProcesses() {
  m_total_proc = LinuxParser::TotalProcesses();
  return m_total_proc;
}

long int System::UpTime() {
  m_uptime = LinuxParser::UpTime();
  return m_uptime;
}
