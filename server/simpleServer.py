"""This is a simple http server written in python, this handles the requests from the smp432 board """

from flask import Flask,render_template
import datetime

app = Flask(__name__)

userDisarm = False

@app.route('/')
def welcome():
    return render_template('welcome.html')


@app.route('/ALARM')
def user_alarm():
    """When the user selects the alarm button"""
    global userDisarm
    userDisarm = False

    return render_template("alarm.html")


@app.route('/OK')
def user_disarmed():
    """When the disarm button was pushed on the disarm.html"""
    global userDisarm
    userDisarm = True

    return render_template("alarmunlocked.html")

@app.route('/disarm')
def disarm_page():
    """this is what the user sees after a warning"""
    return render_template("disarm.html")


@app.route('/warning', methods=['GET'])
def receive_warning_signal():
    """the board triggers this endpoint whenever there is a warning"""

    #TODO: send out the telegrambot warning

    #if there is a warning the server wait two minutes to be disalarmed by the user
    trigger_time = datetime.datetime.now()
    alarm_time = datetime.datetime.now()
    seconds_passed = (alarm_time-trigger_time).seconds

    global userDisarm
    userDisarm = False

    while(seconds_passed<=3):
        #todo change time back
        alarm_time = datetime.datetime.now()
        seconds_passed = (alarm_time - trigger_time).seconds
        #if the user disarmed the alarm it breaks the loop
        if (userDisarm):
            break

    if (1):
        #todo change it back to userDisarm
        return "OK"
    else:
        return "ALARM"





if __name__ == '__main__':
    print("Server is up")
    app.run(debug=True, host='94.44.112.120')
