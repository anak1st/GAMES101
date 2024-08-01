import os
from PIL import Image

os.system(R'.\build\windows\x64\release\RayTracing.exe')
img = Image.open('binary.ppm')
img.save('images/a.bmp')