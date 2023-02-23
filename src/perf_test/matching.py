#!/usr/bin/env python3

from typing import List, Tuple, Iterator
from os.path import basename
from argparse import ArgumentParser
from time import time
import re
import string

from pysdc import SequenceMatcher, Bigrams


def matching_perf(
    sentences: List[List[Tuple[str, bool]]],
    sequences: List[List[str]],
    threshold: float,
):
    """
    Measure matching performance
    :param sentences: Text sentences
    :param sequences: Sequences matched to the text
    :param threshold: Matching threshold
    """
    print("Measuring matching performance...")

    # Pre-compute sequence bigrams
    start = time()
    sequences_bigrams = [
        sum((Bigrams(token) for token in sequence), start=Bigrams())
        for sequence in sequences
    ]
    precomp_time = time() - start

    total_time = 0.0
    match_cnt = 0
    sum_match_len = 0
    sum_match_score = 0.0
    for sentence in sentences:
        start = time()

        # Match all sequences to sentence
        matcher = SequenceMatcher(sentence)
        for sequence in sequences_bigrams:
            for match in matcher.match(sequence, threshold):
                match_cnt += 1
                sum_match_len += match.end - match.begin
                sum_match_score += match.score

        sent_time = time() - start
        total_time += sent_time

    seq_time_avg = total_time / len(sentences) / len(sequences)
    match_len_avg = sum_match_len / match_cnt
    match_score_avg = sum_match_score / match_cnt

    print(f"""\
    Precomputation time [ms]: {precomp_time * 1000.0:.3}
    Matching score threshold: {threshold}
    Total matching time [s]: {total_time:.3}
    Average sequence matching time [us]: {seq_time_avg * 1000000.0:.3}
    Total match count: {match_cnt}
    Average match length: {match_len_avg:.3}
    Average match score: {match_score_avg:.3}
""")


def tokenise(file: str) -> Iterator[List[Tuple[str, bool]]]:
    """
    Tokenise text file
    :param file: Text file name (organised in lines)
    :return: Text lines, which are lists of tuple[lowercase token, strip flag]
    """
    def augment_token(token: str) -> str:
        token = token.lower()
        if len(token) == 1: return f" {token}"
        return token

    sep_regex = re.compile(rf"[{string.whitespace + string.punctuation}]+")

    with open(file, encoding="utf-8") as fd:
        for line in (l.strip() for l in fd):
            if not line: continue  # skip empty lines

            tokens: List[Tuple[str, bool]] = []
            offset = 0
            for sep_match in re.finditer(sep_regex, line):
                begin, end = sep_match.span()
                if offset < begin:  # token precedes
                    tokens.append(
                        (augment_token(line[offset:begin]), False))  # common token

                tokens.append(
                    (augment_token(sep_match.group()), True))  # strip token

                offset = end

            if offset < len(line):  # add last token
                tokens.append(
                    (augment_token(line[offset:]), False))  # common token

            yield tokens


def stats(seqs: List[List]):
    """
    Print statistics for token sequences
    :param seqs: Token sequences
    """
    len_avg = sum(len(seq) for seq in seqs) / len(seqs)
    print(f"""\
    Number of sequences: {len(seqs)}
    Average length of sequence [# of tokens]: {len_avg}
""")


def main(argv):
    arg_parser = ArgumentParser(
        prog=basename(argv[0]),
        description="""
            pysdc sequence matcher performance test.
            Measurements are done on a text file and a number of sequences defined
            as lines of another text file.
            Matching is done sentence by sentence.
        """,
        epilog=None,
    )
    arg_parser.add_argument(
        "-t", "--text", metavar="file.txt", action="append",
        help="Text file path (may be used multiple times)",
    )
    arg_parser.add_argument(
        "-s", "--sequences", metavar="file.txt", action="append",
        help="Sequences file path (may be used multiple times)",
    )
    arg_parser.add_argument(
        "-T", "--threshold", type=float, metavar="[0..1]", default=0.65,
        help="Sequences file path (may be used multiple times)",
    )
    args = arg_parser.parse_args(argv[1:])

    # Construct text sentences
    sentences: List[List[Tuple[str,bool]]] = []
    for txt in args.text or []:
        sentences += list(tokenise(txt))

    # Construct matched sequences
    sequences: List[List[Tuple[str,bool]]] = []
    for seqs in args.sequences or []:
        sequences += [[token[0] for token in seq] for seq in tokenise(seqs)]

    # Summarise statistics
    print("Text statistics:")
    stats(sentences)
    print("Sequences statistics:")
    stats(sequences)

    # Produce performance statistics
    matching_perf(sentences, sequences, args.threshold)

    return 0

if __name__ == "__main__":
    from sys import exit, argv
    exit(main(argv))
