## Raspberry Pi based Smart Home Hub
I plan to build a pi based smart home hub which can help me better organize my life. Some key features can be:
- Audio speech recognition(ASR)
- Text to speech(TTS)
- A user-friendly Dashboard
- I/O extension, reading sensor data, control actuators, etc.
- Database for data synchronization

### Software setup
Step by step, design, implement, and test each module. 

#### Test Whisper API for speech recognition 
Dependencies installation for 
```
sudo apt-get update 
sudo apt-get upgrade 
sudo apt-get install portaudio19-dev 
sudo pip install pyaudio

sudo pip install pydub
sudo apt-get install lame

sudo pip install numpy openai
```
