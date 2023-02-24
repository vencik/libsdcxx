import ctypes
from ctypes.util import find_library
from os import environ
from os.path import dirname, realpath
from glob import glob


def _load_libpysdcxx() -> ctypes.CDLL:
    """
    Load libpysdcxx library and set binding code up
    :return: Library functions handle
    """
    shared_obj = find_library("pysdcxx")
    if not shared_obj:  # huh, won't be that easy... :-(
        lib_dir = dirname(realpath(__file__))
        pysdcxx_libs = glob(f"{lib_dir}/../libpysdcxx.*")
        shared_obj = pysdcxx_libs[0] if pysdcxx_libs else None

    if not shared_obj:  # let's try LD_LIBRARY_PATH, I'm beginning to panic... :-((
        for ld_lib_dir in (environ.get("LD_LIBRARY_PATH") or "").split(':'):
            if not ld_lib_dir:
                continue

            pysdcxx_libs = glob(f"{ld_lib_dir}/libpysdcxx.*")
            if pysdcxx_libs:
                shared_obj = pysdcxx_libs[0]
                break

    if not shared_obj:  # bugger! x-(
        raise FileNotFoundError("Failed to find extension library")

    return ctypes.cdll.LoadLibrary(shared_obj)


def _bind_bigrams(libpysdcxx: ctypes.CDLL):
    # Constructors
    libpysdcxx.new_wbigrams.restype = ctypes.c_void_p

    libpysdcxx.new_wbigrams_str.argtypes = (ctypes.c_wchar_p, )
    libpysdcxx.new_wbigrams_str.restype = ctypes.c_void_p

    libpysdcxx.new_wbigrams_copy.argtypes = (ctypes.c_void_p, )
    libpysdcxx.new_wbigrams_copy.restype = ctypes.c_void_p

    # Destructor
    libpysdcxx.delete_wbigrams.argtypes = (ctypes.c_void_p, )
    libpysdcxx.delete_wbigrams.restype = None  # void

    # Size
    libpysdcxx.wbigrams_size.argtypes = (ctypes.c_void_p, )
    libpysdcxx.wbigrams_size.restype = ctypes.c_size_t

    # Iterators
    libpysdcxx.wbigrams_cbegin.argtypes = (ctypes.c_void_p, )
    libpysdcxx.wbigrams_cbegin.restype = ctypes.c_void_p

    libpysdcxx.wbigrams_cend.argtypes = (ctypes.c_void_p, )
    libpysdcxx.wbigrams_cend.restype = ctypes.c_void_p

    libpysdcxx.wbigrams_citer_ne.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdcxx.wbigrams_citer_ne.restype = ctypes.c_int

    libpysdcxx.wbigrams_citer_deref.argtypes = (
        ctypes.c_void_p,
        ctypes.POINTER(ctypes.c_wchar),
        ctypes.POINTER(ctypes.c_wchar),
        ctypes.POINTER(ctypes.c_size_t),
    )
    libpysdcxx.wbigrams_citer_deref.restype = None  # void

    libpysdcxx.wbigrams_citer_inc.argtypes = (ctypes.c_void_p, )
    libpysdcxx.wbigrams_citer_inc.restype = None  # void

    libpysdcxx.delete_wbigrams_citer.argtypes = (ctypes.c_void_p, )
    libpysdcxx.delete_wbigrams_citer.restype = None  # void

    # Union (add operators)
    libpysdcxx.wbigrams_iadd.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdcxx.wbigrams_iadd.restype = ctypes.c_void_p

    libpysdcxx.wbigrams_add.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdcxx.wbigrams_add.restype = ctypes.c_void_p

    # SDC
    libpysdcxx.wbigrams_intersect_size.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdcxx.wbigrams_intersect_size.restype = ctypes.c_size_t

    libpysdcxx.wbigrams_sorensen_dice_coef.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdcxx.wbigrams_sorensen_dice_coef.restype = ctypes.c_double

    # Serialisation
    libpysdcxx.wbigrams_str.argtypes = (
        ctypes.c_void_p,
        ctypes.POINTER(ctypes.c_wchar),
        ctypes.c_size_t,
    )
    libpysdcxx.wbigrams_str.restype = ctypes.c_size_t


