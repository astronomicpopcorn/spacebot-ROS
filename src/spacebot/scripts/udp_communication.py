#!/usr/bin/env python

import rospy
import socket
import struct

from geometry_msgs.msg import Twist

ARDUINO_IP = "192.168.188.17"
PORT = 8888

print(f"Controllino IP: {ARDUINO_IP}")
print(f"Port: {PORT}")

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

msg_index = 0

def send_udp_packet(msg):
    print("Sending packet...")
    global msg_index

    linear = msg.linear.x
    angular = msg.angular.z

    left = int((linear - angular) * 255)
    right = int((linear + angular) * 255)

    packet = struct.pack("<hhI", left, right, msg_index)

    sock.sendto(packet, (ARDUINO_IP, PORT))

    msg_index += 1

    print("Packet sent!")

def udp_communication():
    print("Starting ROS Node...")
    rospy.init_node("spacebot_udp_communication")
    print("ROS Node started!")
    rospy.Subscriber("spacebot_target_speeds", Twist, send_udp_packet)
    print("Subscribed to 'spacebot_target_speeds' successfully!")
    print("Spinning...")
    rospy.spin()
    print("No longer spinning.")

if __name__ == '__main__':
    udp_communication()

    