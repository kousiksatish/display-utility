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
    bool TryGetNumberOfOutputs(unsigned int *numberOfOutputs);
    std::unique_ptr<OutputResolution> GetCurrentResolution(const unsigned int outputIndex);
    std::set<OutputResolution> GetResolutions(const unsigned int outputIndex);
    std::string GetOutputName(const unsigned int outputIndex);

private:
    Display *display_;
    int screen_;
    Window root_;
    ScreenResources resources_;
    bool has_randr_;


    DisplayUtilityX11();

    DISALLOW_COPY_AND_ASSIGN(DisplayUtilityX11);
};
} // namespace remoting

#endif // REMOTING_HOST_DESKTOP_INFO_X11_H_