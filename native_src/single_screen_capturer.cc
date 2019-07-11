#include "../headers/single_screen_capturer.h"
#include <string.h>

namespace remoting
{
SingleScreenCapturer::SingleScreenCapturer(RROutput rROutput)
{
    this->_display = XOpenDisplay(NULL);
    this->_window = DefaultRootWindow(this->_display);
    this->_rROutputOfScreen = rROutput;
    this->InitializeMonitorProperties();
}

void SingleScreenCapturer::InitializeMonitorProperties()
{
    std::unique_ptr<DisplayUtilityX11> desktopInfo = DisplayUtilityX11::Create();
    std::unique_ptr<OutputResolutionWithOffset> resolution = desktopInfo->GetCurrentResolution(this->_rROutputOfScreen);
    if (resolution != nullptr)
    {
        this->_offsetX = resolution->offsetX();
        this->_offsetY = resolution->offsetY();
        this->_width = resolution->width();
        this->_height = resolution->height();
        // To allocate memory
        this->_xImage = XGetImage(this->_display, this->_window, this->_offsetX, this->_offsetY, this->_width, this->_height, AllPlanes, ZPixmap);
    }
    else
    {
        throw "Error in getting resolution and offset";
    }
}

uint8_t *SingleScreenCapturer::GetDataPointer()
{
    return reinterpret_cast<uint8_t *>(this->_xImage->data);
}

void SingleScreenCapturer::CaptureScreen()
{
    // Use XGetSubImage to reuse the same memory
    XGetSubImage(this->_display, this->_window, this->_offsetX, this->_offsetY, this->_width, this->_height, AllPlanes, ZPixmap, this->_xImage, 0, 0);
}

int SingleScreenCapturer::GetWidth()
{
    return this->_width;
}

int SingleScreenCapturer::GetHeight()
{
    return this->_height;
}

SingleScreenCapturer::~SingleScreenCapturer()
{
    XDestroyImage(this->_xImage);
    XCloseDisplay(this->_display);
}
} // namespace remoting