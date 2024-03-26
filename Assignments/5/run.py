import os
from PIL import Image

img = Image.open('binary.ppm')
img.save('images/a.bmp')