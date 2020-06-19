/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 by SimpleHacks, Henry Gabryjelski
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef __INC_SIMPLEHACKS_HW_RNG
#define __INC_SIMPLEHACKS_HW_RNG

#include <stdint.h>
#include <hal/nrf_rng.h>

#if defined(SIMPLEHACKS_HW_RNG_NEVER_INLINE)
    // yes, the __noinline__ attribute still uses inline, so that even non-template
    // functions can be defined in the header file.
    #define SIMPLEHACKS_HW_RNG_INLINE_ATTRIBUTE __attribute__((noinline)) inline
#else     
    #define SIMPLEHACKS_HW_RNG_INLINE_ATTRIBUTE __attribute__((always_inline)) inline
#endif

// This library uses a namespace (e.g., SimpleHacks::HW_RNG instead of just HW_RNG).
// However, it's very easy to make this effectively disappear!
// As an example, at the start of a function (or right after all headers included)
// simply add the following lines:
//     using SimpleHacks::HW_RNG;
// Or, if you want all the types to be avabile, you could instead add the line:
//     using namespace SimpleHacks;
//
// See the examples for more options...

#if !defined(ARDUINO_ARCH_NRF52) && !defined(ARDUINO_NRF52_ADAFRUIT)
    #error "Library currenly only known to support NRF52 architecture"
#endif

// Somewhere between nrfx v1.7.2 (which this library was originally written for) and
// nrfx v2.1.0, Nordic made significant breaking changes to their API.  This breaking
// API change is not listed in the nrfx changelog.
// 
// It is desirable to hide this breaking API change from users.
//
// [HACK] check for existence of NRFX_STATIC_INLINE, which was not defined
//        in nrfx v1.x, but is defined in nrfx v2.x.
//        This works even if NRFX_STATIC_INLINE is defined to nothing
//
#if defined(NRFX_STATIC_INLINE)
    #define SIMPLEHACKS_HW_RNG_NRFX_API_VERSION 2
    // #warn  "Discovered nrfx v2.x.x APIs ... "
#else
    #define SIMPLEHACKS_HW_RNG_NRFX_API_VERSION 1
    // #warn "Discovered nrfx v1.x.x APIs ... "
#endif


namespace SimpleHacks {

    // Difficulty: the RNG is used by the SoftDevice.
    //             In which case, how do we also use it for Arduino?
    // Answer: We set the config to enable bias correction,
    //         Send the START task to the RNG in loop that spins waiting
    //         NEVER  EVER  send the STOP task.
    //         NEVER  EVER  mess with interrupts.
    //         Yes, it's an ugly hack.  No, I don't know how to avoid it.
    //         

    class HW_RNG {
    public: // special members -- all deleted so only static functionality exists
        ~HW_RNG() =delete;
        HW_RNG() =delete;
        HW_RNG(const HW_RNG&) =delete;
        HW_RNG& operator=(const HW_RNG&) =delete;
        HW_RNG(HW_RNG&&) =delete;
        HW_RNG& operator=(const HW_RNG&&) =delete;
    public: // user API
        SIMPLEHACKS_HW_RNG_INLINE_ATTRIBUTE static void begin() {
#if SIMPLEHACKS_HW_RNG_NRFX_API_VERSION == 2
            // Nordic changes API between nrfx v1.7.2 and v2.1.0...
            nrf_rng_error_correction_enable(NRF_RNG);
            nrf_rng_shorts_disable(NRF_RNG, NRF_RNG_SHORT_VALRDY_STOP_MASK);
            nrf_rng_task_trigger(NRF_RNG, NRF_RNG_TASK_START);
#elif SIMPLEHACKS_HW_RNG_NRFX_API_VERSION == 1
            // Nordic changes API between nrfx v1.7.2 and v2.1.0...
            nrf_rng_error_correction_enable();
            nrf_rng_shorts_disable(NRF_RNG_SHORT_VALRDY_STOP_MASK);
            nrf_rng_task_trigger(NRF_RNG_TASK_START);
#else
    #error "Unknown NRFX API version ... aborting compilation"
#endif
        }
        SIMPLEHACKS_HW_RNG_INLINE_ATTRIBUTE static void end() {}
        SIMPLEHACKS_HW_RNG_INLINE_ATTRIBUTE static uint8_t get_uint8() {
            return internal_get_byte();
        }
        SIMPLEHACKS_HW_RNG_INLINE_ATTRIBUTE static uint16_t get_uint16() {
            uint16_t v =
                (((uint16_t)internal_get_byte()) << (8*1)) |
                (((uint16_t)internal_get_byte()) << (8*0)) ;
            return v;
        }
        SIMPLEHACKS_HW_RNG_INLINE_ATTRIBUTE static uint32_t get_uint32() {
            uint32_t v =
                (((uint32_t)internal_get_byte()) << (8*3)) |
                (((uint32_t)internal_get_byte()) << (8*2)) |
                (((uint32_t)internal_get_byte()) << (8*1)) |
                (((uint32_t)internal_get_byte()) << (8*0)) ;
            return v;
        }
        SIMPLEHACKS_HW_RNG_INLINE_ATTRIBUTE static uint64_t get_uint64() {
            uint64_t v =
                (((uint64_t)internal_get_byte()) << (8*7)) |
                (((uint64_t)internal_get_byte()) << (8*6)) |
                (((uint64_t)internal_get_byte()) << (8*5)) |
                (((uint64_t)internal_get_byte()) << (8*4)) |
                (((uint64_t)internal_get_byte()) << (8*3)) |
                (((uint64_t)internal_get_byte()) << (8*2)) |
                (((uint64_t)internal_get_byte()) << (8*1)) |
                (((uint64_t)internal_get_byte()) << (8*0)) ;
            return v;
        }

