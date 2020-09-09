#include <napi.h>
#include <iostream>
#include "../headers/display_utility_x11.h"
#include "../headers/screen_capture_utility.h"
using namespace remoting;

Napi::Array GetConnectedOutputs(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::Array connectedOutputArray;

    std::unique_ptr<DisplayUtilityX11> desktopInfo = DisplayUtilityX11::Create();
    unsigned int numberOfOutputs = 0;
    RROutput *connectedOutputs = nullptr;
    if (desktopInfo->TryGetConnectedOutputs(&numberOfOutputs, &connectedOutputs))
    {
        if (connectedOutputs != nullptr)
        {
            std::cout << "There are " << numberOfOutputs << " outputs connected to this desktop." << std::endl;
            connectedOutputArray = Napi::Array::New(env);
            for (unsigned int i = 0; i < numberOfOutputs; i += 1)
            {
                connectedOutputArray.Set(i, Napi::Number::New(env, connectedOutputs[i]));
            }
            delete[] connectedOutputs;
        }
        return connectedOutputArray;
    }
    Napi::Error::New(env, "Could not get the number of outputs of the display. Please try again.");
    return connectedOutputArray;
}

Napi::String GetOutputName(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::String outputName;

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return outputName;
    }

    if (!info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
        return outputName;
    }

    unsigned int rROutput = info[0].As<Napi::Number>().Int32Value();

    std::unique_ptr<DisplayUtilityX11> desktopInfo = DisplayUtilityX11::Create();

    return Napi::String::New(env, desktopInfo->GetOutputName(rROutput));
}

Napi::Object GetCurrentResolution(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::Object currentResolution;

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return currentResolution;
    }

    if (!info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
        return currentResolution;
    }

    unsigned int rROutput = info[0].As<Napi::Number>().Int32Value();
    std::unique_ptr<DisplayUtilityX11> desktopInfo = DisplayUtilityX11::Create();
    std::unique_ptr<OutputResolutionWithOffset> resolution = desktopInfo->GetCurrentResolution(rROutput);
    if (resolution != nullptr)
    {
        std::cout << "current Resolution : " << resolution->width() << "x" << resolution->height() << std::endl;
        std::cout << "current offset : " << resolution->offsetX() << "x" << resolution->offsetY() << std::endl;
        currentResolution = Napi::Object::New(env);
        currentResolution.Set(Napi::String::New(env, "width"), Napi::Number::New(env, resolution->width()));
        currentResolution.Set(Napi::String::New(env, "height"), Napi::Number::New(env, resolution->height()));
    }
    else
    {
        Napi::Error::New(env, "Could not get the current resolution of the output. Please Try again.");
    }
    return currentResolution;
}

Napi::Array GetResolutions(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::Array resolutionArray;

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return resolutionArray;
    }

    if (!info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
        return resolutionArray;
    }

    unsigned int rROutput = info[0].As<Napi::Number>().Int32Value();
    std::unique_ptr<DisplayUtilityX11> desktopInfo = DisplayUtilityX11::Create();
    std::set<OutputResolution> resolutionsSet = desktopInfo->GetResolutions(rROutput);
    if (resolutionsSet.size() > 0)
    {
        resolutionArray = Napi::Array::New(env);
    }
    int i = 0;
    for (const OutputResolution &resolution : resolutionsSet)
    {
        std::cout << resolution.modeId() << " : " << resolution.width() << " x " << resolution.height() << std::endl;
        Napi::Object currentResolution = Napi::Object::New(env);
        currentResolution.Set(Napi::String::New(env, "width"), Napi::Number::New(env, resolution.width()));
        currentResolution.Set(Napi::String::New(env, "height"), Napi::Number::New(env, resolution.height()));
        resolutionArray.Set(i++, currentResolution);
    }
    return resolutionArray;
}

void SetResolution(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsNumber() || !info[1].IsObject() || !info[1].ToObject().Has("width") || !info[1].ToObject().Has("height"))
    {
        Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
        return;
    }

    unsigned int rROutput = info[0].As<Napi::Number>().Int32Value();
    std::unique_ptr<DisplayUtilityX11> desktopInfo = DisplayUtilityX11::Create();
    
    std::string outputName = desktopInfo->GetOutputName(rROutput);
    std::string width = info[1].ToObject().Get("width").ToString();
    std::string height = info[1].ToObject().Get("height").ToString();
    std::string resolution = width + "x" + height;

    std::string setResolutionCommnad = "xrandr --output " + outputName + " --mode " + resolution;
    int return_value = system(setResolutionCommnad.c_str());
    std::cout << "The value returned by command " << setResolutionCommnad << " was: " << return_value << std::endl;
    return;
}

void MakeScreenBlank(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    std::unique_ptr<DisplayUtilityX11> desktopInfo = DisplayUtilityX11::Create();
    unsigned int numberOfOutputs = 0;
    RROutput *connectedOutputs = nullptr;
    if (desktopInfo->TryGetConnectedOutputs(&numberOfOutputs, &connectedOutputs))
    {
        if (connectedOutputs != nullptr)
        {
            std::string makeScreenBlankCommand = "xrandr";
            for (unsigned int i = 0; i < numberOfOutputs; i++)
            {
                makeScreenBlankCommand += " --output " + desktopInfo->GetOutputName(*connectedOutputs) + " --brightness 0";
                connectedOutputs++;
            }
            int returnValue = system(makeScreenBlankCommand.c_str());
            std::cout << "The value returned by command " << makeScreenBlankCommand << " was: " << returnValue << std::endl;
            return;
        }
    }
    Napi::Error::New(env, "Could not make the screen blank. Please try again.");
    return;
}

