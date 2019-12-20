//
//  videoHandler.h
//  faceBlender
//
//  Created by Stephan Schulz on 2017-08-14.
//
//

#ifndef videoHandler_h
#define videoHandler_h

//new brew does not install ffmpeg with all flags
//so get it from here https://ffmpeg.zeranoe.com/builds/
//i got "static and shared builds for macOS 64-bit"

#include "customDefines.h"

#include "oneVideo.h"


using namespace ofxCv;
using namespace cv;


class videoHandler{
    
public:
    
       ofTrueTypeFont arial;
    
    bool bDebug;
    
    //    oneVideoBuffer videoBuffers[MAX_BUFFER_NUM];
    oneVideo videoBuffers[MAX_BUFFER_NUM];
    
    bool bRecordingDone;
    
    string localRecordingsFolder;
    string recordingsFolder;
    string current_fileName;

//    bool bMirror;
    
    //---playback
    ofParameterGroup parameters_playback;
    ofParameter<float> video_minSpeed;
    ofParameter<float> video_speed;

    ofParameter<int> keyFrame;
    ofParameter<int> keyFrameBuffer;
    ofParameter<bool> bLoopPalindrome;
//    int old_minStartFrame;
    
//    int lastLabel;
    int recIndex;
    
    //----layers used for displaying
    ofParameterGroup parameters_layer;

    
//    oneLayer layer_object;
    
//    ofParameter<int> blendMode;
//    ofBlendMode blMode[6];
    
//     float shuffleTimer;
//    ofParameter<float> shuffleDuration;
//    ofParameter<int> liveAlphaMin,liveAlphaMax;
    ofParameter<int> layerAlpha[MAX_BUFFER_NUM];
    ofParameter<float> blendSpeed;
    
  

    bool bLogToFile;
     ofMatrix4x4 defaultMatrix;
    int onLayerIndex;
    float liveAlpha;
    
    ofParameter<int> x_offset,y_offset;
    ofParameter<float> squareZoom;
    ofParameter<int> outerFramePadding;

    
    void setup(string _path){
        
        
        arial.load("Arial.ttf", 12, false, false);
//        arial.setLineHeight(18.0f);
//        arial.setLetterSpacing(1.037);
        
      
        localRecordingsFolder = _path;
    
        for(int i=0; i<MAX_BUFFER_NUM; i++){
            videoBuffers[i].myID = i;
            videoBuffers[i].myColor = ofColor(ofRandom(100,255),ofRandom(100,255),ofRandom(100,255));
            videoBuffers[i].localRecordingsFolder = localRecordingsFolder;
            videoBuffers[i].setup();
        }
        
  
        
 
//        videoBuffers[MAX_BUFFER_NUM-1].bUnUsed = true;
        //----done loading
      
        
        parameters_playback.setName("play");
//        parameters_playback.add(bAllowScrubbing.set("bAllowScrubbing",true));
        parameters_playback.add(video_speed.set("videoSpeed",1,0,1));
        parameters_playback.add(video_minSpeed.set("minSpeed",0.1,0.01,1));
//        parameters_playback.add(maxSpeed.set("maxSpeed",1,0,1.5));
//        parameters_playback.add(minStartFrame.set("minStartFrame",0,0,MAX_BUFFER_SIZE));
        parameters_playback.add(keyFrame.set("keyFrame",30,0,120));
        parameters_playback.add(keyFrameBuffer.set("keyFrameBuf",20,0,120));
         parameters_playback.add(bLoopPalindrome.set("bLoopPalindrome",true));
  
//        blMode[0] = OF_BLENDMODE_DISABLED;
//        blMode[1] = OF_BLENDMODE_ALPHA;
//        blMode[2] = OF_BLENDMODE_ADD;
//        blMode[3] = OF_BLENDMODE_SUBTRACT;
//        blMode[4] = OF_BLENDMODE_MULTIPLY;
//        blMode[5] = OF_BLENDMODE_SCREEN;
        
        parameters_layer.setName("layers");
         parameters_layer.add(blendSpeed.set("blendSpeed",1,1,20));

         parameters_layer.add(x_offset.set("x_offset",0,-500,1920));
        parameters_layer.add(y_offset.set("y_offset",0,-500,500));
         parameters_layer.add(squareZoom.set("squareZoom",1,0.1,10));
        parameters_layer.add(outerFramePadding.set("framePadding",5,0,100));
        
//        layer_object.setup();

 
 
        bRecordingDone = false;
     
        recIndex = -1;
        
//        lastLabel = -1;
        
        
//        old_bMirror != bMirror;
        
        onLayerIndex = 1;
    
//        shuffleTimer = ofGetElapsedTimef();
    }
    
    void exit(){
        
      
        
        
    }
    
