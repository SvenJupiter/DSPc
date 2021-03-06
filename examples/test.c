#include <stdio.h>
#include <math.h>

// DSP-Math
#include "DSP/Math/Polynomial.h"
#include "DSP/Math/Matrix.h"
#include "DSP/Math/Vector.h"

// DSP-Discrete
#include "DSP/Discrete/Signal.h"
#include "DSP/Discrete/zStateSpace.h"
#include "DSP/Discrete/Integrator.h"
#include "DSP/Discrete/Derivative.h"
#include "DSP/Discrete/pidController.h"
#include "DSP/Discrete/Discontinuous.h"



real_t step(const real_t t, const real_t Tstep) {
    return (t >= Tstep ? 1 : 0);
}

void show_polynomial(const dsp_poly_t* const p) {
    if (p == NULL) { printf("NULL-Polynomial\n"); }
    else if (p->a == NULL) { printf("NULL-Coeffs-Polynomial\n"); }
    else {
        for (size_t k = 0; k <= p->order; ++k) {
            if (k < p->order) { printf("%f * x^%lu + ", p->a[p->order - k], p->order - k); }
            else { printf("%f\n",  p->a[p->order - k]); }
        }
    }
}

void show_vector(const dsp_vector_t* const vec) {
    if (vec == NULL) { printf("NULL-Vector\n"); }
    else if (vec->elements == NULL) { printf("NULL-Elements-Vector\n"); }
    else if (vec->size == 0) { printf("0-Size-Vector\n"); }
    else {
        printf("{");
        for (size_t k = 0; k < vec->size; ++k) {
            if (k < vec->size - 1) { printf("%f, ", vec->elements[k]); }
            else { printf("%f", vec->elements[k]); }
        }
        printf("}\n");
    }
}

void show_matrix(const dsp_matrix_t* const mat) {
    if (mat == NULL) { printf("NULL-Matrix\n"); }
    else if (mat->elements == NULL) { printf("NULL-Elements-Matrix\n"); }
    else if (mat->rows == 0) { printf("0-Rows-Matrix\n"); }
    else if (mat->columns == 0) { printf("0-Columns-Matrix\n"); }
    else {
        printf("{\n");
        for (size_t i = 0; i < mat->rows; ++i) {
            for (size_t j = 0; j < mat->columns; ++j) {
                if (j < mat->columns -1) { printf("%12.6f, ", mat->elements[i * mat->columns + j]); }
                else { printf("%12.6f\n", mat->elements[i * mat->columns + j]); }
            }
        }
        printf("}\n");
    }
}

void show_signal(const dsp_signal_t* const signal) {
    if (signal == NULL) { printf("NULL-Signal\n"); }
    else if (signal->elements == NULL) { printf("NULL-Elements-Signal\n"); }
    else if (signal->size == 0) { printf("0-Size-Signal\n"); }
    else {
        printf("{");
        for (size_t k = 0; k < signal->size; ++k) {
            if (k < signal->size - 1) { printf("%f, ", signal->elements[k]); }
            else { printf("%f", signal->elements[k]); }
        }
        printf("}\n");
    }
}

void export_tuy(const char* const filename, const dsp_signal_t* const t, const dsp_signal_t* const u, const dsp_signal_t* const y) {
    if (t == NULL || u == NULL || y == NULL) { return; }
    if (t->size != u->size || u->size != y->size) { return; }

    // Open file
    FILE* file = fopen(filename, "w");

    // Write header
    fprintf(file, "t, u, y\n");

    for (size_t k = 0; k < t->size; ++k) {
        if (k < t->size - 1) {
            fprintf(file, "%f, %f, %f\n", t->elements[k], u->elements[k], y->elements[k]);
        }
        else {
            fprintf(file, "%f, %f, %f", t->elements[k], u->elements[k], y->elements[k]);
        }
    }

    // Close file
    fclose(file);
}

