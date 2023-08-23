import subprocess
import os


def display_image_on_screen(image_path):
    subprocess.Popen(["./display_image.sh", image_path])

if __name__ == "__main__":
    image_path = "../img/ship.png"
    display_image_on_screen(image_path)
   