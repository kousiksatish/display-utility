{
  "targets": [
    { 
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "include_dirs" : [
        "<!@(node -p \"require('node-addon-api').include\")"],
      "libraries": [
        "-lX11",
        "-lXrandr",
        "-lXtst"],
      "target_name": "display-utility",
      "sources": [ 
        "native_src/display_utility.cc", 
        "native_src/display_utility_x11.cc",
        "native_src/output_resolution.cc",
        "native_src/screen_resources.cc",
        "native_src/x11_util.cc",
        "native_src/output_resolution_with_offset.cc"],
      "defines": [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    },
    { 
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "include_dirs" : [
        "<!@(node -p \"require('node-addon-api').include\")"],
      "libraries": [
        "-lX11",
        "-lXrandr",
        "-lXtst",
        "-lx264",
        "-lswscale"],
      "target_name": "screen-capture-utility",
      "sources": [ 
        "native_src/screen_capture_utility.cc",
        "native_src/encoder.cc",
        "native_src/screen_capturer.cc",
        "native_src/display_utility_x11.cc",
        "native_src/output_resolution.cc",
        "native_src/screen_resources.cc",
        "native_src/x11_util.cc",
        "native_src/output_resolution_with_offset.cc"],
      "defines": [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    }
  ]
}