import sys
from socket import *

ServerSocket = socket(AF_INET, SOCK_DGRAM)
ServerSocket.bind(('', 10080))
clist = []
while True:
    msg, Add = ServerSocket.recvfrom(65565)
    op = msg.decode()[0]  # 0:registration 1:list data 2:chat data 3:keep_alive data 4:unregistration
    if op == "0":
        clist.append(msg.decode()[1:] +" " + Add[0] + " " + str(Add[1]))
        print("{}\t{}:{}\n".format(msg.decode()[1:], Add[0], Add[1]))
        sys.stdout.flush()
        send = "1" + ",".join(clist)
        for data in clist:
            ip = data.split(" ")[1]
            port = int(data.split(" ")[2])
            ServerSocket.sendto(send.encode(),(ip, port))
    elif op == "3":
        pass
    elif op == "4":
        for i in range(0, len(clist)):
            tmp = clist[i].split(" ")
            if tmp[1] == Add[0] and tmp[2] == str(Add[1]):
                del clist[i]
                print("{} is unregistered\t{}:{}\n".format(tmp[0], tmp[1], tmp[2]))
                sys.stdout.flush()
                send = "1" + ",".join(clist)
                for data in clist:
                    ip = data.split(" ")[1]
                    port = int(data.split(" ")[2])
                    ServerSocket.sendto(send.encode(),(ip, port))
                break
ServerSocket.close()
