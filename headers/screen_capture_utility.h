#ifndef REMOTING_HOST_SCREEN_CAPTURE_UTILITY_H_
#define REMOTING_HOST_SCREEN_CAPTURE_UTILITY_H_

#include <napi.h>
#include "encoder.h"
#include "get_next_frame_worker.h"

namespace remoting
{
class ScreenCaptureUtility : public Napi::ObjectWrap<ScreenCaptureUtility>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    ScreenCaptureUtility(const Napi::CallbackInfo &info);
    ~ScreenCaptureUtility();

private:
    static Napi::FunctionReference constructor;

    void GetNextFrame(const Napi::CallbackInfo &info);
    void Init(const Napi::CallbackInfo &info);
    void ForceNextFrame(const Napi::CallbackInfo &info);
    void SendNextFrameAsIFrame(const Napi::CallbackInfo &info);
    void SetCRFValue(const Napi::CallbackInfo &info);

    Encoder *_encoder;
    GetNextFrameWorker *_worker;
};
} // namespace remoting

#endif // REMOTING_HOST_SCREEN_CAPTURE_UTILITY_H_