#ifndef REMOTING_HOST_MULTI_SCREEN_CAPTURER_H_
#define REMOTING_HOST_MULTI_SCREEN_CAPTURER_H_

#include "base_screen_capturer.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include "display_utility_x11.h"
#include <stdint.h>

namespace remoting
{
class MultiScreenCapturer : public BaseScreenCapturer
{
public:
    MultiScreenCapturer(Display* display, Window window);
    void InitializeMonitorProperties();
    uint8_t *GetDataPointer();
    void CaptureScreen();
    int GetWidth();
    int GetHeight();
    ~MultiScreenCapturer();

private:
    int _width;
    int _height;
    XImage *_xImage;
    std::vector<OutputResolutionWithOffset> _currentResolutions;
    XShmSegmentInfo _shminfo;
    Pixmap _shm_pixmap;
    GC _shm_gc;
};
} // namespace remoting

#endif // REMOTING_HOST_MULTI_SCREEN_CAPTURER_H_