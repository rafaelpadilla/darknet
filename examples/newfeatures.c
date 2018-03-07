#include "darknet.h"
#include <stdio.h>
#include <dirent.h>
#include <errno.h>

void write_detections(image im, int num, float thresh, box *boxes, float **probs, char **names, image **alphabet, int classes, char *save_name);

void apply_detector(network *net, char **names, char *outputs_folder_im, char *outputs_folder_txt, char *image_name, image im, image sized, image **alphabet, float thresh, float hier_thresh, char *show_img);

/*Example:
./darknet testimages file.data network.cfg file.weights -savetxt -saveimg

The file.data must contain the tags:
names = path to your text file that lists the classes to be detected
results = folder where the output results (bouding boxes coordinates and images) will be stored

If you want to apply your detector for multiple images, you must add a tag "test" in the file.data specifying a text file containing a list of image paths that you want to apply your detector.

Although, if you want to apply the detector in a single image, you must add the argument -img followed by the image path you want to apply the detector.
example:
./darknet testimages file.data network.cfg file.weights -img my_images/image0001.jpg -savetxt -saveimg 

The arguments -savetxt and -saveimg are optionals.
If -savetxt is present, for each image a .txt file will be generated in the results folder containing the bounding boxes of the detected objects.
If -saveimg is present, an image will be generated and saved in the results folder.

Note that the order of the arguments (file.data, network.cfg, file.weights, etc) do not have to follow this exact order. You can make specify then in any order you want. :) 
*/
void test_images(int argc, char **argv)
{
    char *find_argument();
    float thresh = find_float_arg(argc, argv, "-thresh", .24);
    float hier_thresh = find_float_arg(argc, argv, "-hier", .5);
    char *weightsFile = find_argument(argc, argv, ".weights", 0);
    char *cfgFile = find_argument(argc, argv, ".cfg", 0);
    char *dataFile = find_argument(argc, argv, ".data", 0);
    //Optional args
    char *filename = find_arg_next(argc, argv, "-img", 0);
    char *saveImArg = find_argument(argc, argv, "-saveimg", 0);
    char *saveTxtArg = find_argument(argc, argv, "-savetxt", 0);
    char *show_img = find_argument(argc, argv, "-showimg", 0);

    printf("\nArguments:");
    printf("\ndataFile: %s", dataFile);
    printf("\ncfgFile: %s", cfgFile);
    printf("\nweightsFile: %s", weightsFile);
    printf("\nthresh: %f",thresh);
    printf("\nhier_thresh: %f", hier_thresh);
    printf("\nfilename: %s", filename);
    printf("\nsaveImArg: %s",saveImArg);
    printf("\nsaveTxtArg: %s",saveTxtArg);
    printf("\nshowimg: %s",show_img);

   //Validate input parameters
   if (!cfgFile){ printf("\nError: Missing .cfg mandatory argument!\n"); return; }
   if (!dataFile){ printf("\nError: Missing .data mandatory argument!\n"); return; }
   if (!weightsFile){ printf("\nError: Missing .weights mandatory argument!\n"); return; }
   list *options = read_data_cfg(dataFile);

   //Get path of the .name file containing labels
   char *names_file = option_find_str(options, "names", 0);
   if (!names_file) { printf("\nError: Tag 'names' not found in the file %s\n", dataFile); return; }
   printf("\n.names file: %s", names_file);
   //get labels
   char **names = get_labels(names_file);
   char *outputs_folderIm = option_find_str(options, "results", 0);
   char *outputs_folderTxt = option_find_str(options, "results", 0);

   // if argument '-saveim' or '-savetxt' was passed
   if (saveImArg || saveTxtArg) {
      //Check if result tag is in the .data file
      if (!outputs_folderIm) { printf("\nError: Tag 'results' not found in the file %s\n", dataFile); return; }
      //Check if folder to save outputs exists        
      DIR* dir = opendir(outputs_folderIm);
      if (dir) closedir(dir);
      else if (ENOENT == errno) { printf("\nError: Output directory %s does not exist!\n", outputs_folderIm); return; }
   }
   // if argument '-saveim' '-savetxt' was not passed, disconsider respective output folder
   if (saveImArg == 0) outputs_folderIm = 0;
   if (saveTxtArg == 0) outputs_folderTxt = 0;

   list *list_files;


   //If there is a filename as argument, make the list with a single file only
   if (filename){
	list_files = make_list();
        list_insert(list_files, filename);
   }
   //Read files from the txt file specified in the 'test' tag of the .data file
   else{
	char *test_file = option_find_str(options, "test", 0);
	//Check if test tag is in the .data file
	if (!test_file) { printf("\nError: Tag 'test' not found in the file %s", dataFile); return; }
        //Check if file to read images from exists        
        FILE* fptr = fopen(test_file, "r");
	if (fptr == NULL){ printf("\nError: File %s not found\n",test_file); return; }
   	fclose(fptr);
	printf("\nFile with list of images: %s", test_file);
        printf("\nFolder to store resulting images: %s", outputs_folderIm);
        list_files = get_paths(test_file);
   }
   image **alphabet = load_alphabet();
   //Load network
   printf("\nLoading network:\n");
   network *net = load_network(cfgFile, weightsFile, 0);
   set_batch_network(net, 1);
   //image sized = letterbox_image(im, net->w, net->h);
   //Get file path of the image to be used for detection
   char *find_filename_no_ext();
   int qtFiles = list_files->size;
   char **filesToTest = (char **)list_to_array(list_files);
   char *fileNameNoExt;
   int i;
   image im;
   image sized;
   char buff[256];
   char *input = buff;
   int countDetectedImages = 0;
   int countSkippedImageFiles = 0;
   //Loop through list of images
   for (i = 0;i<qtFiles;i++) {
      strcpy(input,filesToTest[i]);
      if (input[0] == '\0'){ countSkippedImageFiles++; continue; }
      fileNameNoExt = find_filename_no_ext(filesToTest[i],0);
      fflush(stdout);
      im = load_image_color(input,0,0);
      sized = letterbox_image(im, net->w, net->h);
      //Start detection
      apply_detector(net, names, outputs_folderIm, outputs_folderTxt, fileNameNoExt, im, sized, alphabet, thresh, hier_thresh, show_img);
      countDetectedImages++;
   }
   
   free_list_contents(list_files);
   printf("\n\nFinished detections!");
   printf("\n%d files were used and %d files were skipped\n\n", countDetectedImages, countSkippedImageFiles);
}

