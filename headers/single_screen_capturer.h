#ifndef REMOTING_HOST_SINGLE_SCREEN_CAPTURER_H_
#define REMOTING_HOST_SINGLE_SCREEN_CAPTURER_H_

#include "base_screen_capturer.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include "display_utility_x11.h"
#include <stdint.h>

namespace remoting
{
class SingleScreenCapturer : public BaseScreenCapturer
{
public:
    SingleScreenCapturer(Display* display, Window window, RROutput rROutput);
    uint8_t *GetDataPointer();
    void CaptureScreen();
    int GetWidth();
    int GetHeight();
    ~SingleScreenCapturer();
    void InitializeMonitorProperties();
private:
    RROutput _rROutputOfScreen;
    int _offsetX;
    int _offsetY;
    int _width;
    int _height;
    XImage *_xImage;
    XShmSegmentInfo _shminfo;
};
} // namespace remoting

#endif // REMOTING_HOST_SINGLE_SCREEN_CAPTURER_H_