void ReverseBlankScreen(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    std::unique_ptr<DisplayUtilityX11> desktopInfo = DisplayUtilityX11::Create();
    unsigned int numberOfOutputs = 0;
    RROutput *connectedOutputs = nullptr;
    if (desktopInfo->TryGetConnectedOutputs(&numberOfOutputs, &connectedOutputs))
    {
        if (connectedOutputs != nullptr)
        {
            std::string reverseBlankScreenCommand = "xrandr";
            for (unsigned int i = 0; i < numberOfOutputs; i++)
            {
                reverseBlankScreenCommand += " --output " + desktopInfo->GetOutputName(*connectedOutputs) + " --brightness 1";
                connectedOutputs++;
            }
            int returnValue = system(reverseBlankScreenCommand.c_str());
            std::cout << "The value returned by command " << reverseBlankScreenCommand << " was: " << returnValue << std::endl;
            return;
        }
    }
    Napi::Error::New(env, "Could not reverse the screen blank. Please try again.");
    return;
}

Napi::Number GetPrimaryRROutput(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    std::unique_ptr<DisplayUtilityX11> desktopInfo = DisplayUtilityX11::Create();
    RROutput outputIndex = desktopInfo->GetPrimaryRROutput();

    return Napi::Number::New(env, outputIndex);
}

Napi::Object GetExtendedMonitorResolution(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::Object extendedResolution;

    std::unique_ptr<DisplayUtilityX11> desktopInfo = DisplayUtilityX11::Create();
    std::unique_ptr<OutputResolution> resolution = desktopInfo->GetExtendedMonitorResolution();
    if (resolution != nullptr)
    {
        // std::cout << "extended Resolution : " << resolution->width() << "x" << resolution->height() << std::endl;
        extendedResolution = Napi::Object::New(env);
        extendedResolution.Set(Napi::String::New(env, "width"), Napi::Number::New(env, resolution->width()));
        extendedResolution.Set(Napi::String::New(env, "height"), Napi::Number::New(env, resolution->height()));
    }
    else
    {
        Napi::Error::New(env, "Could not get the current resolution of the output. Please Try again.");
    }
    return extendedResolution;
}

Napi::Object GetAllCurrentResolutions(const Napi::CallbackInfo &info)
{

    Napi::Env env = info.Env();
    Napi::Array currentResolutionsArray;

    std::unique_ptr<DisplayUtilityX11> desktopInfo = DisplayUtilityX11::Create();
    std::vector<OutputResolutionWithOffset> currentResolutions = desktopInfo->GetAllCurrentResolutions();
    
    currentResolutionsArray = Napi::Array::New(env);
    int i = 0;
    for (auto resolutionWithOffset : currentResolutions)
    {
            Napi::Object outputWithResolution = Napi::Object::New(env);
            outputWithResolution.Set("rrOutput", resolutionWithOffset.rrOutput());
            outputWithResolution.Set("offsetX", resolutionWithOffset.offsetX());
            outputWithResolution.Set("offsetY", resolutionWithOffset.offsetY());
            outputWithResolution.Set("width", resolutionWithOffset.width());
            outputWithResolution.Set("height", resolutionWithOffset.height());
            currentResolutionsArray.Set(i, outputWithResolution);
            i++;
    }
    Napi::Error::New(env, "Could not get the number of outputs of the display. Please try again.");
    return currentResolutionsArray;
}

Napi::Boolean IsDisplayAvailable(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (XOpenDisplay(nullptr) == NULL)
        return Napi::Boolean::New(env, false);
    else
        return Napi::Boolean::New(env, true);
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Napi::Object displayUtility = Napi::Object::New(env);

    displayUtility.Set(Napi::String::New(env, "getConnectedOutputs"), Napi::Function::New(env, GetConnectedOutputs));
    displayUtility.Set(Napi::String::New(env, "getOutputName"), Napi::Function::New(env, GetOutputName));
    displayUtility.Set(Napi::String::New(env, "getCurrentResolution"), Napi::Function::New(env, GetCurrentResolution));
    displayUtility.Set(Napi::String::New(env, "getResolutions"), Napi::Function::New(env, GetResolutions));
    displayUtility.Set(Napi::String::New(env, "setResolution"), Napi::Function::New(env, SetResolution));
    displayUtility.Set(Napi::String::New(env, "makeScreenBlank"), Napi::Function::New(env, MakeScreenBlank));
    displayUtility.Set(Napi::String::New(env, "reverseBlankScreen"), Napi::Function::New(env, ReverseBlankScreen));
    displayUtility.Set(Napi::String::New(env, "getPrimaryRROutput"), Napi::Function::New(env, GetPrimaryRROutput));
    displayUtility.Set(Napi::String::New(env, "getExtendedMonitorResolution"), Napi::Function::New(env, GetExtendedMonitorResolution));
    displayUtility.Set(Napi::String::New(env, "getAllCurrentResolutionsWithOffset"), Napi::Function::New(env, GetAllCurrentResolutions));
    displayUtility.Set(Napi::String::New(env, "isDisplayAvailable"), Napi::Function::New(env, IsDisplayAvailable));

    exports.Set("DisplayUtility", displayUtility);
    
    return ScreenCaptureUtility::Init(env, exports);
}

NODE_API_MODULE(desktop_info, Init);
