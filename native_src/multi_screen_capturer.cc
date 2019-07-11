#include "../headers/multi_screen_capturer.h"
#include <string.h>

namespace remoting
{
MultiScreenCapturer::MultiScreenCapturer()
{
    this->_display = XOpenDisplay(NULL);
    this->_window = DefaultRootWindow(this->_display);
    this->InitializeMonitorProperties();
}

void MultiScreenCapturer::InitializeMonitorProperties()
{
    XWindowAttributes attributes;
    XGetWindowAttributes(this->_display, this->_window, &attributes);
    this->_width = attributes.width;
    this->_height = attributes.height;

    std::unique_ptr<DisplayUtilityX11> desktopInfo = DisplayUtilityX11::Create();
    this->_currentResolutions = desktopInfo->GetAllCurrentResolutions();

    // To allocate memory
    this->_xImage = XGetImage(this->_display, this->_window, 0, 0, this->_width, this->_height, AllPlanes, ZPixmap);
    // Black out the image
    memset(this->_xImage->data, 0, this->_width * this->_height * this->_xImage->bits_per_pixel / 8);
}

uint8_t *MultiScreenCapturer::GetDataPointer()
{
    return reinterpret_cast<uint8_t *>(this->_xImage->data);
}

void MultiScreenCapturer::CaptureScreen()
{
    // Use XGetSubImage to reuse the same memory
    // Loop through all available screens and capture into their appropriate offsets
    for (auto res : this->_currentResolutions)
    {
        XGetSubImage(this->_display, this->_window, res.offsetX(), res.offsetY(), res.width(), res.height(), AllPlanes, ZPixmap, this->_xImage, res.offsetX(), res.offsetY());
    }
}

int MultiScreenCapturer::GetWidth()
{
    return this->_width;
}

int MultiScreenCapturer::GetHeight()
{
    return this->_height;
}

MultiScreenCapturer::~MultiScreenCapturer()
{
    XDestroyImage(this->_xImage);
    XCloseDisplay(this->_display);
}
} // namespace remoting