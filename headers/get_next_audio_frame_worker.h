#ifndef REMOTING_HOST_GETNEXTAUDIOFRAME_WORKER_H_
#define REMOTING_HOST_GETNEXTAUDIOFRAME_WORKER_H_

#include <napi.h>
#include "../headers/audio_encoder.h"

namespace remoting
{
class GetNextAudioFrameWorker : public Napi::AsyncWorker {
    public:
        GetNextAudioFrameWorker(AudioEncoder* encoder, Napi::Function& callback);
        void Execute();
        void OnOK();
    private:
        AudioEncoder* _encoder;
        uint8_t* _next_frame;
        int _frame_size;
};
}

#endif