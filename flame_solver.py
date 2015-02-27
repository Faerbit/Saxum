#!/usr/bin/env python3

from math import pi, sin, cos, exp
from numpy import arange

# f(x) = p1 * sin(p2 * x)

parameter_sin_1=3
parameter_sin_2=0.3

transition_point = (1/parameter_sin_2) * pi * 0.75

sin_value = parameter_sin_1 * sin(parameter_sin_2 * transition_point) 

derived_sin_value = parameter_sin_1 * parameter_sin_2 * cos(parameter_sin_2 * transition_point)

#g(x) = e^(p1 - p2 * x)
parameter_ex_p1 = list(arange(0,10, 0.01))
parameter_ex_p2 = list(arange(0, 1, 0.00001))

best_parameter_ex_p1 = -1
best_parameter_ex_p2 = -1
value = exp(parameter_ex_p1[0] - parameter_ex_p2[0] * transition_point)
derived_value = - parameter_ex_p2[0] * exp(parameter_ex_p1[0] - parameter_ex_p2[0] * transition_point)
value_error = abs(value - sin_value)
derived_value_error = abs(derived_value - derived_sin_value)

for p1 in parameter_ex_p1:
    for p2 in parameter_ex_p2:
            value = exp(p1 - p2 * transition_point)
            derived_value = - p2 * exp(p1 - p2 * transition_point)
            if abs(value - sin_value) < value_error and abs(derived_value - derived_sin_value) < derived_value_error:
                value_error = abs(value -sin_value)
                derived_value_error = abs(derived_value - derived_sin_value)
                best_parameter_ex_p1 = p1
                best_parameter_ex_p2 = p2

print("p1: " + str(best_parameter_ex_p1))
print("p2: " + str(best_parameter_ex_p2))
print("value error: " + str(value_error))
print("derived value error: " + str(derived_value_error))
