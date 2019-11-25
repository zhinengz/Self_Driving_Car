
## Writeup Template

### You can use this file as a template for your writeup if you want to submit it as a markdown file, but feel free to use some other method and submit a pdf if you prefer.

---

**Advanced Lane Finding Project**

The goals / steps of this project are the following:

* Compute the camera calibration matrix and distortion coefficients given a set of chessboard images.
* Apply a distortion correction to raw images.
* Use color transforms, gradients, etc., to create a thresholded binary image.
* Apply a perspective transform to rectify binary image ("birds-eye view").
* Detect lane pixels and fit to find the lane boundary.
* Determine the curvature of the lane and vehicle position with respect to center.
* Warp the detected lane boundaries back onto the original image.
* Output visual display of the lane boundaries and numerical estimation of lane curvature and vehicle position.

[//]: # (Image References)

[image1_1]: ./camera_cal/calibration2.jpg "distorted"
[image12]: ./test_images/calibrated2.jpg "Undistorted"
[image2]: ./test_images/test1.jpg "Road Transformed"
[image3]: ./examples/binary_combo_example.jpg "Binary Example"
[image4]: ./examples/warped_straight_lines.jpg "Warp Example"
[image5]: ./examples/color_fit_lines.jpg "Fit Visual"
[image6]: ./examples/example_output.jpg "Output"
[video1]: ./project_video.mp4 "Video"

## [Rubric](https://review.udacity.com/#!/rubrics/571/view) Points

### Here I will consider the rubric points individually and describe how I addressed each point in my implementation.  

---

### Writeup / README

#### 1. Provide a Writeup / README that includes all the rubric points and how you addressed each one.  You can submit your writeup as markdown or pdf.  [Here](https://github.com/udacity/CarND-Advanced-Lane-Lines/blob/master/writeup_template.md) is a template writeup for this project you can use as a guide and a starting point.  

You're reading it!

### Camera Calibration

#### 1. Briefly state how you computed the camera matrix and distortion coefficients. Provide an example of a distortion corrected calibration image.

The code for this step is contained in the 2nd and 3rd cells of the notebook located in "Advaned_Lane_Finding.ipynb".  

In $2nd$ cell, two variables are difined: 'objpoints' and 'imgpoints'. 'objpoints' is defined with the shape of [9\*6, 3] based on the number of the chessboard corners in the given camera calibration images.
It is assumed that the chessboard is fixed on the (x, y) plane at z=0, therefore the object points are the same for each calibration image.  Thus, `objp` is just a replicated array of coordinates, and `objpoints` will be appended with a copy of it every time when it is successfully detected all chessboard corners in a test image.  `imgpoints` will be appended with the (x, y) pixel position of each of the corners in the tested image plane with each successful chessboard detection.  

In $3rd$ cell, the output `objpoints` and `imgpoints` are used to compute the camera calibration and distortion coefficients using the `cv2.calibrateCamera()` function. Then this distortion correction is applied to the test image using the `cv2.undistort()` function and obtained this result: 

<table><tr>
<td><figure>
    <img  src="./camera_cal/calibration2.jpg" alt="Drawing" style="width: 350px;"/>
    <figcaption>Distorted</figcaption>
    </figure></td>
<td><figure>
    <img  src="./output_images/calibrated2.jpg" alt="Drawing" style="width: 350px;"/>
    <figcaption>Un_Distorted</figcaption></figure></td>
</tr></table>


### Pipeline (single images)

#### 1. Provide an example of a distortion-corrected image.

To demonstrate this step, the same distortion correction is appled to one of the test images like this one:
<table><tr>
<td><figure>
    <img  src="./test_images/test6.jpg" alt="Drawing" style="width: 350px;"/>
    <figcaption>Distorted</figcaption>
    </figure></td>
<td><figure>
    <img  src="./output_images/calibrated_test6.jpg" alt="Drawing" style="width: 350px;"/>
    <figcaption>Un_Distorted</figcaption></figure></td>
</tr></table>

#### 2. Describe how (and identify where in your code) you performed a perspective transform and provide an example of a transformed image.

In $4th$ cell, the code for perspective transform is included: 
First the source points and destination points are defined as shown below:
```python
src = np.float32([[205,720],[565,470],[720,470],[1100,720]])
h, w = img.shape[:2]
dst = np.float32([[350, 720],[350, 0],[w-350, 0],[w-350, 720]])
```

This resulted in the following source and destination points:

| Source        | Destination   | 
|:-------------:|:-------------:| 
| 205, 720      | 350, 720        | 
| 565, 470      | 350, 0      |
| 720, 470     | 930, 0      |
| 1100, 720      | 930, 720        |

It is verified that the perspective transform was working as expected by drawing the `src` and `dst` points onto a test image and its warped counterpart to verify that the lines appear parallel in the warped image.

<table><tr>
<td><figure>
    <img  src="./output_images/source_points_drawn.jpg" alt="Drawing" style="width: 350px;"/>
    <figcaption>Test image</figcaption>
    </figure></td>
<td><figure>
    <img  src="./output_images/dest_points_drawn.jpg" alt="Drawing" style="width: 350px;"/>
    <figcaption>Warped image</figcaption></figure></td>
</tr></table>

#### 3. Describe how (and identify where in your code) you used color transforms, gradients or other methods to create a thresholded binary image.  Provide an example of a binary image result.

The color space been explored include:
* RGB: in $9th$ cell   
*** Red channel is chosen with threshold of 215/255
* HLS: in $10th$ cell   
*** S channel is chosen with threshold of 210/255
* HSV: in $11th$ cell    
*** V channel is chosen with threshold of 220/255
* The code for all three above color space are of the same template, therefore the code for LUV is ignored   
*** V channel in LUV is chosen with threshold of 155/255

* X Gradient is chosen with threshold of 14/100

The following table <font color=red>__$\checkmark$__</font> shows the channels which generate highest lane information

---------------------------------------------    
#### Observations of RGB color space:
| images  | Red(215,255)   | Green(195,255)   | Blue(60,210)   | Notes |
|-------- |--------------|--------------|--------------|
|straigh_line1.jpg| <font color=red>__$\checkmark$__</font>| <font color=red>__$\checkmark$__</font>|  | |
|straigh_line2.jpg| <font color=red>__$\checkmark$__</font>| <font color=red>__$\checkmark$__</font>|  | |
|test1.jpg| <font color=red>__$\checkmark$__</font>|  |  | bright picture|
|test2.jpg| <font color=red>__$\checkmark$__</font>| <font color=red>__$\checkmark$__</font>|  ||
|test3.jpg| <font color=red>__$\checkmark$__</font>| <font color=red>__$\checkmark$__</font>| ||
|test4.jpg| <font color=red>__$\checkmark$__</font>| |  |bright picture|
|test5.jpg|  |  |   | with shadow|
|test6.jpg| <font color=red>__$\checkmark$__</font>|  |   ||
<table><tr>
<td><figure>
    <figcaption>Warped Image</figcaption>
    <img  src="./output_images/warped_straight_lines1.jpg" alt="Drawing" style="width: 350px;"/>
    </figure></td>
<td><figure>
    <figcaption>Lanes info in Red channel</figcaption>
    <img  src="./output_images/straight_lines1R_RGB.jpg" alt="Drawing" style="width: 350px;"/>
</figure></td>
</tr></table>


---------------------------------------------
#### Observations of HLS color space:
| images  | H(14,85)   | L(120,255)   | S(210,255)   | Notes |
|-------- |--------------|--------------|--------------|
|straigh_line1.jpg| | <font color=red>__$\checkmark$__</font>| | |
|straigh_line2.jpg| | <font color=red>__$\checkmark$__</font>|  | |
|test1.jpg| | |<font color=red>__$\checkmark$__</font>| bright picture|
|test2.jpg| |<font color=red>__$\checkmark$__</font>|  ||
|test3.jpg| |<font color=red>__$\checkmark$__</font>| ||
|test4.jpg| | | <font color=red>__$\checkmark$__</font> |bright picture|
|test5.jpg| | | | with shadow|
|test6.jpg| |  <font color=red>__$\checkmark$__</font>|   ||
<table><tr>
<td><figure>
    <figcaption>Warped Image</figcaption>
    <img  src="./output_images/warped_straight_lines1.jpg" alt="Drawing" style="width: 350px;"/>
    </figure></td>
<td><figure>
    <figcaption>Lanes info in S/HLS channel</figcaption>
    <img  src="./output_images/straight_lines1S_HLS.jpg" alt="Drawing" style="width: 350px;"/>
</figure></td>
</tr></table>

----------------------------------------
#### Observations of HSV color space:
| images  | H(14,85)   | S(80,255)   | V(220,255)   | Notes |
|-------- |--------------|--------------|--------------|
|straigh_line1.jpg| ||<font color=red>__$\checkmark$__</font>| |
|straigh_line2.jpg| || <font color=red>__$\checkmark$__</font>| |
|test1.jpg| | | <font color=red>__$\checkmark$__</font>| bright picture|
|test2.jpg| | |<font color=red>__$\checkmark$__</font>|  ||
|test3.jpg| | |<font color=red>__$\checkmark$__</font>||
|test4.jpg| | | <font color=red>__$\checkmark$__</font> |bright picture|
|test5.jpg| | | | with shadow|
|test6.jpg| | | <font color=red>__$\checkmark$__</font>||
<table><tr>
<td><figure>
    <figcaption>Warped Image</figcaption>
    <img  src="./output_images/warped_straight_lines1.jpg" alt="Drawing" style="width: 350px;"/>
    </figure></td>
<td><figure>
    <figcaption>Lanes info in V/HSV channel</figcaption>
    <img  src="./output_images/straight_lines1V_HSV.jpg" alt="Drawing" style="width: 350px;"/>
</figure></td>
</tr></table>

----------------------------
#### Observations of gradient:
| images  | X(14,100)   | Y(0,255)   | ABS(0,255)   | Notes |
|-------- |--------------|--------------|--------------|
|straigh_line1.jpg| <font color=red>__$\checkmark$__</font>||| |
|straigh_line2.jpg| <font color=red>__$\checkmark$__</font>|| | |
|test1.jpg| | | | bright picture|
|test2.jpg|<font color=red>__$\checkmark$__</font> | ||  ||
|test3.jpg|<font color=red>__$\checkmark$__</font> | |||
|test4.jpg| | |  |bright picture|
|test5.jpg|<font color=red>__$\checkmark$__</font> | | | with shadow|
|test6.jpg| <font color=red>__$\checkmark$__</font>| | ||
<table><tr>
<td><figure>
    <figcaption>Warped Image</figcaption>
    <img  src="./output_images/warped_straight_lines1.jpg" alt="Drawing" style="width: 350px;"/>
    </figure></td>
<td><figure>
    <figcaption>Lanes info in X_gradient channel</figcaption>
    <img  src="./output_images/straight_lines1X_Gradient.jpg" alt="Drawing" style="width: 350px;"/>
</figure></td>
</tr></table>


### The final combined image is:
The final combined image is the sume of all above images, then set pixel value <=1 as 0
<figure>
    <figcaption>            Lanes info in final combined image</figcaption>
    <img  src="./output_images/straight_lines1_binary.jpg" alt="Drawing" style="width: 350px;"/>
</figure>




#### 4. Describe how (and identify where in your code) you identified lane-line pixels and fit their positions with a polynomial?

This step is done in cell $14$    
Function 'find_lane_pixels(binary_warped)' is to find pixels belonging to left lane and right lane.

The histogram of the first video frame is calculated, and the two highest points of the histogram are taken as the lane starting points.  

Two sliding windows are defined for both left and right lanes and been moved upward to search all potential lane pixels.   

The a 2nd-order polynomial lines are calculated to fit those lane pixels. 

<table><tr>
<td><figure>
    <figcaption>Original Image</figcaption>
    <img  src="./output_images/warped_test6.jpg" alt="Drawing" style="width: 350px;"/>
    </figure></td>
<td><figure>
    <figcaption>Curve_fit image</figcaption>
    <img  src="./output_images/curve_fit.png" alt="Drawing" style="width: 350px;"/>
</figure></td>
</tr></table>

#### 5. Describe how (and identify where in your code) you calculated the radius of curvature of the lane and the position of the vehicle with respect to center.

The curvatures are calculated in cell $14$
First define meters per pix in both x and y direction
```python
my_per_pix = 30/ 720
mx_per_pix = 3.7/ 700
```
Then find the 2nd order polynominal curve fitting for both left and right lanes
```python
left_fit = np.polyfit(lefty, leftx, 2)
right_fit = np.polyfit(righty, rightx, 2)
```

Then conver those coefficients 'left_fit' and 'right_fit', which have unit in pixels, to unit in 'meter'

```python
left_fit_cr_0 = mx_per_pix / my_per_pix**2 * left_fit[0]
left_fit_cr_1 = mx_per_pix / my_per_pix * left_fit[1]
right_fit_cr_0 = mx_per_pix / my_per_pix**2 * right_fit[0]
right_fit_cr_1 = mx_per_pix / my_per_pix * right_fit[1]    
```
The curvature is calculated as:
```python
left_curverad = ((1 + (2*left_fit_cr_0*y_eval*my_per_pix + left_fit_cr_1)**2)**1.5) / np.absolute(2*left_fit_cr_0)
right_curverad = ((1 + (2*right_fit_cr_0*y_eval*my_per_pix + right_fit_cr_1)**2)**1.5) / np.absolute(2*right_fit_cr_0)
```
based on the following equation:
\begin{equation*}
R_{curve} =\frac{(1+(2Ay+B)^2)^{\frac32})}{|2A|}
\end{equation*}

#### 6. Provide an example image of your result plotted back down onto the road such that the lane area is identified clearly.

I implemented this step in cell $15$.  Here is an example of my result on a test image:

<figure>
    <figcaption>Image with lane area</figcaption>
    <img  src="./output_images/lane_area.png" alt="Drawing" style="width: 350px;"/>
</figure>

---

### Pipeline (video)

#### 1. Provide a link to your final video output.  Your pipeline should perform reasonably well on the entire project video (wobbly lines are ok but no catastrophic failures that would cause the car to drive off the road!).

Here's a [link to my video result](./output_videos/final_video.mp4)

---

### Discussion

#### 1. Briefly discuss any problems / issues you faced in your implementation of this project.  Where will your pipeline likely fail?  What could you do to make it more robust?

If there is no clear histogram peaks in the first image frame, it will be hard to find these two starting points   
If there is no clear starting points for those frames in the middle of video, the ending point of the past frame can be saved and used as the starting point of future frames    
If there is no clear starting points for $several$ $consecutive$ frames, the lane pixels of past several frams can be save and a 2nd-order polynomial fitting can be made to predict the lane pixels for the future frames   
In this project, only X direction gradient is used. If there is a sharp turn, the combined (of x and y direction) could be better
  

