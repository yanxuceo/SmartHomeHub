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

                departure_stop = transit_details["departure_stop"]["name"]
                arrival_stop = transit_details["arrival_stop"]["name"]

                departure_time = transit_details["departure_time"]["text"]
                arrival_time = transit_details["arrival_time"]["text"]

                headsign = transit_details["headsign"]
                line = line_info["short_name"]

                # Concatenate values into a list
                result_list = [departure_stop, arrival_stop, departure_time, arrival_time, headsign, line]
                return result_list
        
        return None

    except json.JSONDecodeError as e:
        print(f"Error decoding JSON: {e}")



def print_transit_info(nav_info):
    print(f"Departure Stop: {nav_info[0]}")
    print(f"Arrival Stop: {nav_info[1]}")
    print(f"Departure Time: {nav_info[2]}")
    print(f"Arrival Time: {nav_info[3]}")
    print(f"Headsign: {nav_info[4]}")
    print(f"Line: {nav_info[5]}")


def format_transit_info(nav_info):
    transit_string = (
        f"Departure Stop: {nav_info[0]}\n"
        f"Arrival Stop: {nav_info[1]}\n"
        f"Departure Time: {nav_info[2]}\n"
        f"Arrival Time: {nav_info[3]}\n"
        f"Headsign: {nav_info[4]}\n"
        f"Line: {nav_info[5]}"
    )
    return transit_string



if __name__ == "__main__":
    # Example usage:
    origin = "Sendlinger Tor,Munich"
    destination = "Hauptbahnhof,Munich"
    prefer_mode = "subway"  #bus, subway, train, tram, rail

    status, directions = get_directions(origin, destination, prefer_mode)

    if status == "OK":
        navi_info = parse_directions_data(directions)
        if navi_info == None:
            print("Sorry, we don't get valid data, please try again later")
        else:
            print_transit_info(navi_info)
    else:
        print("Error:", status)
   

