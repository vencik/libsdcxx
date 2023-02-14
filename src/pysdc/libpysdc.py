import ctypes
from ctypes.util import find_library
from os import environ
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
        pysdc_libs = glob(f"{lib_dir}/../libpysdc.*")
        shared_obj = pysdc_libs[0] if pysdc_libs else None

    if not shared_obj:  # let's try LD_LIBRARY_PATH, I'm beginning to panic... :-((
        for ld_lib_dir in (environ.get("LD_LIBRARY_PATH") or "").split(':'):
            if not ld_lib_dir:
                continue

            pysdc_libs = glob(f"{ld_lib_dir}/libpysdc.*")
            if pysdc_libs:
                shared_obj = pysdc_libs[0]
                break

    if not shared_obj:  # bugger! x-(
        raise FileNotFoundError("Failed to find extension library")

    return ctypes.cdll.LoadLibrary(shared_obj)


def _bind_bigrams(libpysdc: ctypes.CDLL):
    # Constructors
    libpysdc.new_wbigrams.restype = ctypes.c_void_p

    libpysdc.new_wbigrams_str.argtypes = (ctypes.c_wchar_p, )
    libpysdc.new_wbigrams_str.restype = ctypes.c_void_p

    libpysdc.new_wbigrams_copy.argtypes = (ctypes.c_void_p, )
    libpysdc.new_wbigrams_copy.restype = ctypes.c_void_p

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
        ctypes.POINTER(ctypes.c_size_t),
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
        ctypes.c_size_t,
    )
    libpysdc.wbigrams_str.restype = ctypes.c_size_t


def _bind_bigram_multiset(libpysdc: ctypes.CDLL):
    # Constructors
    libpysdc.new_wbigram_multiset.restype = ctypes.c_void_p

    libpysdc.new_wbigram_multiset_str.argtypes = (ctypes.c_wchar_p, )
    libpysdc.new_wbigram_multiset_str.restype = ctypes.c_void_p

    libpysdc.new_wbigram_multiset_copy.argtypes = (ctypes.c_void_p, )
    libpysdc.new_wbigram_multiset_copy.restype = ctypes.c_void_p

    # Destructor
    libpysdc.delete_wbigram_multiset.argtypes = (ctypes.c_void_p, )
    libpysdc.delete_wbigram_multiset.restype = None  # void

    # Size
    libpysdc.wbigram_multiset_size.argtypes = (ctypes.c_void_p, )
    libpysdc.wbigram_multiset_size.restype = ctypes.c_size_t

    # Iterators
    libpysdc.wbigram_multiset_cbegin.argtypes = (ctypes.c_void_p, )
    libpysdc.wbigram_multiset_cbegin.restype = ctypes.c_void_p

    libpysdc.wbigram_multiset_cend.argtypes = (ctypes.c_void_p, )
    libpysdc.wbigram_multiset_cend.restype = ctypes.c_void_p

    libpysdc.wbigram_multiset_citer_ne.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdc.wbigram_multiset_citer_ne.restype = ctypes.c_int

    libpysdc.wbigram_multiset_citer_deref.argtypes = (
        ctypes.c_void_p,
        ctypes.POINTER(ctypes.c_wchar),
        ctypes.POINTER(ctypes.c_wchar),
    )
    libpysdc.wbigram_multiset_citer_deref.restype = None  # void

    libpysdc.wbigram_multiset_citer_inc.argtypes = (ctypes.c_void_p, )
    libpysdc.wbigram_multiset_citer_inc.restype = None  # void

    libpysdc.delete_wbigram_multiset_citer.argtypes = (ctypes.c_void_p, )
    libpysdc.delete_wbigram_multiset_citer.restype = None  # void

    # Union (add operators)
    libpysdc.wbigram_multiset_iadd.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdc.wbigram_multiset_iadd.restype = ctypes.c_void_p

    libpysdc.wbigram_multiset_add.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdc.wbigram_multiset_add.restype = ctypes.c_void_p

    # SDC
    libpysdc.wbigram_multiset_intersect_size.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdc.wbigram_multiset_intersect_size.restype = ctypes.c_size_t

    libpysdc.wbigram_multiset_sorensen_dice_coef.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdc.wbigram_multiset_sorensen_dice_coef.restype = ctypes.c_double

    # Serialisation
    libpysdc.wbigram_multiset_str.argtypes = (
        ctypes.c_void_p,
        ctypes.POINTER(ctypes.c_wchar),
        ctypes.c_size_t,
    )
    libpysdc.wbigram_multiset_str.restype = ctypes.c_size_t


