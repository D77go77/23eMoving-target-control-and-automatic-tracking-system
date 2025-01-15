#!/user/bin/env python3
# -*- coding: utf-8 -*-
import cv2
import numpy as np

def getContours(img):
    while True:
        imgGray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
        daptiveThreshold = cv2.adaptiveThreshold(imgGray, 255, cv2.ADAPTIVE_THRESH_MEAN_C, cv2.THRESH_BINARY_INV, 11, 5)
        imgBlur = cv2.GaussianBlur(daptiveThreshold,(5,5),1)
        imgCanny = cv2.Canny(imgBlur,200,200)
        kernel = np.ones((5,5))
        imgDial = cv2.dilate(imgCanny,kernel,iterations=1)
        imgThres = cv2.erode(imgDial,kernel,iterations=1)
        cv2.imshow("Result", imgThres)

        approx_contours = []

        contours, hierarchy = cv2.findContours(imgThres, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)
        for cnt in contours:
            area = cv2.contourArea(cnt)
            if area > 10000 and area < 50000:
                cv2.drawContours(img, cnt, -1, (255, 0, 0), 1)
                peri = cv2.arcLength(cnt, True)
                approx = cv2.approxPolyDP(cnt, 0.02 * peri, True)
                if len(approx) == 4:
                    approx_contours.append(approx)
                    cv2.drawContours(img, approx, -1, (240, 155, 0), 5)
                    
        cv2.imwrite("img_small_rec.jpg",img)
        cv2.imshow("Contour", img)

        if len(approx_contours) == 2:
            return approx_contours
        else:
            return None
            break