from __future__ import annotations
from typing import Optional, ClassVar, Tuple, Union, Type
from dataclasses import dataclass
import ctypes

from .libpysdcxx import libpysdcxx
from .bigrams import Bigrams


class SequenceMatcher:
    """
    String (token) sequence matcher.

    Uses Sørensen–Dice coefficient calculated for bigram mnultisets of (sequences of)
    tokens to find "fuzzy" matches in given text.
    """

    class Error(Exception):
        """
        Sequence matcher error
        """

    TokenOrBigrams = Union[str, Bigrams]                        # token or token Bigrams
    Token = Union[TokenOrBigrams, Tuple[TokenOrBigrams, bool]]  # token (with strip flag)

    @dataclass
    class Match:
        """
        Token sub-sequence match
        :param begin: Index of the 1st token of the matching sub-sequence
        :param end: Index just past the last token of the matching sub-sequence
        :param score: Sørensen–Dice similarity of the sub-sequence
        :param bigrams: Matching sub-sequence bigrams (if required)
        """
        begin: int
        end: int
        score: float
        bigrams: Optional[Bigrams]

    def __init__(
        self,
        tokens: Optional[Iterable[Token]] = None,
        reserve: int = 0,
    ):
        """
        The matcher manages upper triangular matrix of token bigrams.
        If `tokens` parameter is provided and its size can be found then reservation
        of space for the token bigrams is done automatically (e.g. if it's a `list`).
        In other cases, the `reserve` parameter may be used for that purpose.
        Note that space reservation is not required; yet may notably speed up
        construction of the matcher.

        The `tokens` parameter is expected to iterate the sequence tokens.
        The following values are acceptable:
        * `str` token
        * `Bigrams` object
        * `tuple[str,bool]` of `str` token together with "strip" flag (see `append`)
        * `tuple[Bigrams,bool]` the same for `Bigrams` object

        :param tokens: Token sequence
        :param reserve: Reserve space for `reserve` tokens of text (for 1-by-1 additions)
        """
        self._impl = libpysdcxx.new_wsequence_matcher()

        if tokens and hasattr(tokens, "__len__"):
            reserve = len(tokens)
        self.reserve(reserve)

        if tokens:
            for token in tokens:
                strip = False
                if isinstance(token, tuple):
                    token, strip = token

                self.append(token, strip)

    def reserve(self, size: int):
        """
        Reserve space for token bigrams
        :param size: Anticipated number of tokens
        """
        libpysdcxx.wsequence_matcher_reserve(self._impl, size)

    def append(self, token: TokenOrBigrams, strip: bool = False):
        """
        Append another `token` to the sequence

        If the `strip` parameter is set to true, the matcher shall not produce matches
        which begin or end with that token.

        :param token: Appended token (may be a string or `Bigrams` object)
        :param strip: Whether matches may begin/ end by the token or not
        """
        strip_int = 1 if strip else 0

        if isinstance(token, Bigrams):
            libpysdcxx.wsequence_matcher_push_back(self._impl, token._impl, strip_int)
        elif isinstance(token, str):
            libpysdcxx.wsequence_matcher_emplace_back(self._impl, token, strip_int)
        else:
            raise SequenceMatcher.Error(f"Unsupported token: {token}")

    def __len__(self):
        """
        :return: Number of token bigrams (i.e. full sequence length)
        """
        return libpysdcxx.wsequence_matcher_size(self._impl)

    def __deepcopy__(self, memo):
        """
        Make a copy on the native level
        :param memo: IDs of already copied objects (unused, we're non-recursive)
        """
        raise SequenceMatcher.Error(
            f"Copying {self.__class__.__name__} objects is not supported")

    def __copy__(self):
        """
        We don't do shallow copies
        """
        return self.__deepcopy__(None)

    def match(
        self,
        tokens: Union[TokenOrBigrams, Iterable[TokenOrBigrams]],
        threshold: float,
        include_bigrams: bool = False,
    ) -> Iterator[SequenceMatcher.Match]:
        """
        Match `tokens` to the matcher-managed token sequence

        The function can, again, accept various types for the `tokens` parameter:
        * A `str` token
        * A `Bigrams` object
        * An iterable of `str` tokens
        * An iterable of `Bigrams` objects

        Produced match may take 2 possible forms:
        * `tuple[int,int,float]`: a tuple containing match begin, end and score
        * `tuple[int,int,float,Bigrams]` same, enriched with matching sub-seq. bigrams

        Match begin is the index of matching sub-sequence 1st token.
        Match end is index just past matching sub-sequence last token (following usual
        Python begin-end iteration specification convention).
        Match score is the calculated Sørensen–Dice similarity of the matching
        sub-sequence and the specified `token`.
        If `include_bigrams` is `True`, the tuple shall also contain the matching
        sub-sequence bigrams.

        Matches are produced in lexicographic order of ascending begin and length.

        :param tokens: Matched tokens specification
        :param threshold: Matching score (Sørensen–Dice coefficient) threshold
        :param include_bigrams: Include matching sub-sequence bigrams in match tuple
        :return: Generator of matches
        """
        if isinstance(tokens, Bigrams):  # single bigram multiset
            bgrms = tokens

        elif isinstance(tokens, str):  # single token
            bgrms = Bigrams(tokens)

        elif hasattr(tokens, "__iter__"):  # create Bigrams union
            bgrms = Bigrams()
            for token in tokens:
                if isinstance(token, Bigrams):
                    pass
                elif isinstance(token, str):
                    token = Bigrams(token)
                else:
                    raise SequenceMatcher.Error(f"Unsupported token: {token}")

                bgrms += token

        else:
            raise SequenceMatcher.Error(f"Unsupported token: {token}")

        itr = libpysdcxx.wsequence_matcher_begin(self._impl, bgrms._impl, threshold)
        end = libpysdcxx.wsequence_matcher_end(self._impl)
        try:
            while libpysdcxx.wsequence_matcher_iter_ne(itr, end):
                yield SequenceMatcher.Match(
                    begin=libpysdcxx.wsequence_matcher_iter_begin(itr),
                    end=libpysdcxx.wsequence_matcher_iter_end(itr),
                    score=libpysdcxx.wsequence_matcher_iter_sdc(itr),
                    bigrams=Bigrams(_impl=libpysdcxx.wsequence_matcher_iter_deref(itr)) \
                        if include_bigrams else None,
                )

                libpysdcxx.wsequence_matcher_iter_inc(itr)

        finally:
            libpysdcxx.delete_wsequence_matcher_iter(end)
            libpysdcxx.delete_wsequence_matcher_iter(itr)

    def __del__(self):
        libpysdcxx.delete_wsequence_matcher(self._impl)
