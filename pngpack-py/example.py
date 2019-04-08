import pngpack

# determine image size and data
width = 3
height = 3

temp_data = [1, 1, 3,
             1, 2, 1,
             3, 1, 1]

# create bounds
bounds = pngpack.PngpackBounds(0, 360, -180, 180)

# initialize pngpack with size and bounds
pp = pngpack.Pngpack(width, height, bounds, "pp-example")

# create a channel with the data
channel = pngpack.PngpackChannel('temp', temp_data)
# add optional metadata
channel.add_textfield("units", "degrees")

# add the channel to pngpack
pp.add_channel(channel)

# output to disk
result = pp.write('example.png')
print("result: ", "success" if result else "failed")
