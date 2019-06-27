#ifndef REMOTING_HOST_DISPLAY_UTILITY_X11_H_
#define REMOTING_HOST_DISPLAY_UTILITY_X11_H_
#include <set>
#include "base_macros.h"
#include "screen_resources.h"

namespace remoting
{
class DisplayUtilityX11
{
public:
    static std::unique_ptr<DisplayUtilityX11> Create();
    ~DisplayUtilityX11();

    // DesktopResizer interface
    bool TryGetConnectedOutputs(unsigned int *numberOfOutputs, RROutput **connectedOutputs);
    std::unique_ptr<OutputResolutionWithOffset> GetCurrentResolution(RROutput rROutput);
    std::set<OutputResolution> GetResolutions(RROutput rROutput);
    std::string GetOutputName(RROutput rROutput);
    RROutput GetPrimaryRROutput();
    std::unique_ptr<OutputResolution> GetExtendedMonitorResolution();
    std::set<OutputResolutionWithOffset> GetAllCurrentResolutions();

private:
    Display *display_;
    int screen_;
    Window root_;
    ScreenResources resources_;
    bool has_randr_;

    static int handler(Display *d, XErrorEvent *e);

    DisplayUtilityX11();

    DISALLOW_COPY_AND_ASSIGN(DisplayUtilityX11);
};
} // namespace remoting

#endif // REMOTING_HOST_DESKTOP_INFO_X11_H_