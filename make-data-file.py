#!/usr/bin/env python3

import argparse
import random

def _random_int():
    return random.randint(-1000,1000)

def _seq_int():
    _seq_int.N += 1
    return _seq_int.N
_seq_int.N = -1

def _random_double():
    return random.gauss(0.0, 100.0)

def _random_str():
    l = random.randint(1, 8)
    return "".join([chr(random.randint(ord('a'), ord('z'))) for x in range(l)])

RANDOM_CTORS = [_random_int, _seq_int, _random_str, _random_str]

def main():
    parser = argparse.ArgumentParser(prog="make-data-file")
    parser.add_argument("--count", "-n", default=1000, type=int)
    parser.add_argument("--distr", "-d", default="gauss", choices=["gauss"])
    parser.add_argument("--mean", "-m", default=0.0, type=float)
    parser.add_argument("--stdev", "-s", default=1.0, type=float)
    parser.add_argument("--csv-col", "-c", default=None, type=int)
    parser.add_argument("--csv-col-max", default=8, type=int)
    args = parser.parse_args()

    if args.distr == "gauss":
        def rng(_gauss=random.gauss, _mean=args.mean, _stdev=args.stdev):
            return _gauss(_mean, _stdev)
    else:
        raise AssertionError("Invalid distr '" + args.distr + "'")

    if args.csv_col is None:
        def print_one(x):
            print(x)
    else:
        other_ctors = [random.choice(RANDOM_CTORS) if i != args.csv_col else
                       None for i in range(args.csv_col_max)]

        def print_one(x):
            data = [str(ctor()) if ctor is not None else str(x)
                    for ctor in other_ctors]
            print(",".join(data))

    for x in range(args.count):
        print_one(rng())


if __name__ == "__main__":
    main()
