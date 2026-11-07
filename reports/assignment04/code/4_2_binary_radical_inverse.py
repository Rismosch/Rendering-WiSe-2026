def binary_radical_inverse(idx: int) -> float:
    inverse = 0
    exponent = 1

    while idx: 
        inverse = (inverse << 1) | (idx & 1)
        idx >>=  1
        exponent <<= 1

    return inverse / exponent