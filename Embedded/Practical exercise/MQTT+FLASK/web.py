from flask import Flask, render_template

app = Flask(__name__)

@app.route('/')
def index():
    with open("sw1.txt", "r") as sw1, open("sw2.txt", "r") as sw2:
        cnt1 = sw1.read()
        cnt2 = sw2.read()
        print(cnt1,cnt2)
        return render_template('index.html', sw1 = cnt1, sw2 = cnt2) 

@app.route('/reset')
def reset():
    with open("/dev/rpigpio", "w") as fd:
        fd.write("r\0")
        return render_template('reset.html')

if __name__ == '__main__':
    app.run(host="192.168.1.10")
