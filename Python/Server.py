# author: Yashaswi Mohanty
# date: 3/7/2017
# file: Server.py

import socket
import sys
import json
from Tkinter import *
from turtle import *
import turtle
import os.path


class Server:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.data = None
        self.scale = 1

        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        except socket.error, msg:
            print "Failed to create socket. Error: " + str(msg[0]) + "\n" + str(msg[1])
            sys.exit(-1)

        try:
            self.sock.bind((host, port))
            print "Running server on %s:%s" % (host, port)
        except socket.error, msg:
            print "Bind failed. Error: " + str(msg[0]) + "\n" + str(msg[1])

        self.get_data()

    def get_host(self):
        return self.host

    def get_port(self):
        return self.port

    def close(self):
        self.sock.close()

    def get_data(self):

        # Get address of client
        self.sock.listen(1)
        conn, addr = self.sock.accept()
        print "Connected accepted from %s:%s" % (str(addr[0]), str(addr[1]))

        # make a file in which all the incoming byte streams are stored
        local_file = conn.makefile()

        string = local_file.readline()
        print string

        self.data = json.loads(string)
        self.draw_data()

    def set_scale(self):
        if self.data is not None:
            sum = 0
            for item in self.data:
                if item[0] == "forward":
                    sum += item[1]

            self.scale = 500/sum

    def draw_data(self):
        self.set_scale()

        turtle.left(90)
        turtle.speed(1)

        turtle.dot(10)
        for item in self.data:
            if item[0] == "forward":
                turtle.forward(self.scale*item[1])

            elif item[0] == "right":
                turtle.right(item[1])

            elif item[0] == "left":
                turtle.left(item[1])

        turtle.color(1,0,0)
        turtle.dot(10)

        turtle.color(0,0,0)

        ts = turtle.getscreen()

        i = 0
        while True:
            if not os.path.isfile("path_%d" % i):
                ts.getcanvas().postscript(file="path_%d" % i)
                break
            i += 1

        turtle.mainloop()

def main(argv):
    server = Server(argv[1],38787)

if __name__ == '__main__':
    main(sys.argv)
