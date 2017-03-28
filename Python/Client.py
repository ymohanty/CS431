# author: Yashaswi Mohanty
# date: 3/9/2017
# file: Client.py

import socket
import json
import sys
import bluetooth
import serial

#bs = serial.Serial("/dev/rfcomm0", baudrate=115200)

class Client:
    def __init__(self, host, port, data):
        self.host = host
        self.port = port
        self.data = data


        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        except socket.error, msg:
            print 'Failed to create socket. Error: ' + str(msg[0]) + "\n" + str(msg[1])
            sys.exit(-1)

        self.sock.connect((self.host, self.port))
        self.file = self.sock.makefile()

        print "Connected with server at %s:%s" % (self.host, self.port)

    def send_data(self):
        self.sock.sendall(self.data + "\n")

def main(argv):
    l = []
    while True:
        print l
        try:
            string = raw_input("Enter Command-Value pair: ")
            command = string.split(',')[0]
            value = int(string.split(',')[1])
            l.append([command,value])

            if len(l) >1 and l[-1][0] == "END":
                break

        except ValueError:
            print "Input command-value pairs only!"

    print l

    client = Client(argv[1],38787,json.dumps(l))
    client.send_data()

if __name__ == '__main__':
    main(sys.argv)



