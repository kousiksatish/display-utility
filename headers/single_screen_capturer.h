#ifndef REMOTING_HOST_SINGLE_SCREEN_CAPTURER_H_
#define REMOTING_HOST_SINGLE_SCREEN_CAPTURER_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "display_utility_x11.h"
#include "base_screen_capturer.h"
#include <stdint.h>

namespace remoting
{
class SingleScreenCapturer : public BaseScreenCapturer
{
public:
    SingleScreenCapturer(RROutput rROutput);
    uint8_t *GetDataPointer();
    void CaptureScreen();
    int GetWidth();
    int GetHeight();
    ~SingleScreenCapturer();
    void InitializeMonitorProperties();
private:
    Display *_display;
    Window _window;
    RROutput _rROutputOfScreen;
    int _offsetX;
    int _offsetY;
    int _width;
    int _height;
    XImage *_xImage;
};
} // namespace remoting

#endif // REMOTING_HOST_SINGLE_SCREEN_CAPTURER_H_