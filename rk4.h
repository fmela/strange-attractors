/*
 * rk4.h
 * Copyright (C) Farooq Mela 2003-2013.
 */

#ifndef _RK4_H_
#define _RK4_H_

/* Fourth-order Runge-Kutta step, using array of functions.
 *
 * Input parameters:
 *   n: number of equations
 *   f: array of pointers to differential equations
 *   t: current time parameter
 *   h: time step or delta
 *   y: value of y at time t
 * Output parameter:
 *   y_next: value of y at time t + h
 */
void
rk4(const int n, double (*f[])(double t, const double y[]),
    double t, double h, const double y[], double y_next[])
{
    /* k1 = h * f(t, y) */
    double k[n];
    for (int i = 0; i < n; ++i) {
	k[i] = h * (*f[i])(t, y);
	y_next[i] = k[i];
    }

    /* k2 = h * f(t + h/2, y + k1/2) */
    double temp[n];
    for (int i = 0; i < n; ++i)
	temp[i] = y[i] + k[i]/2;
    for (int i = 0; i < n; ++i) {
	k[i] = h * (*f[i])(t + h/2, temp);
	y_next[i] += k[i] * 2;
    }

    /* k3 = h * f(t + h/2, y + k2/2) */
    for (int i = 0; i < n; ++i)
	temp[i] = y[i] + k[i]/2;
    for (int i = 0; i < n; ++i) {
	k[i] = h * (*f[i])(t + h/2, temp);
	y_next[i] += k[i] * 2;
    }

    /* k4 = h * f(t + h, y + k3) */
    for (int i = 0; i < n; ++i)
	temp[i] = y[i] + k[i];
    for (int i = 0; i < n; ++i) {
	k[i] = h * (*f[i])(t + h, temp);
	y_next[i] += k[i];
    }

    /* y_next = y + (k1 + 2(k2 + k3) + k4) / 6 */
    for (int i = 0; i < n; ++i)
	y_next[i] = y[i] + y_next[i] / 6.;
}

/* Fourth-order Runge-Kutta step, using vector-valued function.
 *
 * Input parameters:
 *   n: number of equations
 *   f: pointer to vector-valued differential equation
 *   t: current time parameter
 *   h: time step or delta
 *   y: value of y at time t
 * Output parameter:
 *   y_next: value of y at time t + h
 */
void
rk4v(const int n, void (*f)(double t, const double y[], double dy[]),
     double t, double h, const double y[], double y_next[])
{
    /* k1 = h * f(t, y) */
    double k[n];
    f(t, y, k);
    for (int i = 0; i < n; ++i) {
	k[i] *= h;
	y_next[i] = k[i];
    }

    /* k2 = h * (t + h/2, y + k1/2) */
    double temp[n];
    for (int i = 0; i < n; ++i)
	temp[i] = y[i] + k[i]/2;
    f(t + h/2, temp, k);
    for (int i = 0; i < n; ++i) {
	k[i] *= h;
	y_next[i] += k[i] * 2;
    }

    /* k3 = h * (t + h/2, y + k2/2) */
    for (int i = 0; i < n; ++i)
	temp[i] = y[i] + k[i]/2;
    f(t + h/2, temp, k);
    for (int i = 0; i < n; ++i) {
	k[i] *= h;
	y_next[i] += k[i] * 2;
    }

    /* k4 = h * (t + h, y + k3) */
    for (int i = 0; i < n; ++i)
	temp[i] = y[i] + k[i];
    f(t + h, temp, k);
    for (int i = 0; i < n; ++i) {
	k[i] *= h;
	y_next[i] += k[i];
    }

    /* y_next = y + (k1 + 2(k2 + k3) + k4) / 6 */
    for (int i = 0; i < n; ++i)
	y_next[i] = y[i] + y_next[i] / 6.;
}

#endif
