#pragma once
#include "esphome/components/light/addressable_light.h"

namespace esphome {
namespace light {

class WhiteEffect : public AddressableLightEffect {
 public:
  WhitesEffect() : AddressableLightEffect("White") {}

  void apply(AddressableLight &leds, const Color & /*current_color*/) override {
    for (size_t i = 0; i < leds.size(); i++) {
      if (i % 2 == 0) {
        leds[i] = Color(0,0,0); // clear RGB LEDs
      } else {
        leds[i] = Color(
          static_cast<uint8_t>(255 * id(warm_white_brightness)), // R channel
          static_cast<uint8_t>(255 * id(cold_white_brightness)), // G channel
          0
        );
      }
    }

    leds.schedule_show();
  }
};

}  // namespace light
}  // namespace esphome
