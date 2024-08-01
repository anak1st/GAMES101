import os
from PIL import Image

# os.system(R'.\build\windows\x64\release\RayTracing.exe')
img = Image.open('binary_16.ppm')
img.save('images/16.bmp')