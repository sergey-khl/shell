#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <sstream>
#include <vector>
 
#include "shell379.h"

using namespace std;

ProcessTable processTable;

int main(void) {
    string cmd;

    // get user input
    while(getline(cin, cmd)) {
        string arg;
        vector<string> args;
        string inp = "", out = "";
        bool background_process = false;


        for (istringstream line(cmd); line >> arg;) {
            if (arg[0] == '<') { inp = arg.substr(1); }
            else if (arg[0] == '>') { out = arg.substr(1); }
            else if (arg == "&") { background_process = true; } 

            args.push_back(arg);
        }

        // test user input
        switch(validate_args(args)) {
            case 0: {
                cout << "invalid command" << endl;
                break;
            }
            case 1: {
                // exit
                exit(processTable, out, background_process, cmd);
                break;
            }
            case 2: {
                // job
                find_jobs(processTable, out, background_process, cmd);
                break;
            } 
            case 3: {
                // kill
                kill(stoi(args[1]), SIGKILL);
                processTable.rem(stoi(args[1]));
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