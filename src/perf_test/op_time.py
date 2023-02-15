#!/usr/bin/env python3

from typing import List, Iterator, Type
from os.path import basename
from argparse import ArgumentParser
from time import time
import re
import string
import random

from pysdc import Bigrams, BigramMultiset, UnorderedBigramMultiset

measure_pybigrams = True
try:  # try to import Python multiset based bigrams implementation for comparison
    from pybigrams import PyBigrams
except ImportError:  # Python multiset package probably wasn't installed
    measure_pybigrams = False


def measure_perf(
    bigrams_type: Type,
    text: List[List[str]],
    union_size_hwm: int,
):
    """
    Measure performance of bigram multiset implementation
    :param bigrams_type: Bigram multiset type
    :param text: Test text
    :param union_size_hwm: High watermark for continued union construction
    """
    print(f"Measuring performance of {bigrams_type.__name__}:")

    # Bigram multiset construction
    start = time()
    text_bigrams = [[bigrams_type(token) for token in sentence] for sentence in text]
    constr_time = time() - start
    constr_time_avg = constr_time / sum(len(sentence) for sentence in text)

    # Sentence token bigram multiset union upper triangular matrix
    # For each sentence (i.e. sequence of tokens), the matrix contains
    # unions of adjacent token bigrams.
    # The 1st line has the length of the sentence (in tokens), conatining token bigrams.
    # The 2nd line contains unions of 2 neighbouring token bigrams.
    # The 3rd line contains unions of 3 neighbouring token bograms and so on,
    # the last line has only one column containing union of all the token bigrams.
    # Generally, the bigrams union matrix U is generated like this:
    # U_{1,j} = Bigrams(T_j) (where T_j is j-th token in the sentence)
    # U_{i,j} = U_{i-1,j} + U_{1,i-1+j}
    unions: List[List[List[bigram_type]]] = []
    start = time()
    for sent_bigrams in text_bigrams:
        # NOTE: we're using 0-based indeces here
        union_matrix: List[List[bigram_type]] = [sent_bigrams]  # 1st row: token bigrams
        for i in range(1, len(union_matrix[0])):  # i is line index
            union_matrix.append([
                union_matrix[-1][j] + union_matrix[0][i+j]
                for j in range(len(union_matrix[-1]) - 1)  # j is column index
            ])
            if all(len(bigrams) >= union_size_hwm for bigrams in union_matrix[-1]):
                break  # all unions sizes reach the high watermark

        unions.append(union_matrix)

    union_time = time() - start

    # Count unions
    union_cnt = 0
    union_size_sum = 0
    for union_matrix in unions:
        for line in union_matrix[1:]:
            union_cnt += len(line)
            union_size_sum += sum(len(bigrams) for bigrams in line)

    union_size_avg = union_size_sum / union_cnt
    union_time_avg = union_time / union_cnt

    # Sørensen-Dice coefficient
    # We shall just calculate SDC from U_{i,1} with U_{i,jmax} and also from
    # U_{i,jmax/2} with U{i+1,1} and U{i+2,jmax}.
    # This should give us relatively good idea of how similarly sized strings should fare
    # (strings which differ in size too much are poor candidates for similarity).
    sdc_cnt = 0
    start = time()
    for union_matrix in unions:
        for line in union_matrix:
            bigrams_type.sorensen_dice_coef(line[0], line[-1])
            sdc_cnt += 1

        for i in range(len(union_matrix) - 1):
            bigrams_type.sorensen_dice_coef(
                union_matrix[i][int(len(union_matrix[i])/2)],
                union_matrix[i+1][0])

        for i in range(len(union_matrix) - 2):
            bigrams_type.sorensen_dice_coef(
                union_matrix[i][int(len(union_matrix[i])/2)],
                union_matrix[i+2][-1])

    sdc_time = time() - start
    sdc_time_avg = sdc_time / sdc_cnt

    print(f"""\
    Total construction time [s]: {constr_time:.3}
    Average instance construction time [us]: {constr_time_avg * 1000000:.3}
    Total unions produced: {union_cnt}
    Average union size: {union_size_avg:.3}
    Total union computation time [s]: {union_time:.3}
    Average union operation time [us]: {union_time_avg * 1000000:.3}
    Total Sørensen-Dice coefficients calculated: {sdc_cnt}
    Total Sørensen-Dice coefficients computation time [s]: {sdc_time:.3}
    Average Sørensen-Dice coefficient computation time [us]: {sdc_time_avg * 1000000:.3}
""")


