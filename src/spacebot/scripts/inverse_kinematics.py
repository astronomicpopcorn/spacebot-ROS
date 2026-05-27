#!/usr/bin/env python

import rospy

from geometry_msgs.msg import Twist
from std_msgs.msg import Float32
from std_msgs.msg import Float32MultiArray



#Funktionen für die inverse Kinematik

r = 0.2 #[m] Radius Reifen
b = 0.5 #[m] Radabstand
pub = None #Globale variable für den ROS Publisher

velocity_L = 0.0
velocity_R = 0.0

def caluculateInverseKinematic(data):
    global velocity_L, velocity_R
    w = data.angular.z
    v = data.linear.x
    velocity_L = getAngularVelocityLeft(v, w)
    velocity_R = getAngularVelocityRight(v, w)

#Gibt die Winkelgeschw. des r. Rades zurück 
def getAngularVelocityRight(v, w): # v=lineare geschw.; w=drehgeschw.
    return (v+w*(b/2))/r

def getAngularVelocityLeft(v, w): # v=lineare geschw.; w=drehgeschw.
    return (v-w*(b/2))/r



#Funktionen für die ROS-Kommunikation

def initilizeROSNode():
    global pub
    rospy.init_node("inverse_kinematics")
    rospy.Subscriber("spacebot_twist_input", Twist, caluculateInverseKinematic)
    pub = rospy.Publisher('spacebot_angular_velocities', Float32MultiArray, queue_size=10)
    rate = rospy.Rate(10)  # 10 Hz

    rospy.loginfo("Node started")

    while not rospy.is_shutdown():
        postMessage()
        rate.sleep()

def postMessage():
    msg = Float32MultiArray()
    msg.data = [velocity_L, velocity_R]
    pub.publish(msg)

if __name__ == '__main__':
    initilizeROSNode()
    print("Ich bin der inverse Kinematics script! :)")