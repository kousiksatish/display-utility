#ifndef REMOTING_HOST_SCREEN_RESOURCES_H_
#define REMOTING_HOST_SCREEN_RESOURCES_H_

extern "C"
{
// Xlib.h defines base types so it must be included before the less
// central X11 headers can be included.
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
}
#include "output_resolution.h"
#include "output_resolution_with_offset.h"

namespace remoting
{
// Wrapper class for the XRRScreenResources struct.
class ScreenResources
{
public:
    ScreenResources();
    ~ScreenResources();

    bool Refresh(Display *display, Window window);
    void Release();

    OutputResolution *GetResolutionUsingModeId(const RRMode modeId, const Rotation rotation);

    bool TryGetOutput(const unsigned int outputIndex, RROutput *output);
    bool TryGetCrtc(const unsigned int crtcIndex, RRCrtc *crtc);
    XRROutputInfo *GetOutputInfo(Display *display, const RROutput output_id);
    XRRScreenResources *get();

private:
    XRRScreenResources *resources_;
};
} // namespace remoting

#endif //REMOTING_HOST_SCREEN_RESOURCES_H_