// 
// 2-dimensional FFT for image processing
//
// 2023, Jonathan Tainer
//

#include <stdlib.h>
#include <string.h>

#include "fft_2d.h"

surface_complex create_surface_complex(unsigned int width, unsigned int height) {
	surface_complex surface = { width, height, NULL };
	surface.data = malloc(sizeof(float complex) * width * height);
	memset(surface.data, 0, sizeof(float complex) * width * height);
	return surface;
}

void destroy_surface_complex(surface_complex surface) {
	free(surface.data);
}

void fft_2d(surface_complex surface) {

}

void ifft_2d(surface_complex surface) {

}
