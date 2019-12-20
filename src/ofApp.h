#pragma once

#include "ofMain.h"
#include "customDefines.h"


#include "ofxCv.h"

#include "ofxGui.h"
#include "ofParameterGroup.h"
#include "ofParameter.h"

#include "videoHandler.h"

#include "threaded_baslerCam.h"


class ofApp : public ofBaseApp{
public:
    string versionString;
    ofxLabel versionLabel;
    
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void drawLiveVideoDebug(int _x, int _y, float _scaler, bool _features, bool _mirror);
    void drawLiveVideo(int _x, int _y, float _scaler, bool _features, bool _mirror);
    
    //------main
    void saveGui();
    void checkGui();
    
    ofxPanel gui_main, gui_videoObj,gui_playback, gui_layers;
    
    ofParameter<bool> bShowGUI;
    ofParameter<int> fps;
    ofParameter<int> appFrameRate;
    int old_appFrameRate;
    
    ofParameter<bool> bShowDebug;
    ofParameter<int> debugVideoX, debugVideoY;
    ofParameter<float> debugVideoScaler;
    
    ofParameter<bool> bShowLive;
    ofParameter<int> liveVideoX, liveVideoY;
    ofParameter<float> liveVideoScaler;
    
    ofParameter<int> originX, originY;
    ofParameter<float> global_scaler;
    
    videoHandler video_object;

    
    ofFbo mainFBO;
    
    bool bInfo;
      ofTrueTypeFont arial;
    
    bool bLogToFile;
    
   
    
    //--cam
//    threaded_camera cam_object;
    threaded_baslerCam cam_object;
    ofParameter<bool> bMirror;
    bool old_bMirror;
    
    string localRecordingsFolder;
    string localTempRecordingsFolder;
    string recordingsFolder;
    string current_fileName;
    

};