def _bind_unordered_bigram_multiset(libpysdc: ctypes.CDLL):
    # Constructors
    libpysdc.new_unordered_wbigram_multiset.restype = ctypes.c_void_p

    libpysdc.new_unordered_wbigram_multiset_str.argtypes = (ctypes.c_wchar_p, )
    libpysdc.new_unordered_wbigram_multiset_str.restype = ctypes.c_void_p

    libpysdc.new_unordered_wbigram_multiset_copy.argtypes = (ctypes.c_void_p, )
    libpysdc.new_unordered_wbigram_multiset_copy.restype = ctypes.c_void_p

    # Destructor
    libpysdc.delete_unordered_wbigram_multiset.argtypes = (ctypes.c_void_p, )
    libpysdc.delete_unordered_wbigram_multiset.restype = None  # void

    # Size
    libpysdc.unordered_wbigram_multiset_size.argtypes = (ctypes.c_void_p, )
    libpysdc.unordered_wbigram_multiset_size.restype = ctypes.c_size_t

    # Iterators
    libpysdc.unordered_wbigram_multiset_cbegin.argtypes = (ctypes.c_void_p, )
    libpysdc.unordered_wbigram_multiset_cbegin.restype = ctypes.c_void_p

    libpysdc.unordered_wbigram_multiset_cend.argtypes = (ctypes.c_void_p, )
    libpysdc.unordered_wbigram_multiset_cend.restype = ctypes.c_void_p

    libpysdc.unordered_wbigram_multiset_citer_ne.argtypes = (
        ctypes.c_void_p,
        ctypes.c_void_p,
    )
    libpysdc.unordered_wbigram_multiset_citer_ne.restype = ctypes.c_int

    libpysdc.unordered_wbigram_multiset_citer_deref.argtypes = (
        ctypes.c_void_p,
        ctypes.POINTER(ctypes.c_wchar),
        ctypes.POINTER(ctypes.c_wchar),
    )
    libpysdc.unordered_wbigram_multiset_citer_deref.restype = None  # void

    libpysdc.unordered_wbigram_multiset_citer_inc.argtypes = (ctypes.c_void_p, )
    libpysdc.unordered_wbigram_multiset_citer_inc.restype = None  # void

    libpysdc.delete_unordered_wbigram_multiset_citer.argtypes = (ctypes.c_void_p, )
    libpysdc.delete_unordered_wbigram_multiset_citer.restype = None  # void

    # Union (add operators)
    libpysdc.unordered_wbigram_multiset_iadd.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdc.unordered_wbigram_multiset_iadd.restype = ctypes.c_void_p

    libpysdc.unordered_wbigram_multiset_add.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdc.unordered_wbigram_multiset_add.restype = ctypes.c_void_p

    # SDC
    libpysdc.unordered_wbigram_multiset_intersect_size.argtypes = (
        ctypes.c_void_p,
        ctypes.c_void_p,
    )
    libpysdc.unordered_wbigram_multiset_intersect_size.restype = ctypes.c_size_t

    libpysdc.unordered_wbigram_multiset_sorensen_dice_coef.argtypes = (
        ctypes.c_void_p,
        ctypes.c_void_p,
    )
    libpysdc.unordered_wbigram_multiset_sorensen_dice_coef.restype = ctypes.c_double

    # Serialisation
    libpysdc.unordered_wbigram_multiset_str.argtypes = (
        ctypes.c_void_p,
        ctypes.POINTER(ctypes.c_wchar),
        ctypes.c_size_t,
    )
    libpysdc.unordered_wbigram_multiset_str.restype = ctypes.c_size_t


libpysdc = _load_libpysdc()
_bind_bigrams(libpysdc)
_bind_bigram_multiset(libpysdc)
_bind_unordered_bigram_multiset(libpysdc)