void export_tuxy(const char* const filename, const dsp_signal_t* const t, const dsp_signal_t* const u, const dsp_signal_t* const x, const dsp_signal_t* const y) {
    if (t == NULL || u == NULL || x== NULL || y == NULL) { return; }
    if (t->size != u->size || u->size != x->size || x->size != y->size) { return; }

    // Open file
    FILE* file = fopen(filename, "w");

    // Write header
    fprintf(file, "t, u, x, y\n");

    for (size_t k = 0; k < t->size; ++k) {
        if (k < t->size - 1) {
            fprintf(file, "%f, %f, %f, %f\n", t->elements[k], u->elements[k], x->elements[k], y->elements[k]);
        }
        else {
            fprintf(file, "%f, %f, %f, %f", t->elements[k], u->elements[k], x->elements[k], y->elements[k]);
        }
    }

    // Close file
    fclose(file);
}

void export_treuym(const char* const filename, 
    const dsp_signal_t* const t, const dsp_signal_t* const r, 
    const dsp_signal_t* const e, const dsp_signal_t* const u, 
    const dsp_signal_t* const y, const dsp_signal_t* const m) {

    if (t == NULL || r == NULL || e == NULL || u == NULL || y == NULL || m == NULL) { return; }
    if (t->size != r->size || r->size != e->size || e->size != u->size || u->size != y->size || y->size != m->size) { return; }

    // Open file
    FILE* file = fopen(filename, "w");

    // Write header
    fprintf(file, "t, r, e, u, y, m\n");

    for (size_t k = 0; k < t->size; ++k) {
        if (k < t->size - 1) {
            fprintf(file, "%f, %f, %f, %f, %f, %f\n", t->elements[k], r->elements[k], e->elements[k], u->elements[k], y->elements[k], m->elements[k]);
        }
        else {
            fprintf(file, "%f, %f, %f, %f, %f, %f", t->elements[k], r->elements[k], e->elements[k], u->elements[k], y->elements[k], m->elements[k]);
        }
    }

    // Close file
    fclose(file);
}

void export_trexuym(const char* const filename,
    const dsp_signal_t* const t, const dsp_signal_t* const r, const dsp_signal_t* const e,
    const dsp_signal_t* const x, const dsp_signal_t* const u,
    const dsp_signal_t* const y, const dsp_signal_t* const m) {

    if (t == NULL || r == NULL || e == NULL || x == NULL || u == NULL || y == NULL || m == NULL) { return; }
    if (t->size != r->size || r->size != e->size || e->size != x->size || x->size != u->size || u->size != y->size || y->size != m->size) { return; }

    // Open file
    FILE* file = fopen(filename, "w");

    // Write header
    fprintf(file, "t, r, e, x, u, y, m\n");

    for (size_t k = 0; k < t->size; ++k) {
        if (k < t->size - 1) {
            fprintf(file, "%f, %f, %f, %f, %f, %f, %f\n", t->elements[k], r->elements[k], e->elements[k], x->elements[k], u->elements[k], y->elements[k], m->elements[k]);
        }
        else {
            fprintf(file, "%f, %f, %f, %f, %f, %f, %f", t->elements[k], r->elements[k], e->elements[k], x->elements[k], u->elements[k], y->elements[k], m->elements[k]);
        }
    }

    // Close file
    fclose(file);
}







void test_polyfit() {
    const real_t x[] = {1, 2, -1, 0, 5};
    const real_t y[] = {0.2, 0.4, -3, -0.5, 0.6};

    dsp_poly_t* const p = dsp_polynomial_create_fit(2, x, y, 5);
    show_polynomial(p);

    dsp_polynomial_destroy(p);
}

void test_polyfit2() {
    const real_t x[] = {1, 2, -1, 0, 5};
    const real_t y[] = {0.2, 0.4, -3, -0.5, 0.6};

    // dsp_poly_t* const p = dsp_polynomial_create_fit(2, x, y, 5);
    // show_polynomial(p);

    dsp_matrix_t* const V = dsp_matrix_create_vandermonde(2, x, 5);
    show_matrix(V);
    dsp_matrix_destroy(V);

    // dsp_polynomial_destroy(p);
}

