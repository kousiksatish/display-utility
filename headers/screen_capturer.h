#ifndef REMOTING_HOST_SCREEN_CAPTURER_H_
#define REMOTING_HOST_SCREEN_CAPTURER_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdint.h>

namespace remoting
{
    class ScreenCapturer {
        public:
            void Init();
            void InitializeMonitorProperties();
            uint8_t* GetDataPointer();
            void CaptureScreen();
            int GetWidth();
            int GetHeight();
            ~ScreenCapturer();
        private: 
            Display* _display;
            Window _window;
            XWindowAttributes _attributes;
            XImage* _xImage;
    };
}

#endif // REMOTING_HOST_SCREEN_CAPTURER_H_