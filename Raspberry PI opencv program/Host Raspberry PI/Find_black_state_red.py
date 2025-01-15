import cv2
import numpy as np
from FL import findLaser

def empty(a):
    pass

rL_H = 117
rL_S = 179
rL_V = 0
redLow = [rL_H, rL_S, rL_V]
rH_H = 255
rH_S = 0
rH_V = 255
redHigh = [rH_H, rH_S, rH_V]

cap = cv2.VideoCapture(0)

cv2.namedWindow("TrackBars")
cv2.resizeWindow("TrackBars",640,240)
cv2.createTrackbar("Hue Min","TrackBars",0,360,empty)
cv2.createTrackbar("Hue Max","TrackBars",19,360,empty)
cv2.createTrackbar("Sat Min","TrackBars",110,255,empty)
cv2.createTrackbar("Sat Max","TrackBars",240,255,empty)
cv2.createTrackbar("Val Min","TrackBars",153,255,empty)
cv2.createTrackbar("Val Max","TrackBars",255,255,empty)

while(1):
    # get a frame
    ret, img = cap.read()
    # show a frame
    cv2.imshow("capture", img)
    imgHSV = cv2.cvtColor(img,cv2.COLOR_BGR2HSV)
    h_min = cv2.getTrackbarPos("Hue Min","TrackBars")
    h_max = cv2.getTrackbarPos("Hue Max", "TrackBars")
    s_min = cv2.getTrackbarPos("Sat Min", "TrackBars")
    s_max = cv2.getTrackbarPos("Sat Max", "TrackBars")
    v_min = cv2.getTrackbarPos("Val Min", "TrackBars")
    v_max = cv2.getTrackbarPos("Val Max", "TrackBars")
    print(h_min,h_max,s_min,s_max,v_min,v_max)
    lower = np.array([h_min,s_min,v_min])
    upper = np.array([h_max,s_max,v_max])
    # 设置罩子对特点区域操作
    mask = cv2.inRange(imgHSV,lower,upper)
    # 所以按位和得到 mask 区域里原图像
    imgResult = cv2.bitwise_and(img,img,mask=mask)
    
    cv2.imshow("imgHSV",imgHSV)
    cv2.imshow("imgResult",imgResult)
    
    cv2.waitKey(1)
    
cap.release()
cv2.destroyAllWindows()