void test_solve() {
    // const real_t a[] =  {
    //      1.000000,     4.000000,     2.000000,
    //     -3.000000,    -5.000000,    -2.000000,
    //      1.000000,    -1.000000,     0.000000,
    // };

    // const real_t b[] =  {3, -2, 4};
    // const real_t x[] =  {1, -3, 7};

    const real_t a[] =  {
         1.000000,     4.000000,     2.000000,
        -3.000000,    -5.000000,    -2.000000,
         1.000000,    -1.000000,     0.000000,
    };

    const real_t b[] =  {1, -2, 4};
    // const real_t x[] =  {1, -3, 7};

    // Create a new Matrix
    dsp_matrix_t* const A = dsp_matrix_create_from_array(3, 3, a);

    // Create a new Vector
    dsp_vector_t* const B = dsp_vector_create_from_array(3, b);

    // Crate a new vector
    dsp_vector_t* const x = dsp_vector_create_solution(A, B);

    show_vector(x);

    dsp_matrix_destroy(A);
    dsp_vector_destroy(B);
    dsp_vector_destroy(x);
}

void test_solve2() {
    const real_t a[] =  {
         1.000000,     4.000000,     2.000000,
        -3.000000,    -5.000000,    -2.000000,
         1.000000,    -1.000000,     0.000000,
         1.000000,     7.000000,     4.000000,
        -9.000000,    -4.000000,     1.000000
    };

    const real_t b[] =  {1, -2, 4, 5, -2};

    // Create a new Matrix
    dsp_matrix_t* const A = dsp_matrix_create_from_array(5, 3, a);
    show_matrix(A);

    // Create a new Matrix
    dsp_matrix_t* const At = dsp_matrix_create_transpose(A);
    show_matrix(At);

    // Create a new Matrix
    dsp_matrix_t* const An = dsp_matrix_create_product(At, A);
    show_matrix(An);

    // Create a new Matrix
    dsp_matrix_t* const Ai = dsp_matrix_create_inv(An);
    show_matrix(Ai);

    // Create a new Matrix
    dsp_matrix_t* const Ap = dsp_matrix_create_product(Ai, At);
    show_matrix(Ap);

    // Create a new Vector
    dsp_vector_t* const B = dsp_vector_create_from_array(5, b);
    show_vector(B);

    // Crate a new vector
    dsp_vector_t* const x = dsp_vector_create_matrix_product(Ap, B);
    show_vector(x);

    dsp_matrix_destroy(A);
    dsp_matrix_destroy(At);
    dsp_matrix_destroy(An);
    dsp_matrix_destroy(Ai);
    dsp_matrix_destroy(Ap);
    dsp_vector_destroy(B);
    dsp_vector_destroy(x);
}

void test_solve3() {
    const real_t a[] =  {
         1.000000,     4.000000,     2.000000,
        -3.000000,    -5.000000,    -2.000000,
         1.000000,    -1.000000,     0.000000,
         1.000000,     7.000000,     4.000000,
        -9.000000,    -4.000000,     1.000000
    };

    const real_t b[] =  {1, -2, 4, 5, -2};

    // Create a new Matrix
    dsp_matrix_t* const A = dsp_matrix_create_from_array(5, 3, a);
    show_matrix(A);

    // Create a new Vector
    dsp_vector_t* const B = dsp_vector_create_from_array(5, b);
    show_vector(B);

    // Crate a new vector
    dsp_vector_t* const x = dsp_vector_create_solution(A, B);
    show_vector(x);

    dsp_matrix_destroy(A);
    dsp_vector_destroy(B);
    dsp_vector_destroy(x);
}

void test_state_space_pt1() {

    const real_t Ts = 0.01;
    const real_t duration = 20;
    const size_t n_samples = 1 + roundf(duration / Ts);

    // Create state space object
    dsp_zss_t* const pt1 = dsp_zss_create_pt1(2, 3, Ts, 0);

    // Create Signals
    dsp_signal_t* const t = dsp_signal_create(n_samples);
    dsp_signal_t* const u = dsp_signal_create(n_samples);
    dsp_signal_t* const y = dsp_signal_create(n_samples);

    real_t tk, uk, yk;
    for (size_t k = 0; k < n_samples; ++k) {
        tk = k * Ts;
        uk = 1;
        dsp_zss_update(pt1, &uk, &yk);

        dsp_signal_push_back(t, &tk);
        dsp_signal_push_back(u, &uk);
        dsp_signal_push_back(y, &yk);
    }

    // Export
    export_tuy("PT1-Test.csv", t, u, y);

    // Destroy
    dsp_zss_destroy(pt1);
    dsp_signal_destroy(t);
    dsp_signal_destroy(u);
    dsp_signal_destroy(y);
}

