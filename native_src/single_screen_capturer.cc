#include "../headers/single_screen_capturer.h"
#include <string.h>

namespace remoting
{
SingleScreenCapturer::SingleScreenCapturer(Display* display, Window window, RROutput rROutput)
{   
    this->_display = display;
    this->_window = window;
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

        XWindowAttributes window_attributes;
        XGetWindowAttributes(this->_display, this->_window, &window_attributes);
        Screen* screen = window_attributes.screen;
        this->_xImage = XShmCreateImage(this->_display, DefaultVisualOfScreen(screen), DefaultDepthOfScreen(screen), ZPixmap, NULL, &this->_shminfo, this->_width, this->_height);

        this->_shminfo.shmid = shmget(IPC_PRIVATE, this->_xImage->bytes_per_line * this->_xImage->height, IPC_CREAT | 0777);
        this->_shminfo.shmaddr = this->_xImage->data = (char*)shmat(this->_shminfo.shmid, 0, 0);
        this->_shminfo.readOnly = False;

        if (this->_shminfo.shmid < 0)
            throw "Fatal this->_shminfo error!";
        
        Status s1 = XShmAttach(this->_display, &this->_shminfo);
        if (!s1) {
            throw "Failure";
        }

        // To allocate memory
        // this->_xImage = XGetImage(this->_display, this->_window, this->_offsetX, this->_offsetY, this->_width, this->_height, AllPlanes, ZPixmap);
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
    // XGetSubImage(this->_display, this->_window, this->_offsetX, this->_offsetY, this->_width, this->_height, AllPlanes, ZPixmap, this->_xImage, 0, 0);
    XShmGetImage(this->_display, this->_window, this->_xImage, this->_offsetX, this->_offsetY, AllPlanes);
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
    XShmDetach(this->_display, &this->_shminfo);
    shmdt(this->_shminfo.shmaddr);
}
} // namespace remoting