    void init(){
        
//        old_minStartFrame = -1;
        
         //-----load old buffers in to buffer array
  
            ofDirectory mainDir(localRecordingsFolder);
   
            mainDir.listDir();
             mainDir.sort();
            int fileCount = mainDir.size();
         
            ofLog();
            ofLog()<<"mainDir "<< mainDir.getAbsolutePath();
            ofLog()<<"loadAllFrames from nFolders "<<fileCount;
            ofLog();
            
            if(fileCount < MAX_BUFFER_NUM) ofLog()<<"not enough files in videos folder";
            for(int i=0; i<MAX_BUFFER_NUM; i++){
                
                ofLog()<<"load files in to buffer # "<<i;
                
                int fileIndex = fileCount-1-i; //ofRandom(nFolders);
                ofLog()<<"folderIndex "<<fileIndex;
                
//                ofLog()<<"videosDir.getName(folderIndex) "<<mainDir.getName(fileIndex);
                videoBuffers[i].load(mainDir.getName(fileIndex));

            }

    
    }
    
    void update(){
        
        checkGui();

        if(bRecordingDone == true){
            bRecordingDone = false;
            
            recIndex++;
            recIndex %= MAX_BUFFER_NUM;
            
            videoBuffers[recIndex].prepareNewFile(current_fileName);

        }


        //update all buffer processes
        for(int i=0; i<MAX_BUFFER_NUM; i++){
            videoBuffers[i].update();
        }
        

    }
    
    void drawLayers(int _x, int _y, float _scale, bool _justLive, bool _mirror){

        ofSetColor(255);
        ofPushMatrix();
        
//        ofLog()<<"_scale "<<_scale;
        float x_scale = _scale;
        if(_mirror) x_scale = _scale * -1;
//        ofScale(x_scale,1);
        
               
        ofMatrix4x4 scale_0;
        scale_0.makeScaleMatrix(x_scale,_scale,1);
        
        ofMatrix4x4 translate_0;
       if(_mirror) translate_0.makeTranslationMatrix(_x+ROTATED_WIDTH,_y,0);
       else translate_0.makeTranslationMatrix(_x,_y,0);

    
        ofMatrix4x4 main_0;
        main_0 = scale_0 * translate_0;
        
        ofPushStyle();
//        ofEnableBlendMode(blMode[blendMode]);
        
         ofMultMatrix(main_0);
        
        int temp_w = DISPLAY_WIDTH / 3;
        int temp_h = DISPLAY_HEIGHT / 3;
        for(int i=0; i<MAX_BUFFER_NUM; i++){
//            if(i==0)ofLog()<<"bDebug "<<bDebug;
            int xx = videoBuffers[i].x_coordinate * temp_w;
            int yy = videoBuffers[i].y_coordinate * temp_h;
            
            if(_mirror){
                 videoBuffers[i].draw((temp_w*3)-xx,yy,temp_w,temp_h,x_offset,y_offset,temp_w/float(squareZoom),outerFramePadding, bDebug);
            }else{
                videoBuffers[i].draw(xx,yy,temp_w,temp_h,x_offset,y_offset,temp_w/float(squareZoom),outerFramePadding, bDebug);
            }
        }
       
     
        
        ofDisableBlendMode();
        ofPopStyle();
        
        ofPopMatrix();
     

    }
    
  
    
    void drawLayerAlphaInfo(int _x = 0, int _y = 0){
        ofPushMatrix();
        ofTranslate(_x,_y);
//        arial.drawString("buffers",0,10);
        arial.drawString("buffers",0,10);
        for(int i=0; i<MAX_BUFFER_NUM; i++){
            
           
            ofSetColor( videoBuffers[i].myColor);
             ofFill();
            ofDrawRectangle(0,0,10,-videoBuffers[i].alphaCurrent/2);
            
            ofSetColor(255);
            ofNoFill();
             ofDrawRectangle(0,0,10,-255/2);
            
            ofTranslate(14,0);

        }
        

        
        ofPopMatrix();
    }
    
    void drawBuffers(int _x, int _y){
        
        ofPushMatrix();
        ofTranslate(_x,_y);
        //----draw small buffer frames
        int tempW = min(ofGetWidth()/MAX_BUFFER_NUM,200);

        int tempH = tempW * ROTATED_RATIOHW;
        
        for(int i=0; i<MAX_BUFFER_NUM; i++){
            videoBuffers[i].drawDebugVideo(i*(tempW+5), 0, tempW, tempH);
            videoBuffers[i].drawGraphic(i*(tempW+5), tempH, tempW, tempH);
        }
        ofPopMatrix();
    }
    
    void drawBuffersGraphic(int _x, int _y){
        
        ofPushMatrix();
        ofTranslate(_x,_y);
        //----draw small buffer frames
        int tempW = min(ofGetWidth()/(MAX_BUFFER_NUM+1),200);
        
        int tempH = tempW * ROTATED_RATIOHW;
        
        for(int i=0; i<MAX_BUFFER_NUM; i++){
            videoBuffers[i].drawGraphic(i*(tempW+5), tempH, tempW, tempH);
        }
        ofPopMatrix();
    }

    
    void checkGui(){

        for (int i = 0; i<MAX_BUFFER_NUM; i++) {
            videoBuffers[i].blendSpeed = blendSpeed;
        }
    }
};

#endif /* videoHandler_h */
