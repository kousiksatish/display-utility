#include "../headers/screen_resources.h"

namespace remoting
{
ScreenResources::ScreenResources() : resources_(nullptr)
{
}

ScreenResources::~ScreenResources()
{
    Release();
}

bool ScreenResources::Refresh(Display *display, Window window)
{
    Release();
    resources_ = XRRGetScreenResources(display, window);
    return resources_ != nullptr;
}

void ScreenResources::Release()
{
    if (resources_)
    {
        XRRFreeScreenResources(resources_);
        resources_ = nullptr;
    }
}

OutputResolution * ScreenResources::GetResolutionUsingModeId(const RRMode modeId, const Rotation rotation)
{
    int width = 0;
    int height = 0;
    OutputResolution *resolution = nullptr;
    if (resources_ == nullptr)
        return resolution;
    for (int i = 0; i < resources_->nmode; ++i)
    {
        const XRRModeInfo &mode = resources_->modes[i];

        if (modeId == mode.id)
        {
            switch (rotation & 0xf)
            {
            case RR_Rotate_90:
            case RR_Rotate_270:
                width = mode.height;
                height = mode.width;
                break;
            case RR_Rotate_0:
            case RR_Rotate_180:
            default:
                width = mode.width;
                height = mode.height;
            }
            resolution = new OutputResolution(width, height, mode.id);
            break;
        }
    }
    return resolution;
}

bool ScreenResources::TryGetOutput(const unsigned int outputIndex, RROutput *output)
{
    if (resources_ == nullptr)
        return false;
    *output = resources_->outputs[outputIndex];
    return true;
}

bool ScreenResources::TryGetCrtc(const unsigned int crtcIndex, RRCrtc *crtc)
{
    if (resources_ == nullptr)
        return false;
    *crtc = resources_->crtcs[crtcIndex];
    return true;
}

XRROutputInfo *ScreenResources::GetOutputInfo(Display *display, const RROutput output_id)
{
    if (resources_ == nullptr)
        return nullptr;
    return XRRGetOutputInfo(display, resources_, output_id);
}

XRRScreenResources *ScreenResources::get() { return resources_; }
} // namespace remoting