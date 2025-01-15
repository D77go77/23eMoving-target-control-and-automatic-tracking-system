"""
计算矩形的中心点坐标。
参数:
points: 一个包含四个点坐标的列表，每个点坐标为一个二元组(x, y)。
返回:
一个二元组，表示矩形的中心点坐标。
"""

def get_rectangle_center(points):
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