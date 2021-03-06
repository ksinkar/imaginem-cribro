#include"cv.h"
#include"highgui.h"
#include<cstdio>
#include<cstdlib>
#include<iostream>
#include<iomanip>

using namespace std;

int main(int argc, char** argv)
{
 CvRNG rng = cvRNG(-1);
 IplImage* img0 = cvLoadImage(argv[1],1);
 IplImage* temp = cvCreateImage(cvGetSize(img0),IPL_DEPTH_8U,3);
 IplImage *markers=0, *marker_mask=0, *wshed=0, *img_gray, *img_bw, *img=0, *g_skel=0;

 cvNamedWindow("Original Image",CV_WINDOW_AUTOSIZE);
 cvShowImage("Original Image",img0);
 img = cvCloneImage( img0 );//copy the original image to img0
 img_gray = cvCloneImage( img);//copy the original image to img0
 img_bw = cvCreateImage(cvGetSize(img), 8, 1);//copy the original image
 cvCvtColor( img, img_bw, CV_BGR2GRAY );//save the the grayscale image to a 3-channel image as a grayscale image img_gray
 cvNamedWindow("Grayscale",CV_WINDOW_AUTOSIZE);
 cvShowImage("Grayscale",img_bw);
 cvCvtColor(img_bw, img_gray, CV_GRAY2BGR);
 wshed = cvCloneImage( img0 );//copy the original image to the wshed
 cvWaitKey(0);
 cvDestroyWindow("Grayscale");
 
 //smoothing the colors of the image
  IplImage *colorlevel=cvCreateImage(cvGetSize(img0),8,3);
  cout<<"Please enter the color radius: ";
  uint color;
  cin>>color;
  cout<<"Please enter the spatial radius: ";
  uint spatial;
  cin>>spatial;
  cvPyrMeanShiftFiltering(img0, colorlevel, spatial, color);
  cvNamedWindow("Color Levelled Image", CV_WINDOW_AUTOSIZE);
  cvShowImage("Color Levelled Image", colorlevel);
  cvWaitKey(0);
  cvDestroyWindow("Color Levelled Image");
 
 //defining the structuring element
 IplConvKernel* element = 0; 
 int element_shape=CV_SHAPE_RECT;
 int an=1;
 cout<<"Please enter the value of x in 2x+1 structuring element: ";
 cin>>an;
 element = cvCreateStructuringElementEx( an*2+1, an*2+1, an, an, element_shape, 0 );
 
 //Skeletonization
 IplImage* img_skel = cvCreateImage(cvGetSize(img0),IPL_DEPTH_8U,3); 
 cvMorphologyEx(colorlevel,img_skel,temp,element,CV_MOP_GRADIENT,1);
 cvNamedWindow("Skeleton",CV_WINDOW_AUTOSIZE);
 cvShowImage("Skeleton",img_skel);
 g_skel = cvCreateImage( cvGetSize(img), 8, 1 );//create a marker_mask with the size of the original image
 cvCvtColor( img_skel, g_skel, CV_BGR2GRAY );//convert the color skeleton to grayscale and save it in marker_mask as 
 cvNamedWindow("Grayscale Skeleton",CV_WINDOW_AUTOSIZE);
 cvShowImage("Grayscale Skeleton",g_skel);
 cvWaitKey(0);
 cvDestroyWindow("Skeleton");
 cvDestroyWindow("Grayscale Skeleton");
 
 //get the threshold values
 int type,adaptive;
 uint t,max, parameter_type, parameter_adaptive, blocksize=3, constant=5;
 abc: cout<<"Enter the Threshold parameter: ";
 cin>>t;
 cout<<"Enter the maximum value parameter: ";
 cin>>max;
 cout<<"Enter the Thresholding type: \n 1. CV_THRESH_BINARY \n 2. CV_THRESH_BINARY_INV \n 3. CV_THRESH_TRUNC \n 4. CV_THRESH_TOZERO_INV \n 5. CV_THRESH_TOZERO\n\t TYPE:";
 cin>>parameter_type;
 
 switch (parameter_type)
   {
   case 1:
     type = CV_THRESH_BINARY;
     break;
   case 2:
     type = CV_THRESH_BINARY_INV;
     break;
   case 3:
     type = CV_THRESH_TRUNC;
     break;
   case 4:
     type = CV_THRESH_TOZERO_INV;
     break;
   case 5:
     type = CV_THRESH_TOZERO;
     break;
   default:
     cout<<"INVALID INPUT!!";
     goto abc;  
   }
   
 xyz:cout<<"Enter the Adaption type: \n 1. CV_ADAPTIVE_THRESH_MEAN_C \n 2. CV_ADAPTIVE_THRESH_GAUSSIAN_C \n\t TYPE:";
 cin>>parameter_adaptive;
 
 switch (parameter_adaptive)
   {
   case 1:
     adaptive = CV_ADAPTIVE_THRESH_MEAN_C;
     break;
   case 2:
     adaptive = CV_ADAPTIVE_THRESH_GAUSSIAN_C;
     break;
   default:
     cout<<"INVALID INPUT!!";
     goto abc;  
   }

 //Adaptive Thresholding
 IplImage *threshold = cvCreateImage(cvGetSize(img),8,1);
 cvAdaptiveThreshold(g_skel, threshold, max, adaptive, type, blocksize, constant);
 cvNamedWindow("Thresholding",CV_WINDOW_AUTOSIZE);
 cvShowImage("Thresholding",threshold);
 cvWaitKey(0);
 cvDestroyWindow("Thresholding");
 
 //Inputting the Canny threshold values
 uint high, low, aperture;
 cout<<"PLease enter the value for High Threshold: ";
 cin>>high;
 cout<<"Please enter the value for Low Threshold: ";
 cin>>low;
 cout<<"Please enter the aperture size: ";
 cin>>aperture;
 
 //Edge Detection
 IplImage *canny_edge = cvCreateImage(cvGetSize(img),8,1);
 cvCanny(threshold, canny_edge, high, low, aperture);
 cvNamedWindow("Canny Edge",CV_WINDOW_AUTOSIZE);
 cvShowImage("Canny Edge",canny_edge);
 cvWaitKey(0);
 cvDestroyWindow("Canny Edge");
 
 //Assigning the marker mask
 marker_mask = cvCloneImage(threshold);
 //marker_mask = cvCreateImage( cvGetSize(img), 8, 1 );//create a marker_mask with the size of the original image
 //cvCvtColor( img_skel, marker_mask, CV_BGR2GRAY );//convert the color skeleton to grayscale and save it in marker_mask as a 1-channel image
 markers = cvCreateImage( cvGetSize(img), IPL_DEPTH_32S, 1 );//create a marker image with the specifications for cvWatershed
 
 //cvZero( marker_mask );//set all the elements of marker_mask to zero
 cvZero( wshed );//set all the elements of wshed to zero
 cvShowImage( "Watershed Transform", wshed );//it shows the black dark image in the window which later on shows the final watershed image is imposed
 CvMemStorage* storage = cvCreateMemStorage(0);//creates memory storage
 CvSeq* contours = 0;//openCV dynamic data structure; contours are lines that connect same altitude points on a map
 CvMat* color_tab;//openCV matrix structure
    int i, j, comp_count = 0;
    //cvSaveImage( "wshed_mask.png", marker_mask );
    //marker_mask = cvLoadImage( "wshed_mask.png", 0 );
    cvFindContours( marker_mask, storage, &contours, sizeof(CvContour),CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );//get contours from marker_mask and store them in storage
    cvZero( markers );//set elements of markers to zero
    
    //printing the contour points sequence
    for( int i=0; i<contours->total; ++i ) 
    {
      CvPoint* p = (CvPoint*)cvGetSeqElem ( contours, i );
      printf("(%d,%d)\n", p->x, p->y );
    }
    //Drawing the contours
    for( ; contours != 0; contours = contours->h_next, comp_count++ )
    {
      cvDrawContours( markers, contours, cvScalarAll(comp_count+1),cvScalarAll(comp_count+1), -1, -1, 8, cvPoint(0,0) );//draws contours on the markers image from CvSeq* contours
    }
    cout<<comp_count<<endl;
	    
    color_tab = cvCreateMat( 1, comp_count, CV_8UC3 );//creates a cvMat 
    for( i = 0; i < comp_count; i++ )
    {
     uchar* ptr = color_tab->data.ptr + i*3;
     ptr[0] = (uchar)(cvRandInt(&rng)%180 + 50);
     ptr[1] = (uchar)(cvRandInt(&rng)%180 + 50);
     ptr[2] = (uchar)(cvRandInt(&rng)%180 + 50);
    }
    //calculate time for computing the watershed
    {
     double t = (double)cvGetTickCount();
     cvWatershed( img0, markers );//segment the image using markers
     t = (double)cvGetTickCount() - t;
     printf( "exec time = %gms\n", t/(cvGetTickFrequency()*1000.) );
    }

            //cvNamedWindow("markers",CV_WINDOW_AUTOSIZE);
	    //cvShowImage("markers",markers);

    // paint the watershed image
    for( i = 0; i < markers->height; i++ )
      for( j = 0; j < markers->width; j++ )
      {
       int idx = CV_IMAGE_ELEM( markers, int, i, j );
       uchar* dst = &CV_IMAGE_ELEM( wshed, uchar, i, j*3 );
       if( idx == -1 )
	 dst[0] = dst[1] = dst[2] = (uchar)255;
       else 
	 if( idx <= 0 || idx > comp_count )
	   dst[0] = dst[1] = dst[2] = (uchar)0; // should not get here
	 else
	 {
          uchar* ptr = color_tab->data.ptr + (idx-1)*3;
          dst[0] = ptr[0]; dst[1] = ptr[1]; dst[2] = ptr[2];
         }
      }
      
 cvAddWeighted( wshed, 0.5, img_gray, 0.5, 0, wshed );//
 cvShowImage( "Watershed Transform", wshed );
 cvReleaseMemStorage( &storage );
 cvReleaseMat( &color_tab );

 cvWaitKey(0);
 
 cvReleaseImage(&wshed);
 cvDestroyWindow("Watershed Transform");
 cvReleaseImage(&markers);
 //cvReleaseImage(&canny_edge);
 cvReleaseImage(&threshold);
 cvReleaseImage(&g_skel);
 cvReleaseImage(&img_skel);
 cvReleaseImage(&colorlevel);
 cvReleaseImage(&img0);
 cvDestroyWindow("Original Image");
}