        template<typename T>
        SIMPLEHACKS_HW_RNG_INLINE_ATTRIBUTE static void fill( T& item ) {
            static_assert(sizeof(size_t) >= sizeof(uint16_t)); // 1999 ISO C standard (C99): size_t is an unsigned integer type of at least 16 bits
            static_assert(sizeof(T) >= 0);
            internal_fill_buffer(&item, sizeof(T));
            return;
        }
        SIMPLEHACKS_HW_RNG_INLINE_ATTRIBUTE static void fill_buffer(void * buffer, size_t count) {
            internal_fill_buffer(buffer, count);
            return;
        }
        //
        // TODO: function to get a value between given start & end
        // TODO: Consider adding capability to be a C++ "random_device"
        //       See: http://www.cplusplus.com/reference/random/random_device/
        //       Requirements:
        //       [ ] (constructor)
        //           Construct random device (public member function)
        //       [ ] min
        //           Minimum value (public static member function) == 0
        //       [ ] max
        //           Maximum value (public static member function) == 255
        //       [ ] operator()
        //           Generate random number (public member function)
        //       [ ] entropy
        //           Return entropy (public member function) == 8 (true RNG)
        //


    private: // to support one-time init...
        SIMPLEHACKS_HW_RNG_INLINE_ATTRIBUTE static uint8_t internal_get_byte() {
#if SIMPLEHACKS_HW_RNG_NRFX_API_VERSION == 2
            // Nordic changes API between nrfx v1.7.2 and v2.1.0...
            while (!nrf_rng_event_check(NRF_RNG, NRF_RNG_EVENT_VALRDY));
            uint8_t v = nrf_rng_random_value_get(NRF_RNG);
            nrf_rng_event_clear(NRF_RNG, NRF_RNG_EVENT_VALRDY);
#elif SIMPLEHACKS_HW_RNG_NRFX_API_VERSION == 1
            // Nordic changes API between nrfx v1.7.2 and v2.1.0...
            while (!nrf_rng_event_get(NRF_RNG_EVENT_VALRDY));
            uint8_t v = nrf_rng_random_value_get();
            nrf_rng_event_clear(NRF_RNG_EVENT_VALRDY);
#else
    #error "Unknown NRFX API version ... aborting compilation"
#endif
            return v;
        }
        SIMPLEHACKS_HW_RNG_INLINE_ATTRIBUTE static void internal_fill_buffer( void * buffer, size_t count ) {
            uint8_t * t = static_cast<uint8_t*>(buffer);
            while (count != 0) {
                count--;
                *t = internal_get_byte();
                t++;
            }
            return;
        }
    };
}


#endif // #ifndef __INC_SIMPLEHACKS_HW_RNG



