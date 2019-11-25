# **Traffic Sign Recognition** 


---

### The goals / steps of this project are the following:
* Load the data set (see below for links to the project data set)
* Explore, summarize and visualize the data set
* Design, train and test a model architecture
* Use the model to make predictions on new images
* Analyze the softmax probabilities of the new images
* Summarize the results with a written report


[//]: # (Image References)

[image1]: ./examples/visualization.jpg "Visualization"
[image2]: ./examples/rotated.jpg "Rotated"
[image3]: ./examples/random_noise.jpg "Random Noise"
[image4]: ./examples/placeholder.png "Traffic Signs"



### Data Set Summary & Exploration

#### 1. Basic summary of the data set. 

I used the pandas library to calculate summary statistics of the traffic
signs data set:

* The size of training set is ?        
    **The size of training set is 34799 **
* The size of the validation set is ?  
    **The size of validation set is 4410 **
* The size of test set is ?  
    **The size of test set is 12630 **
* The shape of a traffic sign image is ?  
    **The shape of a traffic sign image is (32, 32, 3) **
* The number of unique classes/labels in the data set is ?  
    **The number of unique classes is 43**

#### 2. Include an exploratory visualization of the dataset.

Here is an exploratory visualization of the data set. It is a bar chart showing how the data ...
It is obviously showing the data set is not balanced among those classes. Therefore, image augmentation is justified

![alt text][image1]

### Design and Test a Model Architecture

I decided to generate additional data because data set is not balanced among classes

To add more data to the the data set, I used the following techniques: **roation**, **adding noise** and **resize** the original image 

As a first step, I decided to roate image for those classes have smaller data samples. 

Here is an example of a traffic sign image before and after rotation.

![alt text][image2]


Here is an example of an original image and an augmented image after adding noise:

![alt text][image3]


As a last step, I normalized the image data using `(pixel-mean)/std`, while the other method of `(pixel - 128)/ 128` is prone to outlier effect


#### 2. Final model architecture:

My final model consisted of the following layers:

| Layer         		|     Description	        					| 
|:---------------------:|:---------------------------------------------:| 
| Input         		| 32x32x3 RGB image   							| 
| Convolution 5x5     	| 1x1 stride, valid padding, outputs 28x28x16 	|
| RELU					|												|
| Batch Normalization   |
| Max pooling	      	| 2x2 stride, valid padding outputs 14x14x16    |
| Convolution 5x5     	| 1x1 stride, valid padding, outputs 10x10x32 	|
| RELU					|												|
| Batch Normalization   |                                               |  
| Max pooling	      	| 2x2 stride, valid padding, outputs 5x5x32     |
| Fully connected		| outputs 800 x 120        						|
| RELU  				|         									    |
| Batch Normalization   |                                               |  
| Dropout               | 0.7                                           | 
| Fully connected		| outputs 120 x 84        			            |
| RELU  				|         								     	|
| Batch Normalization   |                                               |  
| Dropout               | 0.7                                           |  
| Fully connected		| outputs 84 x 43        		                |
|						|												|
 


#### 3. Train model. T

To train the model, I used Adam optimizer, the `batch_size` is set to 64 and number of `epochs` is 100.
Also the learning rate is set to decayed learning rate with `initial learning rate` as 0.001, and `decay_steps` = 35000, `decay_rate` as 0.1
```python
lr_rate = tf.train.exponential_decay(initial_learning_rate, global_step,
                                           decay_steps, decay_rate, staircase=True) 
```

#### 4. Describe the approach taken for finding a solution and getting the validation set accuracy to be at least 0.93. Include in the discussion the results on the training, validation and test sets and where in the code these were calculated. Your approach may have been an iterative process, in which case, outline the steps you took to get to the final solution and why you chose those steps. Perhaps your solution involved an already well known implementation or architecture. In this case, discuss why you think the architecture is suitable for the current problem.

My final model results were:
* training set accuracy of ?
**99.99%**
* validation set accuracy of ? 
**98.4%**
* test set accuracy of ?
**96.1%**

If an iterative approach was chosen:
* What was the first architecture that was tried and why was it chosen? 

  **The basic Lenet architecture without `batch normalization` and `dropout`**
  
* What were some problems with the initial architecture?

  **This basic Lenet architecutre can only achieve around 93% training accuracy**

* How was the architecture adjusted and why was it adjusted? Typical adjustments could include choosing a different model architecture, adding or taking away layers (pooling, dropout, convolution, etc), using an activation function or changing the activation function. One common justification for adjusting an architecture would be due to overfitting or underfitting. A high accuracy on the training set but low accuracy on the validation set indicates over fitting; a low accuracy on both sets indicates under fitting.

  **The following adjustments are tried to improve the accuracy**
  * Bactch Normalization
  * Maxpooling
  * Drop-out
  * Decayed learning rate
  * Regularization


* Which parameters were tuned? How were they adjusted and why?

  * **Learning rate and drop-out are tuned based on gap between trainning accuracy and validation accuracy**
  
* What are some of the important design choices and why were they chosen? For example, why might a convolution layer work well with this problem? How might a dropout layer help with creating a successful model?

  * **The most importand design choices is to: a) add batch normalization; b) add drop-out**. This two choices helped model generalization and improved both validation and test accuracy by more than 3%

