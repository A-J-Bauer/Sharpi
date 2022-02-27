#include <unistd.h>
#include <stdio.h>

#include <fcntl.h> // Contains file controls like O_RDWR
#include <sys/ioctl.h>
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

#include <string.h>
#include <cstdlib>
#include <string>
#include <chrono>

#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <errno.h>

#include "sharpi.h"

#include "SysPwm.h"


#include <gpiod.h>

using namespace std;
using namespace chrono;

int verbose = 0;


int main(int argc, char* argv[])
{
    std::cout << "waiting for debugger to attach - put a breakpoint on sleep(1)" << endl;
    std::chrono::steady_clock::time_point tp1, tp2;
    while (true)
    {
        tp1 = std::chrono::steady_clock::now();
        sleep(1);
        tp2 = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<milliseconds>(tp2 - tp1).count() > 1100)
        {
            break;
        }
    }

    // test here

    return 0;

}