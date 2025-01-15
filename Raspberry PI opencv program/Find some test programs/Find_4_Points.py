import cv2
import numpy as np
import RPi.GPIO as GPIO
import time

cap = cv2.VideoCapture(0)
cap.set(3, 640)
cap.set(4, 480)

myColor = [146, 89, 133, 179, 255, 255]#色域HSV
myColorValue = (51, 153, 255)#画点的色域

# GPIO初始化 key BCM 26
GPIO.setmode(GPIO.BCM)
GPIO.setup(26, GPIO.IN, pull_up_down=GPIO.PUD_UP)
get_key_flag = 0

points = []  # 存储点的坐标
point_count = 0  # 记录已确定的点数
start_flag = False  #确定中心点结束标志
IS_DEBUG = True#调试图视化
def key_control():
    global get_key_flag
    if GPIO.input(26) == 0:
        time.sleep(.20)
        if GPIO.input(26) == 1:
            get_key_flag = 1
def find_and_mark_point(img):
    global point_count
     # 将BGR图像转换为HSV色彩空间
    imgHSV = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    lower = np.array(myColor[:3])
    upper = np.array(myColor[3:])
     # 应用颜色掩码，得到对应颜色区域的二值化图像
    mask = cv2.inRange(imgHSV, lower, upper)
     # 在二值化图像中找到轮廓中心
    contour_center = get_contour_center(mask)
    if contour_center and point_count < 4:  # 确保不超过四个点
        cv2.circle(img, contour_center, 5, myColorValue, -1)
        points.append(contour_center)
        point_count += 1
        print(f"初始化点坐标 Point {point_count}: {contour_center}")# 打印当前已标记点的数量和坐标
    # 绘制所有已记录的点和连线
    for pt in points:
        cv2.circle(img, pt, 5, myColorValue, -1)
    if len(points) >= 2: # 如果已标记的点数量大于等于2，则在这些点之间绘制连线
        for i in range(len(points) - 1): # 遍历所有点，除了最后一个点，连接每两个相邻的点
            cv2.line(img, points[i], points[i + 1], (0, 255, 0), 2)
        if len(points) == 4: # 连接最后一个点和第一个点
            cv2.line(img, points[-1], points[0], (0, 255, 0), 2)
    return contour_center

def get_contour_center(mask):
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    center = None
    if contours:
        c = max(contours, key=cv2.contourArea)
        moments = cv2.moments(c)
        if moments["m00"] != 0:
            cx = int(moments["m10"] / moments["m00"])
            cy = int(moments["m01"] / moments["m00"])
            center = (cx, cy)
    return center
def get_rectangle_center(points):
    """
    计算矩形的中心点坐标。
    参数:
    points: 一个包含四个点坐标的列表，每个点坐标为一个二元组(x, y)。
    返回:
    一个二元组，表示矩形的中心点坐标。
    """
    # 提取所有点的x坐标
    #计算由四个点构成的矩形的中心坐标。
    x_coords = [pt[0] for pt in points]
    # 提取所有点的y坐标
    y_coords = [pt[1] for pt in points]
    # 计算矩形中心的x坐标
    center_x = sum(x_coords) // 4
    # 计算矩形中心的y坐标
    center_y = sum(y_coords) // 4
    # 返回矩形的中心点坐标
    return (center_x, center_y)


while True:
    ret, frame = cap.read()

    key_control()

    # 按下按键获取点
    if get_key_flag == 1:
        find_and_mark_point(frame)
        get_key_flag = 0

    # 可视化点线
    if IS_DEBUG:
        if points:
            for pt in points:
                cv2.circle(frame, pt, 5, myColorValue, -1)
            if len(points) >= 2:
                for i in range(len(points) - 1):
                    cv2.line(frame, points[i], points[i + 1], (0, 255, 0), 2)
                if len(points) == 4:
                    cv2.line(frame, points[-1], points[0], (0, 255, 0), 2)

    # 确保有四个点后再计算中心
    if points and len(points) == 4:
        center_of_rectangle = get_rectangle_center(points)

        if IS_DEBUG:# 可视化中心点
            cv2.circle(frame, center_of_rectangle, 5, (255, 0, 0), -1)

        if not start_flag:#防止多次打印
            start_flag = True
            print(f"中心点坐标: {center_of_rectangle}")

    cv2.imshow("Frame", frame)
    cv2.waitKey(1)


cap.release()
cv2.destroyAllWindows()

