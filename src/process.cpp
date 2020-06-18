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

Process::Process(int id) : pid_(id) {}
//  Return this process's ID
int Process::Pid() const { return pid_; }

// Return this process's CPU utilization
//https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
float Process::CpuUtilization() const 
{
    long total_time = LinuxParser::ActiveJiffies(Pid());
    float ActiveSeconds = LinuxParser::UpTime() - (LinuxParser::UpTime(Process::pid_) / sysconf(_SC_CLK_TCK));
    float usage = (total_time / sysconf(_SC_CLK_TCK)) / ActiveSeconds;
    return usage;
}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Process::Pid()); }

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid()); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

//  Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const &a) const
{
    return CpuUtilization() > a.CpuUtilization();
}