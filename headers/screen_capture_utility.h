#ifndef REMOTING_HOST_SCREEN_CAPTURE_UTILITY_H_
#define REMOTING_HOST_SCREEN_CAPTURE_UTILITY_H_

#include <napi.h>
#include "encoder.h"

namespace remoting
{
    class ScreenCaptureUtility : public Napi::ObjectWrap<ScreenCaptureUtility>
    {
        public:
            static Napi::Object Init(Napi::Env env, Napi::Object exports);
            ScreenCaptureUtility(const Napi::CallbackInfo& info);
            ~ScreenCaptureUtility();
        private:
            static Napi::FunctionReference constructor;
            
            Napi::Value GetNextFrame(const Napi::CallbackInfo& info);
            void Init(const Napi::CallbackInfo& info);

            Encoder* _encoder;
    };
}



#endif // REMOTING_HOST_SCREEN_CAPTURE_UTILITY_H_