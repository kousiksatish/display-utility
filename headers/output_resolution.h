#ifndef REMOTING_HOST_OUTPUT_RESOLUTION_H_
#define REMOTING_HOST_OUTPUT_RESOLUTION_H_

#include <memory>
// On Linux, we use the xrandr extension to change the desktop resolution
extern "C"
{
#include <X11/extensions/Xrandr.h>
}

namespace remoting
{
class OutputResolution
{
public:
    ~OutputResolution();
    OutputResolution(unsigned int width, unsigned int height, RRMode modeId);
    unsigned int width() const;
    unsigned int height() const;
    RRMode modeId() const;
    bool operator<(const OutputResolution &other) const;

private:
    unsigned int width_;
    unsigned int height_;
    RRMode modeId_;
};
} // namespace remoting
#endif // REMOTING_HOST_OUTPUT_RESOLUTION_H_