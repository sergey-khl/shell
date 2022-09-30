#ifndef shell379_h
#define shell379_h
#include <map>
#include <sys/time.h>
#include <sys/resource.h>
using namespace std;
struct ProcessEntry {
    string command;
    pid_t pid;
};
class ProcessTable {
    public:
        ProcessTable(pid_t ppid);
        void print_num_processes();
        void print_running_processes();
        void print_completed_processes();
        void add_process_entry(string cmd, pid_t pid);
        void rem_latest();
    private:
        map<int, string> table;
        struct rusage usage;
        pid_t ppid;
};
int validate_args(vector<string> args);
bool exit();
void find_jobs(ProcessTable& processTable);
void execute(vector<string> args, string inp, string out, bool background_process, ProcessTable& processTable, string cmd);
#endif