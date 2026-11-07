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

    boundaries_x = [0, 1]
    boundaries_y = [0, 1]

    for p in dataset:
        boundaries_x.append(p[0])
        boundaries_y.append(p[1])

    max_discrepancy = -1
    for i in range(0, len(boundaries_x)):
        print(f"progress {i}/{len(boundaries_x)}")
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
# run
#===============================================================================
print("1/3 calculate random dataset...")
np.random.seed(42)
dataset = np.random.rand(N, 2)
calc_and_plot(dataset, "Discrepancy of independent uniform samples $\\in [0, 1)^2$", "random")

print("2/3 calculate halton dataset...")
print("TODO")

print("2/3 calculate hammersley dataset...")
print("TODO")

print("done!")
