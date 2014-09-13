/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2013 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "Screen/Layout.hpp"
#include "Hardware/DisplayDPI.hpp"
#include "Interface.hpp"
#include "UISettings.hpp"
// FIXME testing only - remove or make debug conditional later
#include "LogFile.hpp"

#include <algorithm>

namespace Layout
{
  bool landscape = false;
  int scale = 1;
  unsigned scale_1024 = 1024;
  unsigned small_scale = 1024;
  unsigned pen_width_scale = 1024;
  UPixelScalar text_padding = 2;
  UPixelScalar minimum_control_height = 20, maximum_control_height = 44;
  UPixelScalar hit_radius = 10;
}

unsigned
Layout::GetXDPI()
{
  return Display::GetXDPI();
}


void
Layout::Initialize(PixelSize new_size)
{
  const unsigned width = new_size.cx, height = new_size.cy;
  const UISettings &settings = CommonInterface::GetUISettings();

  landscape = width > height;
  const bool square = width == height;

  if (!ScaleSupported())
    return;

  const unsigned x_dpi = GetXDPI();

  unsigned minsize = std::min(width, height);
  // always start w/ shortest dimension
  // square should be shrunk
  scale_1024 = std::max(1024U, minsize * 1024 / (square ? 320 : 240));
  scale = scale_1024 / 1024;

  small_scale = (scale_1024 - 1024) / 2 + 1024;

  pen_width_scale = std::max(1024u, x_dpi * 1024u / 80u);

  text_padding = Scale(2);

  minimum_control_height = Scale(30);

  if (UseTouchScreenLayout()) {
    /* larger rows for touch screens */
    maximum_control_height = Display::GetYDPI() * 3 / 7;
    if (maximum_control_height < minimum_control_height)
      maximum_control_height = minimum_control_height;
  } else {
    maximum_control_height = minimum_control_height;
  }
  hit_radius = x_dpi / ((UseTouchScreenLayout() || settings.dialog.using_remote) ? 3 : 12);
  /* leaving debug messages in as a reminder that this gets run twice on startup
   * and it perhaps might be worthwhile removing in future - FIXME
   */
  LogFormat("Layout::Initialize: hit radius = %d pixels, using_remote = %d", hit_radius, settings.dialog.using_remote);
}
