#ifndef SCREEN_RESOLUTION_EVENTS_CAPTURER_H
#define SCREEN_RESOLUTION_EVENTS_CAPTURER_H

#include <napi.h>
#include <thread>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xutil.h>

typedef struct {
    unsigned int width;
    unsigned int height;
    RROutput rrOutput;
}outputResolutionList;


void CreateListener (const Napi::CallbackInfo& info);
void StartListener (const Napi::CallbackInfo& info);
void CloseListener (const Napi::CallbackInfo& info);

bool ProcessPendingXEvents();
void DeinitXlib();
bool InitXlib();
bool checkPrimaryMonitorChange();
bool checkResolutionChange(char*);
outputResolutionList* getOutputResolutionDetails(unsigned int *n);

/*
std::thread nativeThread;
Napi::ThreadSafeFunction tsfn;

outputResolutionList *list = nullptr;
unsigned int numMonitors = 0;
bool isClosing = false;

Display *display_ = nullptr;
Window root_window_;
int change_event_base_ = 0;
int change_error_base_ = 0;
*/
#endif
