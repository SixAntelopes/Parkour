import cv2,os
import numpy as np
from Mtcnndnn import MTCNNDetector

import win32file

def drawDetection(img, box, point):
    if len(box) == 0:
        return
    box = [int(b) for b in box]
    cv2.rectangle(img,(box[0],box[1]),(box[2],box[3]),(147,20,255),2)
    for j in range(5):
        cv2.circle(img,(int(point[j]),int(point[j+5])),2,(0,255,0), -1)

# 只处理距离摄像头最近的头，也就是最大的脸
def selBigFace(boxes, points):
    if boxes.size == 0:
        return [], []
    maxI = -1
    maxV = -1
    for i in range(boxes.shape[0]):
        box = boxes[i]
        box = [int(b) for b in box]
        w = box[2] - box[0]
        h = box[3] - box[1]
        size = pow(w,2)+pow(h,2)
        if size > maxV:
            maxI = i
            maxV = size
    point = [t[maxI] for t in points]
    return boxes[maxI], point

objectPoints = np.array([[2.37427, 110.322, 21.7776],
                         [70.0602, 109.898, 20.8234],
                         [36.8301, 78.3185, 52.0345],
                         [14.8498, 51.0115, 30.2378],
                         [58.1825, 51.0115, 29.6224]], np.float64)
colors = [[255,0,0],[0,255,0],[0,0,255]]

# 估计姿态
def drawDetection(img, point):
    if len(point) == 0:
        return
    for j in range(5):
        cv2.circle(img,(int(point[j]),int(point[j+5])),2,(0,255,0), -1)
    # 相机内参矩阵
    camera_matrix = np.array([[658.57647859, 0.0, 333.35516441],
                            [0.0, 657.41143152, 246.48333067],
                            [0.0, 0.0, 1.0]],dtype=np.float32)
    # 相机畸变系数
    dist_coeffs = np.array([[-7.72463838e-03, -2.90139073e-01, 3.59360155e-04, -1.87956972e-03, 1.15327477e+00]], dtype=np.float32)
    imagePoints = np.array([[point[i], point[i+5]] for i in range(5)],dtype=np.float32)
    ret, rVec, tVec = cv2.solvePnP(objectPoints, imagePoints, camera_matrix, dist_coeffs, flags = cv2.SOLVEPNP_EPNP)
    if ret:
        # 画3D轴
        axislength = 40
        mp2 = objectPoints[2]
        axis3d = np.array([[mp2[0],mp2[1],mp2[2]],
                        [mp2[0]+axislength,mp2[1],mp2[2]],
                        [mp2[0],mp2[1]+axislength,mp2[2]],
                        [mp2[0],mp2[1],mp2[2]+axislength]],dtype = np.float32)
        axis2d, _ = cv2.projectPoints(axis3d,rVec,tVec,camera_matrix, dist_coeffs)
        axis2d = np.int32(axis2d).reshape(-1,2)
        # 计算显示欧拉角
        R = cv2.Rodrigues(rVec)[0]
        T = np.hstack((R,tVec))
        pitch, yaw, roll = cv2.decomposeProjectionMatrix(T, camera_matrix)[-1]
        eulers= [pitch, yaw, roll]
        ctrl_btn = '\0'
        if (-15 < roll) and (roll < 10):
            cv2.putText(img, '-',(200,100),2,5,(147,20,255),3)
            ctrl_btn = 'R'
        elif (roll < -15):
            cv2.putText(img, '<-',(200,100),2,5,(147,20,255),3)
            ctrl_btn = 'A'
        elif (15 < roll):
            cv2.putText(img, '->',(200,100),2,5,(147,20,255),3)
            ctrl_btn = 'D'
        cv2.putText(img, str(eulers[0]),(0,15+20*0),1,1,(0,255,0))
        cv2.putText(img, str(eulers[2]),(0,15+20*2),1,1,(147,20,255))
        return ctrl_btn

def test_camera(detector, pipe):
    cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)
    ret, img = cap.read()   #捕获帧，返回分辨率和图像
    buf = []
    while ret:
        img = cv2.flip(img, 1)
        boxes, points= detector.detect(img)
        box, point = selBigFace(boxes, points)
        ctrl_btn = drawDetection(img, point)
        buf.append(ctrl_btn)
        try:
            if ctrl_btn!='\0' and ctrl_btn!=None:
                win32file.WriteFile(pipe, ctrl_btn.encode())
        except:
            win32file.CloseHandle(pipe)
            print("服务端已关闭，此程序要退出了。")
            return
        cv2.imshow('capCam',img)
        cv2.waitKey(1)
        ret, img = cap.read()

if __name__ == '__main__':
    pipe = win32file.CreateFile(r'\\.\pipe\ReadPipe',
                                win32file.GENERIC_WRITE,
                                0,
                                None,
                                win32file.OPEN_EXISTING,
                                win32file.FILE_ATTRIBUTE_NORMAL,
                                None)
    detector = MTCNNDetector()
    test_camera(detector, pipe)