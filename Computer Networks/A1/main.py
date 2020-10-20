import threading
import time
import sys
import math

def func(sname, dname):
    src = open(sname, 'rb')
    des = open(dname, 'wb')
    while 1:
        tmp = src.read(10240)
        if not tmp:
            break
        des.write(tmp)
    rtime = str(math.floor((time.time() - stime)*100)/100)
    log.write(rtime + '\t' + dname + ' is copied completely\n')
    src.close()
    des.close()

stime = time.time()
log = open('log.txt', 'w')
while 1:    
    sname = input('Input the file name: ')
    if sname == 'exit':
        log.close()
        sys.exit()
    dname = input('Input the new name: ')
    thd = threading.Thread(target = func, args = (sname, dname))
    rtime = str(math.floor((time.time() - stime)*100)/100)
    log.write(rtime + '\t' + 'Start copying ' + sname + ' to ' + dname + '\n')
    thd.start()
    print('\n')
    

    


        
    



