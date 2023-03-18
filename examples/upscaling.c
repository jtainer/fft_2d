// 
// Test case using 2D FFT for image upscaling
//
// 2023, Jonathan Tainer
//

#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include "../fft_2d.h"

surface_complex LoadSurfaceFromImage(Image image, char channel);
Image LoadImageFromSurface(surface_complex surface);
void UpdateImageFromSurface(Image image, surface_complex surface, char channel);
surface_complex UpscaleSurface(surface_complex surf_old, unsigned int scale);

int main() {
	Image original = LoadImage("resources/flopping.png");
	ImageFormat(&original, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
	surface_complex r_old, g_old, b_old, a_old;
	r_old = LoadSurfaceFromImage(original, 'r');
	g_old = LoadSurfaceFromImage(original, 'g');
	b_old = LoadSurfaceFromImage(original, 'b');
	a_old = LoadSurfaceFromImage(original, 'a');
	UnloadImage(original);

	fft_2d(r_old);
	fft_2d(g_old);
	fft_2d(b_old);
	fft_2d(a_old);

	unsigned int scale = 1;
	surface_complex r_new, g_new, b_new, a_new;
	r_new = UpscaleSurface(r_old, scale);
	g_new = UpscaleSurface(g_old, scale);
	b_new = UpscaleSurface(b_old, scale);
	a_new = UpscaleSurface(a_old, scale);

	destroy_surface_complex(r_old);
	destroy_surface_complex(g_old);
	destroy_surface_complex(b_old);
	destroy_surface_complex(a_old);

	for (unsigned int i = 0; i < r_new.width * r_new.height; i++) {
		r_new.data[i] *= scale * scale;
		g_new.data[i] *= scale * scale;
		b_new.data[i] *= scale * scale;
		a_new.data[i] *= scale * scale;
	}

	ifft_2d(r_new);
	ifft_2d(g_new);
	ifft_2d(b_new);
	ifft_2d(a_new);

	Image upscaled = LoadImageFromSurface(r_new);
	UpdateImageFromSurface(upscaled, r_new, 'r');
	UpdateImageFromSurface(upscaled, g_new, 'g');
	UpdateImageFromSurface(upscaled, b_new, 'b');
	UpdateImageFromSurface(upscaled, a_new, 'a');
	ExportImage(upscaled, "upscaled.png");
	UnloadImage(upscaled);

	destroy_surface_complex(r_new);
	destroy_surface_complex(g_new);
	destroy_surface_complex(b_new);
	destroy_surface_complex(a_new);

	return 0;
}

surface_complex LoadSurfaceFromImage(Image image, char channel) {
	surface_complex surface = create_surface_complex(image.width, image.height);
	Color* pixel = (Color*) image.data;
	unsigned int pixelCount = image.width * image.height;
	for (unsigned int i = 0; i < pixelCount; i++) {
		switch (channel) {
		case 'r':
			surface.data[i] = pixel[i].r;
			break;
		case 'g':
			surface.data[i] = pixel[i].g;
			break;
		case 'b':
			surface.data[i] = pixel[i].b;
			break;
		case 'a':
			surface.data[i] = pixel[i].a;
			break;
		}
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

void UpdateImageFromSurface(Image image, surface_complex surface, char channel) {
	Color* pixel = (Color*) image.data;
	unsigned int pixelCount = surface.width * surface.height;
	for (unsigned int i = 0; i < pixelCount; i++) {
		float val = Clamp(cabsf(surface.data[i]), 0, 255);
		switch (channel) {
		case 'r':
			pixel[i].r = val;
			break;
		case 'g':
			pixel[i].g = val;
			break;
		case 'b':
			pixel[i].b = val;
			break;
		case 'a':
			pixel[i].a = val;
			break;
		}
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

