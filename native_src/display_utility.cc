#include <napi.h>
#include <iostream>
#include "../headers/display_utility_x11.h"
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

    unsigned int outputIndex = info[0].As<Napi::Number>().Int32Value();

    std::unique_ptr<DisplayUtilityX11> desktopInfo = DisplayUtilityX11::Create();

    return Napi::String::New(env, desktopInfo->GetOutputName(outputIndex));
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

    unsigned int outputIndex = info[0].As<Napi::Number>().Int32Value();
    std::unique_ptr<DisplayUtilityX11> desktopInfo = DisplayUtilityX11::Create();
    std::unique_ptr<OutputResolution> resolution = desktopInfo->GetCurrentResolution(outputIndex);
    if (resolution != nullptr)
    {
        std::cout << "current Resolution : " << resolution->width() << "x" << resolution->height() << std::endl;
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

    unsigned int outputIndex = info[0].As<Napi::Number>().Int32Value();
    std::unique_ptr<DisplayUtilityX11> desktopInfo = DisplayUtilityX11::Create();
    std::set<OutputResolution> resolutionsSet = desktopInfo->GetResolutions(outputIndex);
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

    if (!info[0].IsString() || !info[1].IsString())
    {
        Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
        return;
    }

    std::string outputName = info[0].As<Napi::String>();
    std::string resolution = info[1].As<Napi::String>();

    std::string setResolutionCommnad = "xrandr --output " + outputName + " --mode " + resolution;
    int return_value = system(setResolutionCommnad.c_str());
    std::cout << "The value returned by command " << setResolutionCommnad << " was: " << return_value << std::endl;
    return;
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set(Napi::String::New(env, "getConnectedOutputs"), Napi::Function::New(env, GetConnectedOutputs));
    exports.Set(Napi::String::New(env, "getOutputName"), Napi::Function::New(env, GetOutputName));
    exports.Set(Napi::String::New(env, "getCurrentResolution"), Napi::Function::New(env, GetCurrentResolution));
    exports.Set(Napi::String::New(env, "getResolutions"), Napi::Function::New(env, GetResolutions));
    exports.Set(Napi::String::New(env, "setResolution"), Napi::Function::New(env, SetResolution));
    return exports;
}

NODE_API_MODULE(desktop_info, Init);
