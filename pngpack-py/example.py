import pngpack

width = 3
height = 3
bounds = pngpack.PngpackBounds(0, 360, -180, 180)

pp = pngpack.Pngpack(width, height, bounds, "pp-example")
