#include <iostream>
#include "../headers/encoder.h"

namespace remoting
{
Encoder::Encoder()
{
    _isInitialised = false;
}
void Encoder::Init(bool singleMonitorCapture, RROutput rROutput)
{
    if (_isInitialised)
    {
        std::cout << "Deleting stuff for reinitialising..";
        this->CleanUp();
    }
    try
    {
        if (singleMonitorCapture)
        {
            _screenCapturer = new SingleScreenCapturer(rROutput);
        }
        else
        {
            _screenCapturer = new MultiScreenCapturer();
        }
    }
    catch (std::string msg)
    {
        throw "ERROR: x264 Encoder initialisation failed." + msg;
    }

    _width = _screenCapturer->GetWidth();
    _height = _screenCapturer->GetHeight();

    _i_frame_counter = 0;

    // RGB input information
    _rgbData = _screenCapturer->GetDataPointer();
    _rgbPlanes[0] = _rgbData;
    _rgbPlanes[1] = NULL;
    _rgbPlanes[2] = NULL;
    _rgbStride[0] = 4 * _width;
    _rgbStride[1] = 0;
    _rgbStride[2] = 0;

    // YUV output information
    _yuvData = new uint8_t[3 * _width * _height / 2];
    _yuvPlanes[0] = _yuvData;
    _yuvPlanes[1] = _yuvData + _width * _height;
    _yuvPlanes[2] = _yuvData + _width * _height + _width * _height / 4;
    _yuvStride[0] = _width;
    _yuvStride[1] = _width / 2;
    _yuvStride[2] = _width / 2;

    try
    {
        // Initialise x264 encoder
        _x264Encoder = OpenEncoder(_width, _height);
    }
    catch (const char *msg)
    {
        throw "ERROR: x264 Encoder initialisation failed. " + std::string(msg);
    }
    // InitializeConverter(_width, _height);

    _isInitialised = true;
}

void Bitmap2Yuv420p_calc2(uint8_t *destination, uint8_t *rgb, size_t width, size_t height)
{
    size_t image_size = width * height;
    size_t upos = image_size;
    size_t vpos = upos + upos / 4;
    size_t i = 0;

    for (size_t line = 0; line < height; ++line)
    {
        if (!(line % 2))
        {
            for (size_t x = 0; x < width; x += 2)
            {
                uint8_t b = rgb[4 * i];
                uint8_t g = rgb[4 * i + 1];
                uint8_t r = rgb[4 * i + 2];

                destination[i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;

                destination[upos++] = ((-38 * r + -74 * g + 112 * b) >> 8) + 128;
                destination[vpos++] = ((112 * r + -94 * g + -18 * b) >> 8) + 128;

                b = rgb[4 * i];
                g = rgb[4 * i + 1];
                r = rgb[4 * i + 2];

                destination[i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
            }
        }
        else
        {
            for (size_t x = 0; x < width; x += 1)
            {
                uint8_t b = rgb[4 * i];
                uint8_t g = rgb[4 * i + 1];
                uint8_t r = rgb[4 * i + 2];

                destination[i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
            }
        }
    }
}

/*void Encoder::InitializeConverter(int W, int H)
{
    // Initialise swscale converter
    _swsConverter = sws_getContext(W, H, AV_PIX_FMT_BGRA, W, H, AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);
    if (_swsConverter == NULL)
    {
        throw "Could not create scaling context.";
    }
    else
    {
        std::cout << "created scaling context succeeded." << std::endl;
    }
}
 */
uint8_t *Encoder::GetNextFrame(int *frame_size)
{
    if (!_isInitialised)
    {
        throw "ERROR: ScreenCaptureUtility not initialised before use.";
    }

    int W = _width;
    int H = _height;

    try
    {
        _screenCapturer->CaptureScreen();
    }
    catch (const char *msg)
    {
        throw "ERROR: Screen capture of next frame failed. " + std::string(msg);
    }

    // int returnValue = sws_scale(_swsConverter, _rgbPlanes, _rgbStride, 0, H, _yuvPlanes, _yuvStride);
    // if (returnValue == H)
    // {
    //     std::cout << "Converted the color space of the image." << std::endl;
    // }
    // else
    // {
    //     throw("Failed to convert the color space of the image.");
    // }

    try
    {
        Bitmap2Yuv420p_calc2(_yuvData, _rgbData, W, H);
    }
    catch (const char *msg)
    {
        throw "ERROR: RGB to YUV conversion failed. " + std::string(msg);
    }

    int luma_size = W * H;
    int chroma_size = luma_size / 4;

    _inputPic.img.plane[0] = _yuvData;
    _inputPic.img.plane[1] = _yuvData + luma_size;
    _inputPic.img.plane[2] = _yuvData + luma_size + chroma_size;
    _inputPic.i_pts = _i_frame_counter;
    _i_frame_counter++;

    int i_frame_size = 0;
    try
    {
        i_frame_size = x264_encoder_encode(_x264Encoder, &_nal, &_noOfNal, &_inputPic, &_outputPic);
        // std::cout<<i_frame_size;
        *frame_size = i_frame_size;
        if (i_frame_size <= 0)
        {
            throw "No NAL is produced out of encoder.";
        }
    }
    catch (const char *msg)
    {
        throw "ERROR : Encoding failed. " + std::string(msg);
    }

    return _nal->p_payload;
}

x264_t *Encoder::OpenEncoder(int width, int height)
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
        throw "Failed to apply default preset.";
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
        throw "Failed to apply profile.";
    }

    returnValue = x264_picture_alloc(&_inputPic, x264Params.i_csp, x264Params.i_width, x264Params.i_height);
    if (returnValue == 0)
    {
        std::cout << "x264_picture_alloc succeeded." << std::endl;
    }
    else
    {
        throw "x264_picture_alloc Failed.";
    }

    h = x264_encoder_open(&x264Params);

    return h;
}

void Encoder::CleanUp()
{
    std::cout << "Cleanup invoked";
    delete this->_screenCapturer;
    // delete[] this->_yuvData;  // Not necessary as x264_picture_clean clears yuvData
    x264_picture_clean(&this->_inputPic);
    x264_encoder_close(this->_x264Encoder);
    // sws_freeContext(_swsConverter);
}

Encoder::~Encoder()
{
    this->CleanUp();
}
} // namespace remoting
