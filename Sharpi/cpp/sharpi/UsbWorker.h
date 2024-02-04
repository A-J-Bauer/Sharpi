#pragma once

#include <string>
#include <atomic>
#include <thread>
#include <mutex>
#include <cstddef>
#include <cstdlib>
#include <chrono>
#include <functional>
#include <algorithm>
#include <map>

#include <fcntl.h> // file controls like O_RDWR
#include <sys/ioctl.h>
#include <termios.h> //POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <errno.h>
//#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

using namespace std;

class UsbWorker
{
private:
    enum TtyDevState
    {
        Unused = 0,
        BeingTried = 1,
        Used
    };

    struct TtyDevs
    {
        const char file[20];
        atomic<TtyDevState> state;
    };

    static int ttyDevsSize;
    static TtyDevs ttyDevs[];
    
    /*atomic<bool> bdevtrying = false;*/

    static const int RECV_BUFFER_SIZE = 64;
    static const int SEND_BUFFER_SIZE = 256;
    static const char DEVICEID_S = '\xF';
    static const char DEVICEID_E = '\xE';

    function<void(int, char *)> _callback_data = NULL;
    function<void(int, int)> _callback_state = NULL;

    const int DEAD = 0;
    const int ALIVE = 1;
    
    char _recvBuffer[RECV_BUFFER_SIZE];
    char _sendBuffer[SEND_BUFFER_SIZE];
    
    thread worker;
    atomic<bool> sendBEmpty = true;
    atomic<bool> stop = false;

    mutex mtxState;
    mutex mtxCallbackState;
    mutex mtxCallbackData;

public:
    UsbWorker(int deviceId, int baud, int deadAfterMs, function<void(int, char*)> callback_data, function<void(int, int)> callback_state);
    ~UsbWorker();

    bool Send(string data);

private:    
    useconds_t sleepForBaudAndBytes(int baud, int bufferSize);
    void work(int deviceId, unsigned int baud, int deadAfterMs);

    
   
};