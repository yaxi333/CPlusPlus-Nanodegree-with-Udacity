#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  std::string line, key, value;
  int memTotal, memFree, buffers;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), 'k', ' ');
      std::replace(line.begin(), line.end(), 'B', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal") {
        memTotal = stoi(value);
      } else if (key == "MemFree") {
        memFree = stoi(value);
      } else if (key == "Buffer") {
        buffers = stoi(value);
      }
    }
    return (memTotal - memFree - buffers) * 1.0 / memTotal;
  }
  return 0.0;
}

long LinuxParser::UpTime() {
  std::string line, uptime;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    double time = stod(uptime);
    return static_cast<long>(time);
  }
  return 0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  vector<std::string> util = CpuUtilization();
  long totalTime = stol(util[kNice_]) + stol(util[kUser_]) + stol(util[kSystem_]) + stol(util[kIRQ_]) + stol(util[kSoftIRQ_]) + stol(util[kSteal_]) + stol(util[kIdle_]) + stol(util[kIOwait_]);
  return  totalTime;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
    string line, value;
  vector<long long> values;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 17; i++) {
      linestream >> value;
      if (i >= 13) {
        long long time = stoll(value);
        values.emplace_back(time);
      }
    }
    long long uTime = values[kUTime_];
    long long sTime = values[kSTime_];
    long long cuTime = values[kCUTime_];
    long long csTime = values[kCSTime_];

    long totalTime = (uTime + sTime + csTime + cuTime);
    return totalTime;
  }
  return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return Jiffies() - IdleJiffies();
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<std::string> util = CpuUtilization();
  long m_idle = stol(util[kIdle_]);
  long m_iowait = stol(util[kIOwait_]);
  return m_idle + m_iowait;
}

vector<string> LinuxParser::CpuUtilization() {
  std::string line, key, value;
  vector<std::string> util;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key;
    while (linestream >> value) {
    	util.emplace_back(value);
    }
    return util;
  }
  return {};
}

int LinuxParser::TotalProcesses() { 
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        return stoi(value);
      }
    }
  }  
  return 0;
}

int LinuxParser::RunningProcesses() {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        return stoi(value);
      }
    }
  }  
  return 0;
}

string LinuxParser::Command(int pid) {
  string line, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> value;
    return value;
  }
  return string();
}

string LinuxParser::Ram(int pid) {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key != "VmSize:") {
        continue;
      } else {
        linestream >> value;
        int res = stod(value) * 0.001;
        return std::to_string(res);
      }
    }
  }
  return string();
}

string LinuxParser::Uid(int pid) {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while(getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key != "Uid:") {
        continue;
      } else {
        linestream >> value;
        return value;
      }      
    }
  }
  return string();
}

string LinuxParser::User(int pid) {
  string line, key, value, uid, x_value;
  uid = Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while(getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> value >> x_value >> key;
      if (key != uid) {
        continue;
      } else {
        return value;
      }      
    }
  }
  return string();
}

long LinuxParser::UpTime(int pid) {
  string line, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; i++) {
      linestream >> value;
    }
    long long uptime = stoll(value);
    long seconds = uptime / sysconf(_SC_CLK_TCK);
    return seconds;
  }
  return 0;
}


