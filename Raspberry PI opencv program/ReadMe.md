# 2023电赛E题 云台 ver 1.0

## Opencv程序

![image-20240627202945933](https://jumpknight.obs.cn-north-4.myhuaweicloud.com/img/image-20240627202945933.png)

<img src="https://jumpknight.obs.cn-north-4.myhuaweicloud.com/img/image-20240627212731373.png" alt="image-20240627212731373" style="zoom:50%;" />

## LCD屏幕引脚看这边

![image-20240627212901645](https://jumpknight.obs.cn-north-4.myhuaweicloud.com/img/image-20240627212901645.png)

### 1、main.py

主函数 main.py 用的是ST7735S显示屏，通过显示屏显示状态，切换状态，执行任务

按键用到左边一竖列S7-S4

<img src="https://jumpknight.obs.cn-north-4.myhuaweicloud.com/img/image-20240627204701693.png" alt="image-20240627204701693" style="zoom:25%;" />

#### 状态1

运行程序，先开始是调整HSV色域模式，状态1

<img src="https://jumpknight.obs.cn-north-4.myhuaweicloud.com/img/image-20240627204616094.png" alt="image-20240627204616094" style="zoom: 25%;" />



按键 S7 是否显示摄像头

<img src="https://jumpknight.obs.cn-north-4.myhuaweicloud.com/img/image-20240627204754718.png" alt="image-20240627204754718" style="zoom:25%;" />

按键 S6 在多种HSV色域之间切换（这里就不放全了）

能显示当前红色坐标，如果读不到就显示None说明要调整色域

<img src="https://jumpknight.obs.cn-north-4.myhuaweicloud.com/img/image-20240627204912914.png" alt="image-20240627204912914" style="zoom:25%;" />

<img src="https://jumpknight.obs.cn-north-4.myhuaweicloud.com/img/image-20240627204925920.png" alt="image-20240627204925920" style="zoom:25%;" />

<img src="https://jumpknight.obs.cn-north-4.myhuaweicloud.com/img/image-20240627204937466.png" alt="image-20240627204937466" style="zoom:25%;" />

按键 S5 更新当前色域，会在屏幕上现实RENEW更新

<img src="https://jumpknight.obs.cn-north-4.myhuaweicloud.com/img/image-20240627205052765.png" alt="image-20240627205052765" style="zoom:25%;" />

按键 S4 在确定几个色域调整完成后，从状态1切换到状态2

<img src="https://jumpknight.obs.cn-north-4.myhuaweicloud.com/img/image-20240627205248427.png" alt="image-20240627205248427" style="zoom:25%;" />

#### 状态2

按键 S7 是否显示摄像头（这里不要按！！！）

按键 S5 记录四个点坐标，如果记录不对就继续按S5从头开记录四个点

<img src="https://jumpknight.obs.cn-north-4.myhuaweicloud.com/img/image-20240627212040040.png" alt="image-20240627212040040" style="zoom:25%;" />

#### 任务1

按键 S4 切换到任务1（切换条件是必须准确记录到了4个点，少一个点就卡在状态2）

<img src="https://jumpknight.obs.cn-north-4.myhuaweicloud.com/img/image-20240627205340074.png" alt="image-20240627205340074" style="zoom:25%;" />

任务一 利用四个点坐标，生成中点坐标，串口发送四个点和中点坐标

<img src="https://jumpknight.obs.cn-north-4.myhuaweicloud.com/img/image-20240627205729718.png" alt="image-20240627205729718" style="zoom:25%;" />

#### 任务2

按下 S4 切换到任务2（注意到任务2时候会直接拍照，找中间小黑色矩形）

<img src="https://jumpknight.obs.cn-north-4.myhuaweicloud.com/img/image-20240627210122939.png" alt="image-20240627210122939" style="zoom: 80%;" />

如果没放黑色矩形框，则返回None

按键 S5 实现手动更改黑色矩形框为，实现更新黑色矩形框功能

![image-20240627210159781](https://jumpknight.obs.cn-north-4.myhuaweicloud.com/img/image-20240627210159781.png)

![image-20240627210205292](https://jumpknight.obs.cn-north-4.myhuaweicloud.com/img/image-20240627210205292.png)

### 2、FS.py

实现找黑色框框返回内层4个外层四个坐标点

```python
getContours函数
```



### 3、FM.py

实现用四个点，生成中点坐标

```python
get_rectangle_center函数
```



### 3、FL.py

三种状态，实现识别红色，绿色，红色和绿色

```
findLaser函数
```



## 明天实现功能：

### 1、串口发送坐标

### 2、优化下代码

### 3、完成任务三，识别黑色矩形上的点，然后返回红色点坐标和绿色点坐标



## 注意

### 1、屏幕显示卡顿

目前屏幕还是有点卡，体现在按键的切换上，可能是因为按键一直调整值，SPI占用较大原因

### 2、题目要求

在识别大矩形时候不管在什么地方，只要按下按键都回回到中心点，需要注意



# 2023电赛E题 云台 ver 1.5

# 主机树莓派ver 2.0

### 1、按键 S6 在多种HSV色域之间切换（修改）

这里只要找红色色域就行，现在想法是设置红色点的极端值三种切换，然后找红色点

### 2、任务2（后续要添加，复位的时候激光关闭，再打开，因为这个激光对8个点读取有影响）

### 3、完成串口发送的一开始5个点和后续的8个点坐标（后续8个点坐标要进行排序！！！）

#### 

# 从机树莓派ver 1.0

### 1、完成了再黑色线框上分别识别绿色和红色

### 2、红色和绿色合在一起返回一个点的值

### 3、串口发送还没加

```python
# 导入OpenCV和NumPy库
import cv2
import numpy as np

# 初始化HSV色彩空间中绿色和红色的阈值范围
h_min_g_r = 0
h_max_g_r = 179
s_min_g_r = 0
s_max_g_r = 255
v_min_g_r = 175
v_max_g_r = 255

h_min_r = 0
h_max_r = 23
s_min_r = 0
s_max_r = 255
v_min_r = 134
v_max_r = 255

h_min_g = 62
h_max_g = 89
s_min_g = 109
s_max_g = 255
v_min_g = 136
v_max_g = 255

# 定义一个用于膨胀和侵蚀操作的结构元素
kernel = np.ones((4,4),np.uint8)

# 处理图像点，进行边缘检测和形态学操作
def deal_the_point(img,num):
    # 将BGR图像转换为灰度图像
    imgGray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # 使用高斯模糊减少图像噪声
    # 模糊处理 ksize 只能是奇数 ( ? , ? )
    imgBlur = cv2.GaussianBlur(imgGray, (7, 7), 0)
    # 应用Canny算法进行边缘检测
    # 边缘检测 canny
    imgCanny = cv2.Canny(imgBlur, 150, 200)
    # 对边缘图像进行膨胀操作，增强边缘
    # 膨胀处理 kernel 要一个全是1的矩阵 厚度
    imgDialation = cv2.dilate(imgCanny, kernel, iterations=1)
    # 对膨胀后的图像进行侵蚀操作，去除小的噪声点
    # 侵蚀处理 kernel 要一个全是1的矩阵 厚度
    imgEroded = cv2.erode(imgDialation, kernel, iterations=num)
    cv2.imshow("imgEroded",imgEroded)
    return imgEroded

# 初始化视频捕获设备
cap = cv2.VideoCapture(0)

# 无限循环，处理视频流中的每一帧
while True:
    # 读取当前帧
    img = cv2.imread('takephoto1.jpg')
    # 截取图像的指定区域
    img = img[100:460,100:460]
    # 复制当前帧用于绘制结果
    img_draw = img.copy()
    # 将BGR图像转换为HSV图像，用于色彩分割
    # red and green
    imgHSV_g_r = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    # 定义绿色和红色的HSV阈值范围
    lower_g_r = np.array([h_min_g_r, s_min_g_r, v_min_g_r])
    upper_g_r = np.array([h_max_g_r, s_max_g_r, v_max_g_r])
    # 根据阈值范围对HSV图像进行掩码处理，得到绿色和红色区域的掩码图像
    mask_g_r = cv2.inRange(imgHSV_g_r, lower_g_r, upper_g_r)
    # 使用掩码图像对原图像进行色彩分离，得到绿色和红色区域的图像
    find_red_and_green = cv2.bitwise_and(img, img, mask=mask_g_r)
    # 对分离出的绿色和红色区域图像进行灰度化、高斯模糊、Canny边缘检测、膨胀和侵蚀操作
    imgGray = cv2.cvtColor(find_red_and_green, cv2.COLOR_BGR2GRAY)
    imgBlur = cv2.GaussianBlur(imgGray, (7, 7), 0)
    imgCanny = cv2.Canny(imgBlur, 150, 200)
    imgDialation = cv2.dilate(imgCanny, kernel, iterations=1)
    imgEroded = cv2.erode(imgDialation, kernel, iterations=1)
    find_red_and_green_gray = imgEroded

    # 查找侵蚀后图像的轮廓
    contours, hierarchy = cv2.findContours(find_red_and_green_gray, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    # 如果找到两个轮廓
    # 计算每个轮廓的面积并输出
    if len(contours) == 2:
        for contour in contours:
            area = cv2.contourArea(contour)
            # 如果轮廓面积在指定范围内，绘制轮廓并计算轮廓中心
            if 10 < area < 150:
                cv2.drawContours(img_draw, [contour], -1, (0, 255, 0), 2)
                cv2.imshow("img_draw", img_draw)
                M = cv2.moments(contour)
                if M["m00"] != 0:  # 防止除以零错误
                    cX = int(M["m10"] / M["m00"])  # 中心X坐标
                    cY = int(M["m01"] / M["m00"])  # 中心Y坐标
                    print(f"{cX}, {cY}")
        print("Find two")

        # 对红色区域进行相同的处理
        # red
        imgHSV_r = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        lower_r = np.array([h_min_r, s_min_r, v_min_r])
        upper_r = np.array([h_max_r, s_max_r, v_max_r])
        mask_r = cv2.inRange(imgHSV_r, lower_r, upper_r)
        find_red = cv2.bitwise_and(img, img, mask=mask_r)
        find_red_final = cv2.bitwise_and(find_red, find_red_and_green, mask=mask_g_r)
        cv2.imshow("find_red_final", find_red_final)

        contours, hierarchy = cv2.findContours(deal_the_point(find_red_final,1), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        if len(contours) == 1:
            for contour in contours:
                area = cv2.contourArea(contour)
                if 10 < area < 150:
                    cv2.drawContours(img_draw, [contour], -1, (0, 255, 0), 2)
                    cv2.imshow("img_draw", img_draw)
                    M = cv2.moments(contour)
                    if M["m00"] != 0:  # 防止除以零错误
                        cX = int(M["m10"] / M["m00"])  # 中心X坐标
                        cY = int(M["m01"] / M["m00"])  # 中心Y坐标
                        print(f"Find red ({cX}, {cY})")

        # 对绿色区域进行相同的处理
        # green
        imgHSV_g = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        lower_g = np.array([h_min_g, s_min_g, v_min_g])
        upper_g = np.array([h_max_g, s_max_g, v_max_g])
        mask_g = cv2.inRange(imgHSV_g, lower_g, upper_g)
        find_green = cv2.bitwise_and(img, img, mask=mask_g)
        find_green_final = cv2.bitwise_and(find_green, find_red_and_green, mask=mask_g_r)
        cv2.imshow("find_green", find_green_final)

        contours, hierarchy = cv2.findContours(deal_the_point(find_green_final,2), cv2.RETR_EXTERNAL,
                                               cv2.CHAIN_APPROX_SIMPLE)
        if len(contours) == 1:
            for contour in contours:
                area = cv2.contourArea(contour)
                if 10 < area < 150:
                    cv2.drawContours(img_draw, [contour], -1, (0, 255, 0), 2)
                    cv2.imshow("img_draw", img_draw)
                    M = cv2.moments(contour)
                    if M["m00"] != 0:  # 防止除以零错误
                        cX = int(M["m10"] / M["m00"])  # 中心X坐标
                        cY = int(M["m01"] / M["m00"])  # 中心Y坐标
            print(f"Find green ({cX}, {cY})")

    # 如果只找到一个轮廓
    elif len(contours) == 1:
        for contour in contours:
            area = cv2.contourArea(contour)
            if 10 < area < 150:
                cv2.drawContours(img_draw, [contour], -1, (0, 255, 0), 2)
                cv2.imshow("img_draw", img_draw)
                M = cv2.moments(contour)
                if M["m00"] != 0:  # 防止除以零错误
                    cX = int(M["m10"] / M["m00"])  # 中心X坐标
                    cY = int(M["m01"] / M["m00"])  # 中心Y坐标
                    print(f"Contour Area: {area} pixels, Center Point: ({cX}, {cY})")
        print("Find one")

    # 如果没有找到轮廓
    else:
        print("Find None")

    # 等待按键事件，延迟1毫秒
    cv2.waitKey(1)
```

