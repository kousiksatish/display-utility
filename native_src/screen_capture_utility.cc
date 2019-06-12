#include "../headers/screen_capture_utility.h"
using namespace remoting;
Napi::FunctionReference ScreenCaptureUtility::constructor;

Napi::Object ScreenCaptureUtility::Init(Napi::Env env, Napi::Object exports)
{
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "ScreenCaptureUtility", {
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

Napi::Value ScreenCaptureUtility::GetNextFrame(const Napi::CallbackInfo& info) {
    int frame_size;
    uint8_t* nextFrame = this->_encoder->GetNextFrame(&frame_size);
    return Napi::ArrayBuffer::New(info.Env(), nextFrame, frame_size);
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    return ScreenCaptureUtility::Init(env, exports);
}
    
NODE_API_MODULE(addon, InitAll);
