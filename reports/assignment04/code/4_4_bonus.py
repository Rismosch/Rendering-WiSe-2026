import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle

N = 64

#===============================================================================
# star discrepancy
#===============================================================================
def calculate_star_discrepancy(dataset):
    print("find star discrepancy...")

    boundaries_x = [0, 1]
    boundaries_y = [0, 1]

    for p in dataset:
        boundaries_x.append(p[0])
        boundaries_y.append(p[1])
    
    max_discrepancy = -1
    for x in boundaries_x:
        for y in boundaries_y:
            aabb_min_x = 0
            aabb_min_y = 0
            aabb_max_x = x
            aabb_max_y = y

            aabb_area = (aabb_max_x - aabb_min_x) * (aabb_max_y - aabb_min_y)
            points_inside = 0

            for q in dataset:    
                x = q[0]
                y = q[1]

                if aabb_min_x <= x and x <= aabb_max_x and aabb_min_y <= y and y <= aabb_max_y:
                    points_inside += 1

            discrepancy = abs(aabb_area - points_inside / N)
            if discrepancy > max_discrepancy:
                max_discrepancy = discrepancy
                max_aabb_min = [aabb_min_x, aabb_min_y]
                max_aabb_max = [aabb_max_x, aabb_max_y]
        
    print(f"found: {max_discrepancy} aabb: {max_aabb_min} {max_aabb_max}")
    return (max_aabb_min, max_aabb_max)

#===============================================================================
# extreme discrepancy
#===============================================================================
def calculate_extreme_discrepancy(dataset):
    print("find extreme discrepancy...")
    print("this may take a while, since we check every boundary combination...")

    boundaries_x = [0, 1]
    boundaries_y = [0, 1]

    for p in dataset:
        boundaries_x.append(p[0])
        boundaries_y.append(p[1])

    max_discrepancy = -1
    for i in range(0, len(boundaries_x)):
        print(f"progress {100 * i / len(boundaries_x):0.2f}% ({i}/{len(boundaries_x)})")
        for j in range(i, len(boundaries_x)):
            for k in range(0, len(boundaries_y)):
                for l in range(k, len(boundaries_y)):
                    aabb_min_x = min(boundaries_x[i], boundaries_x[j])
                    aabb_max_x = max(boundaries_x[i], boundaries_x[j])
                    aabb_min_y = min(boundaries_y[k], boundaries_y[l])
                    aabb_max_y = max(boundaries_y[k], boundaries_y[l])

                    aabb_area = (aabb_max_x - aabb_min_x) * (aabb_max_y - aabb_min_y)
                    points_inside = 0

                    for q in dataset:
                        x = q[0]
                        y = q[1]

                        if aabb_min_x <= x and x <= aabb_max_x and aabb_min_y <= y and y <= aabb_max_y:
                            points_inside += 1

                    discrepancy = abs(aabb_area - points_inside / N)
                    if discrepancy > max_discrepancy:
                        max_discrepancy = discrepancy
                        max_aabb_min = [aabb_min_x, aabb_min_y]
                        max_aabb_max = [aabb_max_x, aabb_max_y]
    
    print(f"found: {max_discrepancy} aabb: {max_aabb_min} {max_aabb_max}")
    return (max_aabb_min, max_aabb_max)

#===============================================================================
# calc and plot
#===============================================================================
def calc_and_plot(dataset, title, filename):
    star_min, star_max = calculate_star_discrepancy(dataset)
    extreme_min, extreme_max = calculate_extreme_discrepancy(dataset)
    
    # Create plot
    fig, ax = plt.subplots()

    fig.tight_layout()
    ax.set_aspect("equal")
    ax.set_title(title)

    # plot samples
    ax.scatter(dataset[:, 0], dataset[:, 1], label="pointset $P_N$")

    # plot rectangles
    xmin, ymin = star_min
    xmax, ymax = star_max
    rect_star = Rectangle(
        (xmin, ymin),
        xmax - xmin,
        ymax - ymin,
        linewidth=2,
        edgecolor="green",
        facecolor="none",
        label="star-dicrepancy $D^*(P_N)$"
    )
    ax.add_patch(rect_star)

    xmin, ymin = extreme_min
    xmax, ymax = extreme_max
    rect_extreme = Rectangle(
        (xmin, ymin),
        xmax - xmin,
        ymax - ymin,
        linewidth=2,
        edgecolor="red",
        facecolor="none",
        label="extreme-dicrepancy $D(P_N)$"
    )
    ax.add_patch(rect_extreme)

    # add legend
    ax.legend(loc="upper left", bbox_to_anchor=(1.02, 1))

    # Save image
    plt.savefig(f"{filename}.svg", format="svg", bbox_inches="tight")
    plt.close()
    print(f"saved to {filename}.svg!")

