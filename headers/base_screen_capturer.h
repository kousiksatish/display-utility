#ifndef REMOTING_HOST_BASE_SCREEN_CAPTURER_H_
#define REMOTING_HOST_BASE_SCREEN_CAPTURER_H_

#include <stdint.h>

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
};
} // namespace remoting

#endif // REMOTING_HOST_BASE_SCREEN_CAPTURER_H_