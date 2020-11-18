#ifndef REMOTING_HOST_AUDIO_CAPTURE_UTILITY_H_
#define REMOTING_HOST_AUDIO_CAPTURE_UTILITY_H_

#include <napi.h>
#include "audio_encoder.h"
#include "get_next_audio_frame_worker.h"

namespace remoting
{
class AudioCaptureUtility : public Napi::ObjectWrap<AudioCaptureUtility>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    AudioCaptureUtility(const Napi::CallbackInfo &info);
    ~AudioCaptureUtility();

private:
    static Napi::FunctionReference constructor;

    void GetNextFrame(const Napi::CallbackInfo &info);
    void Init(const Napi::CallbackInfo &info);

    AudioEncoder *_encoder;
    GetNextAudioFrameWorker *_worker;
};
} // namespace remoting

#endif // REMOTING_HOST_AUDIO_CAPTURE_UTILITY_H_