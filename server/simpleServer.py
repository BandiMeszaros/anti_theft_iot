"""This is a simple http server written in python, this handles the requests from the smp432 board """

from flask import Flask

app = Flask(__name__)

@app.route('/')
def index():
    return 'Hello world'

if __name__ == '__main__':
    print("Server is up")
    app.run(debug=True, host='0.0.0.0')
