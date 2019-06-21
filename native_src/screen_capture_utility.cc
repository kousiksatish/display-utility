#include "../headers/screen_capture_utility.h"
#include <iostream>
using namespace remoting;
Napi::FunctionReference ScreenCaptureUtility::constructor;

Napi::Object ScreenCaptureUtility::Init(Napi::Env env, Napi::Object exports)
{
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "ScreenCaptureUtility", {
        InstanceMethod("init", &ScreenCaptureUtility::Init),
        InstanceMethod("getNextFrame", &ScreenCaptureUtility::GetNextFrame)        
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("ScreenCaptureUtility", func);
    return exports;
}

ScreenCaptureUtility::ScreenCaptureUtility(const Napi::CallbackInfo& info) : Napi::ObjectWrap<ScreenCaptureUtility> (info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    this->_encoder = new Encoder();
}

void ScreenCaptureUtility::Init(const Napi::CallbackInfo& info) {
    try
    {
        this->_encoder->Init();
    }
    catch(const char* message)
    {
        Napi::Error::New(info.Env(), message).ThrowAsJavaScriptException();
        return;
    }
    catch(std::string message)
    {
        Napi::Error::New(info.Env(), message).ThrowAsJavaScriptException();
        return;
    }
}

Napi::Value ScreenCaptureUtility::GetNextFrame(const Napi::CallbackInfo& info) {
    int frame_size;
    uint8_t* nextFrame;
    try
    {
        nextFrame = this->_encoder->GetNextFrame(&frame_size);
    }
    catch(const char* message)
    {
        Napi::Error::New(info.Env(), message).ThrowAsJavaScriptException();
    }
    catch(std::string message)
    {
        Napi::Error::New(info.Env(), message).ThrowAsJavaScriptException();
    }
    
    return Napi::ArrayBuffer::New(info.Env(), nextFrame, frame_size);   
}

ScreenCaptureUtility::~ScreenCaptureUtility() {
    delete this->_encoder;
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    return ScreenCaptureUtility::Init(env, exports);
}
    
NODE_API_MODULE(addon, InitAll);
