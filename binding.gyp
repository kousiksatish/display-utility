{
  "variables": {
    # x264_root as relative path to the module root
    "x264_root%": "<(module_root_dir)/x264"
  },
  "targets": [
    { 
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "include_dirs" : [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "link_settings": {
        "libraries": [
          "-lX11", # provided by libx11-dev
          "-lXrandr", # provided by libxrandr-dev
          "-lXtst", # provided by libxtst-dev
          "-lXdamage",
          "-lXfixes",
          "-lswscale",
          "<@(x264_root)/lib/libx264.a" # static library exported with the package
        ]
      },
      "target_name": "display-utility",
      "sources": [ 
        "native_src/display_utility.cc", 
        "native_src/display_utility_x11.cc",
        "native_src/output_resolution.cc",
        "native_src/screen_resources.cc",
        "native_src/x11_util.cc",
        "native_src/output_resolution_with_offset.cc",
        "native_src/screen_capture_utility.cc",
        "native_src/encoder.cc",
        "native_src/base_screen_capturer.cc",
        "native_src/single_screen_capturer.cc",
        "native_src/multi_screen_capturer.cc",
        "native_src/get_next_frame_worker.cc"],
      # To avoid native node modules from throwing cpp exception and raise pending JS exception which can be handled in JS
      "defines": [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    }
  ]
}