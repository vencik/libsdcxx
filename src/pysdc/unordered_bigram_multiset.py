from __future__ import annotations
from typing import Optional, ClassVar, Generator
import ctypes

from .libpysdc import libpysdc
from .util import serialise


class UnorderedBigramMultiset(Generator[str, None, None]):
    """
    Unordered bigram multiset (implemented by std::unordered_multiset)
    """

    _str_fixed_len: ClassVar[int] = \
        len("unordered_unordered_wbigram_multiset(size: XXXXXXXXXX, {})")

    def __init__(self, string: Optional[str] = None, _impl: Optional = None):
        """
        :param string: String from which bigrams multiset shall be created
        """
        self._impl = libpysdc.new_unordered_wbigram_multiset_str(ctypes.c_wchar_p(string)) \
            if string is not None else _impl or libpysdc.new_unordered_wbigram_multiset()

    def __deepcopy__(self, memo):
        """
        Make a copy on the native level
        :param memo: IDs of already copied objects (unused, we're non-recursive)
        """
        return UnorderedBigramMultiset(
            _impl=libpysdc.new_unordered_wbigram_multiset_copy(self._impl))

    def __copy__(self):
        """
        We don't do shallow copies
        """
        return self.__deepcopy__(None)

    def __len__(self):
        return libpysdc.unordered_wbigram_multiset_size(self._impl)

    def __iter__(self):
        """
        :return: Generator of bigrams together with their counts as tuple[str, int]
        """
        itr = libpysdc.unordered_wbigram_multiset_cbegin(self._impl)
        end = libpysdc.unordered_wbigram_multiset_cend(self._impl)
        try:
            while libpysdc.unordered_wbigram_multiset_citer_ne(itr, end):
                ch1, ch2 = ctypes.c_wchar(), ctypes.c_wchar()
                libpysdc.unordered_wbigram_multiset_citer_deref(
                    itr, ctypes.byref(ch1), ctypes.byref(ch2))

                yield ch1.value + ch2.value

                libpysdc.unordered_wbigram_multiset_citer_inc(itr)

        finally:
            libpysdc.delete_unordered_wbigram_multiset_citer(end)
            libpysdc.delete_unordered_wbigram_multiset_citer(itr)

    def send(self):
        pass

    def throw(self):
        pass

    def __iadd__(self, other: UnorderedBigramMultiset) -> UnorderedBigramMultiset:
        """
        Update by `other` bigrams (in-place union)
        """
        assert isinstance(other, UnorderedBigramMultiset)
        libpysdc.unordered_wbigram_multiset_iadd(self._impl, other._impl)
        return self

    def __add__(self, other: UnorderedBigramMultiset) -> UnorderedBigramMultiset:
        """
        :return: Union of `self` and `other` bigrams
        """
        assert isinstance(other, UnorderedBigramMultiset)
        return UnorderedBigramMultiset(
            _impl=libpysdc.unordered_wbigram_multiset_add(self._impl, other._impl))

    @staticmethod
    def intersect_size(
        bgrms1: UnorderedBigramMultiset,
        bgrms2: UnorderedBigramMultiset,
    ) -> int:
        """
        :return: Cardinality of intersection of `bgrms1` and `bgrms2` multisets
        """
        assert isinstance(bgrms1, UnorderedBigramMultiset)
        assert isinstance(bgrms2, UnorderedBigramMultiset)
        return libpysdc.unordered_wbigram_multiset_intersect_size(
            bgrms1._impl, bgrms2._impl)

    @staticmethod
    def sorensen_dice_coef(
        bgrms1: UnorderedBigramMultiset,
        bgrms2: UnorderedBigramMultiset,
    ) -> float:
        """
        :return: Sørensen–Dice coefficient of `bgrms1` and `bgrms2` multisets
        """
        assert isinstance(bgrms1, UnorderedBigramMultiset)
        assert isinstance(bgrms2, UnorderedBigramMultiset)
        return libpysdc.unordered_wbigram_multiset_sorensen_dice_coef(
            bgrms1._impl, bgrms2._impl)

    def __str__(self):
        return serialise(
            self,
            UnorderedBigramMultiset._str_fixed_len + 4 * len(self),
            libpysdc.unordered_wbigram_multiset_str,
        )

    def __del__(self):
        libpysdc.delete_unordered_wbigram_multiset(self._impl)
