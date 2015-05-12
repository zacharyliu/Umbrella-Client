#include <PacketSerial.h>
#include "FastLED.h"
#include "FastFader.h"

// How many leds are in the strip?
#define NUM_LEDS 20

// Data pin that led data will be written out over
#define DATA_PIN 3

#define STEPS_PER_SECOND 60

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

PacketSerial serial;

uint8_t pixel_buffer[NUM_LEDS][3];
FastFader pixel_fader;

// Is there currently an animation running?
bool isRunning = false;

void setup() {
  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
  setColor(CRGB::Green);
  
  serial.setPacketHandler(&onPacket);
  serial.begin(9600);
  
  pixel_fader.bind(pixel_buffer, leds, NUM_LEDS, FastLED);
}

void loop() {
  serial.update();
}

void onPacket(const uint8_t* buffer, size_t size)
{
//  if (isRunning) return;

  isRunning = true;
  
  // Check that the size is correct
  if (size > 4 + NUM_LEDS * 3) return;
  
  // Copy the packet into the LEDs array
//  memcpy(leds, buffer, size);
  
  // Show new colors
//  FastLED.show();
  
  // Load LEDs into pixel fader
  int numLedsGiven = (size - 4) / 3;
  for (int i = 0; i < numLedsGiven; i++) {
    memcpy(pixel_buffer[i], &buffer[4 + 3*i], 3);
  }
  
  // Run animation
  int duration = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16) + (buffer[3] << 24);
  int steps = max(10, (long) duration * STEPS_PER_SECOND / 1000);
  pixel_fader.push(duration, steps, LINEAR_DECAY);
  
  isRunning = false;
}

void setColor(CRGB color) {
  // Set color on all LEDs
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
  
  // Show new colors
  FastLED.show();
}

