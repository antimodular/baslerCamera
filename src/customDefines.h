//
//  customDefines.h
//  faceSplitting
//
//  Created by Stephan Schulz on 2016-10-17.
//
//

#ifndef faceSplitting_customDefines_h
#define faceSplitting_customDefines_h

//#include "ofxCv.h"

#define USE_HAP


#define USE_MONO8

#define DISPLAY_WIDTH 1200//720 //1080  //960 //1080 //960
#define DISPLAY_HEIGHT 900//1080 //1920 //1080 //1920 //1920 //1080


//with basler 960x600  or 1920x1200
#define  CAM_WIDTH 960
#define  CAM_HEIGHT 600

#define ROTATED_WIDTH  CAM_WIDTH
#define ROTATED_HEIGHT  CAM_HEIGHT
#define ROTATED_RATIOHW 0.5625 // 1920/1080

#define MAX_BUFFER_NUM 3 //amount of displayed videos
#define MAX_BUFFER_SIZE 200 // max amount of recorded frames
#endif
