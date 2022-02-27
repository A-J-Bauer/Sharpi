#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>

#include <drm.h>
#include <drm_mode.h>

#include <xf86drm.h>
#include <xf86drmMode.h>

#include "Display.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkBitmap.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))


using namespace std;

class DisplayDrm : Display
{
private:
    static string description;

private:
    int error = 0;

private:
    string _driDevice = ""; 
    string _mode = "";
    unsigned int _frequency = -1;


    int _fd = -1;   
    drmModeConnectorPtr _connectorPtr = NULL;
    drmModeModeInfoPtr _modeInfoPtr = NULL;
    drmModeCrtcPtr _modeCrtcOrigPtr = NULL;
    drmEventContextPtr _eventContextPtr = NULL;

    uint32_t _crtcId;
    struct drm_mode_create_dumb _createDumbReq[2];
    uint32_t _bufId[2] = { 0,0 };
    void* _mmapPtr[2] = { NULL, NULL };
    
    uint8_t bidx = 0;    
    atomic<bool> flipdone = true;
     
    bool _isOn = false;
    SkImageInfo skImageInfo;

public:
    uint16_t width;
    uint16_t height;    
    
private:    
    void Cleanup();
    static void PageFlipHandler(int fd, unsigned int frame, unsigned int sec, unsigned int usec, void* data);

public:
    SkBitmap skBitmap;

    DisplayDrm();
    DisplayDrm(string drmDevice, string mode, unsigned int frequency);    
    ~DisplayDrm();

    string GetDescription();
    const char* GetDescriptionC();

    string ListResources();
    
    void PowerOn();
    void PowerOff();
    bool IsOn();

    void Update();

public:
    static const int MAX_DRM_DEVICES = 64;
    static const int NUM_BUFFERS = 2;
      
  
};


