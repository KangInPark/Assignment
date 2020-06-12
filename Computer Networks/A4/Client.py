import sys
import threading
import time
from socket import *


def receiver():
    global clist
    while True:
        msg, add = ClientSocket.recvfrom(65565)
        # 0:registration 1:list data 2:chat data 3:keep_alive data 4:unregistration
        op = msg.decode()[0]
        if op == "1":
            clist = msg.decode()[1:].split(',')
        elif op == "2":
            print("From {}\t[{}]".format(
                msg.decode()[1:33].strip(), msg.decode()[33:]))
            sys.stdout.flush()


def keep_alive():
    send = "3" + "keep_alive"
    while True:
        time.sleep(10)
        ClientSocket.sendto(send.encode(), (Server_IP, 10080))


if len(sys.argv) < 3:
    print("Error: 2개의 인자를 명령행을 통해 입력해야 합니다.(Client ID, Server IP)")
    sys.exit()

clist = []
Client_ID = sys.argv[1]
Server_IP = sys.argv[2]
ClientSocket = socket(AF_INET, SOCK_DGRAM)
ClientSocket.bind(('', 10081))
send = "0" + Client_ID
ClientSocket.sendto(send.encode(), (Server_IP, 10080))
th1 = threading.Thread(target=receiver)
th1.daemon = True
th2 = threading.Thread(target=keep_alive)
th2.daemon = True
th1.start()
th2.start()
while True:
    line = input()
    mode = line.split(" ")[0]
    if mode == "@show_list":
        buff = ""
        for data in clist:
            tmp = data.split(" ")
            buff = buff + "{}\t{}:{}".format(tmp[0], tmp[1], tmp[2])
        print(buff)
        sys.stdout.flush()
    elif mode == "@chat":
        msg = " ".join(line.split(" ")[2:])
        target = line.split(" ")[1]
        for data in clist:
            tmp = data.split(" ")
            if target == tmp[0]:
                send = "2" + Client_ID + (" " * (32 - len(Client_ID))) + msg
                ClientSocket.sendto(send.encode(), (tmp[1], int(tmp[2])))
                break
    elif mode == "@exit":
        send = "4" + "unregistration"
        ClientSocket.sendto(send.encode(), (Server_IP, 10080))
        break
    else:
        print("잘못된 입력입니다.\n")
        sys.stdout.flush()
ClientSocket.close()
