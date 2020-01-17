#ifndef REMOTING_HOST_ENCODER_H_
#define REMOTING_HOST_ENCODER_H_

extern "C"
{
#include <stdint.h>
#include <X11/Xutil.h>
#include "../x264/headers/x264.h"
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
    uint8_t *GetNextFrame(int *frameSize, bool getIFrame = false);
    ~Encoder();

private:
    BaseScreenCapturer *_screenCapturer;
    x264_t *_x264Encoder;
    // SwsContext *_swsConverter;
    uint8_t *_rgbData;
    // uint8_t *_rgbPlanes[3];
    // int _rgbStride[3];
    uint8_t *_prevYUVData;
    uint8_t *_yuvData;
    // uint8_t *_yuvPlanes[3];
    // int _yuvStride[3];
    bool _isInitialised;
    x264_picture_t _inputPic;
    x264_picture_t _outputPic;
    x264_nal_t *_nal;
    int _noOfNal;
    int _width;
    int _height;
    int64_t _i_frame_counter;
    // void InitializeConverter(int width, int height);
    x264_t *OpenEncoder(int width, int height);
    void CleanUp();
};

} // namespace remoting

#endif // REMOTING_HOST_ENCODER_H_