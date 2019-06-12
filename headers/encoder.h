#ifndef REMOTING_HOST_ENCODER_H_
#define REMOTING_HOST_ENCODER_H_

#include <stdint.h>
#include <X11/Xutil.h>
extern "C"
{
#include <x264.h>
#include <libswscale/swscale.h>
#include <libavutil/pixfmt.h>
}
#include "screen_capturer.h"

namespace remoting
{
    class Encoder 
    {
        public:
            void Init();
            uint8_t* GetNextFrame(int* frameSize);
            void CleanUp();
        private:
            ScreenCapturer* _screenCapturer;
            x264_t* _x264Encoder;
            SwsContext* _swsConverter;
            uint8_t* _rgbData;
            x264_picture_t _inputPic;
            int _width;
            int _height;
            int64_t _i_frame_counter;
            SwsContext* InitializeConverter(int width, int height);
            x264_t* OpenEncoder(int width, int height);
    };

}

#endif // REMOTING_HOST_ENCODER_H_