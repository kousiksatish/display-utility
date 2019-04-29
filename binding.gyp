{
  "targets": [
    { 
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "include_dirs" : [
        "<!@(node -p \"require('node-addon-api').include\")"],
      "libraries": [
        "/usr/lib/x86_64-linux-gnu/libX11.so",
        "/usr/lib/x86_64-linux-gnu/libXrandr.so",
        "/usr/lib/x86_64-linux-gnu/libXtst.so"],
      "target_name": "display-utility",
      "sources": [ 
        "native_src/display_utility.cc", 
        "native_src/display_utility_x11.cc",
        "native_src/output_resolution.cc",
        "native_src/screen_resources.cc",
        "native_src/x11_util.cc"],
      "defines": [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    }
  ]
}