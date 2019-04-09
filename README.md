# pngpack

pngpack is a library used to pack floating point values into 16-bit PNG images.
It supports 1-4 channels per image, storing metadata as PNG tEXt chunks.
Metadata includes global bounds to give the image context, values to allow
unpacking each channel by a client, and statistics for each channel.

There are 2 parts to pngpack: [libpngpack](libpngpack/), the C library, and
[pngpack-py](pngpack-py/), the Python module which wraps libpngpack.

pngpack depends on libpng.

## Usage

See the [libpngpack](libpngpack/) or [pngpack-py](pngpack-py/) README files
for specific instructions for each library.

At a high level, to use pngpack, you initialize a pngpack instance, initialize
channels for each array you wish to pack, optionally add additional metadata
to each channel, add the channels to the pngpack instance.
Then you can write the PNG to disk.
The example file in each library shows how to do just this.

### Usage notes:

- Once a channel is added to a pngpack instance, it becomes owned and managed
by pngpack. It should not be modified or freed by the user after this.
- Adding a channel to a pngpack instance is irreversible. pngpack should be
initialized and used after you know what data is going into the PNG file.
- The length of each array must exactly equal the `width * height` of the
pngpack instance.
- Floating point NaN values are supported
- Depending on how many channels are added, pngpack will choose the correct
color type for the image. 1 channel: grayscale; 2 channels: grayscale, alpha;
3 channels: red, green, blue; 4 channels: red, green, blue, alpha.
