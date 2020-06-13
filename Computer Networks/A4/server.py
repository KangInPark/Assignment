import sys
import time
import threading
from socket import *

def alive_chk():
    global alive
    global clist
    while True:
        tmp = list(alive.items())
        for key, val in tmp:
            if val + 30 < time.time():
                print("{} is off-line\t{}:{}".format(key.split(" ")[0], key.split(" ")[1], key.split(" ")[2]))
                sys.stdout.flush()
                del alive[key]
                clist.remove(key)
                send = "1" + ",".join(clist)
                for data in clist:
                    ip = data.split(" ")[1]
                    port = int(data.split(" ")[2])
                    ServerSocket.sendto(send.encode(),(ip, port))
                break
        time.sleep(0.1)


ServerSocket = socket(AF_INET, SOCK_DGRAM)
ServerSocket.bind(('', 10080))
clist = []
alive = {}
th = threading.Thread(target=alive_chk)
th.daemon = True
th.start()
while True:
    msg, Add = ServerSocket.recvfrom(65565)
    op = msg.decode()[0]  # 0:registration 1:list data 2:chat data 3:keep_alive data 4:unregistration
    if op == "0":
        clist.append(msg.decode()[1:33].strip() +" " + Add[0] + " " + str(Add[1]) + " " + msg.decode()[33:])
        alive[clist[-1]] = time.time()
        print("{}\t{}:{}".format(msg.decode()[1:33].strip(), Add[0], Add[1]))
        sys.stdout.flush()
        send = "1" + ",".join(clist)
        for data in clist:
            ip = data.split(" ")[1]
            port = int(data.split(" ")[2])
            ServerSocket.sendto(send.encode(),(ip, port))
    elif op == "3":
        alive[msg.decode()[1:33].strip() +" " + Add[0] + " " + str(Add[1]) + " " + msg.decode()[33:]] = time.time()
    elif op == "4":
        for i in range(0, len(clist)):
            tmp = clist[i].split(" ")
            if tmp[1] == Add[0] and tmp[2] == str(Add[1]):
                del alive[clist[i]]
                del clist[i]
                print("{} is unregistered\t{}:{}".format(tmp[0], tmp[1], tmp[2]))
                sys.stdout.flush()
                send = "1" + ",".join(clist)
                for data in clist:
                    ip = data.split(" ")[1]
                    port = int(data.split(" ")[2])
                    ServerSocket.sendto(send.encode(),(ip, port))
                break
ServerSocket.close()
