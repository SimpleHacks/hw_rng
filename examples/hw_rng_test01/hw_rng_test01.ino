#include "hw_rng.h"

void setup(){
    Serial.begin(115200);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
    // prints title with ending line break
    Serial.println("HW_RNG -- test sketch");
    Serial.flush();
    delay(1000);

    SimpleHacks::HW_RNG::begin(); // ensure rng hardware is initialized
    Serial.println("Setup complete");
    Serial.flush();
    delay(1000);

    uint8_t alpha  = SimpleHacks::HW_RNG::get_uint8() ; (void)alpha;
    uint16_t beta  = SimpleHacks::HW_RNG::get_uint16(); (void)beta;
    uint32_t gamma = SimpleHacks::HW_RNG::get_uint32(); (void)gamma;
    uint64_t delta = SimpleHacks::HW_RNG::get_uint64(); (void)delta;

    StaticTask_t epsilon;
    SimpleHacks::HW_RNG::fill(epsilon);

    StaticTask_t zeta;
    SimpleHacks::HW_RNG::fill_buffer(&zeta, sizeof(zeta));
    
}

void loop() {
  uint16_t delayTime = SimpleHacks::HW_RNG::get_uint16();
  while (delayTime > 5000) {
    delayTime = SimpleHacks::HW_RNG::get_uint16();
  }
  Serial.print("Delay: "); Serial.println(delayTime);
  delay(delayTime);
}
