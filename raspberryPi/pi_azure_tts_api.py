import os

import azure.cognitiveservices.speech as speechsdk
import xml.etree.ElementTree as ET
import argparse

import pyaudio
import wave

all_voice = [
    'en-US-AnaNeural',
    'en-AU-CarlyNeural',
    'en-GB-MaisieNeural',
    'here starts FEMALE',
    'en-US-JaneNeural',
    'en-US-JennyNeural', 
    'en-US-MichelleNeural', 
    'en-US-NancyNeural',   
    'en-US-SaraNeural', 
    'en-AU-FreyaNeural', 
    'en-AU-KimNeural', 
    'en-AU-NatashaNeural', 
    'en-GB-LibbyNeural',
    'en-GB-SoniaNeural',
    'en-HK-YanNeural',
    'here starts MALE',
    'en-US-AIGenerate1Neural',
    'en-US-BrandonNeural',
    'en-US-ChristopherNeural',
    'en-US-DavisNeural',
    'en-US-EricNeural',
    'en-US-GuyNeural',
    'en-US-JacobNeural',
    'en-US-RogerNeural',
    'en-CA-LiamNeural',
    'en-US-JasonNeural',
    'en-US-SteffanNeural', 
    'en-US-TonyNeural',
    'en-AU-NeilNeural', 
    'en-AU-WilliamNeural',
    'en-GB-OliverNeural', 
    'en-GB-RyanNeural',
    'here starts CHINESE',
    'zh-CN-sichuan-YunxiNeural',
    'zh-CN-shandong-YunxiangNeural',
    'zh-CN-shaanxi-XiaoniNeural',
    'zh-CN-liaoning-XiaobeiNeural',
    'zh-CN-henan-YundengNeural',
    'zh-CN-XiaochenNeural',
    'zh-CN-XiaohanNeural',
    'zh-CN-XiaomengNeural',
    'zh-CN-XiaomoNeural',
    'zh-CN-XiaoqiuNeural',
    'zh-CN-XiaoruiNeural',
    'zh-CN-XiaoshuangNeural',
    'zh-CN-XiaoxiaoNeural',
    'zh-CN-XiaoxuanNeural',
    'zh-CN-XiaoyanNeural',
    'zh-CN-XiaoyiNeural',
    'zh-CN-XiaoyouNeural',
    'zh-CN-XiaozhenNeural',
    'zh-CN-YunfengNeural',
    'zh-CN-YunhaoNeural',
    'zh-CN-YunjianNeural',
    'zh-CN-YunxiaNeural',
    'zh-CN-YunxiNeural',
    'zh-CN-YunyangNeural',
    'zh-CN-YunyeNeural',
    'zh-CN-YunzeNeural'
]

which_speed_to_use = ['0.5', '0.7', '0.8', '0.9', '1.0', '1.2', '1.4', '1.6', '1.8', '2.0']

emotion_list = ['terrified', 'unfriendly', 'sad', 'serious', 'shouting', 'affectionate', 'angry',             
                'embarrassed', 'empathetic', 'envious', 'excited', 'fearful', 'friendly', 
                'gentle', 'hopeful', 'whispering', 'lyrical', 'calm', 'chat', 'cheerful', 
                'depressed', 'disgruntled', 'poetry-reading', 'customerservice', 'assistant', 
                'advertisement_upbeat', 'documentary-narration', 'narration-professional', 
                'narration-relaxed', 'newscast', 'newscast-casual', 'newscast-formal', 
                'sports_commentary', 'sports_commentary_excited'
            ]

emotion_degree = ['0.1', '0.2', '0.3', '0.4', '0.5', '0.6', '0.7', '0.8', '0.9', '1.0', '1.1', 
                  '1.2', '1.3', '1.4', '1.5', '1.6', '1.7', '1.8', '1.9', '2.0'
            ]

# helper function for ssml updates
def remove_namespace_prefixes(element, keep_mstts=False):
    for child in element.iter():
        if "}" in child.tag:
            if keep_mstts and 'mstts' in child.tag:
                child.tag = 'mstts:' + child.tag.split("}")[-1]
            else:
                child.tag = child.tag.split("}")[-1]
    return element


