from copy import copy, deepcopy

from pysdcxx import UnorderedBigramMultiset


def test_empty():
    bgrms = UnorderedBigramMultiset()

    assert isinstance(bgrms, UnorderedBigramMultiset)
    assert len(bgrms) == 0
    assert list(bgrms) == []
    assert dict(bgrms) == {}
    assert str(bgrms) == \
        "UnorderedBigramMultiset.unordered_wbigram_multiset(size: 0, {})"


def test_copy():
    bgrms = UnorderedBigramMultiset()
    bgrms_copy = copy(bgrms)
    bgrms_deepcopy = deepcopy(bgrms)

    assert id(bgrms) != id(bgrms_copy)
    assert id(bgrms) != id(bgrms_deepcopy)
    assert id(bgrms_copy) != id(bgrms_deepcopy)

    assert id(bgrms._impl) != id(bgrms_copy._impl)
    assert id(bgrms._impl) != id(bgrms_deepcopy._impl)
    assert id(bgrms_copy._impl) != id(bgrms_deepcopy._impl)


def test_union():
    bgrms_abcd = UnorderedBigramMultiset("abcd")

    assert isinstance(bgrms_abcd, UnorderedBigramMultiset)
    assert len(bgrms_abcd) == 3
    assert sorted(list(bgrms_abcd)) == ["ab", "bc", "cd"]
    assert str(bgrms_abcd).startswith(
        "UnorderedBigramMultiset.unordered_wbigram_multiset(size: 3, {")

    bgrms_bcd = UnorderedBigramMultiset("bcd")

    assert isinstance(bgrms_bcd, UnorderedBigramMultiset)
    assert len(bgrms_bcd) == 2
    assert sorted(list(bgrms_bcd)) == ["bc", "cd"]
    assert str(bgrms_bcd).startswith(
        "UnorderedBigramMultiset.unordered_wbigram_multiset(size: 2, {")

    bgrms_union = bgrms_abcd + bgrms_bcd

    assert isinstance(bgrms_union, UnorderedBigramMultiset)
    assert len(bgrms_union) == 5
    assert sorted(list(bgrms_union)) == ["ab", "bc", "bc", "cd", "cd"]
    assert str(bgrms_union).startswith(
        "UnorderedBigramMultiset.unordered_wbigram_multiset(size: 5, {")

    # Operands are unchanged
    assert sorted(list(bgrms_abcd)) == ["ab", "bc", "cd"]
    assert sorted(list(bgrms_bcd)) == ["bc", "cd"]

    bgrms_abcd += bgrms_bcd

    assert len(bgrms_abcd) == 5
    assert sorted(list(bgrms_abcd)) == ["ab", "bc", "bc", "cd", "cd"]
    assert str(bgrms_abcd).startswith(
        "UnorderedBigramMultiset.unordered_wbigram_multiset(size: 5, {")

    # Right operand is unchanged
    assert sorted(list(bgrms_bcd)) == ["bc", "cd"]


def test_sdc():
    bgrms_abcd = UnorderedBigramMultiset("abcd")
    bgrms_bcd = UnorderedBigramMultiset("bcd")

    isect_size = UnorderedBigramMultiset.intersect_size(bgrms_abcd, bgrms_bcd)
    assert isect_size == 2  # intersection is {bc, cd}

    sdc = UnorderedBigramMultiset.sorensen_dice_coef(bgrms_abcd, bgrms_bcd)
    assert sdc == 2 * 2 / (3 + 2)


def test_unicode():
    bgrms_sorensen = UnorderedBigramMultiset("Sørensen")

    assert isinstance(bgrms_sorensen, UnorderedBigramMultiset)
    assert len(bgrms_sorensen) == 7
    assert sorted(list(bgrms_sorensen)) == ["Sø", "en", "en", "ns", "re", "se", "ør"]
    assert str(bgrms_sorensen).startswith(
        "UnorderedBigramMultiset.unordered_wbigram_multiset(size: 7, {")
