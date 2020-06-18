#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;

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

//  An example of how to read data from the filesystem
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

//  Update this to use std::filesystem
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

//  Read and return the system memory utilization
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
  return -1;
}

//  Read and return the system uptime
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

//  Read and return the number of jiffies for the system
long LinuxParser::Jiffies()
{
  long jiffs = sysconf(_SC_CLK_TCK) * UpTime();

  return jiffs;
}

//  Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid)
{
  string utime, stime, cutime, cstime;
  string line, var;
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + kStatFilename);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream StringStream(line);
      for (int i = 1; i <= 13; i++)
      {
        StringStream >> var;
      }
      // get active jiffies: 14 15 16 17
      StringStream >> utime >> stime >> cutime >> cstime;
      long totaltime = stol(utime) + stol(stime) + stol(cutime) + stol(cstime);
      return totaltime;
    }
  }
  return -1;
}

//  Read and return the number of active jiffies for the system
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

//  Read and return the number of idle jiffies for the system
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
  return (stoi(idle) + stoi(iowait));
}

//  Read and return CPU utilization
// vector<string> LinuxParser::CpuUtilization() { return {}; }

//  Read and return the number of running processes
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

//  Read and return the total number of processes
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
          return std::stoi(pros);
        }
      }
    }
  }
  return -1 ;
}

//  Read and return the command associated with a process
string LinuxParser::Command(int pid)
{
  string line;
  std::ifstream stream(kProcDirectory + "/" + to_string(pid) + kCmdlineFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    return line;
  }
  return "Failed";
}

//  Read and return the memory used by a process
string LinuxParser::Ram(int pid)
{
  std::string id = to_string(pid);
  std::ifstream fileStream{kProcDirectory + "/" + id + kStatusFilename};
  string line, key, value;
  if (fileStream.is_open())
  {
    while (std::getline(fileStream, line))
    {
      std::istringstream stringStream(line);
      while (stringStream >> key >> value)
      {
        if ("VmSize:" == key)
          return to_string(stoi(value) / 1024);
      }
    }
  }
  return "failed";
}

//  Read and return the user ID associated with a process
string LinuxParser::Uid(int pid)
{

  std::string id = to_string(pid);
  std::ifstream fileStream{kProcDirectory + "/" + id + kStatusFilename};
  string line, key, value;
  if (fileStream.is_open())
  {
    while (std::getline(fileStream, line))
    {
      std::istringstream stringStream(line);
      while (stringStream >> key >> value)
      {
        if ("Uid:" == key)
          return value;
      }
    }
  }
  return "failed";
}

//  Read and return the user associated with a process
string LinuxParser::User(int pid)
{
  std::string id{LinuxParser::Uid(pid)};
  std::ifstream fileStream{kPasswordPath};
  string line, user, x, user_id;
  if (fileStream.is_open())
  {
    std::vector<string> lineVec;
    while (std::getline(fileStream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream stringStream(line);
      while (stringStream >> user >> x >> user_id)
      {
        if (id == user_id)
          return user;
      }
    }
  }
  return "failed";
}

//  Read and return the uptime of a process
long LinuxParser::UpTime(int pid)
{

  std::string id = to_string(pid);
  std::ifstream fileStream{kProcDirectory + id + kStatFilename};
  string line, value;
  if (fileStream.is_open())
  {
    while (std::getline(fileStream, line))
    {
      std::istringstream stringStream(line);
      for (int i = 1; i <= 22; i++)
      {
        stringStream >> value;
      }
    }
    return std::stol(value) / sysconf(_SC_CLK_TCK);
  }
  return -1;
}