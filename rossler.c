/*
 * rossler_cairo.c
 * Copyright (C) Farooq Mela 2003-2013.
 *
 * Traces the path of a Rossler attractor.
 * Duffing attractors are defined by 3 coupled differential equations,
 *
 * dx/dt = -(y + z)
 * dy/dt = x + a * y
 * dz/dt = b + z * (x - c)
 *
 * Common constants {a,b,c}: {0.2,0.2,5.7}
 *
 * 06-02-03
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <cairo/cairo.h>

#include "rk4.h"

// Constants.
const double a = 0.2, b = 0.2, c = 5.7;

void f(double t, const double y[], double dy[]) {
    (void)t;
    dy[0] = -(y[1] + y[2]);
    dy[1] = y[0] + a * y[1];
    dy[2] = b + y[2] * (y[0] - c);
}

int
main(void)
{
    const int kWidth = 1200, kHeight = 1200;
    const double kXLeft = -10, kXRight = 12;
    const double kYBottom = -12, kYTop = 10;
    const double h = 0.01;
    const int kIterations = 1000000;
    const char kOutput[] = "rossler.png";

    clock_t start = clock();

#define N   3
    // Initial condition.
    double y[N] = { 0.1, 0.1, 0.1 };
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

    cairo_set_antialias(cr, CAIRO_ANTIALIAS_GOOD);

    // Blank to white.
    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1.0);
    cairo_paint(cr);

    cairo_set_line_width(cr, 0.002);

    cairo_scale(cr, kWidth / (kXRight - kXLeft), kHeight / (kYBottom - kYTop));
    cairo_translate(cr,
		    10.0,
		    -10.0);

    // Trace the attractor.
    for (int i=0; i<kIterations; ++i) {
	// Compute next position.
	double y_next[N];
	rk4v(N, f, i * h, h, y, y_next);
	for (int j = 0; j < N; ++j) {
	    if (min[j] > y_next[j]) min[j] = y_next[j]; else
	    if (max[j] < y_next[j]) max[j] = y_next[j];
	}
	// Draw.
	cairo_set_source_rgba(cr, 1., 1., 1., 1.);
	cairo_move_to(cr, y[0], y[1]);
	cairo_line_to(cr, y_next[0], y_next[1]);
	cairo_stroke(cr);
	// Update position.
	for (int j = 0; j < N; ++j)
	    y[j] = y_next[j];
    }
    clock_t render_finish = clock();
    printf("Rendering: %.2fs\n",
	   (render_finish - start) / (double)CLOCKS_PER_SEC);

    printf("x={%.2f,%.2f} y={%.2f,%.2f} z={%.2f,%.2f}\n",
	   min[0], max[0], min[1], max[1], min[2], max[2]);

    cairo_status_t status = cairo_surface_write_to_png(cr_surf, kOutput);
    if (status != CAIRO_STATUS_SUCCESS) {
	fprintf(stderr, "Failed to save to %s: %s\n",
		kOutput, cairo_status_to_string(status));
	exit(1);
    }

    cairo_surface_destroy(cr_surf);
    cairo_destroy(cr);

    clock_t save_finish = clock();
    printf("Saving to %s: %.2fs\n", kOutput,
	   (save_finish - render_finish) / (double)CLOCKS_PER_SEC);

    exit(0);
}
