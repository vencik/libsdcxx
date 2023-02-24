from copy import copy, deepcopy

from pysdcxx import Bigrams


def test_empty():
    bgrms = Bigrams()

    assert isinstance(bgrms, Bigrams)
    assert len(bgrms) == 0
    assert list(bgrms) == []
    assert dict(bgrms) == {}
    assert str(bgrms) == "Bigrams.wbigrams(size: 0, {})"


def test_copy():
    bgrms = Bigrams()
    bgrms_copy = copy(bgrms)
    bgrms_deepcopy = deepcopy(bgrms)

    assert id(bgrms) != id(bgrms_copy)
    assert id(bgrms) != id(bgrms_deepcopy)
    assert id(bgrms_copy) != id(bgrms_deepcopy)

    assert id(bgrms._impl) != id(bgrms_copy._impl)
    assert id(bgrms._impl) != id(bgrms_deepcopy._impl)
    assert id(bgrms_copy._impl) != id(bgrms_deepcopy._impl)


def test_union():
    bgrms_abcd = Bigrams("abcd")

    assert isinstance(bgrms_abcd, Bigrams)
    assert len(bgrms_abcd) == 3
    assert list(bgrms_abcd) == [("ab", 1), ("bc", 1), ("cd", 1)]
    assert dict(bgrms_abcd) == {"ab": 1, "bc": 1, "cd": 1}
    assert str(bgrms_abcd) == "Bigrams.wbigrams(size: 3, {ab: 1, bc: 1, cd: 1})"

    bgrms_bcd = Bigrams("bcd")

    assert isinstance(bgrms_bcd, Bigrams)
    assert len(bgrms_bcd) == 2
    assert list(bgrms_bcd) == [("bc", 1), ("cd", 1)]
    assert dict(bgrms_bcd) == {"bc": 1, "cd": 1}
    assert str(bgrms_bcd) == "Bigrams.wbigrams(size: 2, {bc: 1, cd: 1})"

    bgrms_union = bgrms_abcd + bgrms_bcd

    assert isinstance(bgrms_union, Bigrams)
    assert len(bgrms_union) == 5
    assert list(bgrms_union) == [("ab", 1), ("bc", 2), ("cd", 2)]
    assert dict(bgrms_union) == {"ab": 1, "bc": 2, "cd": 2}
    assert str(bgrms_union) == "Bigrams.wbigrams(size: 5, {ab: 1, bc: 2, cd: 2})"

    # Operands are unchanged
    assert str(bgrms_abcd) == "Bigrams.wbigrams(size: 3, {ab: 1, bc: 1, cd: 1})"
    assert str(bgrms_bcd) == "Bigrams.wbigrams(size: 2, {bc: 1, cd: 1})"

    bgrms_abcd += bgrms_bcd

    assert len(bgrms_abcd) == 5
    assert list(bgrms_abcd) == [("ab", 1), ("bc", 2), ("cd", 2)]
    assert dict(bgrms_abcd) == {"ab": 1, "bc": 2, "cd": 2}
    assert str(bgrms_abcd) == "Bigrams.wbigrams(size: 5, {ab: 1, bc: 2, cd: 2})"

    # Right operand is unchanged
    assert str(bgrms_bcd) == "Bigrams.wbigrams(size: 2, {bc: 1, cd: 1})"


def test_sdc():
    bgrms_abcd = Bigrams("abcd")
    bgrms_bcd = Bigrams("bcd")

    isect_size = Bigrams.intersect_size(bgrms_abcd, bgrms_bcd)
    assert isect_size == 2  # intersection is {bc, cd}

    sdc = Bigrams.sorensen_dice_coef(bgrms_abcd, bgrms_bcd)
    assert sdc == 2 * 2 / (3 + 2)


def test_unicode():
    bgrms_sorensen = Bigrams("Sørensen")

    assert isinstance(bgrms_sorensen, Bigrams)
    assert len(bgrms_sorensen) == 7
    assert list(bgrms_sorensen) == [
        ("Sø", 1), ("en", 2), ("ns", 1), ("re", 1), ("se", 1), ("ør", 1),
    ]
    assert dict(bgrms_sorensen) == {
        "Sø": 1, "ør": 1, "re": 1, "en": 2, "ns": 1, "se": 1,
    }
    assert str(bgrms_sorensen) == "Bigrams.wbigrams(size: 7, {" \
        "Sø: 1, en: 2, ns: 1, re: 1, se: 1, ør: 1" \
    "})"
