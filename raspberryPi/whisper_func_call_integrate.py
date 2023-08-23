# Demonstration:  Speak to Pi, ask Pi to record my body weight into Notion Page.

# Test Whisper API -> Transcrition -> OpenAI ChatCompletion API 
#      -> Function call -> Write body weight information to Notion database 

import pi_whisper_api
import pi_openai_function_call
import pi_azure_tts_api
import difflib


# transcription = pi_whisper_api.get_transcription_from_whisper()
# response = pi_openai_function_call.openai_function_call(transcription)
# pi_azure_tts_api.azure_tts_run(response)



def similarity(s1, s2):
    """Return the similarity score of two strings. 1.0 means identical."""
    return difflib.SequenceMatcher(None, s1, s2).ratio()


def main_loop():
    previous_transcription = ""
    while True:
        # Get the audio transcription from Whisper
        transcription = pi_whisper_api.get_transcription_from_whisper()
        
        # Check if the user wants to exit
        if transcription.strip().lower() in ["exit", "stop"]:
            print("Exiting the program!")
            break

        # Only proceed if the new transcription is sufficiently different from the previous one
        if similarity(transcription, previous_transcription) < 0.9:  # You can adjust this threshold as needed
            # Pass the transcription to the OpenAI function for processing
            response = pi_openai_function_call.openai_function_call(transcription)
            
            # (Optional) Print the response or take any other action
            print(response)
            
            # Update the previous transcription
            previous_transcription = transcription
        else:
            print("Waiting for a new audio input...")



# Run the main loop
if __name__ == '__main__':
    main_loop()
