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
