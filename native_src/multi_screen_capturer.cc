#include "../headers/multi_screen_capturer.h"
#include <string.h>

namespace remoting
{
MultiScreenCapturer::MultiScreenCapturer(Display* display, Window window)
{
    this->_display = display;
    this->_window = window;
    this->InitializeMonitorProperties();
}

void MultiScreenCapturer::InitializeMonitorProperties()
{
    XWindowAttributes attributes;
    XGetWindowAttributes(this->_display, this->_window, &attributes);
    this->_width = attributes.width + (attributes.width%2);
    this->_height = attributes.height + (attributes.height%2);
    Screen *screen = attributes.screen;
    this->_xImage = XShmCreateImage(this->_display, DefaultVisualOfScreen(screen), DefaultDepthOfScreen(screen), ZPixmap, NULL, &this->_shminfo, this->_width, this->_height);

    this->_shminfo.shmid = shmget(IPC_PRIVATE, this->_xImage->bytes_per_line * this->_xImage->height, IPC_CREAT | 0777);
    this->_shminfo.shmaddr = this->_xImage->data = (char *)shmat(this->_shminfo.shmid, 0, 0);
    this->_shminfo.readOnly = False;

    if (this->_shminfo.shmid < 0)
        throw "Fatal this->_shminfo error!";

    Status s1 = XShmAttach(this->_display, &this->_shminfo);
    if (!s1)
    {
        throw "Failure attaching shared memory";
    }

    // Create shm pixmap
    this->_shm_pixmap = XShmCreatePixmap(this->_display, this->_window, this->_shminfo.shmaddr, &this->_shminfo, this->_width, this->_height, DefaultDepthOfScreen(screen));
    XSync(this->_display, False);

    // Create shm graphics context
    XGCValues shm_gc_values;
    shm_gc_values.subwindow_mode = IncludeInferiors;
    shm_gc_values.graphics_exposures = False;
    this->_shm_gc = XCreateGC(this->_display, this->_window,
                              GCSubwindowMode | GCGraphicsExposures, &shm_gc_values);

    std::unique_ptr<DisplayUtilityX11> desktopInfo = DisplayUtilityX11::Create();
    this->_currentResolutions = desktopInfo->GetAllCurrentResolutions();

    // To allocate memory
    // this->_xImage = XGetImage(this->_display, this->_window, 0, 0, this->_width, this->_height, AllPlanes, ZPixmap);
    // Black out the image
    // memset(this->_xImage->data, 0, this->_width * this->_height * this->_xImage->bits_per_pixel / 8);
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
        // XGetSubImage(this->_display, this->_window, res.offsetX(), res.offsetY(), res.width(), res.height(), AllPlanes, ZPixmap, this->_xImage, res.offsetX(), res.offsetY());
        // Copy monitor by monitor onto shm pixmap
        XCopyArea(this->_display, this->_window, this->_shm_pixmap, this->_shm_gc, res.offsetX(),
                  res.offsetY(), res.width(), res.height(), res.offsetX(),
                  res.offsetY());
    }
    XSync(this->_display, False);
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
    XFreePixmap(this->_display, this->_shm_pixmap);
    XFreeGC(this->_display, this->_shm_gc);
    XShmDetach(this->_display, &this->_shminfo);
    shmdt(this->_shminfo.shmaddr);
}
} // namespace remoting