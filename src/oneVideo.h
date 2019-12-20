//
//  oneVideo.h
//  videoLooper
//
//  Created by Stephan Schulz on 2018-02-12.
//

#ifndef oneVideo_h
#define oneVideo_h

#include "customDefines.h"

#ifdef USE_HAP
#include "ofxHapPlayer.h"
#endif

class oneVideo{
public:
    
    
     ofTrueTypeFont arial;
    
    ofFbo myFBO;
    
    int myID;
    ofColor myColor;

    string localRecordingsFolder;
    string current_fileName;
    
    int x_coordinate, y_coordinate;
    
    ofVec2f position;
    ofRectangle viewPort;
    
#ifdef USE_HAP
    ofxHapPlayer videoFile;
#else
    ofVideoPlayer videoFile;
#endif
    
    bool bFileLoading;
    
     int blendSpeed;
    float alphaTarget;
    float alphaCurrent;
    float alphaSource;
    
    bool bRec;
    
    bool bNewFile;
    float newFileTimer;
    
    //---timing
    float videoDuration;
    int playFrameNum;
    int totalNumFrames;
    
    
    
    void setup(){
#ifdef USE_HAP
//        ofLog()<<"for HAP to work set RUNPATH SEARCH PATHS in target's and project's Build Settings to have";
//        ofLog()<<"@loader_path/../Frameworks and @executable_path/../Frameworks";
#endif
        
        alphaTarget = 0;
        
         arial.load("Arial.ttf", 12, false, false);
        
        bRec = false;
            bNewFile = false;
        
//        bAllowScrubbing = false;
        
        alphaCurrent = 0;
        alphaTarget = 0;
        alphaSource = 0;
        blendSpeed = 1;
        
        x_coordinate = myID % 3;
        y_coordinate = myID / 3;
        
        alphaCurrent = 0;

    }
    
    void update(){
              
        
        videoFile.update();
        
        if(videoFile.isFrameNew()){
//            playFrameNum = videoFile.getCurrentFrame();
        }
 
        float alphaDiff;
        if(bNewFile){
            if(alphaCurrent == 0){
                bNewFile = false;
                load(current_fileName);
            }
            if(ofGetElapsedTimef() - newFileTimer > 2){
                 alphaDiff = -1*blendSpeed;
            }
           
            
        }else{
            alphaDiff = 1*blendSpeed; //alphaTarget - alphaCurrent;
        }
//        ofLog()<<"alphaDiff "<<alphaDiff<<" blendSpeed "<<blendSpeed;
        
        alphaCurrent += alphaDiff; // * blendSpeed; //0.1
        alphaCurrent = ofClamp(alphaCurrent,0,255);
        
//        if(videoFile.getPosition() >= 0.99) 
    }
    
    void draw(float _x, float _y, float _width, float _height, int _x_offset, int _y_offset , float _sqSize, int _outerPadding, bool _debug){

//        videoFile.draw(x,y,_width,_height);
//        videoFile.getTexture().drawSubsection(x,y,0,_width,_height, _x_offset,_y_offset,_sqSize,_sqSize);
        
        //make 
        
        if(myFBO.isAllocated() == false || myFBO.getWidth() != _width || myFBO.getHeight() != _height){
            ofLog()<<myID<<" allocate FBO "<<_width<<" , "<<_height;
            myFBO.clear();
            myFBO.allocate(_width,_height,GL_RGBA);
            
            myFBO.begin();
            ofClear(255,255,255, 0);
            myFBO.end();
            
        }
        
        myFBO.begin();
        
        ofClear(0,0,0, 0);
        ofPushMatrix();
        
        //apply transformation to level eyes and scale face
        //        ofMultMatrix(myMatrix);
        
        
        ofSetColor(255,255,255,255);
        
                
        ofRectangle target_rect(0,0,_width,_height);
        ofRectangle source_rect(_x_offset,_y_offset,_sqSize,_sqSize);

        videoFile.getTexture()->drawSubsection(target_rect, source_rect);

        
        ofPopMatrix();
        
        myFBO.end();
        
        ofSetColor(255,alphaCurrent);
        myFBO.draw(_x,_y);
        
        //        if(myID == 0) ofLog()<<"_debug "<<_debug;
        if(_debug){
            
            ofSetColor(255);
            ofDrawBitmapStringHighlight(ofToString(myID),_x+25,_y+25);
        }
        
    }
    
