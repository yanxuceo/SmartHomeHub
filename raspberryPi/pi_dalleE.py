import openai
import os

import requests

openai.api_key = os.getenv("OPENAI_API_KEY")


def generate_image(prompt_text):
    # Use the OpenAI API to generate an image based on the given prompt
    response = openai.Image.create(
      prompt=prompt_text,
      n=1,
      size="512x512"
    )

    image_url = response['data'][0]['url']
    
    # Use requests to download the image
    image_response = requests.get(image_url)

    # Check if the request was successful
    image_response.raise_for_status()

    # Save the image locally
    local_image_name = "gen.png"
    with open(local_image_name, 'wb') as file:
        file.write(image_response.content)

    print(f"Image saved as {local_image_name}")
    return local_image_name



# Example usage
if __name__ == "__main__":
    local_image_path = generate_image("a ship in the ocean")
