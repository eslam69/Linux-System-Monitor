#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::stoi ;
// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem()
{
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "PRETTY_NAME")
        {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel()
{
  string os, kernel, no;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel >> no;
  }
  return no;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids()
{
  vector<int> pids;
  DIR *directory = opendir(kProcDirectory.c_str());
  struct dirent *file;
  while ((file = readdir(directory)) != nullptr)
  {
    // Is this a directory?
    if (file->d_type == DT_DIR)
    {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit))
      {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Done: Read and return the system memory utilization
float LinuxParser::MemoryUtilization()
{
  float total{0.00f}, free{0.00f};
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open())
  {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> std::skipws >> key >> value)
      {
        if ("MemTotal:" == key)
          total = std::stof(value);
        if ("MemFree:" == key)
        {
          free = std::stof(value);
        }
        break;
      }
    }
    if (total != 0.00f && free != 0.00f)
    {
      return ((total - free)) / total;
    }
  }
  return -1 ;
}

// Done: Read and return the system uptime
long LinuxParser::UpTime()
{
  string time1, time2, line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> time1 >> time2;
  }

  return std::stol(time1);
}

// Done: Read and return the number of jiffies for the system
long LinuxParser::Jiffies()
{
  long jiffs = sysconf(_SC_CLK_TCK) * UpTime();

  return jiffs;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

// Done: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies()
{
  std::ifstream filestream{kProcDirectory + kStatFilename};
  string user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nic;
  string key, line;
  if (filestream.is_open())
  {
    getline(filestream, line);
    std::istringstream stringStream(line);
    stringStream >> key >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nic;
  }
  return (stoi(user) + stoi(nice) + stoi(system) + stoi(idle) + stoi(iowait) + stoi(irq) + stoi(softirq) + stoi(steal) + stoi(guest) + stoi(guest_nic));
}

// Done: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies()
{
  std::ifstream filestream{kProcDirectory + kStatFilename};
  string user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nic;
  string key, line;
  if (filestream.is_open())
  {
    getline(filestream, line);
    std::istringstream stringStream(line);
    stringStream >> key >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nic;
  }
  return (stoi(idle) + stoi(iowait) );
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

// Done: Read and return the number of running processes
int LinuxParser::RunningProcesses()
{
  string key, value, running_procs{"failed"}, line;
  std::ifstream filestram(kProcDirectory + kStatFilename);
  if (filestram.is_open())
  {
    while (std::getline(filestram, line))
    {
      std::istringstream stringstream(line);
      while (stringstream >> key >> value)
      {
        if ("procs_running" == key)
        {
          running_procs = value;
          break;
        }
      }
    }
  }

  return std::stoi(running_procs);
}

// Done: Read and return the total number of processes
int LinuxParser::TotalProcesses()
{
  string key, value, pros{"failed"}, line;
  std::ifstream filestram(kProcDirectory + kStatFilename);
  if (filestram.is_open())
  {
    while (std::getline(filestram, line))
    {
      std::istringstream stringstream(line);
      while (stringstream >> key >> value)
      {
        if ("processes" == key)
        {
          pros = value;
          break;
        }
      }
    }
  }

  return std::stoi(pros);
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid [[maybe_unused]]) { return 0; }