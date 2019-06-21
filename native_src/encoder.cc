#include <iostream>
#include "../headers/encoder.h"

namespace remoting
{
    void Encoder::Init()
    {
        _screenCapturer = new ScreenCapturer();
        _screenCapturer->InitializeMonitorProperties();

        _i_frame_counter = 0;

        int width = _screenCapturer->GetWidth();
        int height = _screenCapturer->GetHeight();

        _width = width;
        _height = height;

        // Initialise x264 encoder and swscale converter
        _x264Encoder = OpenEncoder(width, height);
        InitializeConverter(width, height);

        // RGB input information
        _rgbData = _screenCapturer->GetDataPointer();
        _rgbPlanes[0] = _rgbData;
        _rgbPlanes[1] = NULL;
        _rgbPlanes[2] = NULL;
        _rgbStride[0] = 4 * width;
        _rgbStride[1] = 0;
        _rgbStride[2] = 0;

        // YUV output information
        _yuvData = new uint8_t[3 * width * height / 2];
        _yuvPlanes[0] = _yuvData;
        _yuvPlanes[1] = _yuvData + width*height;
        _yuvPlanes[2] = _yuvData + width*height + width*height / 4;
        _yuvStride[0] = width;
        _yuvStride[1] = width/2;
        _yuvStride[2] = width/2;
    }
    
    void Bitmap2Yuv420p_calc2(uint8_t *destination, uint8_t *rgb, size_t width, size_t height)
    {
        size_t image_size = width * height;
        size_t upos = image_size;
        size_t vpos = upos + upos / 4;
        size_t i = 0;

        for( size_t line = 0; line < height; ++line )
        {
            if( !(line % 2) )
            {
                for( size_t x = 0; x < width; x += 2 )
                {
                    uint8_t b = rgb[4 * i];
                    uint8_t g = rgb[4 * i + 1];
                    uint8_t r = rgb[4 * i + 2];

                    destination[i++] = ((66*r + 129*g + 25*b) >> 8) + 16;

                    destination[upos++] = ((-38*r + -74*g + 112*b) >> 8) + 128;
                    destination[vpos++] = ((112*r + -94*g + -18*b) >> 8) + 128;

                    b = rgb[4 * i];
                    g = rgb[4 * i + 1];
                    r = rgb[4 * i + 2];

                    destination[i++] = ((66*r + 129*g + 25*b) >> 8) + 16;
                }
            }
            else
            {
                for( size_t x = 0; x < width; x += 1 )
                {
                    uint8_t b = rgb[4 * i];
                    uint8_t g = rgb[4 * i + 1];
                    uint8_t r = rgb[4 * i + 2];

                    destination[i++] = ((66*r + 129*g + 25*b) >> 8) + 16;
                }
            }
        }
    }


    void Encoder::InitializeConverter(int W, int H)
    {
        // Initialise swscale converter
        _swsConverter = sws_getContext(W, H, AV_PIX_FMT_BGRA, W, H, AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);
        if (_swsConverter == NULL)
        {
            throw("could not create scaling context.");
        }
        else
        {
            std::cout << "created scaling context succeeded." << std::endl;
        }
    }

    uint8_t* Encoder::GetNextFrame(int* frame_size)
    {
        int W = _width;
        int H = _height;
        _screenCapturer->CaptureScreen();
        
        int returnValue = sws_scale(_swsConverter, _rgbPlanes, _rgbStride, 0, H, _yuvPlanes, _yuvStride);
        if (returnValue == H)
        {
            std::cout << "Converted the color space of the image." << std::endl;
        }
        else
        {
            throw("Failed to convert the color space of the image.");
        }

        // Bitmap2Yuv420p_calc2(_yuvData, _rgbData, W, H);
        
        int luma_size = W * H;
        int chroma_size = luma_size/4;

        _inputPic.img.plane[0] = _yuvData;
        _inputPic.img.plane[1] = _yuvData + luma_size;
        _inputPic.img.plane[2] = _yuvData + luma_size + chroma_size;
        _inputPic.i_pts = _i_frame_counter;
        _i_frame_counter++;

        int i_frame_size = x264_encoder_encode(_x264Encoder, &_nal, &_noOfNal, &_inputPic, &_outputPic);
        std::cout<<i_frame_size;

        *frame_size = i_frame_size;
        if (i_frame_size <= 0)
        {
            throw ("No NAL is produced out of encoder.");
        }

        return _nal->p_payload;
    }

    x264_t* Encoder::OpenEncoder(int width, int height) 
    {
        x264_param_t x264Params;
        x264_t *h;

        int returnValue = x264_param_default_preset(&x264Params, x264_preset_names[2], x264_tune_names[7]);
        if (returnValue == 0)
        {
            std::cout << x264_preset_names[2] << " preset is applied and " << x264_tune_names[7] << " tune is applied." << std::endl;
        }
        else
        {
            throw("Failed to apply default preset.");
        }

        /* Configure non-default params */
        // x264Params.i_bitdepth = 8;
        x264Params.i_csp = X264_CSP_I420;
        x264Params.i_width = width;
        x264Params.i_height = height;
        // x264Params.b_vfr_input = 0;
        x264Params.b_repeat_headers = 1;
        x264Params.b_annexb = 1;

        x264_param_apply_fastfirstpass(&x264Params);

        returnValue = x264_param_apply_profile(&x264Params, x264_profile_names[0]);
        if (returnValue == 0)
        {
            std::cout << x264_profile_names[0] << " profile is applied." << std::endl;
        }
        else
        {
            throw("Failed to apply profile.");
        }

        returnValue = x264_picture_alloc(&_inputPic, x264Params.i_csp, x264Params.i_width, x264Params.i_height);
        if (returnValue == 0)
        {
            std::cout << "x264_picture_alloc succeeded." << std::endl;
        }
        else
        {
            throw("x264_picture_alloc Failed.");
        }

        h = x264_encoder_open(&x264Params);

        return h;
    }

    Encoder::~Encoder()
    {
        delete this->_screenCapturer;
        x264_encoder_close(_x264Encoder);
        delete[] _rgbData;       
        delete[] _yuvData;
        x264_picture_clean(&_inputPic);
        x264_picture_clean(&_outputPic);

        // sws_freeContext(_swsConverter);
    }
}
