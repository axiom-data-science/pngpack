# pngpack

pngpack is a library used to pack floating point values into 16-bit PNG images. It supports 1-4 channels per image, storing metadata as PNG tEXt chunks. Metadata includes global bounds to give the image context, values to allow unpacking each channel by a client, and statistics for each channel.

There are 2 parts to pngpack: libpngpack, the C library, and pngpack-py, the Python module which wraps libpngpack.

pngpack depends on libpng.

## Usage

To use pngpack, you initialize a pngpack instance, initialize channels for each array
you wish to pack, optionally add additional metadata to each channel,add the channels
to the pngpack instance. Then you can write the PNG to disk.

There is an example file doing just this in both libpngpack and pngpack-py.

### Usage notes:

- Once a channel is added to a pngpack instance, it becomes owned and managed by pngpack.
It should not be modified or freed by the user after this.
- Adding a channel to a pngpack instance is irreversible. pngpack should be initialized
and used after you know what data is going into the PNG file.
- The length of each array must exactly equal the width*height of the pngpack instance.
- Floating point NaN values are supported