void test_state_space_hpf() {

    const real_t Ts = 0.1;
    const real_t duration = 20;
    const size_t n_samples = 1 + roundf(duration / Ts);

    // Create state space object
    dsp_zss_t* const hpf = dsp_zss_create_highpass_filter(1, 1, Ts, 0);

    // Create Signals
    dsp_signal_t* const t = dsp_signal_create(n_samples);
    dsp_signal_t* const u = dsp_signal_create(n_samples);
    dsp_signal_t* const y = dsp_signal_create(n_samples);

    real_t tk, uk, yk;
    for (size_t k = 0; k < n_samples; ++k) {
        tk = k * Ts;
        uk = 1;
        dsp_zss_update(hpf, &uk, &yk);

        dsp_signal_push_back(t, &tk);
        dsp_signal_push_back(u, &uk);
        dsp_signal_push_back(y, &yk);
    }

    // Export
    export_tuy("HPF-Test.csv", t, u, y);

    // Destroy
    dsp_zss_destroy(hpf);
    dsp_signal_destroy(t);
    dsp_signal_destroy(u);
    dsp_signal_destroy(y);
}

void test_state_space_di() {

    const real_t Ts = 0.01;
    const real_t duration = 6;
    const size_t n_samples = 1 + roundf(duration / Ts);

    // Create state space object
    dsp_zss_t* const hpf = dsp_zss_create_highpass_filter(1, 1, Ts, 0);
    dsp_zss_t* const di = dsp_zss_create_integrator(1, Ts, ForwardEuler, 0);

    // Create Signals
    dsp_signal_t* const t = dsp_signal_create(n_samples);
    dsp_signal_t* const u = dsp_signal_create(n_samples);
    dsp_signal_t* const x = dsp_signal_create(n_samples);
    dsp_signal_t* const y = dsp_signal_create(n_samples);

    real_t tk, uk, xk, yk;
    for (size_t k = 0; k < n_samples; ++k) {
        tk = k * Ts;
        uk = 1;
        dsp_zss_update(hpf, &uk, &xk);
        dsp_zss_update(di, &xk, &yk);

        dsp_signal_push_back(t, &tk);
        dsp_signal_push_back(u, &uk);
        dsp_signal_push_back(x, &xk);
        dsp_signal_push_back(y, &yk);
    }

    // Export
    export_tuxy("DI-Test.csv", t, u, x, y);

    // Destroy
    dsp_zss_destroy(hpf);
    dsp_zss_destroy(di);
    dsp_signal_destroy(t);
    dsp_signal_destroy(u);
    dsp_signal_destroy(x);
    dsp_signal_destroy(y);
}

void test_state_space_dd() {

    const real_t Ts = 0.01;
    const real_t duration = 2;
    const size_t n_samples = 1 + roundf(duration / Ts);

    // Create state space object
    dsp_zss_t* const dd = dsp_zss_create_derivative(2, 0.2, Ts, ForwardEuler, 0);

    // Create Signals
    dsp_signal_t* const t = dsp_signal_create(n_samples);
    dsp_signal_t* const u = dsp_signal_create(n_samples);
    dsp_signal_t* const y = dsp_signal_create(n_samples);

    real_t tk, uk, yk;
    for (size_t k = 0; k < n_samples; ++k) {
        tk = k * Ts;
        uk = 1; // 1.5 * tk;
        dsp_zss_update(dd, &uk, &yk);

        dsp_signal_push_back(t, &tk);
        dsp_signal_push_back(u, &uk);
        dsp_signal_push_back(y, &yk);
    }

    // Export
    export_tuy("DD-Test.csv", t, u, y);

    // Destroy
    dsp_zss_destroy(dd);
    dsp_signal_destroy(t);
    dsp_signal_destroy(u);
    dsp_signal_destroy(y);
}