def _bind_bigram_multiset(libpysdcxx: ctypes.CDLL):
    # Constructors
    libpysdcxx.new_wbigram_multiset.restype = ctypes.c_void_p

    libpysdcxx.new_wbigram_multiset_str.argtypes = (ctypes.c_wchar_p, )
    libpysdcxx.new_wbigram_multiset_str.restype = ctypes.c_void_p

    libpysdcxx.new_wbigram_multiset_copy.argtypes = (ctypes.c_void_p, )
    libpysdcxx.new_wbigram_multiset_copy.restype = ctypes.c_void_p

    # Destructor
    libpysdcxx.delete_wbigram_multiset.argtypes = (ctypes.c_void_p, )
    libpysdcxx.delete_wbigram_multiset.restype = None  # void

    # Size
    libpysdcxx.wbigram_multiset_size.argtypes = (ctypes.c_void_p, )
    libpysdcxx.wbigram_multiset_size.restype = ctypes.c_size_t

    # Iterators
    libpysdcxx.wbigram_multiset_cbegin.argtypes = (ctypes.c_void_p, )
    libpysdcxx.wbigram_multiset_cbegin.restype = ctypes.c_void_p

    libpysdcxx.wbigram_multiset_cend.argtypes = (ctypes.c_void_p, )
    libpysdcxx.wbigram_multiset_cend.restype = ctypes.c_void_p

    libpysdcxx.wbigram_multiset_citer_ne.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdcxx.wbigram_multiset_citer_ne.restype = ctypes.c_int

    libpysdcxx.wbigram_multiset_citer_deref.argtypes = (
        ctypes.c_void_p,
        ctypes.POINTER(ctypes.c_wchar),
        ctypes.POINTER(ctypes.c_wchar),
    )
    libpysdcxx.wbigram_multiset_citer_deref.restype = None  # void

    libpysdcxx.wbigram_multiset_citer_inc.argtypes = (ctypes.c_void_p, )
    libpysdcxx.wbigram_multiset_citer_inc.restype = None  # void

    libpysdcxx.delete_wbigram_multiset_citer.argtypes = (ctypes.c_void_p, )
    libpysdcxx.delete_wbigram_multiset_citer.restype = None  # void

    # Union (add operators)
    libpysdcxx.wbigram_multiset_iadd.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdcxx.wbigram_multiset_iadd.restype = ctypes.c_void_p

    libpysdcxx.wbigram_multiset_add.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdcxx.wbigram_multiset_add.restype = ctypes.c_void_p

    # SDC
    libpysdcxx.wbigram_multiset_intersect_size.argtypes = (
        ctypes.c_void_p,
        ctypes.c_void_p,
    )
    libpysdcxx.wbigram_multiset_intersect_size.restype = ctypes.c_size_t

    libpysdcxx.wbigram_multiset_sorensen_dice_coef.argtypes = (
        ctypes.c_void_p,
        ctypes.c_void_p,
    )
    libpysdcxx.wbigram_multiset_sorensen_dice_coef.restype = ctypes.c_double

    # Serialisation
    libpysdcxx.wbigram_multiset_str.argtypes = (
        ctypes.c_void_p,
        ctypes.POINTER(ctypes.c_wchar),
        ctypes.c_size_t,
    )
    libpysdcxx.wbigram_multiset_str.restype = ctypes.c_size_t


