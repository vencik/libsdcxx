from ctypes import create_unicode_buffer


def serialise(obj: object, max_len: int, impl):
    """
    Object serialisation helper function

    The function calls native code serialisation implementation, providing it with
    string buffer for the result.
    The produced string is expected to fit in `max_len` characters.
    The native code honours that maximum and returns the number of characters
    written.
    In case of incomplete serialisation, the function adds dots at the end of
    produced string to indicate that situation.
    (The serialisation should never be used for anything serious; just in logs etc.)

    :param obj: Serialised object
    :param max_len: Maximal expected serialisation string length
    :param impl: `ctypes` implementation function pointer
    :return: Serialisation string
    """
    buf = create_unicode_buffer(max_len + 4)  # include space for "...\0"
    written = impl(obj._impl, buf, max_len)
    if not written < max_len:
        buf[max_len + 0] = '.'
        buf[max_len + 1] = '.'
        buf[max_len + 2] = '.'
        buf[max_len + 3] = '\0'

    return f"{obj.__class__.__name__}.{buf.value}"
