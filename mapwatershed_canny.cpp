/*
This program does Watershed Segmentation of Satellite Images using Watershed Segmentation using Edge Region information as markers to the watershed algorithm

Copyright (C) 2010  Koustubh Sinkar

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

You can contact the Maintainer of this code at ksinkar@gmail.com
*/

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
 //cvNamedWindow("Grayscale",CV_WINDOW_AUTOSIZE);
 //cvShowImage("Grayscale",img_bw);
 cvCvtColor(img_bw, img_gray, CV_GRAY2BGR);
 wshed = cvCloneImage( img0 );//copy the original image to the wshed
 cvWaitKey(0);
 //cvDestroyWindow("Grayscale");
 
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
  cvSaveImage("colorlevel.jpg",colorlevel);
  cvWaitKey(0);
  //cvDestroyWindow("Color Levelled Image");
 
 IplImage *colorlevel_gray=cvCreateImage(cvGetSize(colorlevel), 8,1);
 cvCvtColor(colorlevel,colorlevel_gray,CV_BGR2GRAY);
 cvNamedWindow("Color Levelled Grayscale", CV_WINDOW_AUTOSIZE);
 cvShowImage("Color Levelled Grayscale", colorlevel_gray);
 cvSaveImage("colorlevel_gray.jpg", colorlevel_gray);
 
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
 cvCanny(colorlevel_gray, canny_edge, high, low, aperture);
 cvNamedWindow("Canny Edge",CV_WINDOW_AUTOSIZE);
 cvShowImage("Canny Edge",canny_edge);
 cvSaveImage("canny_edge.jpg",canny_edge);
 cvWaitKey(0);
 //cvDestroyWindow("Canny Edge");
 
 //Assigning the marker mask
 marker_mask=cvCloneImage(canny_edge);
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
      
 cvShowImage("Watershed Transform",wshed);
 cvSaveImage("watershed.jpg", wshed);
 cvWaitKey(0);
 cvAddWeighted( wshed, 0.5, img_gray, 0.5, 0, wshed );//
 cvShowImage( "Watershed Transform", wshed );
 cvSaveImage("watershed_superimposed.jpg", wshed);
 cvReleaseMemStorage( &storage );
 cvReleaseMat( &color_tab );

 cvWaitKey(0);
 
 cvReleaseImage(&wshed);
 cvDestroyWindow("Watershed Transform");
 cvReleaseImage(&markers);
 cvReleaseImage(&canny_edge);
 cvReleaseImage(&colorlevel_gray);
 //cvReleaseImage(&threshold);
 //cvReleaseImage(&g_skel);
 //cvReleaseImage(&img_skel);
 cvReleaseImage(&colorlevel);
 cvReleaseImage(&img0);
 cvDestroyWindow("Original Image");
}
