import os
import numpy as np
import pyaudio
import openai
import wave
from pydub import AudioSegment


# Set your OpenAI API key
openai.api_key = os.getenv("OPENAI_API_KEY")

# Set the audio parameters
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 44100
CHUNK = 1024
SILENCE_THRESHOLD = 500  # Silence threshold
SPEECH_END_TIME = 1.0  # Time of silence to mark the end of speech

# Initialize PyAudio
audio = pyaudio.PyAudio()

# Start Recording
stream = audio.open(format=FORMAT, channels=CHANNELS,
                    rate=RATE, input=True,
                    frames_per_buffer=CHUNK)

print("Recording...Waiting for speech to begin.")

frames = []
silence_frames = 0
is_speaking = False
total_frames = 0

while True:
    data = stream.read(CHUNK)
    frames.append(data)
    total_frames += 1
    
    # Convert audio chunks to integers
    audio_data = np.frombuffer(data, dtype=np.int16)
    
    # Check if user has started speaking
    if np.abs(audio_data).mean() > SILENCE_THRESHOLD:
        is_speaking = True

    # Detect if the audio chunk is silence
    if is_speaking:
        if np.abs(audio_data).mean() < SILENCE_THRESHOLD:
            silence_frames += 1
        else:
            silence_frames = 0

    # End of speech detected
    if is_speaking and silence_frames > SPEECH_END_TIME * (RATE / CHUNK):
        print("End of speech detected.")
        break


# Stop Recording
stream.stop_stream()
stream.close()
audio.terminate()

print("Finished recording.")

# Save the recorded data as a byte array
audio_data = b''.join(frames)

# Save the recorded data as a WAV file
with wave.open("temp_audio_file.wav", "wb") as wf:
    wf.setnchannels(CHANNELS)
    wf.setsampwidth(audio.get_sample_size(FORMAT))
    wf.setframerate(RATE)
    wf.writeframes(b''.join(frames))


# Convert the WAV file to MP3
sound = AudioSegment.from_wav("temp_audio_file.wav")
sound.export("temp_audio_file.mp3", format="mp3")


# Open the saved file to send to the API
with open("temp_audio_file.wav", "rb") as f:
    transcript = openai.Audio.transcribe("whisper-1", f)


# Print the transcript
print("Transcript:", transcript['text'])