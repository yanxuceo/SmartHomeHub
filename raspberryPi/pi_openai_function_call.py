import os

import openai
import json


def record_my_body_weight(weight: str):
    print(f"write my weight {weight} into notion page.")


def openai_function_call(query):
    # Step 1: send the conversation and available functions to GPT
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
            }
    ]

    # Set your OpenAI API key
    openai.api_key = os.getenv("OPENAI_API_KEY")
    response = openai.ChatCompletion.create(
        model="gpt-3.5-turbo-0613",
        messages=[{"role": "user", "content": query}],
        functions=functions,
        function_call="auto",  # auto is default, but we'll be explicit
    )
    response_message = response["choices"][0]["message"]

    if(response_message.get("function_call")):
        function_name = response_message["function_call"]["name"]
        arguments = json.loads(response_message["function_call"]["arguments"])
        weight_info = arguments.get('weight_in_kg')
        if (function_name == "record_my_body_weight"):
            record_my_body_weight(weight_info)
    else:
        print("No matching function call found!")     
    #return response_message



# Example usage of the function
if __name__ == '__main__':
    print(openai_function_call("record my body weight 70.5"))
