// 
// 2-dimensional FFT for image processing
//
// 2023, Jonathan Tainer
//

#ifndef FFT_2D_H
#define FFT_2D_H

#include "fft/fft.h"

typedef struct surface_complex {
	unsigned int width;
	unsigned int height;
	float complex* data;
} surface_complex;

surface_complex create_surface_complex(unsigned int width, unsigned int height);
void destroy_surface_complex(surface_complex surface);
void fft_2d(surface_complex surface);
void fft_2d_padded(surface_complex surface, unsigned int pad);
void ifft_2d(surface_complex surface);

#endif
