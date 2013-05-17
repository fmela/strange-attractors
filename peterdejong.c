/*
 * peterdejong_cairo.c
 * Copyright (C) Farooq Mela 2013.
 *
 * Traces the path of a Peter de Jong attractor.
 *
 * x' = sin(a * y) - cos(b * x)
 * y' = sin(c * x) - cos(d * y)
 *
 * 08-15-12
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <cairo/cairo.h>

// Constants.
// const double a = 1.4, b = -2.3, c = 2.4, d = -2.1;
// const double a = 2.01, b = -2.53, c = 1.61, d = -0.33;
// const double a = -2.7, b = -0.09, c = -0.86, d = -2.2;
// const double a = -2.24, b = 0.43, c = -0.65, d = -2.43;
const double a = -2, b = -2, c = -1.2, d = 2;

void f(const double y[], double next[]) {
    next[0] = sin(a * y[1]) - cos(b * y[0]);
    next[1] = sin(c * y[0]) - cos(d * y[1]);
}

int
main(void)
{
    const int kWidth = 900, kHeight = 900;
    const double kXLeft = -2.1, kXRight = 2.1;
    const double kYBottom = -2.1, kYTop = 2.1;
    const int kIterations = 10000000;
    const char kOutput[] = "images/peterdejong.png";

    clock_t start = clock();

#define N   2
    // Initial condition.
    double y[N] = { 0.5, 0.5 };
    // Initialize bounding box.
    double min[N], max[N];
    for (int j = 0; j < N; ++j)
	min[j] = max[j] = y[j];

    cairo_surface_t *cr_surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
							  kWidth, kHeight);
    cairo_t *cr = cairo_create(cr_surf);
    if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
	fprintf(stderr, "Cairo: %s\n",
		cairo_status_to_string(cairo_status(cr)));
	exit(1);
    }

    // cairo_set_antialias(cr, CAIRO_ANTIALIAS_GOOD);

    // Clear canvas.
    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1.0);
    cairo_paint(cr);

    cairo_set_line_width(cr, 0.002);

    cairo_scale(cr,
		kWidth / (kXRight - kXLeft),
		kHeight / (kYBottom - kYTop));
    cairo_translate(cr, kXRight, kYBottom);

    // Trace the attractor.
    for (int i=0; i<kIterations; ++i) {
	// Compute next.
	double y_next[N];
	f(y, y_next);
	// Update bounding box.
	for (int j = 0; j < 3; ++j) {
	    if (min[j] > y_next[j]) min[j] = y_next[j]; else
	    if (max[j] < y_next[j]) max[j] = y_next[j];
	}

	cairo_set_source_rgba(cr, 1., 1., 1., 1.);
	cairo_move_to(cr, y_next[0], y_next[1]);
	cairo_line_to(cr, y_next[0]+.001, y_next[1]+.001);

	// Update position.
	for (int j = 0; j < N; ++j)
	    y[j] = y_next[j];
    }
    cairo_stroke(cr);
    clock_t render_finish = clock();
    printf("Rendering: %.2fs\n",
	   (render_finish - start) / (double)CLOCKS_PER_SEC);

    printf("x={%.2f,%.2f} y={%.2f,%.2f}\n",
	   min[0], max[0], min[1], max[1]);

    cairo_status_t status = cairo_surface_write_to_png(cr_surf, kOutput);
    if (status != CAIRO_STATUS_SUCCESS) {
	fprintf(stderr, "Failed to save to %s: %s\n",
		kOutput, cairo_status_to_string(status));
	exit(1);
    }

    cairo_surface_destroy(cr_surf);
    cairo_destroy(cr);

    clock_t save_finish = clock();
    printf("Saving to PNG: %.2fs\n",
	   (save_finish - render_finish) / (double)CLOCKS_PER_SEC);

    exit(0);
}
