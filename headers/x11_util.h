// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef REMOTING_HOST_LINUX_X11_UTIL_H_
#define REMOTING_HOST_LINUX_X11_UTIL_H_

// Xlib.h (via ui/gfx/x/x11.h) defines XErrorEvent as an anonymous
// struct, so we can't forward- declare it in this header. Since
// Xlib.h is not generally something you should #include into
// arbitrary code, please refrain from #including this header in
// another header.

#include "base_macros.h"
extern "C"
{
// Xlib.h defines base types so it must be included before the less
// central X11 headers can be included.
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
}

namespace remoting
{

// Grab/release the X server within a scope. This can help avoid race
// conditions that would otherwise lead to X errors.
class ScopedXGrabServer
{
public:
  ScopedXGrabServer(Display *display);
  ~ScopedXGrabServer();

private:
  Display *display_;

  DISALLOW_COPY_AND_ASSIGN(ScopedXGrabServer);
};

// Make a connection to the X Server impervious to X Server grabs. Returns
// true if successful or false if the required XTEST extension is not present.
bool IgnoreXServerGrabs(Display *display, bool ignore);

} // namespace remoting

#endif // REMOTING_HOST_LINUX_X11_UTIL_H_
