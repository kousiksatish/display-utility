#include "../headers/base_screen_capturer.h"

namespace remoting
{

BaseScreenCapturer::BaseScreenCapturer()
{
    
}

BaseScreenCapturer::~BaseScreenCapturer()
{
    
}

Display* BaseScreenCapturer::GetDisplay()
{
    return this->_display;
}

Window BaseScreenCapturer::GetWindow()
{
    return this->_window;
}

}
