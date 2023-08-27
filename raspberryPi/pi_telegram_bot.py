"""
Simple Bot to reply to Telegram messages.

First, a few handler functions are defined. Then, those functions are passed to
the Application and registered at their respective places.
Then, the bot is started and runs until we press Ctrl-C on the command line.

Usage:
Basic Echobot example, repeats messages.
Press Ctrl-C on the command line or send a signal to the process to stop the
bot.
"""
import os
import logging

from pydub import AudioSegment

from telegram import ForceReply, Update
from telegram.ext import Application, CommandHandler, ContextTypes, MessageHandler, filters

import pi_whisper_api
import pi_openai_function_call


def convert_oga_to_mp3(oga_file, mp3_file):
    audio = AudioSegment.from_ogg(oga_file)
    audio.export(mp3_file, format="mp3")


# Enable logging
logging.basicConfig(
    format="%(asctime)s - %(name)s - %(levelname)s - %(message)s", level=logging.INFO
)
# set higher logging level for httpx to avoid all GET and POST requests being logged
logging.getLogger("httpx").setLevel(logging.WARNING)
logger = logging.getLogger(__name__)


# Define a few command handlers. These usually take the two arguments update and
# context.
async def start(update: Update, context: ContextTypes.DEFAULT_TYPE) -> None:
    """Send a message when the command /start is issued."""
    user = update.effective_user
    await update.message.reply_html(
        rf"Hi {user.mention_html()}!",
        reply_markup=ForceReply(selective=True),
    )


async def help_command(update: Update, context: ContextTypes.DEFAULT_TYPE) -> None:
    """Send a message when the command /help is issued."""
    await update.message.reply_text("Help!")


async def echo(update: Update, context: ContextTypes.DEFAULT_TYPE) -> None:
    """Echo the user message."""
    await update.message.reply_text(update.message.text)


async def text_memo_handler(update: Update, context: ContextTypes.DEFAULT_TYPE) -> None:
    """Echo the user message."""
    print(update.message.text)
    response = pi_openai_function_call.openai_function_call(update.message.text)
    await update.message.reply_text(response)


async def voice_memo_handler(update: Update, context: ContextTypes.DEFAULT_TYPE) -> None:
    new_file = await context.bot.get_file(update.message.voice.file_id)
    
    downloaded_file = await new_file.download_to_drive()
    
    orin_file_name = downloaded_file.name
    new_filename = orin_file_name.replace(".oga", ".mp3")
    convert_oga_to_mp3(orin_file_name, new_filename)

    transcript = pi_whisper_api.get_transcription_from_audio_file(new_filename)
    response = pi_openai_function_call.openai_function_call(transcript)

    await update.message.reply_text(response)


async def photo_memo_handler(update: Update, context: ContextTypes.DEFAULT_TYPE) -> None:
    """Echo the user message."""
    await update.message.reply_text("this is photo memo")



def main() -> None:
    """Start the bot."""
    bot_api_key = os.getenv("TELEGRAM_BOT_API_KEY")
   
    # Create the Application and pass it your bot's token.
    application = Application.builder().token(bot_api_key).build()

    # on different commands - answer in Telegram
    application.add_handler(CommandHandler("start", start))
    application.add_handler(CommandHandler("help", help_command))

    # on non command i.e message - echo the message on Telegram
    application.add_handler(MessageHandler(filters.TEXT & ~filters.COMMAND, text_memo_handler))
    application.add_handler(MessageHandler(filters.VOICE & ~filters.COMMAND, voice_memo_handler))
    application.add_handler(MessageHandler(filters.PHOTO & ~filters.COMMAND, photo_memo_handler))


    # Run the bot until the user presses Ctrl-C
    application.run_polling(allowed_updates=Update.ALL_TYPES)


if __name__ == "__main__":
    main()