#===============================================================================
# sequence generation
#===============================================================================
def radical_inverse(base: int, idx: int) -> float:
    assert base > 1

    digit = radical =  1 / (base)
    inverse = 0

    while idx: 
        inverse += digit * (idx % base)
        digit *= radical
        idx = idx // base

    return inverse 


def next_radical_inverse(base: int, inverse: float) -> float:
    almost_one = 1 - 1e-10
    radical = 1 / base
    next_inverse = inverse + radical
    if next_inverse < almost_one: 
        return next_inverse
    else:
        digit1 = radical
        digit2 = radical ** 2
        while(inverse + digit2 >= almost_one):
            digit1 = digit2
            digit2 *= radical
        return inverse + (digit1 - 1) + digit2

def halton_sequence(points: int,  primes: list[int]) -> list[int]:
    sequence = []
    for idx in range(points):
        elements = []
        for j, prime in enumerate(primes): 
            if len(sequence):
                elements.append(next_radical_inverse(prime, sequence[-1][j]))
            else:
                elements.append(radical_inverse(prime, idx))
        sequence.append(elements)
    return sequence


def hammersley_set(points: int,  primes: list[int]) -> list[int]:
    N = 1 << points 
    sequence = []
    for idx in range(points):
        elements = [idx / N]
        for j, prime in enumerate(primes): 
            if len(sequence):
                elements.append(next_radical_inverse(prime, sequence[-1][j + 1]))
            else:
                elements.append(radical_inverse(prime, idx))
        sequence.append(elements)
    return sequence


#===============================================================================
# run
#===============================================================================
print("1/7 calculate random dataset...")
np.random.seed(42)
dataset = np.random.rand(N, 2)
calc_and_plot(dataset, f"Discrepancy of independent uniform samples $\\in [0, 1)^2$, $N={N}$", "4_4_random")

print("2/7 calculate halton dataset (1,2)...")
dataset = np.array(halton_sequence(N, [2, 3]))
calc_and_plot(dataset, f"Discrepancy of halton sequence for dimension (1,2), $N={N}$", "4_4_halton_1_2")

print("3/7 calculate halton dataset (3,4)...")
dataset = np.array(halton_sequence(N, [5, 7]))
calc_and_plot(dataset, f"Discrepancy of halton sequence for dimension (3,4), $N={N}$", "4_4_halton_3_4")

print("4/7 calculate halton dataset (5,6)...")
dataset = np.array(halton_sequence(N, [11, 13]))
calc_and_plot(dataset, f"Discrepancy of halton sequence for dimension (5,6), $N={N}$", "4_4_halton_5_6")

print("5/7 calculate hammersley dataset 2...")
dataset = np.array(hammersley_set(N, [2]))
x_min = dataset[:, 0].min()
x_max = dataset[:, 0].max()
dataset[:, 0] = (dataset[:, 0] - x_min) / (x_max - x_min)
calc_and_plot(dataset, f"Discrepancy of hammerley set for prime 2, $N={N}$", "4_4_hammersley_2")

print("6/7 calculate hammersley dataset 3...")
dataset = np.array(hammersley_set(N, [3]))
x_min = dataset[:, 0].min()
x_max = dataset[:, 0].max()
dataset[:, 0] = (dataset[:, 0] - x_min) / (x_max - x_min)
calc_and_plot(dataset, f"Discrepancy of hammerley set for prime 3, $N={N}$", "4_4_hammersley_3")

print("7/7 calculate hammersley dataset 5...")
dataset = np.array(hammersley_set(N, [2]))
x_min = dataset[:, 0].min()
x_max = dataset[:, 0].max()
dataset[:, 0] = (dataset[:, 0] - x_min) / (x_max - x_min)
calc_and_plot(dataset, f"Discrepancy of hammerley set for prime 5, $N={N}$", "4_4_hammersley_5")

print("done!")
