
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

int main(int argc, char **argv) {
    int ans = 2;
    for (int i = 1; i < 100000;++i) {
        //cout << 2*i << endl;
    }
    cout << ans << endl;
    //fflush(stdout);
    return (0);
}