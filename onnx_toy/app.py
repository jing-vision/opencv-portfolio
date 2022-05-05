# https://docs.opencv.org/4.x/dd/d43/tutorial_py_video_display.html
# VMC https://protocol.vmc.info/english
# https://docs.unity3d.com/ScriptReference/HumanBodyBones.html

import cv2
import numpy as np
import onnxruntime as rt
from pythonosc import udp_client
import argparse
import time
import os
from enum import IntEnum, auto

dir_name = os.path.dirname(os.path.realpath(__file__))

class PositionIndex(IntEnum):
    rShldrBend = 0
    rForearmBend = 1
    rHand = 2
    rThumb2 = 3
    rMid1 = 4

    lShldrBend = 5
    lForearmBend = 6
    lHand = 7
    lThumb2 = 8
    lMid1 = 9

    lEar = 10
    lEye = 11
    rEar = 12
    rEye = 13
    Nose = 14

    rThighBend = 15
    rShin = 16
    rFoot = 17
    rToe = 18

    lThighBend = 19
    lShin = 20
    lFoot = 21
    lToe = 22

    abdomenUpper = 23

    # Calculated coordinates
    hip = 24
    head = 25
    neck = 26
    spine = 27

    Count = 28
    none = 29

class JointPoint:
    def __init__(self) -> None:
        self.index = PositionIndex.none
        self.child = None
        self.parent = None
        self.unity_name = "Bone"
        self.score = 0

jointPoints = [None] * PositionIndex.Count
for i in range(PositionIndex.Count):
    jointPoints[i] = JointPoint()
# jointPoints[PositionIndex.hip].unity_name = "transform"

# // Right Arm
jointPoints[PositionIndex.rShldrBend].unity_name = "RightUpperArm"
jointPoints[PositionIndex.rForearmBend].unity_name = "RightLowerArm"
jointPoints[PositionIndex.rHand].unity_name = "RightHand"
jointPoints[PositionIndex.rThumb2].unity_name = "RightThumbIntermediate"
jointPoints[PositionIndex.rMid1].unity_name = "RightMiddleProximal"
# // Left Arm
jointPoints[PositionIndex.lShldrBend].unity_name = "LeftUpperArm"
jointPoints[PositionIndex.lForearmBend].unity_name = "LeftLowerArm"
jointPoints[PositionIndex.lHand].unity_name = "LeftHand"
jointPoints[PositionIndex.lThumb2].unity_name = "LeftThumbIntermediate"
jointPoints[PositionIndex.lMid1].unity_name = "LeftMiddleProximal"
# // Face
jointPoints[PositionIndex.lEar].unity_name = "Head"
jointPoints[PositionIndex.lEye].unity_name = "LeftEye"
jointPoints[PositionIndex.rEar].unity_name = "Head"
jointPoints[PositionIndex.rEye].unity_name = "RightEye"
jointPoints[PositionIndex.Nose].unity_name = "Nose.transform"

# // Right Leg
jointPoints[PositionIndex.rThighBend].unity_name = "RightUpperLeg"
jointPoints[PositionIndex.rShin].unity_name = "RightLowerLeg"
jointPoints[PositionIndex.rFoot].unity_name = "RightFoot"
jointPoints[PositionIndex.rToe].unity_name = "RightToes"

# // Left Leg
jointPoints[PositionIndex.lThighBend].unity_name = "LeftUpperLeg"
jointPoints[PositionIndex.lShin].unity_name = "LeftLowerLeg"
jointPoints[PositionIndex.lFoot].unity_name = "LeftFoot"
jointPoints[PositionIndex.lToe].unity_name = "LeftToes"

# // etc
jointPoints[PositionIndex.abdomenUpper].unity_name = "Spine"
jointPoints[PositionIndex.head].unity_name = "Head"
jointPoints[PositionIndex.hip].unity_name = "Hips"
jointPoints[PositionIndex.neck].unity_name = "Neck"
jointPoints[PositionIndex.spine].unity_name = "Spine"

parent = [26,0,1,2,2, 
        26,5,6,7,7,
        11,14,13,14,25,
        24,15,16,17,
        24,19,20,21,
        24,-1,26,27,23]

start_seconds = time.time()

def main(osc_client):
    # osc_client.send_message("/filter", random.random())

    # define a video capture object
    vid = cv2.VideoCapture(0)

    print(rt.get_device()  )

    model_name = dir_name + '/data/Resnet34_3inputs_448x448_20200609.onnx'
    sess = rt.InferenceSession(model_name, providers=['CUDAExecutionProvider'])
    # sess = rt.InferenceSession(model_name, providers=['TensorrtExecutionProvider', 'CUDAExecutionProvider'])
    inputs = sess.get_inputs()
    print(inputs)

    while(True):
        
        osc_client.send_message("/VMC/Ext/T", time.time() - start_seconds)

        # Capture the video frame
        # by frame
        ret, frame = vid.read()
    
        # Display the resulting frame
        cv2.imshow('frame', frame)

        blob = cv2.dnn.blobFromImage(frame, 1.0/255, (448,448))
        
        pred_onx = sess.run(None,{
            inputs[0].name:blob,
            inputs[1].name:blob,
            inputs[2].name:blob
        })

        offset3D = np.squeeze(pred_onx[2])
        heatMap3D = np.squeeze(pred_onx[3])
        print(offset3D.shape)
        print(heatMap3D.shape)
        print(offset3D.shape[0]/heatMap3D.shape[0])
        
        kps = np.zeros((24,3),np.float32)
        for j in range(0,24):
            # 找到第j个关节的28个特征图，并找到最大值的索引
            joint_heat = heatMap3D[j*28:(j+1)*28,...]
            [x,y,z] = np.where(joint_heat==np.max(joint_heat))
            x=int(x[-1])
            y=int(y[-1])
            z=int(z[-1])
            #通过heatmap的索引找到对应的offset图，并计算3D坐标的xyz值
            pos_x = offset3D[j*28+x,y,z] + x
            pos_y = offset3D[24*28+j*28+x,y,z] + y
            pos_z = offset3D[24*28*2+j*28+x,y,z] + z
            
            kps[j,0] = pos_x
            kps[j,1] = pos_y
            kps[j,2] = pos_z
            print("%2d,%f,%f,%f" %(j, pos_x,pos_y,pos_z))

            # /VMC/Ext/Bone/Pos (string){name} (float){p.x} (float){p.y} (float){p.z} (float){q.x} (float){q.y} (float){q.z} (float){q.w}  
            osc_client.send_message("/VMC/Ext/Bone/Pos", [jointPoints[j].unity_name,
                pos_x / 100, pos_y / 100, pos_z / 100,
                0.0,0.0,0.0,1.0])

        # the 'q' button is set as the
        # quitting button you may use any
        # desired button of your choice
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # After the loop release the cap object
    vid.release()
    # Destroy all the windows
    cv2.destroyAllWindows()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--ip", default="127.0.0.1",
        help="The ip of the OSC server")
    parser.add_argument("--port", type=int, default=39539,
        help="The port the OSC server is listening on")
    args = parser.parse_args()

    osc_client = udp_client.SimpleUDPClient(args.ip, args.port)

    main(osc_client)
