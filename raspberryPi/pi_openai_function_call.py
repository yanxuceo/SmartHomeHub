import os

import openai
import json
import requests

from datetime import datetime, timezone

import pi_notion_api
import pi_azure_tts_api
from pi_dalleE import generate_image 
from pi_display_image import display_image_on_screen

# Set your OpenAI API key
openai.api_key = os.getenv("OPENAI_API_KEY")



def get_weather(location):
    """
    Fetch current weather information for the specified location using the Weatherstack API.
    
    Parameters:
    - location (str): The location query (e.g., 'New York').
    
    Returns:
    - dict: A dictionary containing the weather data, or None if the request was unsuccessful.
    """
    api_key = os.getenv("WEATHERSTACK_API_KEY")
    # Build the API endpoint
    url = f'http://api.weatherstack.com/current?access_key={api_key}&query={location}'
    
    # Send a GET request to the API
    response = requests.get(url)
    if response.status_code == 200:
        # Parse and return the JSON response

        content_string = json.dumps(response.json())
        return content_string
    else:
        # Print an error message if the request was unsuccessful
        print("Failed to get the weather data. HTTP Response Code:", response.status_code)
        return None



def record_my_body_weight(weight: str):
    print(f"write weight {weight} into notion page.")

    name = "Xu Dong"
    body_weight = float(weight)
    record_date = datetime.now().strftime("%Y-%m-%d")

    data = {
        "Name": {"title": [{"text": {"content": name}}]},
        "Body weight": {"number": body_weight},
        "Date": {"date": {"start": record_date, "end": None}}
    }
    # create new item in Notion Database
    pi_notion_api.create_notion_page(pi_notion_api.NOTION_BODYWEIGHT_DATABASE_ID, data)


def write_shopping_item_to_notion(item: str):
    record_date = datetime.now().strftime("%Y-%m-%d")

    data = {
        "Name": {"title": [{"text": {"content": item}}]},
        "Date": {"date": {"start": record_date, "end": None}}
    }
    # create new item in Notion Database
    pi_notion_api.create_notion_page(pi_notion_api.NOTION_SHOPPING_DATABASE_ID, data)


def write_random_memos_to_notion(item: str):
    record_date = datetime.now().strftime("%Y-%m-%d")

    data = {
        "Name": {"title": [{"text": {"content": item}}]},
        "Date": {"date": {"start": record_date, "end": None}}
    }
    # create new item in Notion Database
    pi_notion_api.create_notion_page(pi_notion_api.NOTION_MEMOS_DATABASE_ID, data)



def get_shopping_items(shopping_item: str):
    item_list = shopping_item.split(',')
    for item in item_list:
        print(item.strip())
        write_shopping_item_to_notion(item)


def record_random_thoughts(memos: str):
    write_random_memos_to_notion(memos)


def openai_function_call(query):
    # send the conversation and available functions to GPT
    messages=[{"role": "user", "content": query}]

    functions = [
        {
            "name": "record_my_body_weight",
            "description": "Record my body weight",
            "parameters": {
                "type": "object",
                "properties": {
                    "weight_in_kg": {
                        "type": "string",
                        "description": "Return the body weight information in the query"
                    }
                 },
            },
            "required": ["weight_in_kg"],
        },

        {
            "name": "get_weather",
            "description": "Get the current weather in a given location",
            "parameters": {
                "type": "object",
                "properties": {
                    "location": {
                        "type": "string",
                        "description": "The city name, e.g. Munich"
                    },
                    "unit": {
                        "type": "string",
                        "enum": ["celsius", "fahrenheit"]
                    }
                 },
            },
            "required": ["location"],
        },

        {
            "name": "generate_image",
            "description": "generate image with given prompt description",
            "parameters": {
                "type": "object",
                "properties": {
                    "prompt_text": {
                        "type": "string",
                        "description": "the description of a desired image, such as: a cat lying on sofa"
                    },
                 },
            },
            "required": ["prompt_text"],
        },

        {
            "name": "get_shopping_items",
            "description": "Get all the shopping items I would like to buy",
            "parameters": {
                "type": "object",
                "properties": {
                    "shopping_items": {
                        "type": "string",
                        "description": "a list of the planned shopping items in daily life, such as vegetables, fruits, meat, toilet paper, etc"
                    },
                },
            },
            "required": ["shopping_items"],
        },

        {
            "name": "record_random_thoughts",
            "description": "Record my random thoughts, memos, reflections, ponderings",
            "parameters": {
                "type": "object",
                "properties": {
                    "memos": {
                        "type": "string",
                        "description": "some free-brainstorming, random thoughts, memos, e.g, my memos, blabla"
                    },
                },
            },
            "required": ["memos"],
        },
    ]

    response = openai.ChatCompletion.create(
        model="gpt-3.5-turbo-0613",
        messages=messages,
        functions=functions,
        function_call="auto",  # auto is default, but we'll be explicit
    )
    
    response_message = response["choices"][0]["message"]
    # debugging
    # return response_message

    if(response_message.get("function_call")):
        available_functions = {
            "get_weather": get_weather,
            "record_my_body_weight": record_my_body_weight,
            "generate_image": generate_image,
            "get_shopping_items": get_shopping_items,
            "record_random_thoughts": record_random_thoughts,
        } 

        function_name = response_message["function_call"]["name"]
        fuction_to_call = available_functions[function_name]
        function_args = json.loads(response_message["function_call"]["arguments"])
        
        if (function_name == "record_my_body_weight"):
            weight_info = function_args.get('weight_in_kg')
            record_my_body_weight(weight_info)

        elif(function_name == "get_weather") :
            location = function_args.get('location')
            print(f"Run weather function in city {location}")

            weather_data = get_weather(location)
            if weather_data is not None:
                # print(weather_data)
                # call summarize
                messages.append(response_message)  # extend conversation with assistant's reply
                messages.append(
                    {
                        "role": "function",
                        "name": function_name,
                        "content": weather_data,
                    }
                )  # extend conversation with function response

                messages.append(
                    {
                        "role": "system",
                        "content": """You are a weather query assistant. You return user colloquial style weather information. 
                                    The weather info should contain city name; temperature in celcius(replace "\u00b0" with 'degrees'); weather_description, such as sunny or cloudy; and humidity, 
                                    And in the end, based on the weather, recommend the user what kind of activities they can do.
                                """,
                    }
                )

                second_response = openai.ChatCompletion.create(
                    model="gpt-3.5-turbo-0613",
                    messages=messages,
                )

                weather_summary_message = second_response["choices"][0]["message"]["content"]
                pi_azure_tts_api.azure_tts_run(weather_summary_message)
            
        elif(function_name == "generate_image"):
            prompt_text = function_args.get('prompt_text')
            image_name = generate_image(prompt_text)
            display_image_on_screen(image_name)
        

        elif(function_name == "get_shopping_items"):
            shopping_items = function_args.get('shopping_items')
            get_shopping_items(shopping_items)

        elif(function_name == "record_random_thoughts"):
            notes = function_args.get('memos')
            record_random_thoughts(notes)

        return function_args
    else:
        print("No matching function call found!")    
        return None 

    

# Example usage of the function
if __name__ == '__main__':
    openai_function_call("my memos, maybe I can integrate telegram bot with notion")

    # WeatherStack usage:
    # weather_data = get_weather('Munich')
    # if weather_data is not None:
        # print(weather_data)
        # print("Current Temperature:", weather_data['current']['temperature'], "°C")

