#include <iostream>
#include "../headers/output_resolution_with_offset.h"

namespace remoting
{
OutputResolutionWithOffset::OutputResolutionWithOffset(unsigned int width, unsigned int height, RRMode modeId, int offsetX, int offsetY, RROutput rrOutput)
    : OutputResolution(width, height, modeId), offsetX_(offsetX), offsetY_(offsetY), rrOutput_(rrOutput)
{
    // std::cout << "Constructor Output resolution with offset instance" << std::endl;
}
int OutputResolutionWithOffset::offsetX() const
{
    return offsetX_;
}
int OutputResolutionWithOffset::offsetY() const
{
    return offsetY_;
}
RROutput OutputResolutionWithOffset::rrOutput() const
{
    return rrOutput_;
}
OutputResolutionWithOffset::~OutputResolutionWithOffset()
{
    // std::cout << "Destroying Output resolution instance" << std::endl;
}
} // namespace remoting