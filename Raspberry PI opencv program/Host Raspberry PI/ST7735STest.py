# coding : UTF-8
import time #用于计算spi刷新整个屏幕所用时长
import RPi.GPIO as GPIO #用于操作引脚
import spidev #树莓派与屏幕的交互协议为SPI，说明见：https://github.com/doceme/py-spidev
from PIL import Image, ImageFont, ImageDraw #用于创建画布，或者读取具体路径下的图片。给图片添加文字。

import cv2
import numpy as np
from FL import findLaser
from FM import get_rectangle_center
from FS import getContours

#######FIND_SMALL#######
img_x = 110
img_y = 80
img_width = 50 + 500
img_height = 50 + 450
approx = []
########################

screenWidth = 160 #屏幕长度
screenHeight = 128 #屏幕宽度
PinDC = 29 #GPIO.BOARD引脚模式，第29号引脚
PinReset = 16  #GPIO.BOARD引脚模式，第16号引脚

def hardReset(): #重置电平时序
    GPIO.output(PinReset, 0)
    time.sleep(.2)
    GPIO.output(PinReset, 1)
    time.sleep(.5)

def sendCommand(command, *bytes): #发送指令（DC为低电平）和数据（DC为高电平）
    GPIO.output(PinDC, 0)
    spi.writebytes([command])
    if len(bytes) > 0:
        GPIO.output(PinDC, 1)
        spi.writebytes(list(bytes))

def reset(): #屏幕初始化
    sendCommand(0x11);
    sendCommand(0x26, 0x04);  # Set Default Gamma
    sendCommand(0xB1, 0x0e, 0x10);  # Set Frame Rate
    sendCommand(0xC0, 0x08, 0x00);  # Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    sendCommand(0xC1, 0x05);  # Set BT[2:0] for AVDD & VCL & VGH & VGL
    sendCommand(0xC5, 0x38, 0x40);  # Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    sendCommand(0x3a, 0x05);  # Set Color Format
    sendCommand(0x36, 0xc8);  # RGB
    sendCommand(0x2A, 0x00, 0x00, 0x00, 0x7F);  # Set Column Address
    sendCommand(0x2B, 0x00, 0x00, 0x00, 0x9F);  # Set Page Address
    sendCommand(0xB4, 0x00);
    sendCommand(0xf2, 0x01);  # Enable Gamma bit
    sendCommand(0xE0, 0x3f, 0x22, 0x20, 0x30, 0x29, 0x0c, 0x4e, 0xb7, 0x3c, 0x19, 0x22, 0x1e, 0x02, 0x01, 0x00);
    sendCommand(0xE1, 0x00, 0x1b, 0x1f, 0x0f, 0x16, 0x13, 0x31, 0x84, 0x43, 0x06, 0x1d, 0x21, 0x3d, 0x3e, 0x3f);
    sendCommand(0x29);  # Display On
    sendCommand(0x2C);

def sendManyBytes(bytes): #发送屏幕数据
    GPIO.output(PinDC, 1)
    spi.writebytes(bytes)

def drawImg(img160x128): #入参为160x128像素的image对象
    picReadStartTime = time.time()
    bytes = []
    i = 0  
    for x in range(0, screenWidth):
        for y in range(screenHeight - 1, -1, -1):
            colorValue = img160x128.getpixel((x, y))
            red = colorValue[0]
            green = colorValue[1]
            blue = colorValue[2]
            red = red >> 3;  # st7735s的红色占5位
            green = green >> 2;  # st7735s的绿色占6位
            blue = blue >> 3;  # st7735s的蓝色占5位
            highBit = 0 | (blue << 3) | (green >> 3);  # 每个像素写入个字节，highBit高字节，lowBit低字节
            lowBit = 0 | (green << 5) | red;
            bytes.append(highBit)
            bytes.append(lowBit)
    picReadTimeConsuming = time.time() - picReadStartTime
    startTime = time.time()
    
    # screenWidth*screenHeight*2 每个像素写入个字节。以下for循环是为了控制每次传入的数组长度，防止这个报错,：OverflowError: Argument list size exceeds 4096 bytes.
    for j in range(2000, screenWidth * screenHeight * 2, 2000):  
        sendManyBytes(bytes[i:j])
        i = i + 2000
    sendManyBytes(bytes[i:screenWidth * screenHeight * 2])
    SpiTimeConsuming = time.time() - startTime
    print("picReadTimeConsuming = %.3fs , SpiTimeConsuming = %.3fs" % (picReadTimeConsuming, SpiTimeConsuming))

