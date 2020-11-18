#include "../headers/get_next_audio_frame_worker.h"
#include "../headers/audio_encoder.h"

using namespace remoting;

GetNextAudioFrameWorker::GetNextAudioFrameWorker(AudioEncoder* encoder, Napi::Function& callback)
    : Napi::AsyncWorker(callback), _encoder(encoder)
{
}

void GetNextAudioFrameWorker::Execute()
{
    _next_frame = this->_encoder->GetNextFrame(&_frame_size);
}

void GetNextAudioFrameWorker::OnOK()
{
    Callback().Call(Env().Global(), {Napi::ArrayBuffer::New(Env(), _next_frame, _frame_size)});
}