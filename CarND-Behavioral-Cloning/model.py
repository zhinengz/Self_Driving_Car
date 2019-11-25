import csv
import cv2
import numpy as np
import os
import sklearn
from math import ceil
from random import shuffle
#read in image file path and steering values
lines = []
with open('./data/driving_log.csv') as csvfile:
    reader = csv.reader(csvfile)
    next(reader, None)
    for line in reader:
        lines.append(line)

#split data into training set and validation set
from sklearn.model_selection import train_test_split
train_images, valid_images = train_test_split(lines, test_size=0.2)

# define generator to process image on-fly    
def generator(images, batch_size=32):
    num_images = len(images)
    num_images -= num_images % batch_size
    while 1:
        shuffle(images)
        for offset in range(0,num_images,batch_size):
            batch_images_name = images[offset:offset + batch_size]
            batch_images = []
            batch_angles = []
            for line in batch_images_name:
                steering_center = float(line[3])
                correction = 0.25
                #using data from all three cameras
                for i in range(3):
                    source_path = line[i]
                    filename = source_path.split('/')[-1]
                    current_path = './data/IMG/' + filename
                    image = cv2.imread(current_path)
                    batch_images.append(image)
                    angle = steering_center
                    if i==1: #if data from left camera add correction
                        angle +=correction
                    if i==2: #if data from right camera add minus correction
                        angle -=correction
                    batch_angles.append(angle)
                    #flip image to augment data
                    batch_images.append(cv2.flip(image,1))
                    batch_angles.append(angle*(-1.0))

            X_train = np.array(batch_images)
            y_train = np.array(batch_angles)
            yield sklearn.utils.shuffle(X_train, y_train)


from keras.models import Sequential
from keras.layers import Flatten, Dense, Lambda, Cropping2D,Dropout
from keras.layers.convolutional import Convolution2D
from keras.layers.pooling import MaxPooling2D

batch_size = 16
train_generator = generator(train_images, batch_size=batch_size)
validation_generator = generator(valid_images, batch_size = batch_size)
#define model
model = Sequential()
model.add(Lambda(lambda x: x/255.0 - 0.5,input_shape=(160,320,3)))
model.add(Cropping2D(cropping=((70,25),(0,0))))
model.add(Convolution2D(24,5,5,subsample=(2,2),activation='relu'))
model.add(Convolution2D(36,5,5,subsample=(2,2),activation='relu'))
model.add(Convolution2D(48,5,5,subsample=(2,2),activation='relu'))
model.add(Convolution2D(64,3,3,activation='relu'))
model.add(Convolution2D(64,3,3,activation='relu'))

model.add(Flatten())
model.add(Dense(100))
model.add(Dropout(0.5))
model.add(Dense(50))
model.add(Dropout(0.2))
model.add(Dense(10))
model.add(Dense(1))

model.compile(loss='mse', optimizer='adam')
#model.fit(X_train, y_train, validation_split=0.2,shuffle=True,epochs=5)
model.fit_generator(train_generator,\
        steps_per_epoch=ceil(len(train_images)/batch_size),\
        validation_data = validation_generator,\
        validation_steps = ceil(len(valid_images)/batch_size),\
        epochs = 6, verbose =2)    
model.save('model.h5')







