from __future__ import annotations
from typing import Generator
from copy import deepcopy

from multiset import Multiset


class PyBigrams(Generator[str, None, None]):
    """
    Bigram multiset (implemented on top of Python multiset module)
    """

    def __init__(self, string: str = ""):
        """
        :param string: String to make bigrams from
        """
        self._impl = Multiset(string[i] + string[i+1] for i in range(len(string) - 1))

    def __len__(self):
        return len(self._impl)

    def __iter__(self):
        return iter(self._impl)

    def send(self):
        pass

    def throw(self):
        pass

    def __iadd__(self, other: PyBigrams) -> PyBigrams:
        """
        Update by `other` bigrams (in-place union)
        """
        self._impl.update(other._impl)
        return self

    def __add__(self, other: PyBigrams) -> PyBigrams:
        """
        :return: Union of `self` and `other` bigrams
        """
        result = deepcopy(self)
        result += other
        return result

    @staticmethod
    def intersect_size(bgrms1: PyBigrams, bgrms2: PyBigrams) -> int:
        """
        :return: Cardinality of intersection of `bgrms1` and `bgrms2` multisets
        """
        return len(bgrms1._impl & bgrms2._impl)

    @staticmethod
    def sorensen_dice_coef(bgrms1: PyBigrams, bgrms2: PyBigrams) -> float:
        """
        :return: Sørensen–Dice coefficient of `bgrms1` and `bgrms2` multisets
        """
        isize = PyBigrams.intersect_size(bgrms1, bgrms2)
        return 2.0 * isize / (len(bgrms1) + len(bgrms2))

    def __str__(self):
        return f"{self.__class__.__name__}(size: {len(self._impl)}, {self._impl})"
