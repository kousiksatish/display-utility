#ifndef REMOTING_HOST_ENCODER_H_
#define REMOTING_HOST_ENCODER_H_

extern "C"
{
#include <stdint.h>
#include <X11/Xutil.h>
#include "../x264/headers/x264.h"
#include <X11/extensions/Xdamage.h>
#include <libswscale/swscale.h>
}
#include "base_screen_capturer.h"
#include "single_screen_capturer.h"
#include "multi_screen_capturer.h"

namespace remoting
{
class Encoder
{
public:
    Encoder();
    void Init(bool singleMonitorCapture, RROutput rROutput = 0);
    uint8_t *GetNextFrame(int *frameSize);
    void SetForceNextFrame();
    void SetCRFValue(int crfValue);
    void SendNextFrameAsIFrame();
    ~Encoder();

private:
    // Capture properties
    Display* _display;
    Window _window;
    BaseScreenCapturer *_screenCapturer;
    uint8_t *_rgbData;
    uint8_t *_rgbPlanes[3];
    int _rgbStride[3];

    // Encoder properties
    x264_t *_x264Encoder;
    SwsContext *_swsConverter;
    uint8_t *_yuvData;
    uint8_t *_yuvPlanes[3];
    int _yuvStride[3];
    bool _isInitialised;
    x264_picture_t _inputPic;
    x264_picture_t _outputPic;
    x264_nal_t *_nal;
    int _noOfNal;
    int _width;
    int _height;
    int _outWidth;
    int _outHeight;
    int64_t _i_frame_counter;
    bool _force_next_frame;
    bool _next_frame_as_iframe;
    int _currentCRFValue;
    void InitializeConverter(int width, int height, int outWidth, int outHeight);
    x264_t *OpenEncoder(int width, int height);
    uint8_t *CaptureAndEncode(int *frameSize);
    void CleanUp();

    // XDamage
    void InitXDamage();
    bool _use_xdamage;
    Damage _damage_handle;
    int _damage_event_base;
    int _damage_error_base;
    XserverRegion _damage_region;
    XEvent _event;
};

} // namespace remoting

#endif // REMOTING_HOST_ENCODER_H_