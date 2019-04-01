# cython: language_level=3

cimport cpngpack

from typing import Iterable

from cpython cimport array
import array

cdef class PngpackBounds:
    cdef cpngpack.pngpack_bounds _c_bounds

    def __init__(self, double min_w, double max_w, double min_h, double max_h):
        self._c_bounds.min_w = min_w
        self._c_bounds.max_w = max_w
        self._c_bounds.min_h = min_h
        self._c_bounds.max_h = max_h

cdef class PngpackChannel:
    cdef cpngpack.pngpack_channel* _c_channel
    cdef array.array _data

    # keep track of if we've been added to a pngpack instance or not
    cdef bint _pngpack

    def __init__(self, str name, data: Iterable):
        pass

    def __cinit__(self, str name, data: Iterable):
        self._pngpack = False

        self._data = array.array('d', data)

        self._c_channel = cpngpack.pngpack_channel_new(name.encode('UTF-8'), &self._data.data.as_doubles[0], len(data))
        if self._c_channel is NULL:
            raise MemoryError()

    def __dealloc__(self):
        if self._c_channel is not NULL and self._pngpack is None:
            cpngpack.pngpack_channel_free(self._c_channel)

    def add_textfield(self, str name, str value):
        cpngpack.pngpack_channel_add_textfield(self._c_channel, name.encode('UTF-8'), value.encode('UTF-8'))

cdef class Pngpack:
    cdef cpngpack.pngpack* _c_pngpack

    def __init__(self, size_t width, size_t height, PngpackBounds bounds, str text_namespace):
        pass

    def __cinit__(self, size_t width, size_t height, PngpackBounds bounds, str text_namespace):
        self._c_pngpack = cpngpack.pngpack_new(width, height, bounds._c_bounds, text_namespace.encode('UTF-8'))
        if self._c_pngpack is NULL:
            raise MemoryError()

    def add_channel(self, PngpackChannel channel):
        channel._pngpack = True
        cpngpack.pngpack_add_channel(self._c_pngpack, channel._c_channel)

    def write(self, str path):
        return cpngpack.pngpack_write(self._c_pngpack, path.encode('UTF-8'))

    def __dealloc__(self):
        if self._c_pngpack is not NULL:
            cpngpack.pngpack_free(self._c_pngpack)
