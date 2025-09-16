#include "processor.h"
#include <string>
#include <vector>
#include <iostream>
#include "linux_parser.h"

float Processor::Utilization() {
  return LinuxParser::ActiveJiffies() * 1.0 / LinuxParser::Jiffies();
}