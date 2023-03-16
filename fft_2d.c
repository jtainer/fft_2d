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
	// 1D transform on each row
	unsigned int n = surface.width;
	float complex* sig = surface.data;
	for (unsigned int y = 0; y < surface.height; y++) {
		fft_inpl(sig, n);
		sig += n;
	}

	// 1D transform on each column
	n = surface.height;
	float complex* tmp = malloc(sizeof(float complex) * n);
	for (unsigned int x = 0; x < surface.width; x++) {
		// Column elements are not adjacent in memory so they must be
		// copied into a temporary buffer
		for (unsigned int y = 0; y < n; y++) {
			tmp[y] = surface.data[y * surface.width + x];
		}
		// Transform signal in temp buffer
		fft_inpl(tmp, n);
		// Write temp buffer back to original surface
		for (unsigned int y = 0; y < n; y++) {
			surface.data[y * surface.width + x] = tmp[y];
		}
	}
	free(tmp);
}

void ifft_2d(surface_complex surface) {
	// 1D transform on each row
	unsigned int n = surface.width;
	float complex* sig = surface.data;
	for(unsigned int y = 0; y < surface.height; y++) {
		ifft_inpl(sig, n);
		sig += n;
	}

	// 1D transform on each column
	n = surface.height;
	float complex* tmp = malloc(sizeof(float complex) * n);
	for (unsigned int x = 0; x < surface.width; x++) {
		// Column elements are not adjacent in memory
		// Copy into temp buffer
		for (unsigned int y = 0; y < n; y++) {
			tmp[y] = surface.data[y * surface.width + x];
		}
		// Inverse transform on signal in temp buffer
		ifft_inpl(tmp, n);
		// Write temp buffer back to original buffer
		for (unsigned int y = 0; y < n; y++) {
			surface.data[y * surface.width + x] = tmp[y];
		}
	}
	free(tmp);
}
