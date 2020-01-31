#ifndef REMOTING_HOST_BASE_SCREEN_CAPTURER_H_
#define REMOTING_HOST_BASE_SCREEN_CAPTURER_H_

#include <stdint.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace remoting
{
class BaseScreenCapturer
{
public:
    BaseScreenCapturer();
    virtual void InitializeMonitorProperties() = 0;
    virtual uint8_t *GetDataPointer() = 0;
    virtual void CaptureScreen() = 0;
    virtual int GetHeight() = 0;
    virtual int GetWidth() = 0;
    virtual ~BaseScreenCapturer() = 0;
    Display* GetDisplay();
    Window GetWindow();
protected:
    Display* _display;
    Window _window;
};
} // namespace remoting

#endif // REMOTING_HOST_BASE_SCREEN_CAPTURER_H_