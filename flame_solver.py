#!/usr/bin/env python3

from math import pi, sin, cos, exp
from numpy import arange
from collections import namedtuple

Result = namedtuple("Result", ["p1", "p2", "error"])

def main():
    # f(x) = p1 * sin(p2 * x)

    parameter_sin_1=2
    parameter_sin_2=0.5

    transition_point = (1/parameter_sin_2) * pi * 0.75

    sin_value = parameter_sin_1 * sin(parameter_sin_2 * transition_point)

    derived_sin_value = parameter_sin_1 * parameter_sin_2 * cos(parameter_sin_2 * transition_point)

    #g(x) = e^(p1 - p2 * x)
    result = Result(10, 10, 1)
    step = 10
    while (result.error > 0.0001):
        parameter_ex_p1 = list(arange(result.p1 - step, result.p1 + step, step/10))
        parameter_ex_p2 = list(arange(result.p2 - step, result.p2 + step, step/100))
        step /= 10

        result = calculate(parameter_ex_p1, parameter_ex_p2, transition_point, sin_value, derived_sin_value)

    print("transition point: " + str(transition_point))
    print("p1: " + str(result.p1))
    print("p2: " + str(result.p2))
    print("combined error: " + str(result.error))

def calculate(parameter_ex_p1, parameter_ex_p2, transition_point, sin_value, derived_sin_value):
    best_parameter_ex_p1 = -1
    best_parameter_ex_p2 = -1
    best_combined_error = 100
    for p1 in parameter_ex_p1:
        for p2 in parameter_ex_p2:
                value = exp(p1 - p2 * transition_point)
                derived_value = - p2 * exp(p1 - p2 * transition_point)
                error = abs(value - sin_value)
                derived_error = abs(derived_value - derived_sin_value)
                combined_error = error + derived_error
                if combined_error < best_combined_error:
                    best_parameter_ex_p1 = p1
                    best_parameter_ex_p2 = p2
                    best_combined_error = combined_error
    return Result(best_parameter_ex_p1, best_parameter_ex_p2, best_combined_error)

if __name__ == "__main__":
    main()
