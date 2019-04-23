# cython: language_level=3

cimport cpngpack

from typing import Iterable

from cpython cimport array
import array

cdef class PngpackBounds:
    """
    An object representing the bounds of a packed PNG file.
    These bounds represent the top-left and bottom-right pixels
    of the image, and allow you to give context to the packed data.

    For example, if the packed data represented temperatures over a grid,
    the bounds may represent the max/min lat/lon of the grid.
    """

    cdef cpngpack.pngpack_bounds _c_bounds

    def __init__(self, double min_w, double max_w, double min_h, double max_h):
        """
        Create an instance of PngpackBounds.

        :param min_w: min_w value
        :param max_w: max_w value
        :param min_h: min_h value
        :param max_h: max_h value
        """
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
        """
        Create a channel to be added to a Pngpack object.

        :param name: A name/label to be stored as metadata
        :param data: The data, as floating point values. This data will be copied.
        """
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
        """
        Add a PNG text field namespaced by this channel

        :param name: the key
        :param value: the value
        """
        cpngpack.pngpack_channel_add_textfield(self._c_channel, name.encode('UTF-8'), value.encode('UTF-8'))

cdef class Pngpack:
    cdef cpngpack.pngpack* _c_pngpack

    def __init__(self, size_t width, size_t height, PngpackBounds bounds, str text_namespace):
        """
        Create a Pngpack object.
        Channels can be added to this object, and then the PNG can be written to disk.

        :param width: The width of the resulting image
        :param height: The height of the resulting image
        :param bounds: The bounds for the resulting image
        :param text_namespace: Prefix to be used for all PNG text field names
        """
        pass

    def __cinit__(self, size_t width, size_t height, PngpackBounds bounds, str text_namespace):
        self._c_pngpack = cpngpack.pngpack_new(width, height, bounds._c_bounds, text_namespace.encode('UTF-8'))
        if self._c_pngpack is NULL:
            raise MemoryError()

    def add_textfield(self, str name, str value):
        """
        Add a PNG text field to this instance

        :param name: the key
        :param value: the value
        """
        cpngpack.pngpack_add_textfield(self._c_pngpack, name.encode('UTF-8'), value.encode('UTF-8'))

    def add_channel(self, PngpackChannel channel):
        """
        Add a channel to this PNG file. This is non-reversible.

        :param channel: The channel to add
        """
        channel._pngpack = True
        cpngpack.pngpack_add_channel(self._c_pngpack, channel._c_channel)

    def write(self, str path) -> bool:
        """
        Write the packed PNG file to disk.

        :param path: the path to write to
        :return: boolean indicating success
        """
        return cpngpack.pngpack_write(self._c_pngpack, path.encode('UTF-8'))

    def __dealloc__(self):
        if self._c_pngpack is not NULL:
            cpngpack.pngpack_free(self._c_pngpack)
