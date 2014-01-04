/**
 * @file CosaPCD8544.ino
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
 * @section Description
 * Demonstration of the PCD8544 device driver with mapping to 
 * IOStream::Device.
 * 
 * @section Circuit
 * Connect Arduino to PCD8544 (Arduino => PCD8544):
 * D6 ==> SDIN, D7 ==> SCLK, D8 ==> DC, D9 ==> SCE.
 * RST ==> RST.
 * 
 * The PCD8544 should be connect using 3.3 V signals and VCC. 
 * Back-light should be max 3.3 V. Reduce voltage with 100-500 ohm 
 * resistor to ground.
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Canvas/OffScreen.hh"
#include "Cosa/LCD/Driver/PCD8544.hh"
#include "Cosa/Canvas/Font/FixedNums8x16.hh"
#include "Cosa/Canvas/Icon/arduino_icon_64x32.h"
#include "Cosa/Canvas/Icon/arduino_icon_96x32.h"

PCD8544 lcd;

void setup()
{
  // Set up watchdog for low power sleep
  Watchdog::begin();

  // Initiate the LCD screen and show arduino icon
  lcd.begin();
#if !defined(__ARDUINO_TINY__)
  lcd.putchar('\f');
  lcd.set_cursor((lcd.WIDTH - 64)/2, 0);
  lcd.draw_icon(arduino_icon_64x32);
#endif

  // Use LCD bind to trace and use inverted text mode for banner
  trace.begin(&lcd);
  lcd.set_cursor(8, 4);
  trace << PSTR("\aCosaPCD8544\a");
  SLEEP(2);

  // Dump characters in system font; two pages, 64 characters each
  uint8_t page = 0;
  for (uint8_t c = 0; c < 128; c++) {
    if ((c & 63) == 0) {
      trace << PSTR("\f\aFont page#") << page++ << '\a' << endl;
    }
    if ((c == '\n') || (c == '\f') || (c == '\b') || (c == '\a') || (c == '\t')) 
      trace << ' ';
    else
      trace << (char) c;
    if (((c +  1) & 63) == 0) SLEEP(4);
  }

  // Use the trace iostream onto the LCD with output operator
  trace << PSTR("\f\aSPECIAL CHARACTERS\a\n\n");
  trace << PSTR("01\t23\t45\t67\t89");
  for (uint8_t i = 0; i < 16; i++) {
    Watchdog::delay(256);
    trace << PSTR("\b \b");
  }

  // Scrolling
  trace << PSTR("\f\aSCROLLING\a\n");
  static const char msg[] __PROGMEM = 
    "The quick brown fox jumps over the lazy dog. ";
  for (uint8_t i = 0; i < 4; i++) {
    uint8_t len = strlen_P(msg);
    for (uint8_t j = 0; j < len; j++) {
      trace << (char) pgm_read_byte(msg + j);
      Watchdog::delay(64);
    }
  }
  SLEEP(2);

  // Use number base handling 
  trace << PSTR("\f\aNUMBER BASE\a\n");
  trace << PSTR("bcd = ") << bcd << 0x55 << endl;
  trace << PSTR("oct = ") << oct << 0x55 << endl;
  trace << PSTR("dec = ") << dec << 0x55 << endl;
  trace << PSTR("hex = ") << hex << 0x55 << endl;
  trace << PSTR("ptr = ") << &lcd;
  SLEEP(2);

  // Dump the LCD object raw format with normal print function
  trace << PSTR("\f\aBUFFER DUMP\a\n");
  trace.print(&lcd, 8, IOStream::hex, 2);
  SLEEP(2);

  // Play around with the offscreen canvas
#if !defined(__ARDUINO_TINY__)
  OffScreen<PCD8544::WIDTH, PCD8544::HEIGHT> offscreen;
  offscreen.begin();
  offscreen.draw_rect(0, 0, 10, 10);
  offscreen.fill_rect(2, 2, 7, 7);
  offscreen.draw_rect(20, 20, 10, 10);
  offscreen.draw_circle(20, 20, 10);
  offscreen.fill_circle(20, 20, 8);
  offscreen.draw_roundrect(20, 20, 40, 20, 10);
  offscreen.fill_roundrect(22, 22, 36, 16, 7);
  offscreen.draw_rect(20, 20, 40, 20);
  offscreen.draw_rect(0, 0, offscreen.WIDTH - 1, offscreen.HEIGHT - 1);
  offscreen.set_cursor(15, 2);
  offscreen.draw_string_P(PSTR("OffScreen"));

  // Draw the off-screen canvas on the LCD
  lcd.set_cursor(0, 0);
  lcd.draw_bitmap(offscreen.get_bitmap(), offscreen.WIDTH, offscreen.HEIGHT);
  SLEEP(4);
#endif
}

void loop()
{
  // Draw bars with analog sample values (A0..A5)
  lcd.putchar('\f');
  for (uint8_t i = 0; i < lcd.LINES; i++) {
    trace.printf_P(PSTR("A%d:"), i);
    uint8_t procent = (AnalogPin::sample(i) * 100L) / 1023;
    lcd.draw_bar(procent, lcd.WIDTH - 20);
    if (i != lcd.LINES - 1) trace << endl;
  }
  SLEEP(1);

  // Every 4 seconds display the arduino icon and count down time
#if !defined(__ARDUINO_TINY__)
  static const uint8_t SHOW_BANNER = 4;
  static uint8_t banner = 0;
  if (++banner == SHOW_BANNER) {

    // Display the Arduino icon
    lcd.putchar('\f');
    lcd.set_cursor(0, 1);
    lcd.display_inverse();
    lcd.draw_icon(arduino_icon_96x32);
    SLEEP(2);

    lcd.display_normal();
    banner = 0;

    // Counters
    static uint8_t min = 30;
    static uint8_t sec = 00;

    // Draw the current counter value off-screen
    OffScreen<PCD8544::WIDTH, PCD8544::HEIGHT> offscreen;
    offscreen.begin();
    offscreen.set_text_font(&fixednums8x16);
    offscreen.draw_roundrect(8, 8, lcd.WIDTH - 18, lcd.HEIGHT - 18, 8);
    offscreen.set_cursor((lcd.WIDTH - (fixednums8x16.get_width('0') * 5))/2,
			 (lcd.HEIGHT - fixednums8x16.get_height('0'))/2);
    offscreen.draw_char('0' + min/10);
    offscreen.draw_char('0' + min%10);
    offscreen.draw_char(':');
    offscreen.draw_char('0' + sec/10);
    offscreen.draw_char('0' + sec%10);

    // Draw the off-screen canvas on the LCD
    lcd.putchar('\f');
    lcd.draw_bitmap(offscreen.get_bitmap(), offscreen.WIDTH, offscreen.HEIGHT);

    // Decrement counter
    if (sec == 0) {
      if (min != 00) {
	sec = 50;
	min -= 1;
      }
    } else
      sec -= 10;
    SLEEP(4);
  }
#endif
}
