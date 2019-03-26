# cython: language_level=3

cimport cpngpack

# from cpython cimport array
# import array

cdef class PngpackBounds:
    cdef cpngpack.pngpack_bounds _c_bounds

    def __init__(self, min_w, max_w, min_h, max_h):
        self._c_bounds.min_w = min_w
        self._c_bounds.max_w = max_w
        self._c_bounds.min_h = min_h
        self._c_bounds.max_h = max_h

cdef class PngpackChannel:
    cdef cpngpack.pngpack_channel* _c_channel
    cdef double[:] _data

    def __cinit__(self, str name, double[:] data):
        # copy the data, and if it's an iterable, make it a list:
        self._data = list(data)

        # keep track of if we've been added to a pngpack instance or not
        self._pngpack = None

        self._c_channel = cpngpack.pngpack_channel_new(name.encode('UTF-8'), &self._data[0], len(data))
        if self._c_channel is NULL:
            raise MemoryError()

    def __dealloc__(self):
        if self._c_channel is not NULL and self._pngpack is None:
            cpngpack.pngpack_channel_free(self._c_channel)

cdef class Pngpack:
    cdef cpngpack.pngpack* _c_pngpack

    def __cinit__(self, width, height, PngpackBounds bounds, str text_namespace):
        self._c_pngpack = cpngpack.pngpack_new(width, height, bounds._c_bounds, text_namespace.encode('UTF-8'))
        if self._c_pngpack is NULL:
            raise MemoryError()

    def add_channel(self, PngpackChannel channel):
        channel._pngpack = self
        cpngpack.pngpack_add_channel(self._c_pngpack, channel._c_channel)

    def write(self, path):
        return cpngpack.pngpack_write(self._c_pngpack, path)

    def __dealloc__(self):
        if self._c_pngpack is not NULL:
            cpngpack.pngpack_free(self._c_pngpack)
