#!/user/bin/env python3
# -*- coding: utf-8 -*-
import cv2
import numpy as np

h_min_g_r = 0
h_max_g_r= 179
s_min_g_r = 0
s_max_g_r = 255
v_min_g_r = 33
v_max_g_r = 255

h_min_r = 0
h_max_r = 6
s_min_r = 33
s_max_r = 255
v_min_r = 130
v_max_r = 255

h_min_g = 62
h_max_g= 89
s_min_g = 109
s_max_g = 255
v_min_g = 136
v_max_g = 255

kernel = np.ones((4,4),np.uint8)


def deal_the_point(img,Dia_num,Ero_num,ifisGray):
    if ifisGray:
        imgGray = img
    else:
        imgGray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # 模糊处理 ksize 只能是奇数 ( ? , ? )
    imgBlur = cv2.GaussianBlur(imgGray, (5, 5), 0)
    # 边缘检测 canny
    imgCanny = cv2.Canny(imgBlur, 150, 200)
    # 膨胀处理 kernel 要一个全是1的矩阵 厚度
    imgDialation = cv2.dilate(imgCanny, kernel, iterations=Dia_num)
    # 侵蚀处理 kernel 要一个全是1的矩阵 厚度
    imgEroded = cv2.erode(imgDialation, kernel, iterations=Ero_num)
    #cv2.imshow("imgEroded",imgEroded)
    return imgEroded

cap = cv2.VideoCapture(0)

while True:
    _,img = cap.read()
    #img = cv2.imread('takephoto2.jpg')
    img = img[100:460,100:460]
    img_draw = img.copy()
    # red and green
    imgHSV_g_r = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    lower_g_r = np.array([h_min_g_r, s_min_g_r, v_min_g_r])
    upper_g_r = np.array([h_max_g_r, s_max_g_r, v_max_g_r])
    mask_g_r = cv2.inRange(imgHSV_g_r, lower_g_r, upper_g_r)
    find_red_and_green = cv2.bitwise_and(img, img, mask=mask_g_r)
    imgGray = cv2.cvtColor(find_red_and_green, cv2.COLOR_BGR2GRAY)
    imgBlur = cv2.GaussianBlur(imgGray, (7, 7), 0)
    imgCanny = cv2.Canny(imgBlur, 150, 200)
    imgDialation = cv2.dilate(imgCanny, kernel, iterations=2)
    imgEroded = cv2.erode(imgDialation, kernel, iterations=3)
    
    imgBlur = cv2.GaussianBlur(imgEroded, (7, 7), 0)
    imgCanny = cv2.Canny(imgBlur, 150, 200)
    imgDialation = cv2.dilate(imgCanny, kernel, iterations=1)
    find_red_and_green = imgDialation

    #cv2.imshow("find_red_final", find_red_and_green)
    
    contours, hierarchy = cv2.findContours(find_red_and_green, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    # 计算每个轮廓的面积并输出
    if len(contours) == 2:
        for contour in contours:
            area = cv2.contourArea(contour)
            if 10 < area < 150:
                cv2.drawContours(img_draw, [contour], -1, (0, 255, 0), 2)
                cv2.imshow("img_draw", img_draw)
                M = cv2.moments(contour)
                if M["m00"] != 0:  # 防止除以零错误
                    cX = int(M["m10"] / M["m00"])  # 中心X坐标
                    cY = int(M["m01"] / M["m00"])  # 中心Y坐标
                    print(f"{cX}, {cY}")
        print("Find two")
         # red
        imgHSV_r = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        lower_r = np.array([h_min_r, s_min_r, v_min_r])
        upper_r = np.array([h_max_r, s_max_r, v_max_r])
        mask_r = cv2.inRange(imgHSV_r, lower_r, upper_r)
        find_red = cv2.bitwise_and(img, img, mask=mask_r)
        find_red_final = cv2.bitwise_and(deal_the_point(find_red,5,1,0), find_red_and_green, mask=mask_g_r)
        #
        contours, hierarchy = cv2.findContours(find_red_final, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        if len(contours) == 1:
            for contour in contours:
                area = cv2.contourArea(contour)
                if 10 < area < 150:
                    cv2.drawContours(img_draw, [contour], -1, (0, 255, 0), 2)
                    cv2.imshow("img_draw_r", img_draw)
                    M = cv2.moments(contour)
                    if M["m00"] != 0:  # 防止除以零错误
                        cX = int(M["m10"] / M["m00"])  # 中心X坐标
                        cY = int(M["m01"] / M["m00"])  # 中心Y坐标
                        print(f"Find red ({cX}, {cY})")
    
         # green
        imgHSV_g = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        lower_g = np.array([h_min_g, s_min_g, v_min_g])
        upper_g = np.array([h_max_g, s_max_g, v_max_g])
        mask_g = cv2.inRange(imgHSV_g, lower_g, upper_g)
        find_green = cv2.bitwise_and(img, img, mask=mask_g)
        find_green_final = cv2.bitwise_and(deal_the_point(find_green,2,1,0), find_red_and_green, mask=mask_g_r)
        
        cv2.imshow("find_green", find_green_final)
        ##
        contours2, hierarchy = cv2.findContours(find_green_final, cv2.RETR_EXTERNAL,
                                            cv2.CHAIN_APPROX_SIMPLE)
        if len(contours2) == 1:
            for contour in contours2:
                area = cv2.contourArea(contour)
                cv2.drawContours(img_draw, [contour], -1, (0, 255, 0), 2)
                cv2.imshow("img_draw_g", img_draw)
                M = cv2.moments(contour)
                if M["m00"] != 0:  # 防止除以零错误
                    cX = int(M["m10"] / M["m00"])  # 中心X坐标
                    cY = int(M["m01"] / M["m00"])  # 中心Y坐标
                    print(f"Find green ({cX}, {cY})")
    
    elif len(contours) == 1:
        for contour in contours:
            area = cv2.contourArea(contour)
            cv2.drawContours(img_draw, [contour], -1, (0, 255, 0), 2)
            cv2.imshow("img_draw", img_draw)
            M = cv2.moments(contour)
            if M["m00"] != 0:  # 防止除以零错误
                cX = int(M["m10"] / M["m00"])  # 中心X坐标
                cY = int(M["m01"] / M["m00"])  # 中心Y坐标
                print(f"Find one ({cX}, {cY})")
        #print("Find one")
    else :
        print("Find None")

    cv2.waitKey(1)