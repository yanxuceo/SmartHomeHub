import subprocess

def display_image_on_screen(image_path):
    subprocess.run(["feh", "--hide-pointer", "-x", "-q", "-B", "grey", "-g", "1920x1080", image_path])


if __name__ == "__main__":
    image_path = "../img/ship.png"
    display_image_on_screen(image_path)
   