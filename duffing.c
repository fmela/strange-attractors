/*
 * duffing_cairo.c
 *
 * Traces the path of a Duffing attractor.
 * Duffing attractors are defined by 2 coupled differential equations,
 *
 * dx/dt = y
 * dy/dt = x - x^3 - a*y + b*cos(w*t)
 *
 * Common constants {a,b,w}: {0.25,0.3,1}
 *
 * 06-02-03
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "rk4.h"

#include <cairo/cairo.h>

// Parameters. Usual parameters {a,b,w} = {0.25, 0.3, 1}
const double a = 0.25;
const double b = 0.3;
const double w = 1.0;
const double h = 0.03;

void f(double t, const double y[], double dy[]) {
    dy[0] = y[1];
    dy[1] = y[0] * (1.0 - y[0] * y[0]) - a * y[1] + b * cos(w * t);
}

int
main(void)
{
    const char kOutput[] = "duffing.png";
    const int kWidth = 1350, kHeight = 900;
    const double kXLeft = -1.5, kXRight = 1.5;
    const double kYBottom = -1.0, kYTop = 1.0;
    const int kIterations = 200000;

    clock_t start = clock();

#define N   2
    // Initial condition.
    double y[N] = { 0.1, 0.1 };
    // bounds
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

    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
    cairo_paint(cr);

    cairo_set_line_width(cr, 0.002);

    cairo_scale(cr, kWidth / (kXRight - kXLeft), kHeight / (kYBottom - kYTop));
    cairo_translate(cr, kXRight, kYBottom);

    // Trace the attractor.
    for (int i=0; i<kIterations; i++) {
	const double t = i * h;
	double y_next[N];
	rk4v(N, f, t, h, y, y_next);

	// Update bounds.
	for (int j = 0; j < N; ++j) {
	    if (min[j] > y_next[j]) min[j] = y_next[j]; else
	    if (max[j] < y_next[j]) max[j] = y_next[j];
	}

	float intensity = fabs(sin(t * 2 * M_PI));
	cairo_set_source_rgba(cr, 1.0-intensity, 0, intensity, 0.4);

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

    printf("x={%.2f,%.2f} y={%.2f,%.2f}\n", min[0], max[0], min[1], max[1]);

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
