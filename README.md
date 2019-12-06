# HW_RNG arduino library

A hardware-based true random number generator library, licensed under the MIT license.

Currently, the only supported chipset is the nRF52 series.

## Sample usage
See the library's samples.

```C++
#include "hw_rng.h"
using SimpleHacks;

void setup() {
    HW_RNG::begin(); // ensure rng hardware is initialized
}
void loop() {
    uint8_t alpha  = HW_RNG::get_uint8() ; (void)alpha;
    uint16_t beta  = HW_RNG::get_uint16(); (void)beta;
    uint32_t gamma = HW_RNG::get_uint32(); (void)gamma;
    uint64_t delta = HW_RNG::get_uint64(); (void)delta;

    StaticTask_t epsilon;
    HW_RNG::fill(epsilon);

    StaticTask_t zeta;
    HW_RNG::fill_buffer(&zeta, sizeof(zeta));
}
```
