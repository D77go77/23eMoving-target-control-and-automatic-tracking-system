###########
#   find_red_or_green_or_green_and_red
#   find_red_or_green_or_green_and_red #define to state
#    0   1   2
###########

import cv2
import numpy as np

# rL_H = 0
# rL_S = 60
# rL_V = 60
# redLow = [rL_H, rL_S, rL_V]
# rH_H = 6
# rH_S = 255
# rH_V = 255
# redHigh = [rH_H, rH_S, rH_V]
#
# gL_H = 35
# gL_S = 43
# gL_V = 35
# greenLow = [gL_H, gL_S, gL_V]
# gH_H = 90
# gH_S = 255
# gH_V = 255
# greenHigh = [gH_H, rH_S, gH_V]

def findLaser(img,state,redLow,redHigh,greenLow,greenHigh):
    """
    找到图片中点绿色激光点与红色激光点并定位中心
    :param img: 需要处理点图片
    :return: 绿色激光点中心（x1, y1）;红色激光点中心（x2, y2)
    """
    cX1, cY1, cX2, cY2 = None, None, None, None
    greenLaser = 'green'
    redLaser = 'red'
    if state == 0:
        redL_H, redL_S, redL_V = redLow
        redH_H, redH_S, redH_V = redHigh
        color_dist = {'red': {'Lower': np.array([redL_H, redL_S, redL_V]), 'Upper': np.array([redH_H, redH_S, redH_V])}}

        blurred = cv2.GaussianBlur(img, (11, 11), 0)
        # cv2.imshow('blurred', blurred)
        # 创建运算核
        kernel = np.ones((1, 1), np.uint8)
        # 腐蚀
        # erode = cv2.erode(img, kernel, iterations=1)
        # 膨胀
        # img_dilate = cv2.dilate(blurred, kernel, iterations = 5)
        # 开运算
        opening = cv2.morphologyEx(blurred, cv2.MORPH_OPEN, kernel)
        # 二值化处理
        thresh = cv2.threshold(opening, 225, 255, cv2.THRESH_BINARY)[1]
        # cv2.imshow('thresh', thresh)

        hsv = cv2.cvtColor(thresh, cv2.COLOR_BGR2HSV)  # 转化成HSV图像
        # 颜色二值化筛选处理
        inRange_hsv_red = cv2.inRange(hsv, color_dist[redLaser]['Lower'], color_dist[redLaser]['Upper'])
        cv2.imshow('inrange_hsv_red', inRange_hsv_red)
        # 找红色激光点
        try:
            cnts2 = cv2.findContours(inRange_hsv_red.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[-2]
            c2 = max(cnts2, key=cv2.contourArea)
            M = cv2.moments(c2)
            cX2 = int(M["m10"] / M["m00"])
            cY2 = int(M["m01"] / M["m00"])
            cv2.circle(img, (cX2, cY2), 3, (0, 0, 255), -1)
            rect = cv2.minAreaRect(c2)
            box = cv2.boxPoints(rect)
            cv2.drawContours(img, [np.intp(box)], -1, (0, 0, 255), 2)
        except:
            print('没有找到红色的激光')
        return cX1, cY1, cX2, cY2

    elif state == 1:
        greenL_H, greenL_S, greenL_V = greenLow
        greenH_H, greenH_S, greenH_V = greenHigh
        color_dist = {'green': {'Lower': np.array([greenL_H, greenL_S, greenL_V]), 'Upper': np.array([greenH_H, greenH_S, greenH_V])}}

        blurred = cv2.GaussianBlur(img, (11, 11), 0)
        # cv2.imshow('blurred', blurred)
        # 创建运算核
        kernel = np.ones((1, 1), np.uint8)
        # 腐蚀
        # erode = cv2.erode(img, kernel, iterations=1)
        # 膨胀
        # img_dilate = cv2.dilate(blurred, kernel, iterations = 5)
        # 开运算
        opening = cv2.morphologyEx(blurred, cv2.MORPH_OPEN, kernel)
        # 二值化处理
        thresh = cv2.threshold(opening, 225, 255, cv2.THRESH_BINARY)[1]
        # cv2.imshow('thresh', thresh)

        hsv = cv2.cvtColor(thresh, cv2.COLOR_BGR2HSV)  # 转化成HSV图像
        # 颜色二值化筛选处理
        inRange_hsv_green = cv2.inRange(hsv, color_dist[greenLaser]['Lower'], color_dist[greenLaser]['Upper'])
        cv2.imshow('inrange_hsv_green', inRange_hsv_green)
        # 找绿色激光点
        try:
            cnts1 = cv2.findContours(inRange_hsv_green.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[-2]
            c1 = max(cnts1, key=cv2.contourArea)
            M = cv2.moments(c1)
            cX1 = int(M["m10"] / M["m00"])
            cY1 = int(M["m01"] / M["m00"])
            cv2.circle(img, (cX1, cY1), 3, (0, 255, 0), -1)
            rect = cv2.minAreaRect(c1)
            box = cv2.boxPoints(rect)
            cv2.drawContours(img, [np.intp(box)], -1, (0, 255, 0), 2)
        except:
            print('没有找到绿色的激光')
        return cX1, cY1, cX2, cY2

    elif state == 2:
        redL_H, redL_S, redL_V = redLow
        redH_H, redH_S, redH_V = redHigh

        greenL_H, greenL_S, greenL_V = greenLow
        greenH_H, greenH_S, greenH_V = greenHigh
        # 色系下限上限表
        color_dist = {'red': {'Lower': np.array([redL_H, redL_S, redL_V]), 'Upper': np.array([redH_H, redH_S, redH_V])},
                      'green': {'Lower': np.array([greenL_H, greenL_S, greenL_V]), 'Upper': np.array([greenH_H, greenH_S, greenH_V])},
                      }
        # 灰度图像处理
        # gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)#灰度图
        # cv2.imshow('gray', gray)

        # 高斯滤波
        blurred = cv2.GaussianBlur(img, (11, 11), 0)
        # cv2.imshow('blurred', blurred)
        # 创建运算核
        kernel = np.ones((1, 1), np.uint8)
        # 腐蚀
        # erode = cv2.erode(img, kernel, iterations=1)
        # 膨胀
        # img_dilate = cv2.dilate(blurred, kernel, iterations = 5)
        # 开运算
        opening = cv2.morphologyEx(blurred, cv2.MORPH_OPEN, kernel)
        # 二值化处理
        thresh = cv2.threshold(opening, 225, 255, cv2.THRESH_BINARY)[1]
        #cv2.imshow('thresh', thresh)

        hsv = cv2.cvtColor(thresh, cv2.COLOR_BGR2HSV)  # 转化成HSV图像
        # 颜色二值化筛选处理
        inRange_hsv_green = cv2.inRange(hsv, color_dist[greenLaser]['Lower'], color_dist[greenLaser]['Upper'])
        inRange_hsv_red = cv2.inRange(hsv, color_dist[redLaser]['Lower'], color_dist[redLaser]['Upper'])
        cv2.imshow('inrange_hsv_green', inRange_hsv_green)
        cv2.imshow('inrange_hsv_red', inRange_hsv_red)
        # 找绿色激光点
        try:
            cnts1 = cv2.findContours(inRange_hsv_green.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[-2]
            c1 = max(cnts1, key=cv2.contourArea)
            M = cv2.moments(c1)
            cX1 = int(M["m10"] / M["m00"])
            cY1 = int(M["m01"] / M["m00"])
            cv2.circle(img, (cX1, cY1), 3, (0, 255, 0), -1)
            rect = cv2.minAreaRect(c1)
            box = cv2.boxPoints(rect)
            cv2.drawContours(img, [np.intp(box)], -1, (0, 255, 0), 2)
        except:
            print('没有找到绿色的激光')

        # 找红色激光点
        try:
            cnts2 = cv2.findContours(inRange_hsv_red.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[-2]
            c2 = max(cnts2, key=cv2.contourArea)
            M = cv2.moments(c2)
            cX2 = int(M["m10"] / M["m00"])
            cY2 = int(M["m01"] / M["m00"])
            cv2.circle(img, (cX2, cY2), 3, (0, 0, 255), -1)
            rect = cv2.minAreaRect(c2)
            box = cv2.boxPoints(rect)
            cv2.drawContours(img, [np.intp(box)], -1, (0, 0, 255), 2)
        except:
            print('没有找到红色的激光')
        return cX1, cY1, cX2, cY2

# cap = cv2.VideoCapture(0)
# while True:
#     # get a frame
#     ret, img = cap.read()
#     x1, y1,x2,y2=findLaser(img,1,redLow,redHigh,greenLow,greenHigh)
#     print(f"Green Laser ({x1},{y1})")
#     print(f"  Red Laser ({x2},{y2})")
#     cv2.waitKey(1)