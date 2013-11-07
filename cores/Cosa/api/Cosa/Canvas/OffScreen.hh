/**
 * @file Cosa/Canvas/OffScreen.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_CANVAS_OFFSCREEN_HH__
#define __COSA_CANVAS_OFFSCREEN_HH__

#include "Cosa/Types.h"
#include "Cosa/Canvas.hh"

/**
 * Off-screen canvas for drawing before copying to the canvas device.
 * Supports monochrome, 1-bit, pixel in off-screen buffer. 
 * Minimum implementation; draw_pixel() only.
 * @param[in] width of canvas.
 * @param[in] height of canvas.
 */
template<uint8_t width, uint8_t height>
class OffScreen : public Canvas {
private:
  static const uint16_t COUNT = (width * height) / CHARBITS;
  uint8_t m_bitmap[COUNT];

public:
  /**
   * Construct off-screen canvas with given width and height. A 
   * buffer may be given but must be able to hold the bitmap size.
   * If the buffer pointer is null a buffer is allocated.
   */
  OffScreen() : Canvas(width, height) {}

  /**
   * Get bitmap for the off-screen canvas.
   * @return bitmap pointer.
   */
  uint8_t* get_bitmap()
  {
    return (m_bitmap);
  }

  /**
   * @override Canvas
   * Start interaction with off-screen canvas.
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool begin()
  {
    fill_screen();
    return (true);
  }

  /**
   * @override Canvas
   * Set pixel according to the current pen color.
   * @param[in] x.
   * @param[in] y.
   */
  virtual void draw_pixel(uint8_t x, uint8_t y)
  {
    if ((x > WIDTH) || (y > HEIGHT)) return;
    uint8_t* bp = &m_bitmap[((y >> 3) * WIDTH) + x];
    uint8_t pos = (y & 0x07); 
    if (get_pen_color().rgb == Canvas::BLACK)
      *bp |= (1 << pos);
    else
      *bp &= ~(1 << pos);
  } 

  /**
   * @override Canvas
   * Fill offscreen buffer with canvas background color.
   */
  virtual void fill_screen()
  {
    memset(m_bitmap, (get_canvas_color().rgb == Canvas::BLACK), COUNT);
  }

  /**
   * @override Canvas
   * Stop sequence of interaction with off-screen device.
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool end()
  {
    return (true);
  }
};

#endif
