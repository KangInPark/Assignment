import sys
import os
from time import *
from socket import *

startTime = 0

"Use this method to write Packet log"
def writePkt(logFile, procTime, pktNum, event):
    logFile.write('{:1.3f} pkt: {} | {}\n'.format(procTime, pktNum, event))

"Use this method to write ACK log"
def writeAck(logFile, ackNum, event):
    procTime = time() - startTime
    logFile.write('{:1.3f} ACK: {} | {}\n'.format(procTime, ackNum, event))

"Use this method to write final throughput log"
def writeEnd(logFile, throughput):
    logFile.write('File transfer is finished.\n')
    logFile.write('Throughput : {:.2f} pkts/sec'.format(throughput))


def fileReceiver():
    print('receiver program starts...') 
    #########################
    receiverSocket = socket(AF_INET,SOCK_DGRAM)
    receiverSocket.bind(("", 10080))
    ack = {}
    min_ack = -1
    max_ack = 0
    end_chk = 0
    global startTime
    log = open("log.txt", "w")
    while True:
        recv, Add = receiverSocket.recvfrom(1400)
        seq = int(recv[0:32].decode().strip())
        data = recv[32:]
        t = time()
        if startTime==0:
            startTime = t
        writePkt(log, t - startTime, seq, "received")
        if seq <= min_ack:
            receiverSocket.sendto(str(min_ack).encode(), Add)
            writeAck(log, min_ack, "sent")
            continue
        if len(data)<1350 and seq != 0:
            end_chk = 1
        if(seq == min_ack + 1):
            min_ack += 1
            if(seq == 0):
                dstFilename = data.decode()
                fd = open(dstFilename, "wb")
            else:
                fd.write(data)
                while True:
                    if((min_ack + 1) in ack):
                        fd.write(ack[min_ack + 1])
                        del ack[min_ack + 1]
                        min_ack += 1
                    else:
                        break
                max_ack = max(min_ack, max_ack)
        else:
            ack[seq] = recv[32:]
            max_ack = max(seq, max_ack)
        receiverSocket.sendto(str(min_ack).encode(), Add)
        writeAck(log, min_ack, "sent")
        if(min_ack == max_ack and end_chk == 1):
            fd.close()
            writeEnd(log, max_ack/(time() - startTime))
            log.close()
            os.rename("log.txt", dstFilename+"_receiving_log.txt")
            break    

    receiverSocket.close()
    #########################

if __name__=='__main__':
    fileReceiver()