def text_stats(text: List[List[str]]):
    """
    Print text statistics
    :param text: Text (tokenised)
    """
    text_size = 0
    token_cnt = 0
    min_token_len = 9999999999
    max_token_len = 0
    min_tokens = 9999999999
    max_tokens = 0
    for sentence in text:
        token_cnt += len(sentence)
        if min_tokens > len(sentence):
            min_tokens = len(sentence)
        if max_tokens < len(sentence):
            max_tokens = len(sentence)

        for token in sentence:
            text_size += len(token)
            if min_token_len > len(token):
                min_token_len = len(token)
            if max_token_len < len(token):
                max_token_len = len(token)

    avg_token_len = text_size / token_cnt
    avg_tokens = token_cnt / len(text)

    print(f"""\
Total text size [characters]: {text_size}
Number of sentences: {len(text)}
Total number of tokens: {token_cnt}
Token length [min/avg/max]: {min_token_len}/{avg_token_len:.3}/{max_token_len}
Tokens in sentence [min/avg/max]: {min_tokens}/{avg_tokens:.3}/{max_tokens}
""")


def generate_sentences(
    text_size: int,
    min_token_len: int,
    max_token_len: int,
    min_sent_len: int,
    max_sent_len: int,
) -> Iterator[List[str]]:
    """
    Generate sentences of random tokens
    :param text_size: Target text size (approximate)
    :param min_token_len: Minimal length of token (in characters)
    :param max_token_len: Maximal length of token
    :param min_sent_len: Minimal length of sentence (in tokens)
    :param max_sent_len: Maximal length of sentence
    :return: (Tokenised) "sentences"
    """
    alphabet = string.ascii_lowercase
    while text_size > 0:
        sentence = [
            "".join(random.choice(alphabet)
                for _ in range(random.randint(min_token_len, max_token_len)))
            for _ in range(random.randint(min_sent_len, max_sent_len))
        ]
        yield sentence

        text_size -= sum(len(token) for token in sentence)


def main(argv):
    arg_parser = ArgumentParser(
        prog=basename(argv[0]),
        description="""
            pysdc bigram mutliset implementations operations performance test.
            Measurements are done either on randomly generated token strings or,
            alternatively, taken from text files (sentence per line).
        """,
        epilog=None,
    )
    arg_parser.add_argument(
        "--text-size", type=int, metavar="characters", default=500000,
        help="Size of (generated) text",
    )
    arg_parser.add_argument(
        "--min-token-len", type=int, metavar="number", default=2,
        help="Minimal token string length",
    )
    arg_parser.add_argument(
        "--max-token-len", type=int, metavar="number", default=12,
        help="Maximal token string length",
    )
    arg_parser.add_argument(
        "--min-sent-len", type=int, metavar="number", default=3,
        help="Minimal sentence length",
    )
    arg_parser.add_argument(
        "--max-sent-len", type=int, metavar="number", default=20,
        help="Maximal sentence length",
    )
    arg_parser.add_argument(
        "-U", "--union-size-hwm", type=int, metavar="number", default=9999999999,
        help="""
        Bigram multiset union size high watermark
        (used to limit the size of produced bigram multiset unions)
        """,
    )
    arg_parser.add_argument(
        "-t", "--text", metavar="file.txt", action="append",
        help="Text file path (may be used multiple times)",
    )
    args = arg_parser.parse_args(argv[1:])

    # Construct text sentences
    text: List[List[str]] = []

    if args.text:  # read text from file(s)
        token_sep = re.compile(r'\s+')
        for txt in args.text:
            with open(txt, encoding="utf-8") as txt_fd:
                text += [re.split(token_sep, line.strip()) for line in txt_fd]

    else:  # generate text
        text = list(generate_sentences(
            text_size=args.text_size,
            min_token_len=args.min_token_len,
            max_token_len=args.max_token_len,
            min_sent_len=args.min_sent_len,
            max_sent_len=args.max_sent_len,
        ))

    # Summarise text statistics
    text_stats(text)

    # Produce performance statistics
    measure_perf(Bigrams, text, args.union_size_hwm)
    measure_perf(BigramMultiset, text, args.union_size_hwm)
    measure_perf(UnorderedBigramMultiset, text, args.union_size_hwm)
    if measure_pybigrams:
        measure_perf(PyBigrams, text, args.union_size_hwm)

    return 0

if __name__ == "__main__":
    from sys import exit, argv
    exit(main(argv))
