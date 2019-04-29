#include <iostream>
#include "../headers/output_resolution.h"

namespace remoting
{
OutputResolution::OutputResolution(unsigned int width, unsigned int height, RRMode modeId) : width_(width), height_(height), modeId_(modeId)
{
    // std::cout << "Creating Output resolution instance" << std::endl;
}
unsigned int OutputResolution::width() const
{
    return width_;
}
unsigned int OutputResolution::height() const
{
    return height_;
}
RRMode OutputResolution::modeId() const
{
    return modeId_;
}
bool OutputResolution::operator<(const OutputResolution &other) const
{
    return width_ * height_ < other.width() * other.height();
}
OutputResolution::~OutputResolution()
{
    // std::cout << "Destroying Output resolution instance" << std::endl;
}
} // namespace remoting