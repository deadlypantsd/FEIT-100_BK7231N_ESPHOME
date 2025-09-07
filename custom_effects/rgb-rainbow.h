#pragma once
#include "esphome/components/light/addressable_light.h"
#include <cmath>

namespace esphome {
namespace light {

class RainbowEffect : public AddressableLightEffect {
 public:
  RainbowEffect() : AddressableLightEffect("Rainbow") {}

  void apply(AddressableLight &leds, const Color & /*current_color*/) override {
    static int frame = 0;
    frame++;

    const float ITERATIONS = 4.0f;  // Create 4 rainbows across the strand

    for (size_t i = 0; i < leds.size(); i++) {
      if (i % 2 != 0) {
        leds[i] = Color(0,0,0); 
        continue;
      }

      float hue = fmod(frame + i * 360.0f * ITERATIONS / leds.size(), 360.0f);
      float h = hue / 360.0f;
      float r_f, g_f, b_f;

      if (h < 1.0f/6.0f)      { r_f = 1; g_f = h*6; b_f = 0; }
      else if (h < 2.0f/6.0f) { r_f = 1-(h-1.0f/6.0f)*6; g_f = 1; b_f = 0; }
      else if (h < 3.0f/6.0f) { r_f = 0; g_f = 1; b_f = (h-2.0f/6.0f)*6; }
      else if (h < 4.0f/6.0f) { r_f = 0; g_f = 1-(h-3.0f/6.0f)*6; b_f = 1; }
      else if (h < 5.0f/6.0f) { r_f = (h-4.0f/6.0f)*6; g_f = 0; b_f = 1; }
      else                     { r_f = 1; g_f = 0; b_f = 1-(h-5.0f/6.0f)*6; }

      leds[i] = Color(
        int(r_f * 255 * id(rgb_brightness)),
        int(g_f * 255 * id(rgb_brightness)),
        int(b_f * 255 * id(rgb_brightness))
      );
    }

    leds.schedule_show();
  }
};

}
}
