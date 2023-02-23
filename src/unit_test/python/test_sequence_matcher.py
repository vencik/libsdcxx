from typing import List
import pytest

from pysdc import SequenceMatcher, Bigrams


def test_empty():
    matcher = SequenceMatcher()
    assert isinstance(matcher, SequenceMatcher)


def test_len():
    matcher = SequenceMatcher()
    assert len(matcher) == 0


def test_reserve():
    matcher = SequenceMatcher()
    matcher.reserve(10)
    assert len(matcher) == 0


def check_construction(tokens: List[SequenceMatcher.Token], reserve: int = 0):
    matcher = SequenceMatcher(tokens, reserve)
    assert isinstance(matcher, SequenceMatcher)
    assert len(matcher) == len(tokens)

def test_construction_str():
    check_construction(["Hello", "  ", "world", " !"])

def test_construction_str_reserve():
    check_construction(["Hello", "  ", "world", " !"], 4)

def test_construction_str_reserve_low():
    check_construction(["Hello", "  ", "world", " !"], 2)

def test_construction_str_reserve_high():
    check_construction(["Hello", "  ", "world", " !"], 8)

def test_construction_bigrams():
    check_construction([Bigrams("Hello"), Bigrams("  "), Bigrams("world")])

def test_construction_bigrams_reserve():
    check_construction([Bigrams("Hello"), Bigrams("  "), Bigrams("world")], 3)

def test_construction_bigrams_reserve_low():
    check_construction([Bigrams("Hello"), Bigrams("  "), Bigrams("world")], 2)

def test_construction_bigrams_reserve_high():
    check_construction([Bigrams("Hello"), Bigrams("  "), Bigrams("world")], 7)

def test_construction_str_strip():
    check_construction(["Hello", ("  ",True), "world", (" !",True)])

def test_construction_bigrams_strip():
    check_construction([Bigrams("Hello"), (Bigrams("  "),True), Bigrams("world")])

def test_construction_combined():
    check_construction(["Hello", (Bigrams("  "),True), Bigrams("world"), (" !",True)])


def test_append():
    matcher = SequenceMatcher(["01", "02", "03"])
    assert len(matcher) == 3

    matcher.append("04")
    assert len(matcher) == 4


def test_match():
    strip = True
    matcher = SequenceMatcher([
        "This", ("  ",strip), "uses", ("  ",strip),
        "Sørensen", (" -",strip), "Dice", ("  ",strip),
        "coefficient", (" .",strip),
    ])

    matches = matcher.match(["Sørenson", "and", "Dice"], 0.65, include_bigrams=True)

    match = next(matches)
    assert match.begin == 4
    assert match.end == 7
    assert match.score >= 0.65
    assert sorted(list(match.bigrams)) == sorted([
        ("Sø",1), ("ør",1), ("re",1), ("en",2), ("ns",1), ("se",1),
        (" -",1),
        ("Di",1), ("ic",1), ("ce",1),
    ])

    with pytest.raises(StopIteration):
        next(matches)
