# -*- coding: utf-8 -*-
"""
Created on Mon Dec 27 14:34:02 2021

@author: user
"""

import tensorflow as tf
import numpy as np
import keras
from keras.datasets import mnist
from keras import layers
from keras.utils import np_utils 
print(tf.__version__)

fashion_mnist = tf.keras.datasets.fashion_mnist


(train_images, train_labels), (test_images, test_labels) = mnist.load_data()

# Normalize the data
train_images = train_images.astype(np.float32) / 255
test_images = test_images.astype(np.float32) / 255

# define the iput shap and Classification output
num_classes = 10
input_shape = (784, 1)

#define the model shape 
model = tf.keras.Sequential([
        tf.keras.layers.Flatten(input_shape=(28, 28)),
        layers.Dense(256, activation="relu"),
        tf.keras.layers.Dropout(0.2),
        layers.Dense(512, activation="relu"),
        tf.keras.layers.Dropout(0.2),
        layers.Dense(num_classes, activation="softmax"),
    ]
)

model.summary()

#define the trainning detail
model.compile(optimizer='adam',
              loss='sparse_categorical_crossentropy',
              metrics=['accuracy'])
#start tranning
train_history = model.fit(train_images, train_labels, epochs=100, validation_split=0.2)

#test the mode
score = model.evaluate(test_images, test_labels, verbose=2)
print('Test loss:', score[0])
print('Test accuracy:', score[1])


#save the mode and convert to tflite to move on esp32cam
def representative_data_gen():
  for input_value in tf.data.Dataset.from_tensor_slices(train_images).batch(1).take(100):
    yield [input_value]

model.save("my_model")

converter = tf.compat.v1.lite.TFLiteConverter.from_saved_model('./my_model')
converter.optimizations = [tf.lite.Optimize.DEFAULT]

converter.representative_dataset = representative_data_gen
tflite_model = converter.convert()
tflite_model_size = open('./mnist_model.tflite', "wb").write(tflite_model)
print("Quantized model is %d bytes" % tflite_model_size)

