#include <iostream>
#include "../headers/encoder.h"
#include <unistd.h>

namespace remoting
{
Encoder::Encoder()
{
    _isInitialised = false;
    _use_xdamage = false;
}
void Encoder::Init(bool singleMonitorCapture, RROutput rROutput)
{
    XInitThreads();
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
    // _rgbPlanes[0] = _rgbData;
    // _rgbPlanes[1] = NULL;
    // _rgbPlanes[2] = NULL;
    // _rgbStride[0] = 4 * _width;
    // _rgbStride[1] = 0;
    // _rgbStride[2] = 0;

    // YUV output information
    _yuvData = new uint8_t[3 * _width * _height / 2];
    // _yuvPlanes[0] = _yuvData;
    // _yuvPlanes[1] = _yuvData + _width * _height;
    // _yuvPlanes[2] = _yuvData + _width * _height + _width * _height / 4;
    // _yuvStride[0] = _width;
    // _yuvStride[1] = _width / 2;
    // _yuvStride[2] = _width / 2;

    // Previous frame YUV information
    _prevYUVData = new uint8_t[3 * _width * _height / 2];

    for(int i=0; i<3*_width*_height/2;i++) {
        _prevYUVData[i] = 0;
    }

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

    InitXDamage();

    _isInitialised = true;
}

bool Bitmap2Yuv420p_calc2(uint8_t *destination, uint8_t *rgb, uint8_t *prevYUV, size_t width, size_t height)
{
    bool isFrameDifferent = false;
    size_t image_size = width * height;
    size_t upos = image_size;
    size_t vpos = upos + upos / 4;
    size_t i = 0;
    uint8_t yValue;
    for (size_t line = 0; line < height; ++line)
    {

        if (!(line % 2))
        {
            for (size_t x = 0; x < width; x += 2)
            {
                uint8_t b = rgb[4 * i];
                uint8_t g = rgb[4 * i + 1];
                uint8_t r = rgb[4 * i + 2];

                yValue = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
                if (!isFrameDifferent) {
                    if (yValue != prevYUV[i]) {
                        isFrameDifferent = true;        
                    }
                }
                destination[i++] = yValue;

                destination[upos++] = ((-38 * r + -74 * g + 112 * b) >> 8) + 128;
                destination[vpos++] = ((112 * r + -94 * g + -18 * b) >> 8) + 128;

                b = rgb[4 * i];
                g = rgb[4 * i + 1];
                r = rgb[4 * i + 2];

                yValue = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
                if (!isFrameDifferent) {
                    if (yValue != prevYUV[i]) {
                        isFrameDifferent = true;        
                    }
                }
                destination[i++] = yValue;
            }
        }
        else
        {
            for (size_t x = 0; x < width; x += 1)
            {
                uint8_t b = rgb[4 * i];
                uint8_t g = rgb[4 * i + 1];
                uint8_t r = rgb[4 * i + 2];

                yValue = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
                if (!isFrameDifferent) {
                    
                    if (yValue != prevYUV[i]) {
                        isFrameDifferent = true;        
                    }
                }
                destination[i++] = yValue;
            }
        }
    }

    return isFrameDifferent;
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
uint8_t *Encoder::GetNextFrame(int *frame_size, bool getIFrame)
{
    _force_callback = false;
    if (!_isInitialised)
    {
        throw "ERROR: ScreenCaptureUtility not initialised before use.";
    }

    int W = _width;
    int H = _height;


    // int returnValue = sws_scale(_swsConverter, _rgbPlanes, _rgbStride, 0, H, _yuvPlanes, _yuvStride);
    // if (returnValue == H)
    // {
    //     std::cout << "Converted the color space of the image." << std::endl;
    // }
    // else
    // {
    //     throw("Failed to convert the color space of the image.");
    // }
    bool isFrameDifferent = false;
    while (!isFrameDifferent) {
        try
        {
            _screenCapturer->CaptureScreen();
        }
        catch (const char *msg)
        {
            throw "ERROR: Screen capture of next frame failed. " + std::string(msg);
        }
        try
        {
            isFrameDifferent = Bitmap2Yuv420p_calc2(_yuvData, _rgbData, _prevYUVData, W, H);
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
        if (getIFrame) {
            // Set to force an iFrame
            _inputPic.i_type = X264_TYPE_IDR;
        } else {
            // Set to get back to normal encodings
            _inputPic.i_type = X264_TYPE_AUTO;
        }
        _i_frame_counter++;

        if (_i_frame_counter < 5 || _force_callback) {
            isFrameDifferent = true;   
        }
        
        int i_frame_size = 0;
        try
        {
            if (isFrameDifferent) {
                // std::cout<<"Diff frame";
                i_frame_size = x264_encoder_encode(_x264Encoder, &_nal, &_noOfNal, &_inputPic, &_outputPic);
                // std::cout<<i_frame_size;
                *frame_size = i_frame_size;
                

                if (i_frame_size <= 0)
                {
                    throw "No NAL is produced out of encoder.";
                }

                if (!_force_callback) {
                    uint8_t* temp = _prevYUVData;
                    _prevYUVData = _yuvData;
                    _yuvData = temp;    
                } else {
                    // Refresh prevYUVData in case of forced callback (For viewer minimized case)
                    for(int i=0; i<3*_width*_height/2;i++) {
                        _prevYUVData[i] = 0;
                    }
                }
                
                
            } else {
                // std::cout<<"Same frame";
                usleep(30 * 1000);
            }
        }
        catch (const char *msg)
        {
            throw "ERROR : Encoding failed. " + std::string(msg);
        }
    }
    

   

    return _nal->p_payload;
}

x264_t *Encoder::OpenEncoder(int width, int height)
{
    x264_param_t x264Params;
    x264_t *h;

    int returnValue = x264_param_default_preset(&x264Params, x264_preset_names[0], x264_tune_names[7]);
    if (returnValue == 0)
    {
        std::cout << x264_preset_names[0] << " preset is applied and " << x264_tune_names[7] << " tune is applied." << std::endl;
    }
    else
    {
        throw "Failed to apply default preset.";
    }

    /* Configure non-default params */
    // x264Params.i_bitdepth = 8;
    x264Params.i_csp = X264_CSP_I420;
    // Width and height should be even for encoder. Setting to next even number
    x264Params.i_width = width + (width % 2);
    x264Params.i_height = height + (height % 2);
    // x264Params.b_vfr_input = 0;
    x264Params.b_repeat_headers = 1;
    x264Params.b_annexb = 1;
    
    x264Params.i_keyint_max = INT32_MAX;
    // x264Params.i_keyint_min = INT32_MAX;
    // x264Params.i_avcintra_class

    int crfValue = 25;
    x264Params.rc.f_rf_constant = crfValue;
    std::cout<<"CRF set as "<<crfValue;

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
    if (_damage_handle)
        XDamageDestroy(this->_screenCapturer->GetDisplay(), _damage_handle);
    std::cout << "Cleanup invoked";
    delete this->_screenCapturer;
    // delete[] this->_yuvData;  // Not necessary as x264_picture_clean clears yuvData
    x264_picture_clean(&this->_inputPic);
    x264_encoder_close(this->_x264Encoder);
    // sws_freeContext(_swsConverter);
}

void Encoder::SetForceCallback()
{
    std::cout<<"Forcing callback";
    this->_force_callback = true;
}

void Encoder::InitXDamage()
{
    // Check for XDamage extension.
    if (!XDamageQueryExtension(_screenCapturer->GetDisplay(), &_damage_event_base,
                                &_damage_error_base))
    {
        std::cout << "X server does not support XDamage." << std::endl;
        return;
    }
    // TODO(lambroslambrou): Disable DAMAGE in situations where it is known
    // to fail, such as when Desktop Effects are enabled, with graphics
    // drivers (nVidia, ATI) that fail to report DAMAGE notifications
    // properly.
    // Request notifications every time the screen becomes damaged.
    _damage_handle = XDamageCreate(_screenCapturer->GetDisplay(), _screenCapturer->GetWindow(),
                                    XDamageReportNonEmpty);
    if (!_damage_handle)
    {
        std::cout << "Unable to initialize XDamage." << std::endl;
        return;
    }

    _use_xdamage = true;
    std::cout << "Using XDamage extension." << std::endl;
}

Encoder::~Encoder()
{
    this->CleanUp();
}
} // namespace remoting
