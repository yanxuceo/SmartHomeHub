import json
import requests
import os

from datetime import datetime

google_map_api_key = os.getenv("GOOGLEMAP_API_KEY")

def get_directions(origin, destination, prefer_mode):
    # Define the endpoint and parameters
    url = "https://maps.googleapis.com/maps/api/directions/json"
    params = {
        "origin": origin,
        "destination": destination,
        "mode": "transit",
        "transit_mode": prefer_mode,
        "transit_routing_preference": "less_walking",
        "key": google_map_api_key  # Replace with your API key
    }

    # Make the HTTP GET request
    response = requests.get(url, params=params)

    # Check if the request was successful
    if response.status_code == 200:
        json_data = response.json()
        # Convert to JSON-formatted string
        json_data_str = json.dumps(response.json())

        return json_data.get('status'), json_data_str
    else:
        return f"Request failed with status code {response.status_code}", None



# Check if the request was successful
def parse_directions_data(data):
    # Remove lines containing "points" :
    data_cleaned = '\n'.join([line for line in data.split('\n') if '"points" :' not in line])

    try:
        # Then parse the cleaned JSON data
        parsed_data = json.loads(data_cleaned)
        # Access the routes
        routes = parsed_data["routes"]
        # Access the first route
        first_route = routes[0]
        # Access the legs of the first route
        legs = first_route["legs"]
        # Access the first leg of the route
        first_leg = legs[0]
        # Access the steps of the first leg
        steps = first_leg["steps"]
        
        # Loop through each step
        for step in steps:
            # Check if the step has the "transit_details" key
            if "transit_details" in step:
                transit_details = step["transit_details"]
                line_info = step["transit_details"]["line"]

                print("departure_stop:", transit_details["departure_stop"]["name"])
                print("arrival_stop:", transit_details["arrival_stop"]["name"])

                print("departure_time:", transit_details["departure_time"]["text"])
                print("arrival_time:", transit_details["arrival_time"]["text"])

                print("headsign:", transit_details["headsign"])
                print("line:", line_info["short_name"])

    except json.JSONDecodeError as e:
        print(f"Error decoding JSON: {e}")





if __name__ == "__main__":
    # Example usage:
    origin = "Sendlinger Tor,Munich"
    destination = "Hauptbahnhof,Munich"
    prefer_mode = "subway"  #bus, subway, train, tram, rail

    status, directions = get_directions(origin, destination, prefer_mode)

    if status == "OK":
        parse_directions_data(directions)
    else:
        print("Error:", status)
   

