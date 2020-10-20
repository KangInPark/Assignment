import threading
import time
from socket import *

def handle(Socket, addr):
    cnt = 0
    imglist = ['/w20.jpg', '/w21.jpg', '/w22.jpg', '/back.jpg', '/dummy.jpg']
    stime = time.time()
    while(1):
        request = Socket.recv(100000).decode()
        data = request.split()
        cnt+=1
        if len(data) > 0:
            if (data[0] == "GET" or "POST") and 'Cookie:' in data and (data[1] == "/index.html" or data[1] == "/" or data[1] == "/secret.html"):
                secret = open("secret.html", "br")
                body = secret.read()
                response = "{0} 200 OK\r\nServer:Kangin_server\r\nContent-Length: {1}\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout={2} max={3}\r\nContent-Type: text/html\r\n\r\n".format(data[2], len(body), int(stime - time.time()), 101-cnt)
                Socket.send(response.encode())
                Socket.send(body)
                secret.close()

            elif data[0] == "GET" and data[1] in imglist and 'Cookie:' in data:
                img = open(data[1][1:], "br")
                body = img.read()
                response = "{0} 200 OK\r\nServer:Kangin_server\r\nContent-Length: {1}\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout={2} max={3}\r\nContent-Type: image/jpeg\r\n\r\n".format(data[2], len(body), int(5-time.time()+ stime), 101-cnt)
                Socket.send(response.encode())
                Socket.send(body)
                img.close()
            
            elif data[0] == "GET" and (data[1] == "/index.html" or data[1] == "/"):
                index = open("index.html", "br")
                body = index.read()
                response = "{0} 200 OK\r\nServer:Kangin_server\r\nContent-Length: {1}\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout={2} max={3}\r\nContent-Type: text/html\r\n\r\n".format(data[2], len(body), int(5-time.time()+ stime), 101-cnt)
                Socket.send(response.encode())
                Socket.send(body)
                index.close()
                
            elif data[0] == "POST":
                secret = open("secret.html", "br")
                body = secret.read()
                idpart = data[-1].replace('&', '=').split('=')
                response = "{0} 200 OK\r\nServer:Kangin_server\r\nContent-Length: {1}\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout={2} max={3}\r\nContent-Type: text/html\r\nSet-Cookie: id={4}; max-age=30\r\n\r\n".format(data[2], len(body), int(5-time.time()+ stime), 101-cnt, idpart[1] + '&' + str(time.time()))
                Socket.send(response.encode())
                Socket.send(body)
                secret.close()

            elif data[0] == "GET" and 'Cookie:' in data and data[1] == "/cookie.html":
                cookie = open("cookie.html", "r")
                tmp = data.index("Cookie:")
                idpart = data[tmp+1].replace('&', '=').split('=')       
                body = cookie.read().replace("__id__", idpart[1]).replace("__sec__", str(int(30- time.time() + float(idpart[2])))).encode()
                response = "{0} 200 OK\r\nServer:Kangin_server\r\nContent-Length: {1}\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout={2} max={3}\r\nContent-Type: text/html\r\n\r\n".format(data[2], len(body), int(5-time.time()+ stime), 101-cnt)
                Socket.send(response.encode())
                Socket.send(body)
                cookie.close()

            elif 'Cookie:' not in data:
                forbidden = open("403.html", "br")
                body = forbidden.read()
                response = "{0} 403 Forbidden\r\nServer:Kangin_server\r\nContent-Length: {1}\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout={2} max={3}\r\nContent-Type: text/html\r\n\r\n".format(data[2], len(body), int(5-time.time()+ stime), 101-cnt)
                Socket.send(response.encode())
                Socket.send(body)
                forbidden.close()

            else:
                notfound = open("404.html", "br")
                body = notfound.read()
                response = "{0} 404 Not Found\r\nServer:Kangin_server\r\nContent-Length: {1}\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout={2} max={3}\r\nContent-Type: text/html\r\n\r\n".format(data[2], len(body), int(5-time.time()+ stime), 101-cnt)
                Socket.send(response.encode())
                Socket.send(body)
                notfound.close()
                
        if time.time() - stime > 5 or cnt>100:
            Socket.close()
            break
        
serverport = 10080
serverSocket = socket(AF_INET, SOCK_STREAM)
serverSocket.bind( ('', serverport))
serverSocket.listen(100)
print( 'The server is ready to recive.')
while 1:
    connectSocket, addr = serverSocket.accept()
    thread = threading.Thread(target = handle, args = (connectSocket, addr))
    thread.start()