GPIO.setmode(GPIO.BOARD)
GPIO.setwarnings(False)
GPIO.setup(PinDC, GPIO.OUT)
GPIO.setup(PinReset, GPIO.OUT)

###

GPIO.setup(7, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(11, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(13, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(15, GPIO.IN, pull_up_down=GPIO.PUD_UP)

get_key = 0
###

S7 = 1
S6 = 2
S5 = 3
S4 = 4

def key_control():
    global get_key
    if GPIO.input(7) == 0:
        time.sleep(.20)
        if GPIO.input(7) == 1:
            get_key = S7
    if GPIO.input(11) == 0:
        time.sleep(.20)
        if GPIO.input(11) == 1:
            get_key = S6
    if GPIO.input(13) == 0:
        time.sleep(.20)
        if GPIO.input(13) == 1:
            get_key = S5
    if GPIO.input(15) == 0:
        time.sleep(.20)
        if GPIO.input(15) == 1:
            get_key = S4
###

spi = spidev.SpiDev() #https://github.com/doceme/py-spidev
spi.open(0, 0) 
spi.max_speed_hz = 24000000 #通信时钟最大频率
spi.mode = 0x00 #SPI的模式，ST7735S为模式0，可以参看我这篇内容：
hardReset()
reset()

key = 0

cap = cv2.VideoCapture(0)

trggle = 0
STATE = 0

red_and_green_find_state = 0
red_or_green_find_state_ok = 0
##
rL_H = 0
rL_S = 60
rL_V = 60
redLow = [rL_H, rL_S, rL_V]
rH_H = 6
rH_S = 255
rH_V = 255
redHigh = [rH_H, rH_S, rH_V]
##
gL_H = 35
gL_S = 43
gL_V = 35
greenLow = [gL_H, gL_S, gL_V]
gH_H = 90
gH_S = 255
gH_V = 255
greenHigh = [gH_H, rH_S, gH_V]
#####
big_ret_4_point = [[None, None] for _ in range(4)]
point_4_ok = 0
#####
find_small_res = 0
#####
##
show_scan = 1
##
image = Image.new('RGB', (160, 128))
drawImg(image)

def key_deal():
    global get_key,trggle,red_and_green_find_state,red_or_green_find_state_ok,STATE,find_small_res
    key_control()
    if get_key == S7:# watch the video
        trggle = not trggle
        cv2.destroyAllWindows()
    elif get_key == S6 and not trggle:# HSV State
        red_and_green_find_state += 1
        if red_and_green_find_state > 5:
            red_and_green_find_state = 0
        cv2.destroyAllWindows()
    elif get_key == S5 and not trggle:# HSV Ok
        if STATE == 0:
            red_or_green_find_state_ok = 1
        elif STATE == 1:
            red_or_green_find_state_ok = 1
        elif STATE == 4:
            find_small_res = 1
    elif get_key == S4 and not trggle:# NEXT STATE
        STATE += 1
        if STATE > 5:
            STATE = 0
        cv2.destroyAllWindows()
    get_key = 0

while True:
    key_deal()   
    
    if trggle == 1:
        success, img = cap.read()
        cv2.imwrite("img_now.jpg",img)
        image = Image.open("img_now.jpg")
        image = image.convert('RGBA')
        image = image.resize((160, 128))  #也可以从地址读取图片文件，并缩放为160x128
        draw = ImageDraw.Draw(image)
        
        setFont = ImageFont.load_default()
        draw.text((60, 20), "Show video", font=setFont, fill=255, direction=None)
        drawImg(image)
        show_scan = 1
        cv2.waitKey(1)
        print("True")
    
    else:
        image = Image.new('RGB', (160, 128))
        setFont = ImageFont.load_default()
        draw = ImageDraw.Draw(image)
        
        if STATE == 0:
            if red_and_green_find_state == 0:
                #####
                ret, img = cap.read()
                _, _,x2,y2=findLaser(img,0,redLow,redHigh,None,None)
                cv2.waitKey(1)
                draw.text((10, 30), f"  Red Laser ({x2},{y2})", font=setFont, fill=255, direction=None)
                #####
                draw.text((60, 0), "HSV Red", font=setFont, fill=255, direction=None)
                if red_or_green_find_state_ok == 1:
                    # renew the range of the HSV
                    draw.text((60, 20), "RENEW", font=setFont, fill=255, direction=None)
                    # adjust the HSV
                    rL_H = 0
                    rL_S = 65
                    rL_V = 65
                    #
                    rH_H = 6
                    rH_S = 255
                    rH_V = 255
                    redLow = [rL_H, rL_S, rL_V]
                    redHigh = [rH_H, rH_S, rH_V]
                    red_or_green_find_state_ok = 0
                # show the renew value
                draw.text((20, 50), f"redLow:  {redLow}", font=setFont, fill=255, direction=None)
                draw.text((20, 90), f"redHigh: {redHigh}", font=setFont, fill=255, direction=None)
                drawImg(image)
                show_scan = 1
            elif red_and_green_find_state == 1:
                #####
                ret, img = cap.read()
                x1, y1,_,_=findLaser(img,1,None,None,greenLow,greenHigh)
                cv2.waitKey(1)
                draw.text((10, 30), f"Green Laser ({x1},{y1})", font=setFont, fill=255, direction=None)
                #####
                draw.text((60, 0), "HSV Green", font=setFont, fill=255, direction=None)
                if red_or_green_find_state_ok == 1:
                    # renew the range of the HSV
                    draw.text((60, 20), "RENEW", font=setFont, fill=255, direction=None)
                    # adjust the HSV
                    gL_H = 0
                    gL_S = 0
                    gL_V = 0
                    #
                    gH_H = 0
                    gH_S = 0
                    gH_V = 0
                    greenLow = [gL_H, gL_S, gL_V]
                    greenHigh = [gH_H, gH_S, gH_V]
                    red_or_green_find_state_ok = 0
                # show the renew value
                draw.text((10, 50), f"greenLow:  {greenLow}", font=setFont, fill=255, direction=None)
                draw.text((10, 90), f"greenHigh: {greenHigh}", font=setFont, fill=255, direction=None)
                
            elif red_and_green_find_state == 2:
                draw.text((40, 0), "HSV Red and Green", font=setFont, fill=255, direction=None)
                ret, img = cap.read()
                x1, y1,x2,y2=findLaser(img,2,redLow,redHigh,greenLow,greenHigh)
                cv2.waitKey(1)
                draw.text((10, 40), f"Green Laser ({x1},{y1})", font=setFont, fill=255, direction=None)
                draw.text((10, 30), f"Red Laser ({x2},{y2})", font=setFont, fill=255, direction=None)          
                
            elif red_and_green_find_state == 3:
                draw.text((20, 0), "HSV Red in black Rec", font=setFont, fill=255, direction=None)
                
            elif red_and_green_find_state == 4:
                draw.text((20, 0), "HSV Green in black Rec", font=setFont, fill=255, direction=None)
                
            elif red_and_green_find_state == 5:
                draw.text((1, 0), "HSV Red_Green in black Rec", font=setFont, fill=255, direction=None)
            drawImg(image)
            show_scan = 1
        elif STATE == 1:
            ret, img = cap.read()
            _, _,x2,y2=findLaser(img,0,redLow,redHigh,None,None)
            cv2.waitKey(1)
            
            if show_scan == 1:
                point_cnt = 0
                image = Image.new('RGB', (160, 128))
                setFont = ImageFont.load_default()
                draw = ImageDraw.Draw(image)
                draw.text((10, 0), "Find_Big_Rec", font=setFont, fill=255, direction=None)
                drawImg(image)
                show_scan = 0
            
            if red_or_green_find_state_ok == 1:
                image = Image.new('RGB', (160, 128))
                setFont = ImageFont.load_default()
                draw = ImageDraw.Draw(image)
                draw.text((10, 0), "Find_Big_Rec", font=setFont, fill=255, direction=None)
                
                big_ret_4_point[point_cnt][0]=x2
                big_ret_4_point[point_cnt][1]=y2
                
                for i in range(0,point_cnt+1):
                    draw.text((20, 10+i*10), f"point{i}: ({big_ret_4_point[i][0]},{big_ret_4_point[i][1]})", font=setFont, fill=255, direction=None)
            
                point_cnt+=1
                print(point_cnt)
                if point_cnt == 4:
                    point_4_ok = 1
                else:
                    point_4_ok = 0
                if point_cnt > 3:
                    point_cnt = 0
                drawImg(image)
                red_or_green_find_state_ok = 0
            print(f"Red:({x2},{y2})")
            
        elif STATE == 2:
            if point_4_ok == 1:
                STATE = 3
                show_scan = 1
                ####
                points = [[big_ret_4_point[0][0],big_ret_4_point[0][1]],
                          [big_ret_4_point[1][0],big_ret_4_point[1][1]],
                          [big_ret_4_point[2][0],big_ret_4_point[2][1]],
                          [big_ret_4_point[3][0],big_ret_4_point[3][1]]] 
                center_of_rectangle= get_rectangle_center(points)
                print(center_of_rectangle)
                ####
            else:
                STATE = 1
                show_scan = 1
                big_ret_4_point = [[None, None] for _ in range(4)]
                point_cnt = 0
        
        elif STATE == 3:
             if show_scan == 1:
                point_cnt = 0
                image = Image.new('RGB', (160, 128))
                setFont = ImageFont.load_default()
                draw = ImageDraw.Draw(image)
                draw.text((20, 0), "Task  1", font=setFont, fill=255, direction=None)
                draw.text((30, 10), f"Middle: {center_of_rectangle}", font=setFont, fill=255, direction=None)
                
                for i in range(0,4):
                    draw.text((30, 30+i*20), f"point{i}: ({big_ret_4_point[i][0]},{big_ret_4_point[i][1]})", font=setFont, fill=255, direction=None)
            
                drawImg(image)
                show_scan = 0
                
        elif STATE == 4:
            if show_scan == 0:
                point_cnt = 0
                image = Image.new('RGB', (160, 128))
                setFont = ImageFont.load_default()
                draw = ImageDraw.Draw(image)
                draw.text((10, 0), "Task  2", font=setFont, fill=255, direction=None)
                drawImg(image)
                ## find small rec
                ret, img = cap.read()
                img = img[img_x + 9:img_x + 333, img_y + 73:img_y + 383].copy()
                imgContour = img.copy()
                approx = getContours(img)
                print(approx)
                cap.release()
                show_scan = 1
            
            if find_small_res == 1:
                cv2.destroyAllWindows()
                cap = cv2.VideoCapture(0)
                ret, img = cap.read()
                img = img[img_x + 9:img_x + 333, img_y + 73:img_y + 383].copy()
                imgContour = img.copy()
                approx = getContours(img)
                print(approx)
                find_small_res = 0
                cap.release()
            cv2.waitKey(1)
             
                
        elif STATE == 5:
             if show_scan == 1:
                cap = cv2.VideoCapture(0)
                ret, img = cap.read()
                point_cnt = 0
                image = Image.new('RGB', (160, 128))
                setFont = ImageFont.load_default()
                draw = ImageDraw.Draw(image)
                draw.text((10, 0), "Task  3", font=setFont, fill=255, direction=None)
                drawImg(image)
                show_scan = 0
                
            
        
        