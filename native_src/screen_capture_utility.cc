#include "../headers/screen_capture_utility.h"
#include "../headers/get_next_frame_worker.h"
#include <iostream>
using namespace remoting;
Napi::FunctionReference ScreenCaptureUtility::constructor;

Napi::Object ScreenCaptureUtility::Init(Napi::Env env, Napi::Object exports)
{
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "ScreenCaptureUtility", {
        InstanceMethod("init", &ScreenCaptureUtility::Init), 
        InstanceMethod("getNextFrame", &ScreenCaptureUtility::GetNextFrame),
        InstanceMethod("forceNextFrame", &ScreenCaptureUtility::ForceNextFrame),
        InstanceMethod("sendNextFrameAsIFrame", &ScreenCaptureUtility::SendNextFrameAsIFrame),
        InstanceMethod("setCRFValue", &ScreenCaptureUtility::SetCRFValue)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("ScreenCaptureUtility", func);
    return exports;
}

ScreenCaptureUtility::ScreenCaptureUtility(const Napi::CallbackInfo &info) : Napi::ObjectWrap<ScreenCaptureUtility>(info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    this->_encoder = new Encoder();
}

void ScreenCaptureUtility::Init(const Napi::CallbackInfo &info)
{
    try
    {
        if (info.Length() < 1 || !info[0].IsBoolean())
        {
            throw "Invalid arguments";
        }
        bool singleMonitorCapture = info[0].As<Napi::Boolean>();
        
        if (singleMonitorCapture)
        {
            if (info.Length() < 2 || !info[1].IsNumber())
            {
                throw "Invalid arguments. RROutput required for SingleMonitorCapture";
            }
            else
            {
                unsigned int rROutput = info[1].As<Napi::Number>().Int32Value();
                this->_encoder->Init(singleMonitorCapture, rROutput);
            }
        }
        else
        {
            this->_encoder->Init(singleMonitorCapture);
        }
    }
    catch (const char *message)
    {
        Napi::Error::New(info.Env(), message).ThrowAsJavaScriptException();
        return;
    }
    catch (std::string message)
    {
        Napi::Error::New(info.Env(), message).ThrowAsJavaScriptException();
        return;
    }
}

void ScreenCaptureUtility::GetNextFrame(const Napi::CallbackInfo &info)
{
    try
    {
        int callbackIndex = 0;
        if (info.Length() == 1 && info[0].IsFunction())
        {
            // Callback function in first position
            callbackIndex = 0;
        }
        else
        {
            throw "Wrong parameters provided for getNextFrame";
        }
        Napi::Function cb = info[callbackIndex].As<Napi::Function>();

        GetNextFrameWorker *worker = new GetNextFrameWorker(this->_encoder, cb);
        worker->Queue();

        // cb.Call(env.Global(), {Napi::ArrayBuffer::New(info.Env(), nextFrame, frame_size)});
    }
    catch (const char *message)
    {
        Napi::Error::New(info.Env(), message).ThrowAsJavaScriptException();
    }
    catch (std::string message)
    {
        Napi::Error::New(info.Env(), message).ThrowAsJavaScriptException();
    }
}

void ScreenCaptureUtility::ForceNextFrame(const Napi::CallbackInfo &info)
{
    this->_encoder->SetForceNextFrame();
}

void ScreenCaptureUtility::SendNextFrameAsIFrame(const Napi::CallbackInfo &info)
{
    this->_encoder->SendNextFrameAsIFrame();
}

void ScreenCaptureUtility::SetCRFValue(const Napi::CallbackInfo &info)
{
    try
    {
        if (info.Length() < 1 || !info[0].IsNumber())
        {
            throw "Invalid arguments";
        }
        unsigned int crfValue = info[0].As<Napi::Number>().Int32Value();
        this->_encoder->SetCRFValue(crfValue);
    }
    catch (const char *message)
    {
        Napi::Error::New(info.Env(), message).ThrowAsJavaScriptException();
        return;
    }
}

ScreenCaptureUtility::~ScreenCaptureUtility()
{
    delete this->_encoder;
}
