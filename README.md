# YOLO modifications

1. [(Intro) What is Darknet project?](#intro_darknet)
2. [(Intro) What is YOLO](#intro_yolo)
3. [My modifications](#my_modifications)  
   [- Testing multiple images](#test_multiple_imgs)
## Darknet <a id="intro_darknet"></a>

<p> 
  <img src="http://pjreddie.com/media/files/darknet-black-small.png" width="120" height="120" align="right">
  Darknet is an open source neural network framework written in C and CUDA. It is fast, easy to install, and supports CPU and GPU computation.
  
  For more information see the <a href="http://pjreddie.com/darknet">Darknet project website</a>.
  
  For questions or issues please use the <a href="https://groups.google.com/forum/#!forum/darknet">Google Group</a>
  
</p>

## YOLO <a id="intro_yolo"></a>

<p>
  <img src="https://pjreddie.com/media/image/yologo_1.png" width="133" height="71" align="right"> 

<b>YOLO</b> (You Only <strike>Live</strike> Look Once) is a real-time object detection and classification that obtained excellent results on the Pascal VOC dataset. 

So far, YOLO has two versions: <b>YOLO V1</b> and YOLO V2, also refered as <b>Yolo 9000</b>. Click on the image below to watch YOLO 9000's promo video.
</p>


<!--- Yolo's link for you tube --->
<p align="center">
<a href="http://www.youtube.com/watch?feature=player_embedded&v=VOC3huqHrss"><img src="https://github.com/rafaelpadilla/Detecting-lost-objects-with-Deep-Learning/blob/master/images/yolo_youtube.jpg" width="427" height="240" align="center"/></a>
</p>

The authors have created a website explaining how it works, how to use it and how to train yolo with your images. Check the references below: 

YOLO: **You Only Look Once: Unified, Readl-Time Object Detection** (2016)  
(Joseph Redmon, Santosh Divvala, Ross Girshick, Ali Farhadi)  
	[[site](https://arxiv.org/abs/1506.02640)] 
	[[pdf](https://arxiv.org/pdf/1506.02640)] 
	[[slides](https://docs.google.com/presentation/d/1kAa7NOamBt4calBU9iHgT8a86RRHz9Yz2oh4-GTdX6M/edit#slide=id.p)] 
	[[talk](https://www.youtube.com/watch?v=NM6lrxy0bxs)] 
	[[ted talk](https://www.youtube.com/watch?v=Cgxsv1riJhI)] 
	
**YOLO9000: Better, Faster, Stronger** (2017)  
(Joseph Redmon, Ali Farhadi)  
	[[site](https://arxiv.org/abs/1612.08242)] 
	[[pdf](https://arxiv.org/pdf/1612.08242)] 
	[[talk](https://www.youtube.com/watch?v=GBu2jofRJtk)] 
	[[slides](https://docs.google.com/presentation/d/14qBAiyhMOFl_wZW4dA1CkixgXwf0zKGbpw_0oHK8yEM/edit?usp=sharing)] 
	
**YOLO: People talking about it**  
	[[Andrew NG](https://www.youtube.com/watch?v=9s_FpMpdYW8)] 
	[[Siraj Raval](https://www.youtube.com/watch?v=4eIBisqx9_g)] 

**YOLO: People writing about it (Explanations and codes)**  
	[[Towards data science](https://towardsdatascience.com/yolo-you-only-look-once-real-time-object-detection-explained-492dc9230006)]: A brief summary about yolo and how it works.  
	[[Machine Think blog](http://machinethink.net/blog/object-detection-with-yolo/)]: A brief summary about yolo and how it works.  
	[[Timebutt's github](https://timebutt.github.io/static/how-to-train-yolov2-to-detect-custom-objects/)]: A tutorial explaing how to train yolo 9000 to detect a single class object.  
	[[Timebutt's github](https://timebutt.github.io/static/understanding-yolov2-training-output/)]: Read this if you want to understand yolo's training output.  
	[[Cvjena's github](https://github.com/cvjena/darknet/blob/master/cfg/yolo.cfg)]: Comments of some of the tags used in the cfg files.  
	[[Guanghan Ning's blog](http://guanghan.info/blog/en/my-works/train-yolo/)]: A tutorial explaining how to train yolo v1 with your own data. The author used two classes (yield and stop signs).  
	[[AlexeyAB's github](https://github.com/AlexeyAB/)]: Very good project forked from yolo 9000 supporting Windows and Linux.  
  [[Google's Group](https://groups.google.com/forum/#!forum/darknet)]: Excellent source of information. People ask and answer doubts about darknet and yolo.  
	[[Guanghan Ning's blog](http://guanghan.info/blog/en/my-works/yolo-cpu-running-time-reduction-basic-knowledge-and-strategies)]: Studies and analysis on reducing the running time of Yolo on CPU.  
	[[Guanghan Ning's blog](http://guanghan.info/projects/ROLO/)]: Recurrent YOLO. This is an interesting work mixing recurrent network and yolo for object tracking.  


## My modifications: <a id="my_modifications"></a>

Recently I have forked the official darket project and modified it to attend my demands. Below you can find some additional functions I added to the original project.  

All the examples can be easily run. You just need to clone or download this repository, compile and run the commands :)  

#### Testing multiple images  <a id="test_multiple_imgs"></a>

Let's say you want to **detect objects in a single or multiple images** given a network structure and your weights file. Using this function you can also choose to **visualize the results** (images with bounding boxes) or save your results. **You can save the detections (bounding boxes and classes) in .txt files** and also save the **resulting images**.

Another good thing is that you don't need to pass the arguments in a specific order anyomore. **This function makes the work easier by accepting the arguments in any order you want.**

See the example below to detect **multiple images**:  

 ```./darknet testimages newdata/voc.data newdata/yolo-voc.2.0.cfg newdata/yolo-voc_final.weights -savetxt -saveimg```
 
 Arguments:  
 
 + `newdata/voc.data`: the path for your voc.data file. Your voc.data file must contain the following tags:  
    - `names`: the path to the file containing a list of classes' names.  
    - `test`: the path to the text file containing a list of images to be tested.  
    - `results`: the path to the folder where your results will be saved.  
    See [here](https://github.com/rafaelpadilla/darknet/blob/master/newdata/voc.data) an example of the `voc.data` file.  
 + `-savetxt`: this is an **optional** argument. If you add this argument, a text file will be created for each image containing the bounding boxes and classes detected. It will be saved in the _results_ folder specified in the _voc.data_ file.  
 + `-saveimg`: this is also an **optional** argument. With this argument, the resulting images with the detected objects will be saved in the _results_ folder specified in the _voc.data_ file.  
 + `-threshold`: also **optional**. The default value is 0.24. Only bouding boxes with higher or equal confidence will be considered.  

The output detections will be seen as:  

<!--- Showing example of output --->
<div style="text-align:center">
<p align="center">
  <img src="https://github.com/rafaelpadilla/DeepLearning-VDAO/blob/master/images/aux_images/output.jpg" width="320"/> </p>
</div>

If you add the `-saveimg` and `-savetxt` arguments, the results (_\_dets.txt_ and .png files) will be created in the `results` folder specified in your `newdata/voc.data` file as seen below:  

<!--- Showing results --->
<div style="text-align:center">
<p align="center">
  <img src="https://github.com/rafaelpadilla/DeepLearning-VDAO/blob/master/images/aux_images/output_list.jpg" width="300" > </p>
</div>

See below the content of an image and its corresponding txt file:

<!--- Showing results --->
<div style="text-align:center">
<p align="center">
  <img src="https://github.com/rafaelpadilla/DeepLearning-VDAO/blob/master/images/aux_images/result.jpg" width="470" /> </p>
</div>

Each line of the _\_dets.txt_ file represent a bounding box. The values representing a bounding box are: ```id relative_center_x relative_center_y relative_width relative height```. The ```id``` represents the class order of the detected object that appears in the `names` tag in your `newdata/voc.data` file.  
 
But if you want to apply the detector to a single image, you add the argument `-img` followed by the image's path as shown in the example below:

```./darknet testimages newdata/voc.data newdata/yolo-voc.2.0.cfg newdata/yolo-voc_final.weights -img newdata/images/000058.jpg -savetxt -saveimg```  

Add the argument `-showimg` if you want to visualize the resulting images as soon as the detector evaluates them. (Note: this feature requires openCV compilation. To do so, change the 3rd line of the Makefile to `OPENCV=1` and recompile it). Example:

```./darknet testimages newdata/voc.data newdata/yolo-voc.2.0.cfg newdata/yolo-voc_final.weights -img newdata/images/000058.jpg -showimg```

Remeber that the order of the arguments (file.data, network.cfg, file.weights, etc) do not have to follow this exact order. You can make specify then in any order you want. :) 
