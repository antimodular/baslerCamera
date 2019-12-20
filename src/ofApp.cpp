#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    
   
    
    //MARK: make sure to compile for OS10.8. tried it on OS10.11.6 with xcode 8.1 base sdk10.11 target 10.11 did not work tagret 10.8 did
    
    //-----main

    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofSetFrameRate(60);
    
       arial.load("Arial.ttf", 12, false, false);
    
    ofSetBackgroundColor(0);
   

    mainFBO.clear();
    mainFBO.allocate(DISPLAY_WIDTH,DISPLAY_HEIGHT,GL_RGBA);
    
    mainFBO.begin();
    ofClear(255,255,255, 0);
    mainFBO.end();

    
    //-----------GUI --------
    int gui_x = 30;
    int gui_y = 10;
    int gui_w = 210;
    int gui_h = 350;
    
    //    camWidth = 1280;
    //    camHeight = 720;
    
    //gui column 1
    gui_main.setup("splitting");
    gui_main.setPosition(gui_x,gui_y);
    gui_main.setDefaultHeaderBackgroundColor(ofColor(255,0,0));
    gui_main.add(fps.set("fps",0,0,120));
     gui_main.add(appFrameRate.set("appFrameRate",30,1,120));
    gui_main.add(bShowGUI.set("showGui",false));
//    gui_main.add(bShowFull.set("fullscreen",false));
    
    gui_main.add(bShowDebug.set("showDebug",false));
    gui_main.add(debugVideoX.set("debugVideoX",DISPLAY_WIDTH-(ROTATED_WIDTH*0.2)-10,0,2000));
    gui_main.add(debugVideoY.set("debugVideoY",DISPLAY_HEIGHT/2-(ROTATED_HEIGHT*0.2)-10,0,2000));
    gui_main.add(debugVideoScaler.set("debugVideoScaler",0.2,0.1,1));
  
    gui_main.add(bShowLive.set("showLive",false));
    gui_main.add(liveVideoX.set("liveVideoX",DISPLAY_WIDTH-(ROTATED_WIDTH*0.2)-10,0,2000));
    gui_main.add(liveVideoY.set("liveVideoY",DISPLAY_HEIGHT/2-(ROTATED_HEIGHT*0.2)-10,0,2000));
    gui_main.add(liveVideoScaler.set("liveVideoScaler",0.2,0.1,1));


    gui_main.add(global_scaler.set("scaler",1.5,0.5,5));
    gui_main.add(originX.set("originX",0,-1920,1920));
    gui_main.add(originY.set("originY",0,-1500,1500));


    gui_main.loadFromFile("gui_main.xml");

    ofLog()<<"ofToDataPath( "<<ofToDataPath("",true);
    localRecordingsFolder  = ofToDataPath("",true) + "/recordings_video_hap/";
    localTempRecordingsFolder = ofToDataPath("",true) + "/recordings_video_temp/";
    ofLog()<<"localRecordingsFolder "<<localRecordingsFolder;
    recordingsFolder = localRecordingsFolder;
    
    video_object.setup(recordingsFolder);
    cam_object.setup(recordingsFolder,localTempRecordingsFolder);
    
    gui_videoObj.setup("video");
    gui_videoObj.setPosition(gui_x,gui_y+380);
    gui_videoObj.setDefaultHeaderBackgroundColor(ofColor(255,0,0));
    gui_videoObj.add(bMirror.set("mirror",true));

    gui_videoObj.add(cam_object.parameters_video);
    gui_videoObj.loadFromFile("gui_videoObj.xml");
    
    gui_playback.setup("play");
    //    gui_playback.setName("play");
    gui_playback.setPosition(gui_x,gui_y+650);
    gui_playback.setDefaultHeaderBackgroundColor(ofColor(255,0,0));
    gui_playback.add(video_object.parameters_playback);
    gui_playback.add(cam_object.parameters_rec);
    gui_playback.loadFromFile("gui_playback.xml");
    
    //gui column 3
    gui_layers.setup("layers");
    gui_layers.setPosition(gui_x+=gui_w,gui_y+350);
    gui_layers.setDefaultHeaderBackgroundColor(ofColor(255,0,0));
    gui_layers.add(video_object.parameters_layer);
    gui_layers.loadFromFile("gui_layers.xml");
    
   
     cam_object.init();
    video_object.init();


    bShowDebug = true;
    bShowGUI = false;
    
    bInfo = false;

    ofSetFullscreen(true);
  
    
    ofLog()<<"setup done";
    
}

void ofApp::exit(){
    cam_object.exit();
}

