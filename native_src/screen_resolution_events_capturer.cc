#include "../headers/screen_resolution_events_capturer.h"
#include "../headers/display_utility_x11.h"
#include <iostream>
#include <memory>
#include <chrono>

using namespace Napi;
using namespace std;
using namespace remoting;

std::thread nativeThread;
Napi::ThreadSafeFunction tsfn;

outputResolutionList *list = nullptr;
unsigned int numMonitors = 0;
bool isClosing = false;

Display *display_ = nullptr;
Window root_window_;
int change_event_base_ = 0;
int change_error_base_ = 0;

// Napi functions to create, start and close listener
void CreateListener(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsFunction())
        throw TypeError::New(env, "Expects a single function type argument");

    tsfn = ThreadSafeFunction::New(
        env,
        info[0].As<Function>(),                 // JavaScript function called asynchronously
        "RpcScreenResolEventListener",          // Name
        0,                                      // Unlimited queue
        1);                                     // Only one thread will use this initially

    list = getOutputResolutionDetails(&numMonitors);
    for(unsigned int i=0; i<numMonitors; i++)
        cout << "monitor " << i << " has resolution " << list[i].width << "x" << list[i].height << endl;
};

void StartListener(const Napi::CallbackInfo& info) {
   InitXlib();

    nativeThread = std::thread( [] {
        auto callback = [] (Napi::Env env, Function jsCallback, char* message) {
            String res = String::New(env, message);
            cout << "in cb:" << res.Utf8Value() << "," << message << endl;
            //jsCallback.Call({String::New(env, "data"), String::New(env, message)});
            jsCallback.Call({String::New(env, "resolution-data"), res});
            if (message)
            {
                delete[] message;
                message = NULL;
            }
        };

        auto callback_2 = [] (Napi::Env env, Function jsCallback) {
            jsCallback.Call({String::New(env, "primary-monitor-change")});
        };

        while(!isClosing) {
            if (ProcessPendingXEvents()) {
                if (checkPrimaryMonitorChange())
                    tsfn.NonBlockingCall(callback_2);

                char *message = new char[50] {0};
                if (checkResolutionChange(message)) {
                    napi_status status = tsfn.NonBlockingCall(message, callback);
                    if (napi_ok != status)
                       cout << "Screen resolution changed but got error at C++ in calling node event: " << status << endl;
                } else {
                    cout << "Resolution is same as before, not doing any operation\n";
                    if (message) {
                        delete[] message;
                        message = NULL;
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        if (napi_ok != tsfn.Release())
            cout << "Error on releasing thread-safe function\n";
        else
            cout << "Successfully released thread-safe function\n";
    });
}

void CloseListener(const Napi::CallbackInfo& info) {
    cout << "Close Listener called\n";
    isClosing = true;
    nativeThread.join();

    if (list != nullptr) {
        delete[] list;
        list = nullptr;
    }

    DeinitXlib();
}

// Xrandr related functions to handle events for screen resolution change

bool InitXlib() {
   display_ = XOpenDisplay(NULL);
    if (!display_) {
        cout << "Unable to open display" << endl;
        return false;
    }
    root_window_ = RootWindow(display_, DefaultScreen(display_));
    if (root_window_ == BadValue) {
        cout << "Unable to get the root window" << endl;
        DeinitXlib();
        return false;
    }   

    XRRSelectInput(display_, root_window_, RRScreenChangeNotifyMask);
    if (XRRQueryExtension (display_, &change_event_base_, &change_error_base_)) {
        cout << " Listener created \n";
    } else {
        cout << " Listener failed \n";
    }   

    return true;
}

void DeinitXlib() {
   if (display_ != nullptr) {
      XCloseDisplay(display_);
      display_ = nullptr;
   }
}

bool ProcessPendingXEvents() {
   int events_to_process = XPending(display_);
   XEvent e;

   for (int i = 0; i < events_to_process; i++) {
      XNextEvent(display_, &e);

      if (e.type == change_event_base_ + RRScreenChangeNotify) {
         XRRScreenChangeNotifyEvent *event = reinterpret_cast<XRRScreenChangeNotifyEvent *>(&e);
         cout << "XRRScreenChangeNotifyEvent received, total resolution is: " << event->width << " x " << event->height << endl;
         return true;
      }
   }
   return false;
}

outputResolutionList* getOutputResolutionDetails(unsigned int *n) {
    RROutput *o; 

    unique_ptr<DisplayUtilityX11> desktopInfo = DisplayUtilityX11::Create();
    desktopInfo->TryGetConnectedOutputs(n, &o);

    outputResolutionList *tmpList = new outputResolutionList[*n];
    for(unsigned int i=0; i<*n; i++) {
        unique_ptr<OutputResolutionWithOffset> resolution = desktopInfo->GetCurrentResolution(o[i]);
        tmpList[i].width = resolution->width();
        tmpList[i].height = resolution->height();
        tmpList[i].rrOutput = resolution->rrOutput();
    }   

    delete[] o;
    return tmpList;
}

bool checkPrimaryMonitorChange() {
    unsigned int n = 0;
    outputResolutionList *newResolutionList = getOutputResolutionDetails(&n);
    if (list[0].rrOutput != newResolutionList[0].rrOutput) {
        cout << "primary monitor is changed\n";

        for(unsigned int i=1;i<n;i++) {
            if (list[i].rrOutput != newResolutionList[i].rrOutput) {
                outputResolutionList tmp = list[0];
                list[0] = list[i];
                list[i] = tmp;
                delete[] newResolutionList;
                return true;
            }
        }
    }
    delete[] newResolutionList;
    return false;
}

// currently, it is assumed that only one output's resolution can change at one time
bool checkResolutionChange(char *message) {
    bool flag = false;
    unsigned int n = 0, i = 0;
    outputResolutionList *newResolutionList = getOutputResolutionDetails(&n);

    for(; i<n; i++) {
        if (list[i].width != newResolutionList[i].width || list[i].height != newResolutionList[i].height) {
            flag = true;
            list[i]= newResolutionList[i];
            char temp[20];
            sprintf(temp, "%u*%u*0*%u,",list[i].width, list[i].height,i+1);
            strcat(message, temp);
        }
    }   

    for(unsigned int i=0; i<n; i++)
        cout << "monitor " << i << " has resolution " << list[i].width << "x" << list[i].height << endl;

    // remove the trailing comma
    message[strlen(message)-1] = 0;
    delete[] newResolutionList;
    return flag;
}

