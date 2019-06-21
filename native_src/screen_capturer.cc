#include "../headers/screen_capturer.h"

namespace remoting
{
    ScreenCapturer::ScreenCapturer()
    {
        _display = XOpenDisplay(NULL);
        _window = DefaultRootWindow(_display);
    }

    void ScreenCapturer::InitializeMonitorProperties() {
        XGetWindowAttributes(_display, _window, &_attributes);
        // To allocate memory
        _xImage = XGetImage(_display, _window, 0, 0, _attributes.width, _attributes.height, AllPlanes, ZPixmap);
    }

    uint8_t* ScreenCapturer::GetDataPointer() {
        return reinterpret_cast<uint8_t*>(_xImage->data);
    }

    void ScreenCapturer::CaptureScreen() {
        // Reuse the same memory
        XGetSubImage(_display, _window, 0, 0, _attributes.width, _attributes.height, AllPlanes, ZPixmap, _xImage, 0, 0);
    }

    int ScreenCapturer::GetWidth() {
        return _attributes.width;
    }

    int ScreenCapturer::GetHeight() {
        return _attributes.height;
    }

    ScreenCapturer::~ScreenCapturer() {
        XDestroyImage(_xImage);
        XCloseDisplay(_display);
    }
}