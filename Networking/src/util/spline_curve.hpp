#pragma once

#include <vector>

namespace cgvkp {
namespace util {

template<typename ValType, typename ParamType, int Degree>
class spline_curve {
public:

    // Empty constructor
    spline_curve() {}

    // Constructor that initializes the curve
    spline_curve(const std::vector<ValType>& control_points) {
        set_control_points(control_points);
    }

    // Set control points and init knot vector
    void set_control_points(const std::vector<ValType>& control_points) {
        this->control_points = control_points;
        calculate_knot_vector();
    }

    // Evaluate curve
    ValType evaluate(ParamType t) const {
        // Boundary conditions
        if (t <= 0.0f) return control_points[0];
        if (t >= 1.0f) return control_points.back();

        // Calculate point as weighted sum of all control points
        ValType result;
        for (unsigned int i = 0; i < control_points.size(); ++i) {
            result += evaluate_basis(i, t) * control_points[i];
        }

        return result;
    }

    // Create derivative curve
    spline_curve<ValType, ParamType, Degree - 1> create_derivative() const {
        unsigned int num_derived_points = control_points.size() - 1;
        std::vector<ValType> derived_control_points(num_derived_points);

        for (unsigned int i = 0; i < num_derived_points; ++i) {
            ParamType weight = static_cast<ParamType>(Degree - 1);
            if (i == 0) {
                weight /= knot_vector[Degree + 1];
            } else {
                if (i == num_derived_points - 1) {
                    weight /= 1.0f - knot_vector[knot_vector.size() - Degree - 2];
                } else {
                    weight /= (knot_vector[i + Degree + 1] - knot_vector[i + 1]);
                }
            }
            derived_control_points[i] = weight * (control_points[i + 1] - control_points[i]);
        }
        // Return new curve
        return spline_curve<ValType, ParamType, Degree - 1>(derived_control_points);
    }

private:
    std::vector<ValType> control_points;
    std::vector<ParamType> knot_vector;

    // Calculate knot vector
    void calculate_knot_vector() {
        size_t K = control_points.size();

        knot_vector.clear();
        knot_vector.resize(K + Degree + 1);

        for (size_t i = 0; i <= Degree; i++) knot_vector[i] = 0.0f;

        for (size_t i = Degree + 1; i <= K; i++) {
            knot_vector[i] = static_cast<ParamType>(i - Degree) / static_cast<ParamType>(K - Degree);
        }

        for (size_t i = K + 1; i < K + Degree + 1; i++) knot_vector[i] = 1.0f;
    }

    // Evaluate the basis function for control point i at t with degree g
    ParamType evaluate_basis(unsigned int point_id, ParamType t, int g = Degree) const {
        // Recursion termination
        if (g == 0) {
            if (t >= knot_vector[point_id] && t < knot_vector[point_id + 1]) return static_cast<ParamType>(1);
            return static_cast<ParamType>(0);
        }

        ParamType a = knot_vector[point_id + g] - knot_vector[point_id];
        if (a != 0.0f)
            a = (t - knot_vector[point_id]) / a;

        ParamType b = knot_vector[point_id + g + 1] - knot_vector[point_id + 1];
        if (b != 0.0f) b = (knot_vector[point_id + g + 1] - t) / b;

        return a * evaluate_basis(point_id, t, g - 1)
            + b * evaluate_basis(point_id + 1, t, g - 1);
    }

};

}
}
