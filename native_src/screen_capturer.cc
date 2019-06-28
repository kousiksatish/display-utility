#include "../headers/screen_capturer.h"
#include <string.h>

namespace remoting
{
    ScreenCapturer::ScreenCapturer()
    {
        _display = XOpenDisplay(NULL);
        _window = DefaultRootWindow(_display);
        _multiMonitor = false;
    }

    void ScreenCapturer::InitializeMonitorProperties() {
        XWindowAttributes attributes;
        XGetWindowAttributes(_display, _window, &attributes);
        _offsetX = 0;
        _offsetY = 0;
        _width = attributes.width;
        _height = attributes.height;

        std::unique_ptr<DisplayUtilityX11> desktopInfo = DisplayUtilityX11::Create();
        _currentResolutions = desktopInfo->GetAllCurrentResolutions();

        // To allocate memory
        _xImage = XGetImage(_display, _window, _offsetX, _offsetY, _width, _height, AllPlanes, ZPixmap);
        // Black out the image
        memset(_xImage->data, 0, _width * _height * _xImage->bits_per_pixel / 8);
        _multiMonitor = true;
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
        // Use XGetDubImage to reuse the same memory
        if (!_multiMonitor) {
            XGetSubImage(_display, _window, _offsetX, _offsetY, _width, _height, AllPlanes, ZPixmap, _xImage, 0, 0);
        } else {
            for (auto res : _currentResolutions) {
                XGetSubImage(_display, _window, res.offsetX(), res.offsetY(), res.width(), res.height(), AllPlanes, ZPixmap, _xImage, res.offsetX(), res.offsetY());
            }
        }
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