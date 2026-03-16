#include "ice_math.h"

double compute_quality_tier1(double cardinality, double alpha) {
    return cardinality * (1.0 / (alpha + 0.1)); // Simple stub for Tier 1
}
