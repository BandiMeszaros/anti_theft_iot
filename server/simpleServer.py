"""This is a simple http server written in python, this handles the requests from the smp432 board """

from flask import Flask,render_template
import datetime

app = Flask(__name__)

userDisarm = False
alarmTriggered = False

@app.route('/')
def welcome():
    return render_template('welcome.html')


@app.route('/ALARM')
def user_alarm():
    """When the user selects the alarm button"""
    global userDisarm
    userDisarm = False

    global alarmTriggered
    alarmTriggered = True

    return render_template("alarm.html")


@app.route('/OK')
def user_disarmed():
    """When the disarm button was pushed on the warnuser.html"""
    global userDisarm
    userDisarm = True

    return render_template("alarmunlocked.html")

@app.route('/userwarning')
def disarm_page():
    """this is what the user sees after a warning"""
    return render_template("warnuser.html")


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

    global alarmTriggered
    alarmTriggered = False

    while(seconds_passed<=60):
        #todo change time back
        alarm_time = datetime.datetime.now()
        seconds_passed = (alarm_time - trigger_time).seconds
        #if the user disarmed the alarm it breaks the loop
        if userDisarm or alarmTriggered:
            alarmTriggered = False
            break

    if userDisarm:
        print(userDisarm)
        return r'DISARM'
    else:
        print(userDisarm)
        return r'ALARM'





if __name__ == '__main__':
    print("Server is up")
    app.run(debug=True, host='0.0.0.0')
