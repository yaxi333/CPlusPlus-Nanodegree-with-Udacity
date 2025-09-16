#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  Processor& Cpu();
  std::vector<Process>& Processes();  
  float MemoryUtilization();      
  long UpTime();                
  int TotalProcesses();            
  int RunningProcesses();           
  std::string Kernel();
  std::string OperatingSystem();      

 private:
  Processor m_cpu = {};
  std::vector<Process> m_processes = {};
  float m_memory_util;
  long m_uptime;
  int m_total_proc;
  int m_running_proc;
  std::string m_kernal;
  std::string m_os;
};

#endif