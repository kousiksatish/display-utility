#include <iostream>
#include "../headers/encoder.h"

namespace remoting
{
    Encoder::Encoder() 
    {
        _screenCapturer = new ScreenCapturer();
        _screenCapturer->Init();
        _screenCapturer->InitializeMonitorProperties();

        _i_frame_counter = 0;

        int width = _screenCapturer->GetWidth();
        int height = _screenCapturer->GetHeight();

        _width = width;
        _height = height;

        // Initialise x264 encoder and swscale converter
        _x264Encoder = OpenEncoder(width, height);
        _swsConverter = InitializeConverter(width, height);

        // RGB input information
        _rgbData = _screenCapturer->GetDataPointer();
    }
    
    SwsContext* Encoder::InitializeConverter(int W, int H)
    {
        // Initialise swscale converter
        struct SwsContext *sws;
        sws = sws_getContext(W, H, AV_PIX_FMT_BGRA, W, H, AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);
        if (sws == NULL)
        {
            throw("could not create scaling context.");
        }
        else
        {
            std::cout << "created scaling context succeeded." << std::endl;
        }

        return sws;
    }

    uint8_t* Encoder::GetNextFrame(int* frame_size)
    {
        int W = _width;
        int H = _height;
        _screenCapturer->CaptureScreen();
        _rgbData = _screenCapturer->GetDataPointer();
        uint8_t* rgb_planes[3] = {_rgbData, NULL, NULL};
        int rgb_stride[3] = {4 * W, 0, 0};

        // YUV output information
        uint8_t* yuv_data = new uint8_t[3 * W * H / 2];
        uint8_t* yuv_planes[3] = {yuv_data, yuv_data + W*H, yuv_data + W*H + W*H / 4};
        int yuv_stride[3] = {W, W/2, W/2};

        int returnValue = sws_scale(_swsConverter, rgb_planes, rgb_stride, 0, H, yuv_planes, yuv_stride);
        if (returnValue == H)
        {
            std::cout << "Converted the color space of the image." << std::endl;
        }
        else
        {
            throw("Failed to convert the color space of the image.");
        }

        int luma_size = W * H;
        int chroma_size = luma_size/4;

        _inputPic.img.plane[0] = yuv_data;
        _inputPic.img.plane[1] = yuv_data + luma_size;
        _inputPic.img.plane[2] = yuv_data + luma_size + chroma_size;
        _inputPic.i_pts = _i_frame_counter;
        _i_frame_counter++;

        x264_picture_t outputPic;

        x264_nal_t* nal;
        int i_nal;

        int i_frame_size = x264_encoder_encode(_x264Encoder, &nal, &i_nal, &_inputPic, &outputPic);
        std::cout<<i_frame_size;

        *frame_size = i_frame_size;
        if (i_frame_size <= 0)
        {
            throw ("No NAL is produced out of encoder.");
        }

        delete[] yuv_data;
        // x264_picture_clean(&outputPic);
        
        return nal->p_payload;
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
        delete[] _rgbData;
        sws_freeContext(_swsConverter);
        x264_encoder_close(_x264Encoder);
    }
}