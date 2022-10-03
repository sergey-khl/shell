#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <signal.h>
#include <string.h>
#include <fcntl.h>

#include "shell379.h"

using namespace std;

#define LINE_LENGTH 100 // Max # of characters in an input line
#define MAX_ARGS 7 // Max number of arguments to a command
#define MAX_LENGTH 20 // Max # of characters in an argument
#define MAX_PT_ENTRIES 32 // Max entries in the Process Table
#define BUFFER_SIZER 1024 // max size of buffer when piping


void ProcessTable::print_num_processes() {
    cout << "Processes =\t" << table.size() << " active" << endl;
}

void ProcessTable::print_running_processes() {
    cout << "Running processes:" << endl;
    if (table.size() > 0) {
        cout << "#\tPID\tS\tSEC\tCOMMAND" << endl;
        int i = 0;
        string pid, status, time;
        for (auto &entry: table) {
            // pipe input from ps to get execution time
            // this does not work all the time because my cleanup of pid's is not great
            // just do a kill <pid> and everything should work again.
            char buf[LINE_LENGTH];
            FILE *cmdpoint;
            char cmdbuf[LINE_LENGTH];
            sprintf(cmdbuf, "ps --pid %ld -o state,pid,time,cmd", (long)entry.first);
            cmdpoint = popen(cmdbuf, "r");
            string line = "";
            int j = 0;
            while (fgets(buf, LINE_LENGTH, cmdpoint) != NULL) {
                char *words = strtok(buf, " ");
                while (words != NULL) {
                    if (j == 4) {
                        status = words;
                    } else if (j == 5) {
                        pid = words;
                    } else if (j == 6) {
                        time = words;
                    }
                    words = strtok (NULL, " ");
                    ++j;
                }
                
            }
            ++i;
            printf("%i\t%ld\t%s\t%s %s\n", i,
            (long)entry.first, status.c_str(), time.c_str(), entry.second.c_str());
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
    table.insert(map<pid_t, string>::value_type(pid, cmd));
}

void ProcessTable::rem(pid_t pid) {
    table.erase(pid);
}

int validate_args(vector<string> args) {
    int option = 0;
    bool isNum = false;
    
    if (args[0] == "exit") {
        option = 1;
    } else if (args[0] == "jobs") {
        option = 2;
    } else if (args[0] == "kill") {
        option = 3;
    } else if (args[0] == "resume") {
        option = 4;
    } else if (args[0] == "sleep") {
        option = 5;
    } else if (args[0] == "suspend") {
        option = 6;
    } else if (args[0] == "wait") {
        option = 7;
    } else {
        option = 8;
    }

    return option;
}

void signal_handler(int sig) {
    // once a child finishes executing, we go here to work with the Process Table
    pid_t pid;
    int status;
    while((pid = waitpid(-1, &status, WNOHANG)) > 0){
        processTable.rem(pid);
    }
}

void exit(ProcessTable& processTable, string out, bool background_process, string cmd) {
    int child;

    if ((child = fork()) < 0) {
        perror("could not run command as background process.");
    } else if (child == 0) {

        // only redirect output if needed
        int fdout;
        if (out != "") {
            fdout = open(out.c_str(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
            if (fdout < 0) {
                perror("could not open output");
            } else {
                dup2(fdout, STDOUT_FILENO);
                close(fdout);
            }
        }
        
        // wait for all children to finish up and exit
        while (wait(NULL) > 0);
        cout << "done execution... exiting" << endl;
        processTable.print_completed_processes();
        fflush(stdout);
        
    } else {
        // removes zombie processes
        signal(SIGCHLD, SIG_IGN);
        if (!background_process) { wait(0); }
    }
    exit(0);
}

void find_jobs(ProcessTable& processTable, string out, bool background_process, string cmd) {
    int child;

    if ((child = fork()) < 0) {
        perror("could not run command as background process.");
    } else if (child == 0) {

        // only redirct ouput if needed
        int fdout;
        if (out != "") {
            fdout = open(out.c_str(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
            if (fdout < 0) {
                processTable.rem(getpid());
                perror("could not open output");
            } else {
                dup2(fdout, STDOUT_FILENO);
                close(fdout);
            }
        }
        
        // print all job info
        if (!background_process) {
            processTable.print_running_processes();
            processTable.print_num_processes();
            processTable.print_completed_processes();
        }
        fflush(stdout);
        _exit(0);
    } else {
        // removes zombie processes
        signal(SIGCHLD, signal_handler);
    }
}


void execute(vector<string> args, string inp, string out, bool background_process, ProcessTable& processTable, string cmd) {
    int child;
    
    if ((child = fork()) < 0) {
        perror("could not run command as background process.");
    } else if (child == 0) {
       
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

        // redirect input if needed
        int fdinp;
        if (inp != "") {
            fdinp = open(inp.c_str(), O_RDONLY);
            if (fdinp < 0) {
                processTable.rem(getpid());
                perror("could not open input");
            } else {
                dup2(fdinp, STDIN_FILENO);
                close(fdinp);
            }
        }

        // redirect output if needed
        int fdout;
        if (out != "") {
            fdout = open(out.c_str(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
            if (fdout < 0) {
                processTable.rem(getpid());
                perror("could not open output");
            } else {
                dup2(fdout, STDOUT_FILENO);
                close(fdout);
            }
        }
        
        // execute out command
        if (execvp(command, const_cast<char * const *>(command_args))) {
            processTable.rem(getpid());
            perror("coult not execute given command");
        }
        _exit(0);
    } else {
        processTable.add_process_entry(cmd, child);
        // removes zombie processes
        signal(SIGCHLD, signal_handler);
        // wait if & not provided
        if (!background_process) { waitpid(child, NULL, 0); }
    }
}