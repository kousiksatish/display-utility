// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "../headers/x11_util.h"

namespace remoting {

ScopedXGrabServer::ScopedXGrabServer(Display* display)
    : display_(display) {
  XGrabServer(display_);
}

ScopedXGrabServer::~ScopedXGrabServer() {
  XUngrabServer(display_);
  XFlush(display_);
}


bool IgnoreXServerGrabs(Display* display, bool ignore) {
  int test_event_base = 0;
  int test_error_base = 0;
  int major = 0;
  int minor = 0;
  if (!XTestQueryExtension(display, &test_event_base, &test_error_base,
                           &major, &minor)) {
    return false;
  }

  XTestGrabControl(display, ignore);
  return true;
}

} // namespace remoting