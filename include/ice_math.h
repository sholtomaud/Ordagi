#ifndef ICE_MATH_H
#define ICE_MATH_H

/*
 * TIER 1 — Cardinal Approximation
 *
 * Constants (fixed — not tuning parameters):
 *   ICE_STEPS = 100   sufficient for convergence in static graph evaluation
 *   ICE_DT    = 0.1   fixed time step
 *   ICE_K     = 0.1   decay coefficient
 */
#define ICE_STEPS 100
#define ICE_DT    0.1
#define ICE_K     0.1

/*
 * calculate_fractional_weights()
 *
 * Grünwald-Letnikov weight recurrence for fractional derivative order alpha.
 * Correctly models memory decay.
 */
static inline void calculate_fractional_weights(double *w, double alpha, int steps) {
    w[0] = 1.0;
    for (int j = 1; j < steps; j++) {
        w[j] = w[j - 1] * (1.0 - (alpha + 1.0) / j);
    }
}

/*
 * compute_quality_tier1()
 *
 * Returns the concept's Tier 1 quality score based on cardinality and alpha.
 */
double compute_quality_tier1(double cardinality, double alpha);

#endif /* ICE_MATH_H */