void ofApp::saveGui(){
    
    ofLogNotice("ofApp")<<"saveGui"<<endl;
    gui_main.saveToFile("gui_main.xml");
    gui_videoObj.saveToFile("gui_videoObj.xml");
    gui_playback.saveToFile("gui_playback.xml");
    gui_layers.saveToFile("gui_layers.xml");


    cam_object.saveGui();
    
}

void ofApp::checkGui(){
    
    video_object.bDebug = bShowDebug;
     cam_object.checkGui();
    if(old_appFrameRate != appFrameRate){
        old_appFrameRate = appFrameRate;
        ofSetFrameRate(appFrameRate);
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
    fps = ofGetFrameRate();

    
    checkGui();    

    if(cam_object.bRecordingDone == true){
        ofLog()<<"cam_object.bRecordingDone == true "<<cam_object.current_fileName;
  
        video_object.bRecordingDone = true;
        
         video_object.current_fileName = cam_object.current_fileName;
        /*
        string _dst = ofToDataPath(localRecordingsFolder+cam_object.current_fileName,true);
        string _src = ofToDataPath(localTempRecordingsFolder+cam_object.current_fileName,true);
        
        string cutLoop = "/usr/local/bin/ffmpeg ";
        cutLoop += "-i "+_src+" ";
        cutLoop += " -filter_complex ";
        cutLoop += "\"";
        cutLoop += "[0:v]reverse,fifo[r];[0:v][r] concat=n=2:v=1 [v]\" ";
        cutLoop += "-map \"[v]\" ";
        //        cutLoop += "-c:v rawvideo -preset slow -crf 22 ";
        cutLoop += "-c:v rawvideo -preset ultrafast -crf 22 ";
        
        cutLoop += _dst;
        
        
        //        string rec_codec[3] = {"prores_ks","rawvideo","libx264"};
        //        string rec_bitrate[3] = {"800k","800k","800k"};
        //        string rec_outFormat[3] = {"yuv420p","yuyv422","yuv420p"};
        
        ofLog()<<"cutLoop: "<<cutLoop;
        ofLog()<<ofSystem(cutLoop);
        */
       
        
        cam_object.bRecordingDone = false;
    }
    
     if(bShowDebug){
      cam_object.updateNoLock();
     }
    
    
    //    fps = ofGetFrameRate();
    video_object.update();

}


//--------------------------------------------------------------
void ofApp::draw(){
    
    // ---draw all layers

    ofPushMatrix();
//    ofTranslate(0,ofGetHeight());
//    ofRotate(270);    

    
    video_object.drawLayers(originX,originY, global_scaler, false, bMirror);

   
    if(bShowDebug){
        drawLiveVideoDebug(debugVideoX, debugVideoY, debugVideoScaler,true,bMirror);
    }
    
    if(bShowLive){
        drawLiveVideo(liveVideoX, liveVideoY, liveVideoScaler,true,bMirror);
    }
    if(bShowDebug){

        video_object.drawBuffers(0,ofGetWidth()-400);

    }
    

    ofPopMatrix();

    if(bShowGUI == true){
        gui_main.draw();
        gui_videoObj.draw();
        gui_playback.draw();
        gui_layers.draw();

//        cam_object.gui_pylon.draw();
        cam_object.drawGui();

        ofDrawBitmapStringHighlight("runtime "+ofToString(ofGetElapsedTimef()/60,0), ofGetWidth()/2, 35);

    }
    
    if(bInfo){

        video_object.drawBuffersGraphic(0,ofGetWidth()-400);

        ofDrawBitmapStringHighlight("app fps "+ofToString(fps), 20,ofGetHeight()-70);
        ofDrawBitmapStringHighlight("cam fps "+ofToString((int)cam_object.cameraTimer.getFramerate()), 20, ofGetHeight()-30);
    }
}

void ofApp::drawLiveVideo(int _x, int _y, float _scaler, bool _features, bool _mirror){
    
   
    //---draw live camera view for debug
    
    ofMatrix4x4 temp_translationM_0,temp_translationM_1, temp_scaleM;
    
    temp_translationM_0.makeTranslationMatrix(-ROTATED_WIDTH, 0, 0);
    temp_translationM_1.makeTranslationMatrix(_x, _y, 0);
    
    float x_scale = _scaler;
    if(_mirror) x_scale = _scaler * -1;
    temp_scaleM.makeScaleMatrix(x_scale, _scaler, 1);
    
    ofMatrix4x4 debugM;
    if(_mirror) debugM = temp_translationM_0*temp_scaleM*temp_translationM_1;
    else debugM = temp_scaleM*temp_translationM_1;
    
    
    ofPushMatrix();
    ofMultMatrix(debugM);
    ofSetColor(255);
//    video_object.drawLiveVideo(); // imgRotated.draw(0,0);
 cam_object.drawLiveVideo();
    
    ofPopMatrix();
    
    if(cam_object.bRecording == true){
        ofFill();
        ofSetColor(255,0,0);
    }else{
        ofNoFill();
        ofSetColor(255);
    }
    ofSetLineWidth(2);
    ofDrawCircle(_x+15,_y+15,10);
    ofPopStyle();
    

}


void ofApp::drawLiveVideoDebug(int _x, int _y, float _scaler, bool _features, bool _mirror){
    
    //---draw live camera view for debug
    
    ofMatrix4x4 temp_translationM_0,temp_translationM_1, temp_scaleM;
    
    temp_translationM_0.makeTranslationMatrix(-ROTATED_WIDTH, 0, 0);
    temp_translationM_1.makeTranslationMatrix(_x, _y, 0);
    
    float x_scale = _scaler;
    if(_mirror) x_scale = _scaler * -1;
    temp_scaleM.makeScaleMatrix(x_scale, _scaler, 1);
    
    ofMatrix4x4 debugM;
    if(_mirror) debugM = temp_translationM_0*temp_scaleM*temp_translationM_1;
    else debugM = temp_scaleM*temp_translationM_1;
    
    
    ofPushMatrix();
    ofMultMatrix(debugM);
    ofSetColor(255);
    cam_object.drawLiveVideoDebug(); // imgRotated.draw(0,0);
    
  
    ofPopMatrix();
    

    //draw text info, not needing to be scaled
    ofPushMatrix();
    ofTranslate(_x, _y+200);
    
    //black text
    ofSetColor(0);
    arial.drawString("fps "+ofToString((int) ofGetFrameRate()), 10+1, 40+1);
//    arial.drawString("res "+ofToString(cam_object.imgRotated.getWidth()) + " x "+ofToString(cam_object.imgRotated.getHeight()), 10+1, 55+1);
    arial.drawString("cam fps "+ofToString((int)cam_object.cameraTimer.getFramerate()), 10+1, 85+1);
    
    //white text
    ofSetColor(255);
    arial.drawString("fps "+ofToString((int) ofGetFrameRate()), 10, 40);
//    arial.drawString("res "+ofToString(cam_object.imgRotated.getWidth()) + " x "+ofToString(cam_object.imgRotated.getHeight()), 10, 55);
    arial.drawString("cam fps "+ofToString((int)cam_object.cameraTimer.getFramerate()), 10, 85);
    
    
    video_object.drawLayerAlphaInfo(0,- 450);
    
    
    
    ofPopMatrix();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

    
    if(key == 'g' || key == 'G'){
        bShowGUI =! bShowGUI;
        
        if(bShowGUI == false){
            saveGui();
            ofHideCursor();
        }else{
            ofShowCursor();
        }
    }
    
    if(key == 'd'){
        bShowDebug = !bShowDebug;
    }


    if(key == 'l'){
        bShowLive = !bShowLive;
    }
    
    if(key == 'f'){
        ofToggleFullscreen();
    }
    
    if(key == 'i'){
        bInfo = !bInfo;
    }
    
    if(key == 'r'){
        cam_object.triggerNewRecording();
    }

    
    if(key == 'p'){
          video_object.videoBuffers[video_object.recIndex].prepareNewFile(video_object.current_fileName);
    }
    if(key == '['){
        video_object.videoBuffers[video_object.recIndex].load("2018-02-13-11-48-24-767.mp4");
//        video_object.videoBuffers[video_object.recIndex].prepareNewFile(video_object.current_fileName);
    }
    if(key == ']'){
//        video_object.videoBuffers[video_object.recIndex].direction = 1;
        video_object.videoBuffers[video_object.recIndex].videoFile.setSpeed(1);
//        video_object.videoBuffers[video_object.recIndex].videoFile.play();
        video_object.videoBuffers[video_object.recIndex].videoFile.setPosition(0);
    }
    
     if(key == '2'){
          video_object.videoBuffers[video_object.recIndex].videoFile.setSpeed(1);
     }
    if(key == '3'){
         video_object.videoBuffers[video_object.recIndex].videoFile.play();
    }
    if(key == '4'){
         video_object.videoBuffers[video_object.recIndex].videoFile.setPosition(0);
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
  
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
   
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
   
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}



