#ifndef REMOTING_HOST_SCREEN_CAPTURER_H_
#define REMOTING_HOST_SCREEN_CAPTURER_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "display_utility_x11.h"
#include <stdint.h>

namespace remoting
{
class ScreenCapturer
{
public:
    ScreenCapturer();
    void InitializeMonitorProperties();
    void InitializeMonitorProperties(RROutput rROutput);
    uint8_t *GetDataPointer();
    void CaptureScreen();
    int GetWidth();
    int GetHeight();
    ~ScreenCapturer();

private:
    Display *_display;
    Window _window;
    int _offsetX;
    int _offsetY;
    int _width;
    int _height;
    XImage *_xImage;
    std::set<OutputResolutionWithOffset> _currentResolutions;
    bool _multiMonitor;
};
} // namespace remoting

#endif // REMOTING_HOST_SCREEN_CAPTURER_H_