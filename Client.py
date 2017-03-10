# author: Yashaswi Mohanty
# date: 3/9/2017
# file: Client.py
import socket
import json
import sys


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

def main():
    client = Client('localhost',8787,json.dumps([["forward",100],["right",90],["forward",100]]))
    client.send_data()
    print json.dumps([["forward",100],["right",90],["forward",100]])

if __name__ == '__main__':
    main()



