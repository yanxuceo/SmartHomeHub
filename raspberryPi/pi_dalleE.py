import os

import openai
import json

# Set your OpenAI API key
openai.api_key = os.getenv("OPENAI_API_KEY")


response = openai.Image.create(
  prompt="a ship in the ocean",
  n=1,
  size="512x512"
)


image_url = response['data'][0]['url']
print(image_url)