import sys
from socket import *
from time import *
import signal

ack = [] 
info = [] # [time]
startTime = 0
expired = 0
senderSocket = None
dev_rtt = -1
avg_rtt = -1
timeout = 1
log = None
cnt = 0
seq = 0
rmode = 0
dup = 0
dup_ack = -100

"Use this method to write Packet log"
def writePkt(logFile, procTime, pktNum, event):
    logFile.write('{:1.3f} pkt: {} | {}\n'.format(procTime, pktNum, event))

"Use this method to write ACK log"
def writeAck(logFile, ackNum, event):
    procTime = time() - startTime
    logFile.write('{:1.3f} ACK: {} | {}\n'.format(procTime, ackNum, event))

"Use this method to write final throughput log"
def writeEnd(logFile, throughput, avgRTT):
    logFile.write('File transfer is finished.\n')
    logFile.write('Throughput : {:.2f} pkts/sec\n'.format(throughput))
    logFile.write('Average RTT : {:.1f} ms'.format(avgRTT))

def retrans(pkt):
    global ack
    global info
    global rmode
    global cnt
    global expired
    global timeout
    global dev_rtt
    global avg_rtt
    global dup
    global dup_ack
    if(pkt == 0):
        tmp = dstFilename.encode()
    else:
        fd = open(srcFilename, 'rb')
        fd.seek(1350 * (pkt-1))
        tmp = fd.read(1350)
    num = str(pkt)
    rchk = 0
    rmode = 1
    while rchk==0:
        senderSocket.sendto(num.encode() + (" " * (32 - len(num))).encode() + tmp, (recvAddr, 10080))
        t = time()
        writePkt(log, t - startTime, pkt, "retransmitted")
        while time() < t + 1:
            try:
                re_msg, re_Add = senderSocket.recvfrom(1400)
            except:
                pass
            else:
                writeAck(log, int(re_msg.decode()), "received")
                if(int(re_msg.decode()) >= pkt):
                    rtt = time() - info[ack.index(pkt)]
                    if(avg_rtt<0):
                        avg_rtt = rtt
                        dev_rtt = 0.5 * rtt
                        timeout = avg_rtt + 4*dev_rtt
                        if timeout > 60:
                            timeout = 60
                    else:
                        dev_rtt = (0.75 * dev_rtt) + (0.25 * abs(avg_rtt - rtt))
                        avg_rtt = (0.875 * avg_rtt) + (0.125 * rtt)
                        timeout = avg_rtt + 4*dev_rtt
                        if timeout > 60:
                            timeout = 60
                    rchk = 1
                    break   
    ack_tmp = [x for x in ack if x>int(re_msg.decode())]
    info_tmp = [info[ack.index(x)] for x in ack if x>int(re_msg.decode())]            
    info = info_tmp
    ack = ack_tmp
    del info_tmp
    del ack_tmp   
    cnt = seq - int(re_msg.decode()) - 1
    if(len(info) != 0):
        expired = info[0] + timeout
    dup = 0
    dup_ack = -100
    rmode = 0

def tout_handler(signum, stack):
    global expired
    global info
    global ack
    global rmode
    if(rmode==0 and len(info)!=0 and expired < time()):
        if(len(info)!= 0):
            if(info[0] == expired - timeout):
                writePkt(log, time() - startTime, ack[0], "timeout since {:.3f} (timeout value {:.3f})".format(info[0]-startTime, timeout))
                retrans(ack[0])
    signal.setitimer(signal.ITIMER_REAL, 0.001)

def fileSender():
    print('sender program starts...')
    ##########################
    global ack
    global info
    global startTime
    global senderSocket
    global timeout
    global log
    global cnt
    global seq
    global expired
    global avg_rtt
    global dev_rtt
    global dup
    global dup_ack
    chk = 0
    fd = open(srcFilename, 'rb')
    senderSocket = socket(AF_INET,SOCK_DGRAM)
    senderSocket.setblocking(False)
    log = open(srcFilename+"_sending_log.txt", "w")
    while True: 
        if(chk == 1 and len(ack)==0):
            break
        if cnt < windowSize:
            num = str(seq)
            if(seq == 0):
                senderSocket.sendto(num.encode() + (" " * (32 - len(num))).encode() + dstFilename.encode(), (recvAddr, 10080))
                t = time()
                startTime = t
                ack.append(seq)
                info.append(t)
                writePkt(log, 0, seq, "sent")
                expired = t + timeout
                signal.signal(signal.SIGALRM, tout_handler)
                signal.setitimer(signal.ITIMER_REAL, 0.001)
            else:
                tmp = fd.read(1350)
                if not tmp:
                    chk = 1
                else:
                    senderSocket.sendto(num.encode() + (" " * (32 - len(num))).encode() + tmp, (recvAddr, 10080))
                    t = time()
                    if(len(info)==0):
                        expired = t + timeout
                    ack.append(seq)
                    info.append(t)
                    writePkt(log, t - startTime, seq, "sent")
            cnt += 1
            seq += 1
        else:
            try:
                msg, Add = senderSocket.recvfrom(1400)
            except:
                pass
            else:
                rcv_ack = int(msg.decode())
                writeAck(log, rcv_ack, "received")
                if (rcv_ack in ack):
                    i = 0
                    rtt = time() - info[i]
                    if(avg_rtt<0):
                        avg_rtt = rtt
                        dev_rtt = 0.5 * rtt
                        timeout = avg_rtt + 4*dev_rtt
                        if timeout > 60:
                            timeout = 60
                    else:
                        dev_rtt = (0.75 * dev_rtt) + (0.25 * abs(avg_rtt - rtt))
                        avg_rtt = (0.875 * avg_rtt) + (0.125 * rtt)
                        timeout = avg_rtt + 4*dev_rtt
                        if timeout > 60:
                            timeout = 60
                    ack_tmp = [x for x in ack if x>rcv_ack]
                    info_tmp = [info[ack.index(x)] for x in ack if x>rcv_ack]
                    info = info_tmp
                    ack = ack_tmp
                    del info_tmp
                    del ack_tmp
                    cnt = seq - rcv_ack - 1
                    dup = 0
                    dup_ack = -100
                else:
                    if dup_ack == int(msg.decode()):
                        dup += 1
                        if dup == 3:
                            re_ack = int(dup_ack) + 1
                            writePkt(log, time() - startTime, int(dup_ack), "3 duplicated ACKs")
                            retrans(re_ack)
                            dup = 0
                            dup_ack = -100
                    else:
                        dup_ack = int(msg.decode())
                        dup = 1 
    writeEnd(log, seq/(time() - startTime), avg_rtt*1000)
    log.close()
    senderSocket.close()
    ##########################


if __name__=='__main__':
    recvAddr = sys.argv[1]  #receiver IP address
    windowSize = int(sys.argv[2])   #window size
    srcFilename = sys.argv[3]   #source file name
    dstFilename = sys.argv[4]   #result file name

    fileSender()
