#!/usr/bin/env python3

from pysdc import Bigrams


def main(argv) -> int:
    bgrms_empty = Bigrams()
    print(f"Bigrams(): {bgrms_empty}, size: {len(bgrms_empty)}")

    bgrms_abcd = Bigrams("abcd")
    print(f"Bigrams(\"abcd\"): {bgrms_abcd}, size: {len(bgrms_abcd)}")

    bgrms_bcd = Bigrams("bcd")
    print(f"Bigrams(\"bcd\"): {bgrms_bcd}, size: {len(bgrms_bcd)}")

    bgrms_sorensen = Bigrams("Sørensen")
    print(f"Bigrams(\"Sørensen\"): {bgrms_sorensen}, size: {len(bgrms_sorensen)}")

    print(f"list(Bigrams(\"Sørensen\")): {list(bgrms_sorensen)}")
    print(f"dict(Bigrams(\"Sørensen\")): {dict(bgrms_sorensen)}")

    bgrms_union = bgrms_abcd + bgrms_bcd
    print(f"Bigrams(\"abcd\") + Bigrams(\"bcd\"): {bgrms_union}, size: {len(bgrms_union)}")

    isect_size = Bigrams.intersect_size(bgrms_abcd, bgrms_bcd)
    print(f"|intersect(Bigrams(\"abcd\"), Bigrams(\"bcd\"))| == {isect_size}")

    sdc = Bigrams.sorensen_dice_coef(bgrms_abcd, bgrms_bcd)
    print(f"SDC(Bigrams(\"abcd\"), Bigrams(\"bcd\")) == {sdc}")

    return 0

if __name__ == "__main__":
    from sys import argv, exit
    exit(main(argv))
