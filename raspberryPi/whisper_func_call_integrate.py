# Demonstration:  Speak to Pi, ask Pi to record my body weight into Notion Page.

# Test Whisper API -> Transcrition -> OpenAI ChatCompletion API 
#      -> Function call -> Write body weight information to Notion database 

import pi_whisper_api
import pi_openai_function_call



transcription = pi_whisper_api.get_transcription_from_whisper()
pi_openai_function_call.openai_function_call(transcription)