void apply_detector(network *net, char **names, char *outputs_folder_im, char *outputs_folder_txt, char *image_name, image im, image sized, image **alphabet, float thresh, float hier_thresh, char *show_img)
{
    printf("\nDetecting objects in: %s",image_name);
    fflush(stdout);
    srand(2222222);
    float nms=.3;
    layer l = net->layers[net->n-1];
    box *boxes = calloc(l.w*l.h*l.n, sizeof(box));
    float **probs = calloc(l.w*l.h*l.n, sizeof(float *));
    int j;
    for(j = 0; j < l.w*l.h*l.n; ++j) probs[j] = calloc(l.classes + 1, sizeof(float *));
    float **masks = 0;
    if (l.coords > 4){
       masks = calloc(l.w*l.h*l.n, sizeof(float*));
       for(j = 0; j < l.w*l.h*l.n; ++j) masks[j] = calloc(l.coords-4, sizeof(float *));
    }
    float *X = sized.data;
    double time=what_time_is_it_now();
    //Perform detection
    network_predict(net, X);
    printf("\nPredicted in %f seconds.", what_time_is_it_now()-time); fflush(stdout);
    get_region_boxes(l, im.w, im.h, net->w, net->h, thresh, probs, boxes, masks, 0, 0, hier_thresh, 1);
    //Perform Non-Maximum Suppression to find best bounding box to avoind overlappings
    if (nms) do_nms_sort(boxes, probs, l.w*l.h*l.n, l.classes, nms);
    printf("\n%d objects detected.\n",l.w*l.h*l.n);
    int objsThresholded = draw_detections(im, l.w*l.h*l.n, thresh, boxes, probs, masks, names, alphabet, l.classes);
    printf("(%d objects thresholded)\n",objsThresholded);

    if (outputs_folder_im != 0){
       //Saving image file
       char finalImgFile[1000] = "";
       strcpy(finalImgFile, outputs_folder_im);
       if (ends_with(finalImgFile, "/") == 0) strcat(finalImgFile, "/");    
       strcat(finalImgFile, image_name);  
       //if calling save_image, it will save a .jpg if OPENCV compiled.
       //to avoid inconsistency of extensions, lets save in PNG.
       save_image_png(im, finalImgFile);
       printf("Image saved: %s.png", finalImgFile);
    }
    if (outputs_folder_txt != 0){
       //Saving txt file
       char finalTextFile[1000] = "";
       strcpy(finalTextFile, outputs_folder_txt);
       if (ends_with(finalTextFile, "/") == 0) strcat(finalTextFile, "/");    
       strcat(finalTextFile, image_name);    
       strcat(finalTextFile, "_dets.txt");
       write_detections(im, l.w*l.h*l.n, thresh, boxes, probs, names, alphabet, l.classes, finalTextFile);
       printf("\nDections saved: %s\n", finalTextFile);
    }

    if (show_img) {
#ifdef OPENCV
        cvNamedWindow("predictions", CV_WINDOW_NORMAL); 
        /*if(fullscreen){
            cvSetWindowProperty("predictions", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
        }*/
        show_image(im, "predictions");
        cvWaitKey(0);
        cvDestroyAllWindows();
#endif
    }

    //Release memory of alloc
    free_image(im);
    free_image(sized);
    free(boxes);
    free_ptrs((void **)probs, l.w*l.h*l.n);
//    free_ptrs((void **)masks, l.w*l.h*l.n);  
}

void write_detections(image im, int num, float thresh, box *boxes, float **probs, char **names, image **alphabet, int classes, char *save_name)
{
    int i;
    FILE *f_img;
    f_img = fopen(save_name, "w");

    for(i = 0; i < num; ++i){
        int class = max_index(probs[i], classes);
        float prob = probs[i][class];
        //write the class only if its prob is greater than zero
        if(prob){
            box b = boxes[i];
            //Class, probability, relative x, relative y, relative w, relative h
            fprintf(f_img, "%d %f %f %f %f %f\n", class, prob, b.x, b.y, b.w, b.h);
        }
    }
    fclose(f_img);
}

