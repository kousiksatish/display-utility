#include <iostream>
#include <memory>
#include "../headers/display_utility_x11.h"
#include "../headers/x11_util.h"

namespace remoting
{

DisplayUtilityX11::DisplayUtilityX11()
    : display_(XOpenDisplay(nullptr)),
      screen_(DefaultScreen(display_)),
      root_(XRootWindow(display_, screen_))
{
    int rr_event_base;
    int rr_error_base;

    has_randr_ = XRRQueryExtension(display_, &rr_event_base, &rr_error_base);

    XRRSelectInput(display_, root_, RRScreenChangeNotifyMask);
}

DisplayUtilityX11::~DisplayUtilityX11()
{
    // std::cout << "Destroying an instance of desktop resizer x11" << std::endl;
    XCloseDisplay(display_);
}

std::unique_ptr<DisplayUtilityX11> DisplayUtilityX11::Create()
{
    // std::cout << "Creating an instance of desktop resizer x11" << std::endl;
    return std::unique_ptr<DisplayUtilityX11>(new DisplayUtilityX11());
}

bool DisplayUtilityX11::TryGetConnectedOutputs(unsigned int *numberOfOutputs, RROutput **connectedOutputs)
{
    unsigned int numberOfOutputsConnected = 0;
    if (resources_.Refresh(display_, root_))
    {
        RROutput * tmpOutputs = new RROutput[resources_.get()->noutput];
        RROutput currentRROutput;
        for (int outputIndex = 0; outputIndex < resources_.get()->noutput; outputIndex += 1)
        {
            if (resources_.TryGetOutput(outputIndex, &currentRROutput) == false)
            {
                std::cout << "Could not get output id of index: " << outputIndex << std::endl;
                return false;
            }
            XRROutputInfo *outputInfo = resources_.GetOutputInfo(display_, currentRROutput);
            if (outputInfo == nullptr)
            {
                std::cout << "Could not get output info of index: " << outputIndex << std::endl;
                return false;
            }
            if (outputInfo->connection == 0)
            {
                tmpOutputs[numberOfOutputsConnected++] = currentRROutput;
            }
            XRRFreeOutputInfo(outputInfo);
        }
        *numberOfOutputs = numberOfOutputsConnected;
        *connectedOutputs = tmpOutputs;
        return true;
    }
    return false;
}

std::unique_ptr<OutputResolution> DisplayUtilityX11::GetCurrentResolution(RROutput rROutput)
{
    int height = 0;
    int width = 0;
    std::unique_ptr<OutputResolution> currentResolution = nullptr;
    if (resources_.Refresh(display_, root_) == false)
    {
        return currentResolution;
    }
    XRROutputInfo *outputInfo = resources_.GetOutputInfo(display_, rROutput);
    if (outputInfo != nullptr && outputInfo->crtc)
    {
        XRRCrtcInfo *crtc;
        crtc = XRRGetCrtcInfo(display_, resources_.get(), outputInfo->crtc);
        if (crtc == nullptr)
        {
            return currentResolution;
        }
        switch (crtc->rotation & 0xf)
        {
        case RR_Rotate_90:
        case RR_Rotate_270:
            width = crtc->height;
            height = crtc->width;
            break;
        case RR_Rotate_0:
        case RR_Rotate_180:
        default:
            width = crtc->width;
            height = crtc->height;
        }
        currentResolution = std::unique_ptr<OutputResolution>(new OutputResolution(width, height, crtc->mode));
        XRRFreeCrtcInfo(crtc);
    }
    XRRFreeOutputInfo(outputInfo);
    return currentResolution;
}

std::set<OutputResolution> DisplayUtilityX11::GetResolutions(RROutput rROutput)
{
    std::set<OutputResolution> resolutionsSet;
    resources_.Refresh(display_, root_);
    // Impose a minimum size of 640x480, since anything smaller
    // doesn't seem very useful.
    OutputResolution *minimumDesktopResolution = new OutputResolution(640, 480, 0L);
    XRROutputInfo *outputInfo = resources_.GetOutputInfo(display_, rROutput);
    if (outputInfo->crtc)
    {
        XRRCrtcInfo *crtc;
        crtc = XRRGetCrtcInfo(display_, resources_.get(), outputInfo->crtc);

        if (!crtc)
        {
            std::cout << "could not get the crtc info" << std::endl;
            return resolutionsSet;
        }
        XRRFreeCrtcInfo(crtc);
        for (int i = 0; i < outputInfo->nmode; i++)
        {
            OutputResolution *resolution = resources_.GetResolutionUsingModeId(outputInfo->modes[i], crtc->rotation);
            if (resolution != nullptr && !(*resolution < *minimumDesktopResolution))
            {
                resolutionsSet.insert(*resolution);
            }
            delete resolution;
        }
    }
    delete minimumDesktopResolution;

    XRRFreeOutputInfo(outputInfo);
    return resolutionsSet;
}

std::string DisplayUtilityX11::GetOutputName(RROutput rROutput)
{
    std::string outputName;
    if (resources_.Refresh(display_, root_) == false)
    {
        return outputName;
    }
    XRROutputInfo *outputInfo = resources_.GetOutputInfo(display_, rROutput);
    outputName = std::string(outputInfo->name);
    XRRFreeOutputInfo(outputInfo);
    return outputName;
}

RROutput DisplayUtilityX11::GetPrimaryOutputIndex()
{
    return XRRGetOutputPrimary(display_, root_);
}

} // namespace remoting