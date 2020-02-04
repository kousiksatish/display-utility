#include "../headers/get_next_frame_worker.h"
#include "../headers/encoder.h"

using namespace remoting;

GetNextFrameWorker::GetNextFrameWorker(Encoder* encoder, bool noChangeCheck, bool getIFrame, Napi::Function& callback)
    : Napi::AsyncWorker(callback), _encoder(encoder), _get_i_frame(getIFrame), _no_change_check(noChangeCheck)
{
}

void GetNextFrameWorker::Execute()
{
    _next_frame = this->_encoder->GetNextFrame(&_frame_size, _no_change_check, _get_i_frame);
}

void GetNextFrameWorker::OnOK()
{
    Callback().Call(Env().Global(), {Napi::ArrayBuffer::New(Env(), _next_frame, _frame_size)});
}