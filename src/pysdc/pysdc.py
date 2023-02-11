from __future__ import annotations
from typing import Optional, ClassVar, Generator, Tuple, Dict
import ctypes
from ctypes.util import find_library
from os.path import dirname, realpath
from glob import glob


def _load_libpysdc() -> ctypes.CDLL:
    """
    Load libpysdc library and set binding code up
    :return: Library functions handle
    """
    shared_obj = find_library("pysdc")
    if not shared_obj:  # huh, won't be that easy... :-(
        lib_dir = dirname(realpath(__file__))
        pysdc_libs = glob(f"{lib_dir}/libpysdc.*")
        shared_obj = pysdc_libs[0] if pysdc_libs else None

    if not shared_obj:  # bugger! :-(
        raise FileNotFoundError("Failed to find extension library")

    libpysdc = ctypes.cdll.LoadLibrary(shared_obj)

    # Constructors
    libpysdc.new_wbigrams.restype = ctypes.c_void_p

    libpysdc.new_wbigrams_str.argtypes = (ctypes.c_wchar_p, )
    libpysdc.new_wbigrams_str.restype = ctypes.c_void_p

    # Destructor
    libpysdc.delete_wbigrams.argtypes = (ctypes.c_void_p, )
    libpysdc.delete_wbigrams.restype = None  # void

    # Size
    libpysdc.wbigrams_size.argtypes = (ctypes.c_void_p, )
    libpysdc.wbigrams_size.restype = ctypes.c_size_t

    # Iterators
    libpysdc.wbigrams_cbegin.argtypes = (ctypes.c_void_p, )
    libpysdc.wbigrams_cbegin.restype = ctypes.c_void_p

    libpysdc.wbigrams_cend.argtypes = (ctypes.c_void_p, )
    libpysdc.wbigrams_cend.restype = ctypes.c_void_p

    libpysdc.wbigrams_citer_ne.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdc.wbigrams_citer_ne.restype = ctypes.c_int

    libpysdc.wbigrams_citer_deref.argtypes = (
        ctypes.c_void_p,
        ctypes.POINTER(ctypes.c_wchar),
        ctypes.POINTER(ctypes.c_wchar),
        ctypes.POINTER(ctypes.c_size_t)
    )
    libpysdc.wbigrams_citer_deref.restype = None  # void

    libpysdc.wbigrams_citer_inc.argtypes = (ctypes.c_void_p, )
    libpysdc.wbigrams_citer_inc.restype = None  # void

    libpysdc.delete_wbigrams_citer.argtypes = (ctypes.c_void_p, )
    libpysdc.delete_wbigrams_citer.restype = None  # void

    # Union (add operators)
    libpysdc.wbigrams_iadd.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdc.wbigrams_iadd.restype = ctypes.c_void_p

    libpysdc.wbigrams_add.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdc.wbigrams_add.restype = ctypes.c_void_p

    # SDC
    libpysdc.wbigrams_intersect_size.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdc.wbigrams_intersect_size.restype = ctypes.c_size_t

    libpysdc.wbigrams_sorensen_dice_coef.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdc.wbigrams_sorensen_dice_coef.restype = ctypes.c_double

    # Serialisation
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
        :param string: String from which bigrams multiset shall be created
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
