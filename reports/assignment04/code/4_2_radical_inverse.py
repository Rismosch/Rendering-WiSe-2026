# First 10 prime numbers for plotting 
PRIMES = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29] 
N_POINTS = 256

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
