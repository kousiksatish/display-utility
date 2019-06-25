#include "../headers/screen_capturer.h"

namespace remoting
{
    ScreenCapturer::ScreenCapturer()
    {
        _display = XOpenDisplay(NULL);
        _window = DefaultRootWindow(_display);
    }

    void ScreenCapturer::InitializeMonitorProperties() {
        XWindowAttributes attributes;
        XGetWindowAttributes(_display, _window, &attributes);
        _offsetX = 0;
        _offsetY = 0;
        _width = attributes.width;
        _height = attributes.height;
        // To allocate memory
        _xImage = XGetImage(_display, _window, _offsetX, _offsetY, _width, _height, AllPlanes, ZPixmap);
    }

    void ScreenCapturer::InitializeMonitorProperties(RROutput rROutput) {
        std::unique_ptr<DisplayUtilityX11> desktopInfo = DisplayUtilityX11::Create();
        std::unique_ptr<OutputResolutionWithOffset> resolution = desktopInfo->GetCurrentResolution(rROutput);
        if (resolution != nullptr) {
            _offsetX = resolution->offsetX();
            _offsetY = resolution->offsetY();
            _width = resolution->width();
            _height = resolution->height();
            // To allocate memory
            _xImage = XGetImage(_display, _window, _offsetX, _offsetY, _width, _height, AllPlanes, ZPixmap);
        } else {
            throw "Error in getting resolution and offset";
        }
    }

    uint8_t* ScreenCapturer::GetDataPointer() {
        return reinterpret_cast<uint8_t*>(_xImage->data);
    }

    void ScreenCapturer::CaptureScreen() {
        // Reuse the same memory
        XGetSubImage(_display, _window, _offsetX, _offsetY, _width, _height, AllPlanes, ZPixmap, _xImage, 0, 0);
    }

    int ScreenCapturer::GetWidth() {
        return _width;
    }

    int ScreenCapturer::GetHeight() {
        return _height;
    }

    ScreenCapturer::~ScreenCapturer() {
        XDestroyImage(_xImage);
        XCloseDisplay(_display);
    }
}