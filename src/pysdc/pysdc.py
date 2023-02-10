from __future__ import annotations
from typing import Optional, ClassVar, Generator, Tuple, Dict
import ctypes
from ctypes.util import find_library


def _load_libpysdc() -> ctypes.CDLL:
    """
    Load libpysdc library and set binding code up
    :return: Library functions handle
    """
    shared_obj = find_library("pysdc")
    if not shared_obj:  # libpysdc not found
        raise FileNotFoundError("Failed to find extension library")

    libpysdc = ctypes.cdll.LoadLibrary(shared_obj)

    libpysdc.new_wbigrams_str.argtypes = (ctypes.c_wchar_p, )

    libpysdc.wbigrams_size.restype = ctypes.c_size_t

    libpysdc.wbigrams_citer_deref.argtypes = (
        ctypes.c_void_p,
        ctypes.POINTER(ctypes.c_wchar),
        ctypes.POINTER(ctypes.c_wchar),
        ctypes.POINTER(ctypes.c_size_t)
    )

    libpysdc.wbigrams_intersect_size.restype = ctypes.c_size_t
    libpysdc.wbigrams_sorensen_dice_coef.restype = ctypes.c_double

    libpysdc.wbigrams_str.argtypes = (
        ctypes.c_void_p,
        ctypes.POINTER(ctypes.c_wchar),
        ctypes.c_size_t
    )
    libpysdc.wbigrams_str.restype = ctypes.c_size_t

    return libpysdc


class Bigrams(Generator[Tuple[str, int], None, None]):
    """
    Bigrams multiset
    """

    wbigrams_str_fixed_len: ClassVar[int] = len("wbigrams(size: XXXXXXXXXX, {})")
    libpysdc: ClassVar[ctypes.CDLL] = _load_libpysdc()

    def __init__(self, string: Optional[str] = None, _impl: Optional = None):
        """
        :param string: String from which bigrams ultiset shall be created
        """
        self._impl = Bigrams.libpysdc.new_wbigrams_str(ctypes.c_wchar_p(string)) \
            if string is not None else _impl or Bigrams.libpysdc.new_wbigrams()

    def __len__(self):
        return Bigrams.libpysdc.wbigrams_size(self._impl)

    def __iter__(self):
        """
        :return: Generator of bigrams together with their counts as tuple[str, int]
        """
        itr = Bigrams.libpysdc.wbigrams_cbegin(self._impl)
        end = Bigrams.libpysdc.wbigrams_cend(self._impl)
        try:
            while Bigrams.libpysdc.wbigrams_citer_ne(itr, end):
                ch1, ch2 = ctypes.c_wchar(), ctypes.c_wchar()
                cnt = ctypes.c_size_t()
                Bigrams.libpysdc.wbigrams_citer_deref(
                    itr, ctypes.byref(ch1), ctypes.byref(ch2), ctypes.byref(cnt))

                yield (ch1.value + ch2.value, cnt.value)

                Bigrams.libpysdc.wbigrams_citer_inc(itr)

        finally:
            Bigrams.libpysdc.delete_wbigrams_citer(end)
            Bigrams.libpysdc.delete_wbigrams_citer(itr)

    def send(self):
        pass

    def throw(self):
        pass

    def __iadd__(self, other: Bigrams) -> Bigrams:
        """
        Update by `other` bigrams (in-place union)
        """
        Bigrams.libpysdc.wbigrams_iadd(self._impl, other._impl)
        return self

    def __add__(self, other: Bigrams) -> Bigrams:
        """
        :return: Union of `self` and `other` bigrams
        """
        return Bigrams(_impl=Bigrams.libpysdc.wbigrams_add(self._impl, other._impl))

    @staticmethod
    def intersect_size(bgrms1: Bigrams, bgrms2: Bigrams) -> int:
        """
        :return: Cardinality of intersection of `bgrms1` and `bgrms2` multisets
        """
        return Bigrams.libpysdc.wbigrams_intersect_size(bgrms1._impl, bgrms2._impl)

    @staticmethod
    def sorensen_dice_coef(bgrms1: Bigrams, bgrms2: Bigrams) -> float:
        """
        :return: Sørensen–Dice coefficient of `bgrms1` and `bgrms2` multisets
        """
        return Bigrams.libpysdc.wbigrams_sorensen_dice_coef(bgrms1._impl, bgrms2._impl)

    def __str__(self):
        max_len = Bigrams.wbigrams_str_fixed_len + 10 * len(self)   # should be sufficient
        buf = ctypes.create_unicode_buffer(max_len + 4)             # + space for "...\0"

        written = Bigrams.libpysdc.wbigrams_str(self._impl, buf, max_len)
        if not written < max_len:
            buf[max_len + 0] = '.'
            buf[max_len + 1] = '.'
            buf[max_len + 2] = '.'
            buf[max_len + 3] = '\0'

        return f"{self.__class__.__name__}.{buf.value}"

    def __del__(self):
        Bigrams.libpysdc.delete_wbigrams(self._impl)