    void drawGraphic(int _x, int _y, int _w, int _h){
        
        //
        ofPushMatrix();
        ofTranslate(_x,_y);
        
        float _scalerH = -ofMap(totalNumFrames,0,500,3,1,true);
        
        ofPushStyle();
        
        ofSetColor(255);
        ofNoFill();

        //draw current play head position
        ofSetColor(0);
        ofSetLineWidth(5);
        ofDrawLine(0, playFrameNum*_scalerH, 10, playFrameNum*_scalerH);
        ofSetColor(255,255,0);
        ofSetLineWidth(3);
        ofDrawLine(0, playFrameNum*_scalerH, 10, playFrameNum*_scalerH); 
        
        //total video length
        ofDrawRectangle(0, 0, 10, totalNumFrames*_scalerH);
        
        ofSetColor(0);
        arial.drawString(ofToString(videoDuration),2+1,-40+1);
        arial.drawString(ofToString(myID)+" myID ", 1,totalNumFrames*_scalerH - 40+1);
        arial.drawString(ofToString(alphaCurrent)+" alpha ",1,totalNumFrames*_scalerH - 60+1);
        arial.drawString(ofToString(bRec)+" bRec ", 1,totalNumFrames*_scalerH - 80+1);
        arial.drawString(ofToString(playFrameNum)+" playFrameNum ", 1,totalNumFrames*_scalerH - 100+1);
        arial.drawString(ofToString(videoFile.getSpeed())+" speed ", 1,totalNumFrames*_scalerH - 120+1);
        
        ofSetColor(255);
        arial.drawString(ofToString(videoDuration),2,-40);//MAX_BUFFER_SIZE*_scalerH - 5);
        arial.drawString(ofToString(myID)+" myID ", 0,totalNumFrames*_scalerH - 40);
        arial.drawString(ofToString(alphaCurrent)+" alpha ", 0,totalNumFrames*_scalerH - 60);
        arial.drawString(ofToString(bRec)+" bRec ", 0,totalNumFrames*_scalerH - 80);
        arial.drawString(ofToString(playFrameNum)+" playFrameNum ", 0,totalNumFrames*_scalerH - 100);
        arial.drawString(ofToString(videoFile.getSpeed())+" speed ", 0,totalNumFrames*_scalerH - 120);

        
        
        ofPopStyle();
        ofPopMatrix();
        
        
    }
    
    void drawDebugVideo(int _x, int _y, int _w, int _h){
        if(videoDuration > 0){
            //            ofLog()<<myID<<" frames.size() "<<frames.size();
            ofSetColor(255,255);
            videoFile.draw(_x,_y,_w,_h);
            
//            if(bUnUsed == true){
//                ofSetColor(255,255-alphaCurrent);
//                ofDrawLine(_x,_y, _x+_w,_y+_h);
//                ofDrawLine(_x+_w,_y, _x,_y+_h);
//            }
            
            //draw smaller video with it's tranformation matrix
            ofPushMatrix();
            ofTranslate(_x+_w/2, _y+_h);

            ofScale(_w/2/(float)ROTATED_WIDTH,_h/2/(float)ROTATED_HEIGHT);

            ofPushMatrix();
         
            ofSetColor(0,255);
            ofFill();
            ofDrawRectangle(0,0,ROTATED_WIDTH,ROTATED_HEIGHT);
            
            ofSetColor(255,alphaCurrent);
            videoFile.draw(0,0);
            
            ofPopMatrix();
            
            ofPopMatrix();
            
            ofSetColor(255,255);
            ofDrawCircle(_x+_w/2, _y+_h,2);
        }
    }
    
    void load(string _fileName){
        current_fileName = _fileName;
        
        ofLog()<<myID<<" load video"<<(localRecordingsFolder+_fileName);
        //        videoFile.close();
        
      
        
#ifdef USE_HAP
        videoFile.load(localRecordingsFolder+_fileName);
#else 
        videoFile.loadAsync(localRecordingsFolder+_fileName);
#endif   
        //         ofLog()<< videoFile.load(localRecordingsFolder+_fileName);
        //TODO: try to do looping by hand. might avoid choking
        //        videoFile.setLoopState(OF_LOOP_NONE); //OF_LOOP_PALINDROME); //OF_LOOP_NONE  OF_LOOP_NORMAL);
        videoFile.setLoopState(OF_LOOP_NORMAL); 
        
   
        videoFile.setPosition(0);
//        videoFile.setSpeed(speed*direction);
        videoFile.play();
        
        videoDuration = videoFile.getDuration();
        totalNumFrames = videoFile.getTotalNumFrames();
        
        //set start frame and direction?
        
        bFileLoading = true;
    }
    
    void prepareNewFile(string _fileName){
        current_fileName = _fileName;
        bNewFile = true;
        newFileTimer = ofGetElapsedTimef();
        
    }

private:
    
};

#endif /* oneVideo_h */