def modify_ssml(ssml, target_language, voice_name, style, style_degree, prosody_rate, role, text):
    # Parse the XML string
    root = ET.fromstring(ssml)

    # Find the 'express-as' element
    mstts_namespace = "https://www.w3.org/2001/mstts"
    express_as = root.find(f'.//{{{mstts_namespace}}}express-as')

    # Find the 'prosody' element
    synthesis_namespace = "http://www.w3.org/2001/10/synthesis"
    prosody = express_as.find(f'.//{{{synthesis_namespace}}}prosody')

    # Update the text
    prosody.text = text

    # Remove namespace prefixes, but keep mstts
    root = remove_namespace_prefixes(root, keep_mstts=True)

    # Modify xml:lang attribute
    root.set("{http://www.w3.org/XML/1998/namespace}lang", target_language)

    # Find the 'voice' element and modify the name attribute
    voice = root.find('voice')
    voice.set("name", voice_name)

    # Modify style and styledegree attributes
    express_as.set("style", style)
    express_as.set("styledegree", style_degree)
    #express_as.set("role", role)

    # Modify prosody rate attribute
    prosody.set("rate", prosody_rate)

    # Add the namespace attributes back to the root element
    root.set("xmlns", "http://www.w3.org/2001/10/synthesis")
    root.set("xmlns:mstts", "https://www.w3.org/2001/mstts")

    # Convert the updated XML tree back to a string
    updated_ssml = ET.tostring(root, encoding='unicode')
    return updated_ssml


# temporarily remove role in this version
# <mstts:express-as role="YoungAdultFemale" style="lalala" styledegree="0.5">
ssml = """
<speak version="1.0" xml:lang="en-US" xmlns="http://www.w3.org/2001/10/synthesis" 
xmlns:mstts="https://www.w3.org/2001/mstts">
    <voice name="ABC">
        <mstts:express-as style="lalala" styledegree="0.5">
            <prosody rate="0.1">
                Enjoy using text-to-speech.
            </prosody>
        </mstts:express-as>
    </voice>
</speak>
"""


# azure tts api config
speech_config = speechsdk.SpeechConfig(subscription=os.environ.get('SPEECH_KEY'), region=os.environ.get('SPEECH_REGION'))
speech_synthesizer = speechsdk.SpeechSynthesizer(speech_config=speech_config, audio_config=None)


def generate_audio(updated_ssml, file_name):
    result = speech_synthesizer.speak_ssml_async(updated_ssml).get()
    stream = speechsdk.AudioDataStream(result)
    stream.save_to_wav_file(file_name)   


def play_audio(filename):
    """
    Play an audio file with a given filename using PyAudio and wave libraries.
    
    Parameters:
    filename (str): The file name of the audio file to be played.
    """
    # Open the file
    wf = wave.open(filename, 'rb')
    
    # Create an audio object
    p = pyaudio.PyAudio()
    
    # Open stream
    stream = p.open(format=p.get_format_from_width(wf.getsampwidth()),
                    channels=wf.getnchannels(),
                    rate=wf.getframerate(),
                    output=True)
    
    # Read and play
    data = wf.readframes(1024)
    while data:
        stream.write(data)
        data = wf.readframes(1024)
    
    # Stop stream
    stream.stop_stream()
    stream.close()
    
    # Close PyAudio
    p.terminate()



def azure_tts_run(text: str):
    voice_name = "en-US-SaraNeural"
    radio_speed = "1.0"
    emotion = "friendly"
    emotion_degree = "1.2"
    output_file = "output.wav"

    # adjust voice properties
    updated_ssml = modify_ssml(ssml, voice_name, voice_name, emotion, emotion_degree, radio_speed, "", text)                
    generate_audio(updated_ssml, output_file)    

    # play the generated audio
    play_audio(output_file)

        



# Example usage of the function
if __name__ == '__main__':
    # Command line arguments parsing
    parser = argparse.ArgumentParser(description='Azure TTS Testing')
    parser.add_argument('--text', required=True, help='Text to be converted to speech')
    parser.add_argument('--voice_name', default='en-US-SaraNeural', help='Voice name for TTS')
    parser.add_argument('--radio_speed', default='1.0', help='Radio speed for TTS')
    parser.add_argument('--emotion', default='friendly', help='Emotion for TTS')
    parser.add_argument('--emotion_degree', default='1.0', help='Degree of emotion for TTS')
    parser.add_argument('--output_file', default='output.wav', help='Output file name for the generated audio')

    args = parser.parse_args()

    # Extracting command line arguments
    text = args.text
    voice_name = args.voice_name
    radio_speed = args.radio_speed
    emotion = args.emotion
    emotion_degree = args.emotion_degree
    output_file = args.output_file

    # adjust voice properties
    updated_ssml = modify_ssml(ssml, voice_name, voice_name, emotion, emotion_degree, radio_speed, "", text)                
    generate_audio(updated_ssml, "output.wav")    

    # play the generated audio
    play_audio('output.wav')
