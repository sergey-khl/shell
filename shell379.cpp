#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>// remove later
#include <sstream>
#include <vector>
#include <signal.h>

#include "shell379.h"

using namespace std;

ProcessTable::ProcessTable(pid_t ppid) {
    ppid = ppid;
}

void ProcessTable::print_num_processes() {
    cout << "Processes =\t" << table.size() << " active" << endl;
}

void ProcessTable::print_running_processes() {
    cout << "Running processes:" << endl;
    if (table.size() > 0) {
        cout << "#\tPID\tS\tSEC\tCOMMAND" << endl;
        for (auto const &entry: table) {
            cout << ppid << endl;
            char buf[1024];
            FILE *cmdpoint;
            char cmdbuf[1024];
            sprintf(cmdbuf, "ps --ppid %ld -o state,pid,ppid,cmd", (long)ppid);
            cmdpoint = popen(cmdbuf, "r");
            while (fgets(buf, 1024, cmdpoint) != NULL) {
                cout << buf;
            }
            fflush(stdout);
        }
    }
}

void ProcessTable::print_completed_processes() {
    getrusage(RUSAGE_SELF, &usage);
    cout << "Completed processes:" << endl;
    cout << "User time =\t" << usage.ru_utime.tv_sec << " seconds" << endl;
    cout << "Sys time =\t" << usage.ru_stime.tv_sec << " seconds" << endl;
}

void ProcessTable::add_process_entry(string cmd, pid_t pid) {
    table[table.size()] = cmd;
}

void ProcessTable::rem_latest() {
    table.erase(table.size());
}

int validate_args(vector<string> args) {
    int option = 0;
    bool isNum = false;
    
    if (args[0] == "exit" && args.size() == 1) {
        option = 1;
    } else if (args[0] == "jobs" && args.size() == 1) {
        option = 2;
    } else if (args[0] == "kill" && args.size() == 2) {
        option = 3;
    } else if (args[0] == "resume" && args.size() == 2) {
        option = 4;
    } else if (args[0] == "sleep" && args.size() == 2) {
        option = 5;
    } else if (args[0] == "suspend" && args.size() == 2) {
        option = 6;
    } else if (args[0] == "wait" && args.size() == 2) {
        option = 7;
    } else {
        option = 8;
    }

    return option;
}

bool exit() {
    while (wait(NULL) > 0);
    cout << "done execution... exiting" << endl;
    fflush(stdout);
    _exit(0);
}

void find_jobs(ProcessTable& processTable) {
    processTable.print_completed_processes();
    processTable.print_running_processes();
    processTable.print_num_processes();
}


void execute(vector<string> args, string inp, string out, bool background_process, ProcessTable& processTable, string cmd) {
    int success;

    processTable.add_process_entry(cmd, getpid());
    if ((success = fork()) < 0) {
        perror("could not run command as background process.");
    } else if (success == 0) {
        
        if (inp != "") {
            // todo input
            cout << inp << endl;
        }

        if (out != "") {
            // todo output
            cout << out << endl;
        }
        // convert my string arguments into usable char * so execvp works
        const char *command = args[0].c_str();
        const char *command_args[args.size() + 1];
        int j = 0;
        for (int i = 0; i < args.size(); ++i) {
            if (args[i][0] != '<' && args[i][0] != '>' && args[i][0] != '&') {
                command_args[j] = args[i].c_str();
                j++;
            }   
        }
        command_args[j] = NULL;
        if (execvp(command, const_cast<char * const *>(command_args))) {
            processTable.rem_latest();
            perror("coult not execute given command");
        }
    } else {
        // removes zombie processes
        signal(SIGCHLD, SIG_IGN);
        if (!background_process) { wait(0); }
    }
}