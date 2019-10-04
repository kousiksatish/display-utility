#include <iostream>
#include <memory>
#include "../headers/display_utility_x11.h"
#include "../headers/x11_util.h"

namespace remoting
{

int DisplayUtilityX11::handler(Display *d, XErrorEvent *e)
{
    int length = 1000;
    char errorText[length];
    XGetErrorText(d, e->error_code, errorText, length);
    std::cerr << "XLib Error: " << int(e->error_code) 
            << " - " << errorText
            << std::endl;
    return 0;
}

DisplayUtilityX11::DisplayUtilityX11()
    : display_(XOpenDisplay(nullptr)),
      screen_(DefaultScreen(display_)),
      root_(XRootWindow(display_, screen_))
{
    int rr_event_base;
    int rr_error_base;

    has_randr_ = XRRQueryExtension(display_, &rr_event_base, &rr_error_base);

    XRRSelectInput(display_, root_, RRScreenChangeNotifyMask);

    // Set handler for XErrors
    XSetErrorHandler(DisplayUtilityX11::handler);
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
        RROutput primaryRROutput = XRRGetOutputPrimary(display_, root_);
        int primaryOutputIndex = 0;
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
                // Only consider if primary RROutput is in connected state
                if (currentRROutput == primaryRROutput)
                {
                    primaryOutputIndex = outputIndex;
                }
            }
            XRRFreeOutputInfo(outputInfo);
        }
        
        // If primary output not in first index
        if (primaryOutputIndex != 0)
        {
            // Swap primary output to first index
            RROutput outputAtFirstIndex = tmpOutputs[0];
            tmpOutputs[0] = tmpOutputs[primaryOutputIndex];
            tmpOutputs[primaryOutputIndex] = outputAtFirstIndex;
        }

        *numberOfOutputs = numberOfOutputsConnected;
        *connectedOutputs = tmpOutputs;
        return true;
    }
    return false;
}

std::unique_ptr<OutputResolutionWithOffset> DisplayUtilityX11::GetCurrentResolution(RROutput rROutput)
{
    int height = 0;
    int width = 0;
    int offsetX = 0;
    int offsetY = 0;
    std::unique_ptr<OutputResolutionWithOffset> currentResolution = nullptr;
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
            offsetX = crtc->y;
            offsetY = crtc->x;
            break;
        case RR_Rotate_0:
        case RR_Rotate_180:
        default:
            width = crtc->width;
            height = crtc->height;
            offsetX = crtc->x;
            offsetY = crtc->y;
        }
        currentResolution = std::unique_ptr<OutputResolutionWithOffset>(new OutputResolutionWithOffset(width, height, crtc->mode, offsetX, offsetY, rROutput));
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
    if (outputInfo != nullptr && outputInfo->crtc)
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
            std::cout<<outputInfo->mm_height<<"x"<<outputInfo->mm_width;
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

std::set<OutputResolutionWithOffset> DisplayUtilityX11::GetAllCurrentResolutions()
{
    unsigned int numberOfOutputs = 0;
    RROutput *connectedOutputs = nullptr;

    std::set<OutputResolutionWithOffset> currentResolutionsSet;

    if (this->TryGetConnectedOutputs(&numberOfOutputs, &connectedOutputs))
    {
        if (connectedOutputs != nullptr)
        {
            std::cout << "There are " << numberOfOutputs << " outputs connected to this desktop." << std::endl;

            for (unsigned int i = 0; i < numberOfOutputs; i += 1)
            {
                // Get current resolution with offset for each output
                std::unique_ptr<OutputResolutionWithOffset> resolutionWithOffset = this->GetCurrentResolution(connectedOutputs[i]);
                OutputResolutionWithOffset* ptr = resolutionWithOffset.release();
                currentResolutionsSet.insert(*ptr);
                delete ptr;
            }
        }
    }
    return currentResolutionsSet;
}

std::string DisplayUtilityX11::GetOutputName(RROutput rROutput)
{
    std::string outputName;
    if (resources_.Refresh(display_, root_) == false)
    {
        return outputName;
    }
    XRROutputInfo *outputInfo = resources_.GetOutputInfo(display_, rROutput);
    if (outputInfo != nullptr)
    {
        outputName = std::string(outputInfo->name);
    }
    XRRFreeOutputInfo(outputInfo);
    return outputName;
}

RROutput DisplayUtilityX11::GetPrimaryRROutput()
{
    RROutput primaryRROutput = 0;
    
    unsigned int numberOfOutputs = 0;
    RROutput *connectedOutputs = nullptr;

    // Primary, connected output will always be in the first position on this list if available
    // If primary connected output is not available, one of the connected output will be in the first position and will be returned
    // If no connected output is available, 0 will be returned
    if (this->TryGetConnectedOutputs(&numberOfOutputs, &connectedOutputs))
    {
        if (numberOfOutputs > 0) {
            primaryRROutput = connectedOutputs[0];
        }
    }

    return primaryRROutput;
}

std::unique_ptr<OutputResolution> DisplayUtilityX11::GetExtendedMonitorResolution()
{
    std::unique_ptr<OutputResolution> extendedResolution = nullptr;

    // Get width and height of the complete window
    XWindowAttributes attributes;
    XGetWindowAttributes(display_, root_, &attributes);

    // Mode is set as 0 as it is not applicable
    extendedResolution = std::unique_ptr<OutputResolution>(new OutputResolution(attributes.width, attributes.height, 0));

    return extendedResolution;
}

} // namespace remoting