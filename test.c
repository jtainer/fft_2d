// 
// Test case using 2D FFT for image upscaling
//
// 2023, Jonathan Tainer
//

#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include "fft_2d.h"

surface_complex LoadSurfaceFromImage(Image image);
Image LoadImageFromSurface(surface_complex surface);
void UpdateImageFromSurface(Image image, surface_complex surface);
surface_complex UpscaleSurface(surface_complex surf_old, unsigned int scale);

int main() {
	Image original = LoadImage("flopping.png");
	ImageFormat(&original, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
	surface_complex surf_old = LoadSurfaceFromImage(original);
	UnloadImage(original);

	fft_2d(surf_old);
	unsigned int scale = 4;
	surface_complex surf_new = UpscaleSurface(surf_old, scale);
	for (unsigned int i = 0; i < surf_new.width * surf_new.height; i++) {
		surf_new.data[i] *= scale * scale;
	}
	ifft_2d(surf_new);

	Image fft_image = LoadImageFromSurface(surf_new);
	ExportImage(fft_image, "upscaled.png");
	UnloadImage(fft_image);
	destroy_surface_complex(surf_old);
	destroy_surface_complex(surf_new);

	return 0;
}

surface_complex LoadSurfaceFromImage(Image image) {
	surface_complex surface = create_surface_complex(image.width, image.height);
	Color* pixel = (Color*) image.data;
	unsigned int pixelCount = image.width * image.height;
	for (unsigned int i = 0; i < pixelCount; i++) {
		float avg = (pixel[i].r + pixel[i].g + pixel[i].b) / 3.f;
		surface.data[i] = avg;
	}
	return surface;
}

Image LoadImageFromSurface(surface_complex surface) {
	Image image = GenImageColor(surface.width, surface.height, WHITE);
	Color* pixel = (Color*) image.data;
	unsigned int pixelCount = surface.width * surface.height;
	for (unsigned int i = 0; i < pixelCount; i++) {
		pixel[i].r = crealf(surface.data[i]);
		pixel[i].g = crealf(surface.data[i]);
		pixel[i].b = crealf(surface.data[i]);
		pixel[i].a = 255;
	}
	return image;
}

void UpdateImageFromSurface(Image image, surface_complex surface) {
	Color* pixel = (Color*) image.data;
	unsigned int pixelCount = surface.width * surface.height;
	for (unsigned int i = 0; i < pixelCount; i++) {
		pixel[i].r = Clamp(cabsf(surface.data[i]), 0, 255);
		pixel[i].g = Clamp(cabsf(surface.data[i]), 0, 255);
		pixel[i].b = Clamp(cabsf(surface.data[i]), 0, 255);
		pixel[i].a = 255;
	}
}

surface_complex UpscaleSurface(surface_complex surf_old, unsigned int scale) {
	surface_complex surf_new = create_surface_complex(surf_old.width * scale, surf_old.height * scale);
	
	int quad_width = surf_old.width / 2;
	int quad_height = surf_old.height / 2;

	// Top left quadrant
	for (int y = 0; y < quad_height; y++) {
		for (int x = 0; x < quad_width; x++) {
			unsigned int index_old = y * surf_old.width + x;
			unsigned int index_new = y * surf_new.width + x;
			surf_new.data[index_new] = surf_old.data[index_old];
		}
	}

	// Bottom left quadrant
	for (int y = quad_height; y < surf_old.height; y++) {
		for (int x = 0; x < quad_width; x++) {
			unsigned int index_old = y * surf_old.width + x;
			unsigned int index_new = (y + surf_new.height - surf_old.height) * surf_new.width + x;
			surf_new.data[index_new] = surf_old.data[index_old];
		}
	}

	// Top right quadrant
	for (int y = 0; y < quad_height; y++) {
		for (int x = quad_width; x < surf_old.width; x++) {
			unsigned int index_old = y * surf_old.width + x;
			unsigned int index_new = y * surf_new.width + (x + surf_new.width - surf_old.width);
			surf_new.data[index_new] = surf_old.data[index_old];
		}
	}

	// Botom right quadrant
	for (int y = quad_height; y < surf_old.height; y++) {
		for (int x = quad_width; x < surf_old.width; x++) {
			unsigned int index_old = y * surf_old.width + x;
			unsigned int index_new = (y + surf_new.height - surf_old.height) * surf_new.width + (x + surf_new.width - surf_old.width);
			surf_new.data[index_new] = surf_old.data[index_old];
		}
	}
	return surf_new;
}

