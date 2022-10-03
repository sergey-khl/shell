#ifndef shell379_h
#define shell379_h
#include <map>
#include <sys/time.h>
#include <sys/resource.h>
using namespace std;
// A representation of the process control block which will keep track of all pid's
// of my processes as well as their original commands. Everything else can be piped.
class ProcessTable {
    public:
        // The next 3 print funcitions are helper functions for the jobs command
        void print_num_processes();
        void print_running_processes();
        void print_completed_processes();
        // adds a process to the table
        void add_process_entry(string cmd, pid_t pid);
        // removes a process from the table
        void rem(pid_t pid);
    private:
        // the table itself
        map<pid_t, string> table;
        // used to get sys time info
        struct rusage usage;
};
// made global
extern ProcessTable processTable;
// validates user input
int validate_args(vector<string> args);
// exits program
void exit(ProcessTable& processTable, string out, bool background_process, string cmd);
// jobs command
void find_jobs(ProcessTable& processTable, string out, bool background_process, string cmd);
// execute your own commands.
void execute(vector<string> args, string inp, string out, bool background_process, ProcessTable& processTable, string cmd);
#endif