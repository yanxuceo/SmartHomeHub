import os

import openai
import json
import requests

from datetime import datetime, timezone
import pi_notion_api

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
        return response.json()
    else:
        # Print an error message if the request was unsuccessful
        print("Failed to get the weather data. HTTP Response Code:", response.status_code)
        return None


def summarize_weather_info(query):
    print("Summarizing weather info")

    response = openai.ChatCompletion.create(
        model="gpt-3.5-turbo-0613",
        messages=[
            {
                "role": "system",
                "content": """You are a weather query assistant, you should return user colloquial style weather information. 
                            And the weather result will be sent to TTS service, so replace '%' with 'percentage'.
                            """,
            },
            {
                "role": "user",
                "content": f"""Write a weather summary based on the JSON response from a weather query API.
                        The summary should contain city name; temperature; weather_description, such as sunny or cloudy; and humidity, 
                        And in the end, based on the weather, recommend the user what kind of activities they can do.
                        User query: {query}
                        """,
            }
        ],
        temperature=0,
    )
    return response



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
    pi_notion_api.create_notion_page(data)



def openai_function_call(query):
    # send the conversation and available functions to GPT
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
        }
    ]

    response = openai.ChatCompletion.create(
        model="gpt-3.5-turbo-0613",
        messages=[
            {"role": "user", "content": query}
        ],
        functions=functions,
        function_call="auto",  # auto is default, but we'll be explicit
    )
    
    response_message = response["choices"][0]["message"]
    # debugging
    # return response_message

    if(response_message.get("function_call")):
        function_name = response_message["function_call"]["name"]
        arguments = json.loads(response_message["function_call"]["arguments"])
        
        if (function_name == "record_my_body_weight"):
            weight_info = arguments.get('weight_in_kg')
            record_my_body_weight(weight_info)

        elif(function_name == "get_weather") :
            location = arguments.get('location')
            print(f"Run weather function in city {location}")
    
            weather_data = get_weather(location)
            if weather_data is not None:
                # print(weather_data)
                # call summarize
                weather_summary_response = summarize_weather_info(weather_data)
                weather_summary_message = weather_summary_response["choices"][0]["message"]["content"]
                return weather_summary_message
    else:
        print("No matching function call found!")    
        # return None 

    



# Example usage of the function
if __name__ == '__main__':
    openai_function_call("what's the weather like in Munich?")

    # WeatherStack usage:
    # weather_data = get_weather('Munich')
    # if weather_data is not None:
        # print(weather_data)
        # print("Current Temperature:", weather_data['current']['temperature'], "°C")