If a well known architecture was chosen:
* What architecture was chosen?

  * ** The other advanced network, like Alex net and VGG net, has millions of parameters, which therefore needs much larger dataset to train from scratch, but still, good choice for transfer learning. Lenet is a reasonable network to learn from scratch and also gives resonable performance with the given size of training data**

* Why did you believe it would be relevant to the traffic sign application?
  * **Lenet has reasonable good performance for small image size and small number of classes**
* How does the final model's accuracy on the training, validation and test set provide evidence that the model is working well?
  * **After adding batch normalization and dropout, the achieved accuracy are all >93% for training,validationa and test data set**
 

### Test a Model on New Images

#### 1. Choose five German traffic signs found on the web and provide them in the report. For each image, discuss what quality or qualities might be difficult to classify.

Here are five German traffic signs that I found on the web:

![alt text][image4] 
The first image might be difficult to classify because ...

#### 2. Discuss the model's predictions on these new traffic signs and compare the results to predicting on the test set. At a minimum, discuss what the predictions were, the accuracy on these new predictions, and compare the accuracy to the accuracy on the test set (OPTIONAL: Discuss the results in more detail as described in the "Stand Out Suggestions" part of the rubric).

Here are the results of the prediction:

| Image			        |     Prediction	        					| 
|:---------------------:|:---------------------------------------------:| 
| Bumpy road      		| Bumpy road   									| 
| Chilren crossing		| Chilren crossing						|
| Road narrows in the right	| Bumpy road  						|
| No passing     		| No passing  				 				|
|Road work			| Road work     							|


The model was able to correctly guess 4 of the 5 traffic signs, which gives an accuracy of 80%. This compares favorably to the accuracy on the test set of ...

#### 3. Describe how certain the model is when predicting on each of the five new images by looking at the softmax probabilities for each prediction. Provide the top 5 softmax probabilities for each image along with the sign type of each probability. (OPTIONAL: as described in the "Stand Out Suggestions" part of the rubric, visualizations can also be provided such as bar charts)

The code for making predictions on my final model is located in the 11th cell of the Ipython notebook.

For the first image, the model is relatively sure that this is a Bumpy road  (probability of >0.99), and the image does road work sign. The top five soft max probabilities were

| Probability         	|     Prediction	        					| 
|:---------------------:|:---------------------------------------------:| 
| 1.0         			| Bumpy road 								| 
| 1.0     				|  wild anaimal crossing 										|
| .86					| Road narrows in the right										|
| 1.0	      			| No passing					 				|
| .99			    |Road work    							|


For the second image ... 

### (Optional) Visualizing the Neural Network (See Step 4 of the Ipython notebook for more details)
#### 1. Discuss the visual output of your trained network's feature maps. What characteristics did the neural network use to make classifications?