void test_quantizer() {

    const real_t Ts = 0.0001;
    const real_t duration = 1;
    const size_t n_samples = 1 + roundf(duration / Ts);

    // Create Quantizer
    const size_t precision = 3;
    dsp_quantization_t* const quantizer = dsp_rounding_create(precision, RoundMath);

    // Create Signals
    dsp_signal_t* const t = dsp_signal_create(n_samples);
    dsp_signal_t* const u = dsp_signal_create(n_samples);
    dsp_signal_t* const y = dsp_signal_create(n_samples);

    real_t tk, uk, yk;
    for (size_t k = 0; k < n_samples; ++k) {
        tk = k * Ts;
        uk = tk;
        yk = dsp_quantizer_update(quantizer, uk);
        // yk = powf(10, (-1) * ((real_t) precision));
        // yk = quantizer->interval;

        dsp_signal_push_back(t, &tk);
        dsp_signal_push_back(u, &uk);
        dsp_signal_push_back(y, &yk);
    }

    // Export
    export_tuy("Quantizer-Test.csv", t, u, y);


    // Destroy
    dsp_quantizer_destroy(quantizer);
    dsp_signal_destroy(t);
    dsp_signal_destroy(u);
    dsp_signal_destroy(y);

}

void test_schmitt_quantizer() {

    const real_t f = 1; // Hz
    const real_t w = 2 * M_PI * f;
    const real_t Ts = 0.0001;
    const real_t duration = 2;
    const size_t n_samples = 1 + roundf(duration / Ts);

    // Create Quantizer
    const size_t precision = 3;
    dsp_schmitt_quantization_t* const quantizer = dsp_schmitt_quantizer_create_relative(0.0f, 1.0f, 0.5, 0.3, 0.0f);

    // Create Signals
    dsp_signal_t* const t = dsp_signal_create(n_samples);
    dsp_signal_t* const u = dsp_signal_create(n_samples);
    dsp_signal_t* const y = dsp_signal_create(n_samples);

    real_t tk, uk, yk;
    for (size_t k = 0; k < n_samples; ++k) {
        tk = k * Ts;
        uk = 1.1 * sinf(w * tk);;
        yk = dsp_schmitt_quantizer_update(quantizer, uk);

        dsp_signal_push_back(t, &tk);
        dsp_signal_push_back(u, &uk);
        dsp_signal_push_back(y, &yk);
    }

    // Export
    export_tuy("Schmitt-Quantizer-Test.csv", t, u, y);


    // Destroy
    dsp_schmitt_quantizer_destroy(quantizer);
    dsp_signal_destroy(t);
    dsp_signal_destroy(u);
    dsp_signal_destroy(y);


}

void integrator_test() {

    const real_t K = 1.5;
    const real_t Ts = 0.1;
    const bool limit = true;
    const real_t UL = 1;
    const real_t LL = 0;
    const s_approximation_t IF = ForwardEuler;
    const real_t duration = 4;
    const size_t n_samples = 1 + roundf(duration / Ts);

    // Create Integrator
    dsp_integrator_t* integrator = dsp_integrator_create(K, Ts, IF, limit, UL, LL);

    // Create Signals
    dsp_signal_t* const t = dsp_signal_create(n_samples);
    dsp_signal_t* const u = dsp_signal_create(n_samples);
    dsp_signal_t* const x = dsp_signal_create(n_samples);
    dsp_signal_t* const y = dsp_signal_create(n_samples);

    real_t tk, uk, xk, yk;
    for (size_t k = 0; k < n_samples; ++k) {
        tk = k * Ts;


        uk = step(tk, 1) - step(tk, 2) - step(tk, 2) + step(tk, 4);
        xk = dsp_integrator_get_state(integrator);
        yk = dsp_integrator_update(integrator, uk);


        dsp_signal_push_back(t, &tk);
        dsp_signal_push_back(u, &uk);
        dsp_signal_push_back(x, &xk);
        dsp_signal_push_back(y, &yk);
    }

    // Export
    export_tuxy("Integrator-Test.csv", t, u, x, y);

    dsp_signal_destroy(t);
    dsp_signal_destroy(u);
    dsp_signal_destroy(x);
    dsp_signal_destroy(y);
}

