#ifndef REMOTING_HOST_GETNEXTFRAME_WORKER_H_
#define REMOTING_HOST_GETNEXTFRAME_WORKER_H_

#include <napi.h>
#include "../headers/encoder.h"

namespace remoting
{
class GetNextFrameWorker : public Napi::AsyncWorker {
    public:
        GetNextFrameWorker(Encoder* encoder, bool getIFrame, Napi::Function& callback);
        void Execute();
        void OnOK();
    private:
        Encoder* _encoder;
        uint8_t* _next_frame;
        int _frame_size;
        bool _get_i_frame;
};
}

#endif