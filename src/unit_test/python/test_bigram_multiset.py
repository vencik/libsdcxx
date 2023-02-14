from copy import copy, deepcopy

from pysdc import BigramMultiset


def test_empty():
    bgrms = BigramMultiset()

    assert isinstance(bgrms, BigramMultiset)
    assert len(bgrms) == 0
    assert list(bgrms) == []
    assert dict(bgrms) == {}
    assert str(bgrms) == "BigramMultiset.wbigram_multiset(size: 0, {})"


def test_copy():
    bgrms = BigramMultiset()
    bgrms_copy = copy(bgrms)
    bgrms_deepcopy = deepcopy(bgrms)

    assert id(bgrms) != id(bgrms_copy)
    assert id(bgrms) != id(bgrms_deepcopy)
    assert id(bgrms_copy) != id(bgrms_deepcopy)

    assert id(bgrms._impl) != id(bgrms_copy._impl)
    assert id(bgrms._impl) != id(bgrms_deepcopy._impl)
    assert id(bgrms_copy._impl) != id(bgrms_deepcopy._impl)


def test_union():
    bgrms_abcd = BigramMultiset("abcd")

    assert isinstance(bgrms_abcd, BigramMultiset)
    assert len(bgrms_abcd) == 3
    assert list(bgrms_abcd) == ["ab", "bc", "cd"]
    assert str(bgrms_abcd) == "BigramMultiset.wbigram_multiset(size: 3, {ab, bc, cd})"

    bgrms_bcd = BigramMultiset("bcd")

    assert isinstance(bgrms_bcd, BigramMultiset)
    assert len(bgrms_bcd) == 2
    assert list(bgrms_bcd) == ["bc", "cd"]
    assert str(bgrms_bcd) == "BigramMultiset.wbigram_multiset(size: 2, {bc, cd})"

    bgrms_union = bgrms_abcd + bgrms_bcd

    assert isinstance(bgrms_union, BigramMultiset)
    assert len(bgrms_union) == 5
    assert list(bgrms_union) == ["ab", "bc", "bc", "cd", "cd"]
    assert str(bgrms_union) == \
        "BigramMultiset.wbigram_multiset(size: 5, {ab, bc, bc, cd, cd})"

    # Operands are unchanged
    assert str(bgrms_abcd) == "BigramMultiset.wbigram_multiset(size: 3, {ab, bc, cd})"
    assert str(bgrms_bcd) == "BigramMultiset.wbigram_multiset(size: 2, {bc, cd})"

    bgrms_abcd += bgrms_bcd

    assert len(bgrms_abcd) == 5
    assert list(bgrms_abcd) == ["ab", "bc", "bc", "cd", "cd"]
    assert str(bgrms_abcd) == \
        "BigramMultiset.wbigram_multiset(size: 5, {ab, bc, bc, cd, cd})"

    # Right operand is unchanged
    assert str(bgrms_bcd) == "BigramMultiset.wbigram_multiset(size: 2, {bc, cd})"


def test_sdc():
    bgrms_abcd = BigramMultiset("abcd")
    bgrms_bcd = BigramMultiset("bcd")

    isect_size = BigramMultiset.intersect_size(bgrms_abcd, bgrms_bcd)
    assert isect_size == 2  # intersection is {bc, cd}

    sdc = BigramMultiset.sorensen_dice_coef(bgrms_abcd, bgrms_bcd)
    assert sdc == 2 * 2 / (3 + 2)


def test_unicode():
    bgrms_sorensen = BigramMultiset("Sørensen")

    assert isinstance(bgrms_sorensen, BigramMultiset)
    assert len(bgrms_sorensen) == 7
    assert list(bgrms_sorensen) == ["Sø", "en", "en", "ns", "re", "se", "ør"]
    assert str(bgrms_sorensen) == "BigramMultiset.wbigram_multiset(size: 7, {" \
        "Sø, en, en, ns, re, se, ør" \
    "})"