void derivative_test() {

    const real_t K = 5;
    const real_t N = 5;
    const real_t Ts = 0.1;
    const real_t UL = 1;
    const real_t LL = 0;
    const s_approximation_t DF = ForwardEuler;
    const real_t duration = 10;
    const size_t n_samples = 1 + roundf(duration / Ts);

    // Create Derivative
    dsp_derivative_t* derivative = dsp_derivative_create(K, N, Ts, DF, true, UL, LL);

    // Create Signals
    dsp_signal_t* const t = dsp_signal_create(n_samples);
    dsp_signal_t* const u = dsp_signal_create(n_samples);
    dsp_signal_t* const y = dsp_signal_create(n_samples);

    real_t tk, uk, yk = 0;
    for (size_t k = 0; k < n_samples; ++k) {
        tk = k * Ts;
        uk = step(tk, 1);
        yk = dsp_derivative_update(derivative, uk);

        dsp_signal_push_back(t, &tk);
        dsp_signal_push_back(u, &uk);
        dsp_signal_push_back(y, &yk);
    }

    // Export
    export_tuy("Derivative-Test.csv", t, u, y);

    dsp_signal_destroy(t);
    dsp_signal_destroy(u);
    dsp_signal_destroy(y);
}

void test_pid() {

    // Simulation Time
    const real_t Ts = 0.1f;
    const real_t duration = 10;
    const size_t n_samples = 1 + roundf(duration / Ts);

    // PID -Parameter
    const real_t Kp = 0.35f;
    const real_t Ki = 1.5f;
    const real_t Kd = 0.0f;
    const real_t N = 0.5f / Ts;

    // Saturation
    const bool LimitOutput = true;
    const real_t upper_limit = 1;
    const real_t lower_limit = 0;

    // Rate Limitation
    const bool LimitRate = true;
    const real_t rising_slew_rate = 0.1;
    const real_t falling_slew_rate = -0.1;

    // Anti-Windup
    const bool anti_windup_enabled = true;
    const real_t Kb = 2 * Ki;

    // Tracking
    const bool tracking_enabled = false;
    const real_t Kt = 1;

    // Create pid object
    dsp_pid_t* const pid = dsp_pid_create_controller(Ts, Kp, Ki, Kd, N);
    dsp_pid_set_output_saturation(pid, LimitOutput, upper_limit, lower_limit);
    dsp_pid_set_output_rate_limitation(pid, LimitRate, rising_slew_rate, falling_slew_rate);
    dsp_pid_set_anti_windup(pid, anti_windup_enabled, Kb);
    dsp_pid_set_tracking(pid, tracking_enabled, Kt);

    // Create Signals
    dsp_signal_t* const t = dsp_signal_create(n_samples);
    dsp_signal_t* const u = dsp_signal_create(n_samples);
    dsp_signal_t* const x = dsp_signal_create(n_samples);
    dsp_signal_t* const y = dsp_signal_create(n_samples);

    real_t tk = 0, uk = 0, xk = 0, yk = 0, temp = 0;
    for (size_t k = 0; k < n_samples; ++k) {
        tk = k * Ts;
        uk = step(tk, 1) - step(tk, 2) - step(tk, 2) + step(tk, 4);
        temp = pid->pidsum;
        yk = dsp_pid_update(pid, uk);


        dsp_signal_push_back(t, &tk);
        dsp_signal_push_back(u, &uk);
        // dsp_signal_push_back(x, &xk);
        dsp_signal_push_back(y, &yk);
        dsp_signal_push_back(x, &temp);

        // yk = (yk > 1 ? 1 : (yk < 0 ? 0 : yk));
        // yk = (tk >= 5 ? 0.5 : yk);
        // temp = pid->Output;


    }

    // Export
    export_tuxy("PID-Test.csv", t, u, x, y);

    // Destroy
    dsp_pid_destroy(pid);
    dsp_signal_destroy(t);
    dsp_signal_destroy(u);
    dsp_signal_destroy(x);
    dsp_signal_destroy(y);
}




int main() {

    printf("Hello World!\n");

    // test_polyfit();
    // test_polyfit2();
 
    // test_solve();
    // test_solve2();
    // test_solve3();

    // test_state_space_pt1();
    // test_state_space_hpf();
    // test_state_space_di();
    // test_state_space_dd();

    // test_quantizer();
    // test_schmitt_quantizer();

    // integrator_test();
    // derivative_test();

    test_pid();

    printf("Bye bye...\n");
}