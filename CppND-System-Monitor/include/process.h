#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(const int pid): m_pid(pid) {}
  int Pid();
  std::string User();                      
  std::string Command();
  float CpuUtilization(); 
  std::string Ram();          
  long int UpTime();      
  bool operator>(Process const& a) const;

 private:
  int m_pid;
  float m_cpu_usage;
  int m_ram;
};

#endif