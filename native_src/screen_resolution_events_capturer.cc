#include "../headers/display_utility_x11.h"
#include <iostream>
#include <napi.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xutil.h>
#include <thread>
#include <chrono>

using namespace Napi;
using namespace std;
using namespace remoting;

typedef struct {
    unsigned int width;
    unsigned int height;
}outputResolutionList;

outputResolutionList *list = nullptr;
unsigned int numMonitors = 0;
std::thread nativeThread;
ThreadSafeFunction tsfn;
bool isClosing = false;

Display *display_ = nullptr;
Window root_window_;
int change_event_base_ = 0;
int change_error_base_ = 0;

bool ProcessPendingXEvents();
void DeinitXlib();
bool InitXlib();
outputResolutionList* getOutputResolutionDetails(unsigned int *n);
bool checkResolutionChange(char*);

// Napi functions to create, start and close listener
void CreateListener( const CallbackInfo& info )
{
  Env env = info.Env();
  if ( info.Length() < 1 || !info[0].IsFunction() )
    throw TypeError::New( env, "Expects a single function type argument" );

  tsfn = ThreadSafeFunction::New(
      env,
      info[0].As<Function>(),  // JavaScript function called asynchronously
      "Resource Name",         // Name
      0,                       // Unlimited queue
      1,                       // Only one thread will use this initially
      []( Napi::Env ) {        // Finalizer used to clean threads up
        nativeThread.join();
      } );

  list = getOutputResolutionDetails(&numMonitors);
  for(unsigned int i=0; i<numMonitors; i++)
    cout << "monitor " << i << " has resolution " << list[i].width << "x" << list[i].height << endl;
};

void StartListener(const CallbackInfo& info)
{
    InitXlib();

    nativeThread = std::thread( [] {
        auto callback = []( Env env, Function jsCallback, char* message) {
            jsCallback.Call( {String::New( env, "data" ), String::New( env, message )} );
            delete[] message;
        };

        while(!isClosing)
        {
            if (ProcessPendingXEvents())
            {
                //std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
                char *message = new char[20];
                if (checkResolutionChange(message))
                {
                    napi_status status = tsfn.NonBlockingCall( message, callback );
                    if (napi_ok != status)
                       cout << "Screen resolution changed but got error at C++ in calling node event: " << status << endl;
                }
                else
                    cout << "Resolution is same as before, not doing any operation\n";
            }
        }
    } );
}

Value CloseListener(const CallbackInfo& info)
{
    Env env = info.Env();
    cout << "Close Listener called\n";
    isClosing = true;
    if (list != nullptr)
    {
        delete[] list;
        list = nullptr;
    }
    DeinitXlib();
    if (napi_ok != tsfn.Release())
        cout << "Error on releasing thread-safe function\n";
    return Boolean::New(env, true);
}

Object Init_screenResolutionEventsCapturer( Env env, Object exports )
{
  Object displayEventsUtility = Object::New (env);
  displayEventsUtility.Set("createListener" , Function::New(env, CreateListener));
  displayEventsUtility.Set("startListener" ,  Function::New(env, StartListener));
  displayEventsUtility.Set("closeListener" ,  Function::New(env, CloseListener));

  exports.Set("DisplayEventsUtility", displayEventsUtility);
  return exports;
}

//NODE_API_MODULE( displayEventsUtility, Init )

// Xrandr related functions to handle events for screen resolution change

bool InitXlib()
{
    display_ = XOpenDisplay(NULL);
    if (!display_)
    {   
        cout << "Unable to open display" << endl;
        return false;
    }   
    root_window_ = RootWindow(display_, DefaultScreen(display_));
    if (root_window_ == BadValue)
    {   
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

void DeinitXlib()
{
   if (display_ != nullptr)
   {
      XCloseDisplay(display_);
      display_ = nullptr;
   }
}

bool ProcessPendingXEvents()
{
   int events_to_process = XPending(display_);
   XEvent e;

   for (int i = 0; i < events_to_process; i++)
   {
      XNextEvent(display_, &e);

      if (e.type == change_event_base_ + RRScreenChangeNotify)
      {
         XRRScreenChangeNotifyEvent *event = reinterpret_cast<XRRScreenChangeNotifyEvent *>(&e);
         cout << "XRRScreenChangeNotifyEvent received, total resolution is: " << event->width << " x " << event->height << endl;
         return true;
      }
   }
   return false;
}

outputResolutionList* getOutputResolutionDetails(unsigned int *n)
{
    RROutput *o; 

    unique_ptr<DisplayUtilityX11> desktopInfo = DisplayUtilityX11::Create();
    desktopInfo->TryGetConnectedOutputs(n, &o);

    outputResolutionList *list = new outputResolutionList[*n];
    for(unsigned int i=0; i<*n; i++)
    {
        unique_ptr<OutputResolutionWithOffset> resolution = desktopInfo->GetCurrentResolution(o[i]);
        //unique_ptr<OutputResolution> resolution = desktopInfo->GetCurrentResolution(o[i]);
        list[i].width = resolution->width();
        list[i].height = resolution->height();
    }   

    delete[] o;
    return list;
}

// currently, it is assumed that only one output's resolution can change at one time
bool checkResolutionChange(char *message)
{
    bool flag = false;
    unsigned int n = 0, i = 0;
    outputResolutionList *newResolutionList = getOutputResolutionDetails(&n);
    for(; i<n; i++)
    {   
        if (list[i].width != newResolutionList[i].width || list[i].height != newResolutionList[i].height)
        {
            flag = true;
            break;
        }
    }   

    if (flag)
    {   
        list[i]= newResolutionList[i];
        sprintf(message,"%u*%u*0*%u",list[i].width, list[i].height,i+1);
    }   

    for(unsigned int i=0; i<n; i++)
        cout << "monitor " << i << " has resolution " << list[i].width << "x" << list[i].height << endl;
    
    delete[] newResolutionList;
    return flag;
}

