# pngpack

pngpack is a library used to pack floating point values into 16-bit PNG images. It supports 1-4 channels per image, storing metadata as PNG tEXt chunks. Metadata includes global bounds to give the image context, values to allow unpacking each channel by a client, and statistics for each channel.

There are 2 parts to pngpack: libpngpack, the C library, and pngpack-py, the Python module which wraps libpngpack.

pngpack depends on libpng.
