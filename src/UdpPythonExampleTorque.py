import socket
import time
import struct

UDP_PORT = 1239

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind(('', UDP_PORT))

while True:
	dataRec, addrRec = sock.recvfrom(16) # 2 * double
	data = struct.unpack("dd", dataRec)
	print("%s, %s" % (float(data[0]), float(data[1])))
	time.sleep(0.01) #10 ms