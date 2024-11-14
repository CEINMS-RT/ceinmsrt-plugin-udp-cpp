import socket
import time
import struct

UDP_PORT = 1240

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind(('', UDP_PORT))

data = [10.0, 30.0] # data to send
#https://stackoverflow.com/questions/48867997/sending-data-using-socket-from-pythonclient-to-cserver-arrays
#https://docs.python.org/3/library/struct.html
byteData = struct.pack("dd", *data) # data to double (for C++) and then Byte

dataRec, addrRec = sock.recvfrom(1) # wait to receive number of joint

if int(dataRec[0]) != 2:
	print( "ERROR: Number of Channel needed different from 2.")

print("Data is being send: IP:%s" % str(addrRec))

while True:
	sock.sendto(byteData, addrRec)
	time.sleep(0.01) #10 ms