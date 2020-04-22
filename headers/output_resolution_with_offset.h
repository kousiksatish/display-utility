#ifndef REMOTING_HOST_OUTPUT_RESOLUTION_WITH_OFFSET_H_
#define REMOTING_HOST_OUTPUT_RESOLUTION_WITH_OFFSET_H_

#include <memory>
#include "output_resolution.h"
// On Linux, we use the xrandr extension to change the desktop resolution
extern "C"
{
#include <X11/extensions/Xrandr.h>
}

namespace remoting
{
class OutputResolutionWithOffset : public OutputResolution
{
public:
    ~OutputResolutionWithOffset();
    OutputResolutionWithOffset(unsigned int width, unsigned int height, RRMode modeId, int offsetX, int offsetY, RROutput rrOutput);
    int offsetX() const;
    int offsetY() const;
    RROutput rrOutput() const;
private:
    int offsetX_;
    int offsetY_;
    RROutput rrOutput_;
};
} // namespace remoting

#endif // REMOTING_HOST_OUTPUT_RESOLUTION_WITH_OFFSET_H_