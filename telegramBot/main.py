import constants as keys
from telegram.ext import *
import responses as R
import requests

URL = "http://192.168.43.34:8080/BBS"
print("Bot started...")

def enable_command(update, context):
	update.message.reply_text('You just pressed enable')
	url = URL + "/Activate"
	requests.get(url, {})


def disable_command(update, context):
	update.message.reply_text('You just pressed disable')
	url = URL + "/Deactivate"
	requests.get(url, {})


def handle_message(update, context):
	text = str(update.message.text).lower()
	response = R.sample_responses(text)

	update.message.reply_text(response)

def error(update,context):
	print(f"Update {update} caused error {context.error}")

def main():
	updater = Updater(keys.API_KEY, use_context=True)
	dp = updater.dispatcher

	dp.add_handler(CommandHandler("enable", enable_command))
	dp.add_handler(CommandHandler("disable", disable_command))

	dp.add_handler(MessageHandler(Filters.text, handle_message))

	dp.add_error_handler(error)


	if True:
		updater.bot.send_message(chat_id=262196380, text="Fiiiiuu")

	isMoving = 'F'
	while(isMoving == 'F'):
		response = requests.get(URL+"/State")
		isMoving = response.headers["status"]

	updater.start_polling()
	updater.idle()



main()