#include "../headers/screen_capturer.h"

namespace remoting
{
    void ScreenCapturer::Init()
    {
        _display = XOpenDisplay(NULL);
        _window = DefaultRootWindow(_display);
    }

    void ScreenCapturer::InitializeMonitorProperties() {
        XGetWindowAttributes(_display, _window, &_attributes);
    }

    uint8_t* ScreenCapturer::GetDataPointer() {
        return _data;
    }

    void ScreenCapturer::CaptureScreen() {
        XImage* x_image = XGetImage(_display, _window, 0, 0, _attributes.width, _attributes.height, AllPlanes, ZPixmap);
        _data = reinterpret_cast<uint8_t*>(x_image->data);
    }

    int ScreenCapturer::GetWidth() {
        return _attributes.width;
    }

    int ScreenCapturer::GetHeight() {
        return _attributes.height;
    }

    ScreenCapturer::~ScreenCapturer() {
        XCloseDisplay(_display);
    }
}