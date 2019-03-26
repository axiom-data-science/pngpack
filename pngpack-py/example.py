import pngpack

width = 3
height = 3
bounds = pngpack.PngpackBounds(0, 360, -180, 180)

pp = pngpack.Pngpack(width, height, bounds, "pp-example")

data = [1, 1, 3,
        1, 2, 1,
        3, 1, 1]
channel = pngpack.PngpackChannel('temp', data)

pp.add_channel(channel)

result = pp.write('example.png')
print("result: ", result)
