# https://docs.opencv.org/4.x/dd/d43/tutorial_py_video_display.html

import cv2
import numpy as np
import onnxruntime as rt

# define a video capture object
vid = cv2.VideoCapture(0)

print(rt.get_device()  )

model_name = 'data/Resnet34_3inputs_448x448_20200609.onnx'
sess = rt.InferenceSession(model_name, providers=['CUDAExecutionProvider'])
# sess = rt.InferenceSession(model_name, providers=['TensorrtExecutionProvider', 'CUDAExecutionProvider'])
inputs = sess.get_inputs()
print(inputs)

while(True):
      
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

    # the 'q' button is set as the
    # quitting button you may use any
    # desired button of your choice
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# After the loop release the cap object
vid.release()
# Destroy all the windows
cv2.destroyAllWindows()
