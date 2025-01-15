# Find_4_Points 配置+程序

## 一.按键key

参考赵延昇的GPIO配置

<img src="https://z1r343l-001.obs.cn-north-4.myhuaweicloud.com/ws_C301/image-20240627123308641.png" alt="image-20240627123308641" style="zoom:50%;" />

## 二.HSV红色激光色域查找

参考程序：[3小时学习opencv](https://www.bilibili.com/video/BV1iV411k7SN/) 已放在文档中（ColorPicker.py）

```
myColor = [146, 89, 133, 179, 255, 255]#色域HSV
myColorValue = (51, 153, 255)#画点的色域
```

## 三.四个坐标点的查找

```python
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
```

## 四.四边形图像中点的坐标

```python
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
```

