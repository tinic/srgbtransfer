#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <limits>
#include <algorithm>
#include <chrono>

static uint8_t sRGBTransfer(float a) {
  if (a < 0.0031308f) {
  	  return uint8_t(255.0f * std::clamp(a * 12.92f, 0.0f, 1.0f));
  } else {
	  return uint8_t(255.0f * std::clamp(pow(a, 1.0f/2.4f) * 1.055f - 0.055f, 0.0f, 1.0f));
  }
}

static uint8_t sRGBTransferFast(float a) {
	if (a <= 0.0f) {
		return 0;
	} else if (a < 0.0031308f) {
		return uint8_t(255.0f * a * 12.92f);
	} else if ( a < 1.0f ) {
		union {
			float d;
			int16_t x[2];
		} u = { a };
		u.x[1] = (int16_t)((1.0f/2.40f) * float(u.x[1] - 0x3f7d) + 0x3f7d);
		u.x[0] = 0;
		return uint8_t((u.d * 1.055f - 0.055) * 255.0f);
	} else {
		return 255;
	}
}

int main() {
	int32_t error = 0;
	for(float i = -0.1f; i <= 1.1f; i += 1.0f/1024.0f) {
		uint8_t a = sRGBTransfer(i);
		uint8_t b = sRGBTransferFast(i);
		error += abs(a-b);
		printf("input: %f correct: %d fast: %d error: %d\n", i, a, b, abs(a-b));
	}
	
	{	
		uint64_t acc = 0;
		auto start = std::chrono::high_resolution_clock::now();
		for (int32_t j = 0; j < 1024 * 128; j++) {
			for(float i = -0.1f; i <= 1.1f; i += 1.0f/1024.0f) {
				acc += sRGBTransfer(i);
			}
		}
		auto finish = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = finish - start;
		printf("correct: %f seconds  (%d)\n", elapsed.count(), int(acc));
	}

	{	
		uint64_t acc = 0;
		auto start = std::chrono::high_resolution_clock::now();
		for (int32_t j = 0; j < 1024 * 128; j++) {
			for(float i = -0.1f; i <= 1.1f; i += 1.0f/1024.0f) {
				acc += sRGBTransferFast(i);
			}
		}
		auto finish = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = finish - start;
		printf("fast: %f seconds  (%d)\n", elapsed.count(), int(acc));
	}
}