from __future__ import annotations
from typing import Optional, ClassVar, Generator, Tuple
import ctypes

from .libpysdcxx import libpysdcxx
from .util import serialise


class Bigrams(Generator[Tuple[str, int], None, None]):
    """
    Bigram multiset (custom implementation)
    """

    _str_fixed_len: ClassVar[int] = len("wbigrams(size: XXXXXXXXXX, {})")

    def __init__(self, string: Optional[str] = None, _impl: Optional = None):
        """
        :param string: String from which bigrams multiset shall be created
        """
        self._impl = libpysdcxx.new_wbigrams_str(ctypes.c_wchar_p(string)) \
            if string is not None else _impl or libpysdcxx.new_wbigrams()

    def __deepcopy__(self, memo):
        """
        Make a copy on the native level
        :param memo: IDs of already copied objects (unused, we're non-recursive)
        """
        return self.__class__(_impl=libpysdcxx.new_wbigrams_copy(self._impl))

    def __copy__(self):
        """
        We don't do shallow copies
        """
        return self.__deepcopy__(None)

    def __len__(self):
        return libpysdcxx.wbigrams_size(self._impl)

    def __iter__(self):
        """
        :return: Generator of bigrams together with their counts as tuple[str, int]
        """
        itr = libpysdcxx.wbigrams_cbegin(self._impl)
        end = libpysdcxx.wbigrams_cend(self._impl)
        try:
            while libpysdcxx.wbigrams_citer_ne(itr, end):
                ch1, ch2 = ctypes.c_wchar(), ctypes.c_wchar()
                cnt = ctypes.c_size_t()
                libpysdcxx.wbigrams_citer_deref(
                    itr, ctypes.byref(ch1), ctypes.byref(ch2), ctypes.byref(cnt))

                yield (ch1.value + ch2.value, cnt.value)

                libpysdcxx.wbigrams_citer_inc(itr)

        finally:
            libpysdcxx.delete_wbigrams_citer(end)
            libpysdcxx.delete_wbigrams_citer(itr)

    def send(self):
        pass

    def throw(self):
        pass

    def __iadd__(self, other: Bigrams) -> Bigrams:
        """
        Update by `other` bigrams (in-place union)
        """
        assert isinstance(other, Bigrams)
        libpysdcxx.wbigrams_iadd(self._impl, other._impl)
        return self

    def __add__(self, other: Bigrams) -> Bigrams:
        """
        :return: Union of `self` and `other` bigrams
        """
        assert isinstance(other, Bigrams)
        return Bigrams(_impl=libpysdcxx.wbigrams_add(self._impl, other._impl))

    @staticmethod
    def intersect_size(bgrms1: Bigrams, bgrms2: Bigrams) -> int:
        """
        :return: Cardinality of intersection of `bgrms1` and `bgrms2` multisets
        """
        assert isinstance(bgrms1, Bigrams)
        assert isinstance(bgrms2, Bigrams)
        return libpysdcxx.wbigrams_intersect_size(bgrms1._impl, bgrms2._impl)

    @staticmethod
    def sorensen_dice_coef(bgrms1: Bigrams, bgrms2: Bigrams) -> float:
        """
        :return: Sørensen–Dice coefficient of `bgrms1` and `bgrms2` multisets
        """
        assert isinstance(bgrms1, Bigrams)
        assert isinstance(bgrms2, Bigrams)
        return libpysdcxx.wbigrams_sorensen_dice_coef(bgrms1._impl, bgrms2._impl)

    def __str__(self):
        return serialise(
            self,
            Bigrams._str_fixed_len + 10 * len(self),
            libpysdcxx.wbigrams_str,
        )

    def __del__(self):
        libpysdcxx.delete_wbigrams(self._impl)