def _bind_unordered_bigram_multiset(libpysdcxx: ctypes.CDLL):
    # Constructors
    libpysdcxx.new_unordered_wbigram_multiset.restype = ctypes.c_void_p

    libpysdcxx.new_unordered_wbigram_multiset_str.argtypes = (ctypes.c_wchar_p, )
    libpysdcxx.new_unordered_wbigram_multiset_str.restype = ctypes.c_void_p

    libpysdcxx.new_unordered_wbigram_multiset_copy.argtypes = (ctypes.c_void_p, )
    libpysdcxx.new_unordered_wbigram_multiset_copy.restype = ctypes.c_void_p

    # Destructor
    libpysdcxx.delete_unordered_wbigram_multiset.argtypes = (ctypes.c_void_p, )
    libpysdcxx.delete_unordered_wbigram_multiset.restype = None  # void

    # Size
    libpysdcxx.unordered_wbigram_multiset_size.argtypes = (ctypes.c_void_p, )
    libpysdcxx.unordered_wbigram_multiset_size.restype = ctypes.c_size_t

    # Iterators
    libpysdcxx.unordered_wbigram_multiset_cbegin.argtypes = (ctypes.c_void_p, )
    libpysdcxx.unordered_wbigram_multiset_cbegin.restype = ctypes.c_void_p

    libpysdcxx.unordered_wbigram_multiset_cend.argtypes = (ctypes.c_void_p, )
    libpysdcxx.unordered_wbigram_multiset_cend.restype = ctypes.c_void_p

    libpysdcxx.unordered_wbigram_multiset_citer_ne.argtypes = (
        ctypes.c_void_p,
        ctypes.c_void_p,
    )
    libpysdcxx.unordered_wbigram_multiset_citer_ne.restype = ctypes.c_int

    libpysdcxx.unordered_wbigram_multiset_citer_deref.argtypes = (
        ctypes.c_void_p,
        ctypes.POINTER(ctypes.c_wchar),
        ctypes.POINTER(ctypes.c_wchar),
    )
    libpysdcxx.unordered_wbigram_multiset_citer_deref.restype = None  # void

    libpysdcxx.unordered_wbigram_multiset_citer_inc.argtypes = (ctypes.c_void_p, )
    libpysdcxx.unordered_wbigram_multiset_citer_inc.restype = None  # void

    libpysdcxx.delete_unordered_wbigram_multiset_citer.argtypes = (ctypes.c_void_p, )
    libpysdcxx.delete_unordered_wbigram_multiset_citer.restype = None  # void

    # Union (add operators)
    libpysdcxx.unordered_wbigram_multiset_iadd.argtypes = (
        ctypes.c_void_p,
        ctypes.c_void_p,
    )
    libpysdcxx.unordered_wbigram_multiset_iadd.restype = ctypes.c_void_p

    libpysdcxx.unordered_wbigram_multiset_add.argtypes = (
        ctypes.c_void_p,
        ctypes.c_void_p,
    )
    libpysdcxx.unordered_wbigram_multiset_add.restype = ctypes.c_void_p

    # SDC
    libpysdcxx.unordered_wbigram_multiset_intersect_size.argtypes = (
        ctypes.c_void_p,
        ctypes.c_void_p,
    )
    libpysdcxx.unordered_wbigram_multiset_intersect_size.restype = ctypes.c_size_t

    libpysdcxx.unordered_wbigram_multiset_sorensen_dice_coef.argtypes = (
        ctypes.c_void_p,
        ctypes.c_void_p,
    )
    libpysdcxx.unordered_wbigram_multiset_sorensen_dice_coef.restype = ctypes.c_double

    # Serialisation
    libpysdcxx.unordered_wbigram_multiset_str.argtypes = (
        ctypes.c_void_p,
        ctypes.POINTER(ctypes.c_wchar),
        ctypes.c_size_t,
    )
    libpysdcxx.unordered_wbigram_multiset_str.restype = ctypes.c_size_t


