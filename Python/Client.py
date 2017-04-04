# author: Yashaswi Mohanty
# date: 3/9/2017
# file: Client.py

import socket
import json
import sys


# import bluetooth
# import serial


# bs = serial.Serial("/dev/rfcomm0", baudrate=115200)

class BTListener:
    def __init__(self):
        self.host = "00:0D:19:EA:28:52"
        self.port = 3
        self.sock = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)
        self.sock.connect((self.host, self.port))
        self.sock.setblocking(0)
        self.file = self.sock.makefile()
        self.data = []

    def run(self):
        while True:
            cmd = raw_input("Enter robot movement commands: ")

            if cmd != "":
                self.sock.sendall(cmd)

            string = self.file.readline()
            command = string.split(',')[0]
            value = int(string.split('.')[1])

            if command == "END":
                self.sock.close()
                break

            pair = [command, value]
            self.data.append(pair)

    def get_data(self):
        return self.data


class Client:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.data = None

        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        except socket.error, msg:
            print 'Failed to create socket. Error: ' + str(msg[0]) + "\n" + str(msg[1])
            sys.exit(-1)

        self.sock.connect((self.host, self.port))
        self.file = self.sock.makefile()

        print "Connected with server at %s:%s" % (self.host, self.port)

    def send_data(self):
        if self.data is not None:
            self.sock.sendall(json.dumps(self.data) + "\n")

    def add(self, command_list):
        self.data = command_list


# def main(argv):
#     l = []
#     while True:
#         print l
#         try:
#             string = raw_input("Enter Command-Value pair: ")
#             command = string.split(',')[0]
#             value = int(string.split(',')[1])
#             l.append([command, value])
#
#             if len(l) > 1 and l[-1][0] == "END":
#                 break
#
#         except ValueError:
#             print "Input command-value pairs only!"
#
#     print l
#
#     client = Client(argv[1], 38787, json.dumps(l))
#     client.send_data()

def main(argv):
    print "Creating bluetooth listener...\n"
    listener = BTListener()
    print "Listening for bluetooth transmissions from %s on port %d" % (listener.host,listener.port)
    listener.run()
    data = listener.get_data()

    client = Client(argv[1], argv[2], data)


if __name__ == '__main__':
    main(sys.argv)
