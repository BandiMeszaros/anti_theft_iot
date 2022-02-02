import constants as keys
from telegram.ext import *
import responses as R
import requests
import threading
import time

class bigBrotherbot:
	def __init__(self):

		self.chatID = 1850142336
		self.serverURL = "http://192.168.43.34:8080/BBS"
		self.updater = Updater(keys.API_KEY, use_context=True)

		self.dp = self.updater.dispatcher
		self.dp.add_handler(CommandHandler("start", self.start_command))
		self.dp.add_handler(CommandHandler("help", self.help_command))
		self.dp.add_handler(CommandHandler("enable", self.enable_command))
		self.dp.add_handler(CommandHandler("disable", self.disable_command))
		self.dp.add_handler(MessageHandler(Filters.text, self.handle_message))
		self.dp.add_error_handler(self.error)

		self.statusThread = threading.Thread(target=self.query_status, args=(), daemon=True)


	def query_status(self):

		message_sent = False
		while(True):
			time.sleep(1)
			#print("status Thread: query server...")
			response = requests.get(self.serverURL+"/State")
			isMoving = response.headers["movement"]
			isActive = response.headers["status"]

			if isActive == 'F':
				message_sent = False

			if isMoving == 'T' and message_sent == False:
				#sends out a notification message
				self.updater.bot.send_message(chat_id=self.chatID, text="Movement detected")
				self.updater.bot.send_message(chat_id=self.chatID, text="Please disarm with the /disable command if it was you")
				message_sent = True




	def enable_command(self, update, context):
		update.message.reply_text('You just pressed enable')
		url = self.serverURL + "/Activate"
		requests.get(url, {})


	def help_command(self, update, context):
		response = "Press /enable to activate\nPress /disable to deactivate"
		update.message.reply_text(response)


	def start_command(self, update, context):
		response = "Welcome to Big Brother Security\nType /help to see what we can do!"
		update.message.reply_text(response)


	def disable_command(self, update, context):
		update.message.reply_text('You just pressed disable')
		url = self.serverURL + "/Deactivate"
		requests.get(url, {})


	def handle_message(self, update):
		text = str(update.message.text).lower()
		response = R.sample_responses(text)

		update.message.reply_text(response)

	def error(self, update,context):
		print(f"Update {update} caused error {context.error}")

	def main(self):

		self.statusThread.start()
		print('Bot started...')
		self.updater.start_polling()
		self.updater.idle()






if __name__ == '__main__':
	telegramBot = bigBrotherbot()
	telegramBot.main()