def _bind_sequence_matcher(libpysdcxx: ctypes.CDLL):
    # Constructor
    libpysdcxx.new_wsequence_matcher.restype = ctypes.c_void_p

    # Destructor
    libpysdcxx.delete_wsequence_matcher.argtypes = (ctypes.c_void_p, )
    libpysdcxx.delete_wsequence_matcher.restype = None  # void

    # Reserve space for sequence
    libpysdcxx.wsequence_matcher_reserve.argtypes = (ctypes.c_void_p, ctypes.c_size_t)
    libpysdcxx.wsequence_matcher_reserve.restype = None  # void

    # Size
    libpysdcxx.wsequence_matcher_size.argtypes = (ctypes.c_void_p, )
    libpysdcxx.wsequence_matcher_size.restype = ctypes.c_size_t

    # Push bigrams back
    libpysdcxx.wsequence_matcher_push_back.argtypes = (
        ctypes.c_void_p,
        ctypes.c_void_p,
        ctypes.c_int,
    )
    libpysdcxx.wsequence_matcher_push_back.restype = None  # void

    # Push in-place created bigrams back
    libpysdcxx.wsequence_matcher_emplace_back.argtypes = (
        ctypes.c_void_p,
        ctypes.c_wchar_p,
        ctypes.c_int,
    )
    libpysdcxx.wsequence_matcher_emplace_back.restype = None  # void

    # Match iterators
    libpysdcxx.wsequence_matcher_begin.argtypes = (
        ctypes.c_void_p,
        ctypes.c_void_p,
        ctypes.c_double,
    )
    libpysdcxx.wsequence_matcher_begin.restype = ctypes.c_void_p

    libpysdcxx.wsequence_matcher_end.argtypes = (ctypes.c_void_p, )
    libpysdcxx.wsequence_matcher_end.restype = ctypes.c_void_p

    libpysdcxx.wsequence_matcher_iter_deref.argtypes = (ctypes.c_void_p, )
    libpysdcxx.wsequence_matcher_iter_deref.restype = ctypes.c_void_p

    libpysdcxx.wsequence_matcher_iter_size.argtypes = (ctypes.c_void_p, )
    libpysdcxx.wsequence_matcher_iter_size.restype = ctypes.c_size_t

    libpysdcxx.wsequence_matcher_iter_begin.argtypes = (ctypes.c_void_p, )
    libpysdcxx.wsequence_matcher_iter_begin.restype = ctypes.c_size_t

    libpysdcxx.wsequence_matcher_iter_end.argtypes = (ctypes.c_void_p, )
    libpysdcxx.wsequence_matcher_iter_end.restype = ctypes.c_size_t

    libpysdcxx.wsequence_matcher_iter_sdc.argtypes = (ctypes.c_void_p, )
    libpysdcxx.wsequence_matcher_iter_sdc.restype = ctypes.c_double

    libpysdcxx.wsequence_matcher_iter_inc.argtypes = (ctypes.c_void_p, )
    libpysdcxx.wsequence_matcher_iter_inc.restype = None  # void

    libpysdcxx.wsequence_matcher_iter_ne.argtypes = (ctypes.c_void_p, ctypes.c_void_p)
    libpysdcxx.wsequence_matcher_iter_ne.restype = ctypes.c_int

    libpysdcxx.delete_wsequence_matcher_iter.argtypes = (ctypes.c_void_p, )
    libpysdcxx.delete_wsequence_matcher_iter.restype = None  # void

    # Serialise match iterator
    libpysdcxx.wsequence_matcher_iter_str.argtypes = (
        ctypes.c_void_p,
        ctypes.c_wchar_p,
        ctypes.c_size_t,
    )
    libpysdcxx.wsequence_matcher_iter_str.restype = ctypes.c_size_t


libpysdcxx = _load_libpysdcxx()
_bind_bigrams(libpysdcxx)
_bind_bigram_multiset(libpysdcxx)
_bind_unordered_bigram_multiset(libpysdcxx)
_bind_sequence_matcher(libpysdcxx)
