from __future__ import print_function
import numpy as np
import math
import poisson
import array
from random import uniform

def main():
    # user defined options
    disk = True                # this parameter defines if we look for Poisson-like distribution on a disk/sphere (center at 0, radius 1) or in a square/box (0-1 on x and y)
    repeatPattern = False        # this parameter defines if we look for "repeating" pattern so if we should maximize distances also with pattern repetitions
    num_points = 5              # number of points we are looking for
    num_iterations = 4          # number of iterations in which we take average minimum squared distances between points and try to maximize them
    first_point_zero = False     # should be first point zero (useful if we already have such sample) or random
    iterations_per_point = 128  # iterations per point trying to look for a new point with larger distance
    sorting_buckets = 0         # if this option is > 0, then sequence will be optimized for tiled cache locality in n x n tiles (x followed by y)
    num_dim = 3                 # 1, 2, 3 dimensional version
    num_rotations = 1           # number of rotations of pattern to check against


    poisson_generator = poisson.PoissonGenerator(num_dim, disk, repeatPattern, first_point_zero)
    points = poisson_generator.find_point_set(num_points, num_iterations, iterations_per_point, num_rotations)
    print(points)
    print("")
    print("")
    print("")
    #normalize vectors
    norm_points = []
    for vector in points:
        norm_points.append(vector/np.linalg.norm(vector))
    print(norm_points)
    #ensure all vectors are pointing upwards
    final_points = []
    for vector in norm_points:
        if np.dot(vector, [0,1,0]) < 0:
            final_points.append(-vector)
        else:
            final_points.append(vector)
    print("")
    print("")
    print("")
    print(final_points)
    print("")
    print("")
    print("")
    # format output
    print("const int starCount = {};".format(num_points))
    print("vec3 starPositions[starCount] = vec3[](")
    for i, vector in enumerate(final_points):
        if (i == len(final_points)-1):
            print("    vec3({}, {}, {})".format(vector[0], vector[1], vector[2]))
        else:
            print("    vec3({}, {}, {}),".format(vector[0], vector[1], vector[2]))
    print(");")
    print("")
    # generate star sizes
    sizes = []
    for i in range(num_points):
        sizes.append(uniform(0.3, 1.5))

    print("float starSizes[starCount] = float[](")
    for i, size in enumerate(sizes):
        if (i==len(sizes)):
            print("    float({})".format(size))
        else:
            print("    float({}),".format(size))
    print(");")


if __name__ == '__main__':
    main()
