# -*- coding: utf-8 -*-
"""
Created on Tue Dec 25 19:37:08 2018

@author: niuniumaomao
"""

import cv2
import numpy as np
import random
import matplotlib.pyplot as plt
import matplotlib.image as mpimg

def rotate_img(img):
#    print('img to roate',img.shape)
#    img_rotated = np.copy(img)
    random_degree = random.uniform(-20, 20)
    n_rows, n_cols = img.shape[:2]
#    mean = np.mean(img, axis = (0,1))
    rotation_M = cv2.getRotationMatrix2D((n_cols/2, n_rows/2),random_degree, 1) 
    img_rotated = cv2.warpAffine(img.astype(np.float32), rotation_M, (n_cols, n_rows), borderMode=cv2.BORDER_DEFAULT)#,borderValue=list(mean))
    img_rotated = img_rotated.astype(np.uint8)
    #if len(img_rotated.shape)==2:
    #    img_rotated = img_rotated[:,:,None]
    return img_rotated
    


def add_noise(img):
    shp = img.shape
    noise = np.random.randint(low=-35,high=35,size=shp, dtype='int8')
    img_noise = np.clip(img + noise,0,255)
    #if len(img_noise.shape)==2:
    #    img_noise = img_noise[:,:,None]      
    return img_noise


def resize_img(img):
    n_rows, n_cols = img.shape[:2]
    chg = np.random.randint(-6,6)
    mean = np.int_(np.mean(img, axis = (0,1)))
    img_resized = np.ones_like(img) * mean
    
    img_tmp = cv2.resize(img.astype(np.float32), (n_cols + chg, n_rows + chg))
    img_tmp = img_tmp.astype(np.uint8)
#    print("img",img.shape,"img_tmp", img_tmp.shape, 'img_resized', img_resized.shape)

    if chg > 0:
        delta = chg // 2
        img_resized = img_tmp[delta: n_rows+delta,delta: n_cols+delta]
    else:
        delta1 = np.abs(chg) // 2
        delta2 = abs(chg + delta1)
        img_resized[delta1: n_rows-delta2,delta1: n_cols-delta2] = img_tmp
    #if len(img_resized.shape)==2:
    #    img_resized = img_resized[:,:,None]        
    return img_resized 


def brightness_img(img):

    if np.mean(img)<127:
        shadow = 127
        highlight = 255
    else:
        shadow = 0
        highlight = 128
    alpha = (highlight - shadow)/255
    gamma = shadow

    bright_img = cv2.addWeighted(img, alpha, img, 0, gamma)
    #if len(bright_img.shape)==2:
    #    bright_img = bright_img[:,:,None]
    return bright_img


def image_aug_pipeline(img):
    img_rotated = rotate_img(img) 
#    print('rotated', img_rotated.shape)
    
    img_resized = resize_img(img)
#    print('resized', img_resized.shape)
    #img_bright = brightness_img(img)
    
    img_noise = add_noise(img)
#    print('noise', img_noise.shape)
    
    return np.stack(
            (
             img_rotated,
             img_resized,
             #img_bright,
             img_noise,

             ),
            axis = 0)
    

if __name__ == "__main__":  
##    img = cv2.imread('stopsign.jpg')
    from glob import glob
    test_img = np.array(glob('test_images/*.jpg'))
#    test_img = np.array(glob('*.jpg'))
    
    length = len(test_img)
    fig,*axs = plt.subplots(length,4,figsize=(5,10))
    axs = np.ravel(axs).reshape(length,4)
    for i in range(length):
        img = mpimg.imread(test_img[i])    
#        img = cv2.cvtColor(img,cv2.COLOR_RGB2YUV)     
#        auged_imgs = image_aug_pipeline(img[:,:,0])        
        auged_imgs = image_aug_pipeline(img)
        print('auged_imgs',auged_imgs.shape)
        for j in range(len(auged_imgs)):    
            if len(auged_imgs[j].shape)<=2:
                axs[i,j].imshow( auged_imgs[j].squeeze(), cmap='gray')
            else:
                axs[i,j].imshow( auged_imgs[j])
                
            axs[i,j].axis('off')
