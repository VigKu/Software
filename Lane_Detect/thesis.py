# -*- coding: utf-8 -*-
"""
Created on Thu Feb 23 12:35:52 2017

@author: giray
"""
# algorithms getting from thesis below
# https://theses.lib.vt.edu/theses/available/etd-05262005-103758/unrestricted/bacha_thesis.pdf

import cv2
import numpy as np
import math

def nothing(x):
    pass

img = cv2.imread('igvc.jpg')
# channels
blue_img = img[:,:,0]
green_img = img[:,:,1]
red_img = img[:,:,2]
#cv2.imshow('blue_img', blue_img)
#cv2.imshow('green_img', green_img)
#cv2.imshow('red_img', red_img)

add_image = cv2.add(blue_img,blue_img)
subtract_image= cv2.subtract(green_img,add_image)
subtract_image_2= cv2.subtract(blue_img,subtract_image)

#cv2.imshow('add_image', add_image)
#cv2.imshow('subtract_image', subtract_image)
#cv2.imshow('subtract_image_2', subtract_image_2)

################
ret, threshold = cv2.threshold(subtract_image_2,180,255,cv2.THRESH_BINARY)
ret2, threshold2 = cv2.threshold(blue_img,180,255,cv2.THRESH_BINARY)
difference = cv2.subtract(threshold2,threshold)
#cv2.imshow('threshold', threshold)
#cv2.imshow('threshold2', threshold2)
#cv2.imshow('difference', difference)

h,w = blue_img.shape
cropped = threshold[h/4:h, 0:w]
cv2.imshow('cropped', cropped)

left_image = cropped[0:h, 0:w/2]
right_image = cropped[0:h, w/2:w]
cv2.imshow('left_image', left_image)
cv2.imshow('right_image', right_image)

edges_left = cv2.Canny(left_image,50,255)
edges_right = cv2.Canny(right_image,50,255)

lines_left = cv2.HoughLines(edges_left,1,np.pi/180,1)
lines_right = cv2.HoughLines(edges_right,1,np.pi/180,1)

if lines_left is not None:
    for rho1,theta1 in lines_left[0]:
        a = np.cos(theta1)
        b = np.sin(theta1)
        x0 = a*rho1
        y0 = b*rho1
        x1 = int(x0 + 1000*(-b))
        y1 = int(y0 + 1000*(a))
        x2 = int(x0 - 1000*(-b))
        y2 = int(y0 - 1000*(a))
        cv2.line(left_image,(x1,y1),(x2,y2),(100),4)

if lines_left is not None:
    for rho2,theta2 in lines_right[0]:
        a = np.cos(theta2)
        b = np.sin(theta2)
        x0 = a*rho2
        y0 = b*rho2
        x1 = int(x0 + 1000*(-b))
        y1 = int(y0 + 1000*(a))
        x2 = int(x0 - 1000*(-b))
        y2 = int(y0 - 1000*(a))

        cv2.line(right_image,(x1,y1),(x2,y2),(100),4)
        
cv2.imshow('edges_left', left_image)
cv2.imshow('edges_left2', right_image)
cv2.imshow('img', img)

cv2.waitKey(0)
cv2.destroyAllWindows()