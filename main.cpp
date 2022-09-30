#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <sstream>
#include <vector>
 
#include "shell379.h"

using namespace std;

#define LINE_LENGTH 100 // Max # of characters in an input line
#define MAX_ARGS 7 // Max number of arguments to a command
#define MAX_LENGTH 20 // Max # of characters in an argument
#define MAX_PT_ENTRIES 32 // Max entries in the Process Table
#define BUFFER_SIZER 1024 // max size of buffer when piping


int main(void) {
    string cmd;
    ProcessTable processTable(getpid());

    while(getline(cin, cmd)) {
        string arg;
        vector<string> args;
        // const char *args[MAX_ARGS + 1];
        // int i = 0;
        string inp = "", out = "";
        bool background_process = false;

        for (istringstream line(cmd); line >> arg;) {
            if (arg[0] == '<') { inp = arg.substr(1); }
            else if (arg[0] == '>') { out = arg.substr(1); }
            else if (arg == "&") { background_process = true; } 

            args.push_back(arg);
        }
        
        // for (string arg: args_to_verify) {
        //     args[i] = arg.c_str(); 
        //     ++i;
        // }
        // args[args_to_verify.size() + 1] = NULL;

        // cout << args[0] << args[1] << endl;
        switch(validate_args(args)) {
            case 0: {
                    cout << "invalid command" << endl;
                    break;
                }
            case 1: {
                    // exit
                    exit();
                    break;
                }
            case 2: {
                // jobs
                find_jobs(processTable);
                break;
            } 
            case 3: {
                // kill
                kill(stoi(args[1]), SIGKILL);
                break;
            }
            case 4: {
                // resume
                kill(stoi(args[1]), SIGCONT);
                break;
            }
            case 5: {
                // sleep
                sleep(stoi(args[1]));
                break;
            }
            case 6: {
                // suspend
                kill(stoi(args[1]), SIGSTOP);
                break;
            }
            case 7: {
                // wait
                int status;
                if (waitpid(stoi(args[1]), &status, 0) > 0) {
                    cout << "success" << endl;
                } else {
                    cout << "fail" << endl;
                }
                break;
            }
            case 8: {
                // execute
                execute(args, inp, out, background_process, processTable, cmd);
                break;
            }     
        }
    }

    return(0);
}