#include "../headers/audio_capture_utility.h"
#include "../headers/get_next_audio_frame_worker.h"
#include <iostream>
using namespace remoting;
Napi::FunctionReference AudioCaptureUtility::constructor;

Napi::Object AudioCaptureUtility::Init(Napi::Env env, Napi::Object exports)
{
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "AudioCaptureUtility", {
        InstanceMethod("init", &AudioCaptureUtility::Init), 
        InstanceMethod("getNextFrame", &AudioCaptureUtility::GetNextFrame)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("AudioCaptureUtility", func);
    return exports;
}

AudioCaptureUtility::AudioCaptureUtility(const Napi::CallbackInfo &info) : Napi::ObjectWrap<AudioCaptureUtility>(info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    this->_encoder = new AudioEncoder();
}

void AudioCaptureUtility::Init(const Napi::CallbackInfo &info)
{
    this->_encoder->init();
    return;
}

void AudioCaptureUtility::GetNextFrame(const Napi::CallbackInfo &info)
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

        GetNextAudioFrameWorker *worker = new GetNextAudioFrameWorker(this->_encoder, cb);
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

AudioCaptureUtility::~AudioCaptureUtility()
{
    delete this->_encoder;
}
