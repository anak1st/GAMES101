import os
import argparse
from PIL import Image

# os.system(R'.\build\windows\x64\release\RayTracing.exe')

args = argparse.ArgumentParser()
args.add_argument('-f', type=str, default='16')

args = args.parse_args()

file = args.f
img = Image.open(f'./binary_{file}.ppm')

# save as png
img.save(f'./images/{file}.png')
