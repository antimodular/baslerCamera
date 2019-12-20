//
//  threaded_baslerCam.h
//  videoLooper
//
//  Created by Stephan Schulz on 2018-04-15.
//

#ifndef threaded_baslerCam_h
#define threaded_baslerCam_h


//follow these steps to connect to pylon SDK
//file:///Applications/pylon%20Programmer's%20Guide%20and%20API%20Reference.app/Contents/Resources/Html/pylon_programmingguide.html

//new brew does not install ffmpeg with all flags
//so get it from here https://ffmpeg.zeranoe.com/builds/
//i got "static and shared builds for macOS 64-bit"

#include "customDefines.h"


#include "ofxCv.h"
#include "ofxVideoRecorder.h"

using namespace ofxCv;
using namespace cv;

#include "myRateTimer.h"

// Include files to use the PYLON API.
#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

// Namespace for using pylon objects.
using namespace Pylon;
using namespace GenApi;

#include <pylon/usb/BaslerUsbInstantCamera.h>
typedef Pylon::CBaslerUsbInstantCamera Camera_t;
using namespace Basler_UsbCameraParams;

class threaded_baslerCam: public ofThread {
    
    PylonAutoInitTerm autoInitTerm;
    
public:
    
    
    //---recorder
    ofxVideoRecorder    vidRecorder;
    
    string fileExt;
    
    
    //    bool bDoneWithLastRecording;
    
    string localRecordingsFolder;
    string localTempRecordingsFolder;
    
    bool bRecording;
    bool bRecordingDone;
    //-----camera
    myRateTimer cameraTimer;
    
    // Number of images to be grabbed.
    //    static const uint32_t c_countOfImagesToGrab = 100;
    Camera_t camera;
    //     CInstantCamera camera( CTlFactory::GetInstance().CreateFirstDevice());
    // This smart pointer will receive the grab result data.
    CGrabResultPtr ptrGrabResult;
    CImageFormatConverter converter;
    
    //----gui
    
    ofParameterGroup parameters_rec;
    ofParameter<bool>  bFlip;
    
    ofParameterGroup parameters_video;
//    bool bMirror;
    
    ofParameter<int> deviceID;
    ofRectangle roiRect;
    
    ofParameter<int> camFrameRate;
    int old_camFrameRate;
    
    ofParameter<int> minRecFrames;
    int old_minRecFrames;
    ofParameter<int> maxRecFrames;
    int old_maxRecFrames;
    ofParameter<float> minRecPause;
    float lastRecTime;
    ofParameter<int> endTrim;
    int old_endTrim;
    
    ofParameter<float> recDuration;
    ofParameter<int> recFPS;
    string current_fileName;
    
    bool initDone;
    ofImage outputImg;
    
    //i have tried different video recording settings for the output file as this is done via ffmpeg
    //some are slower during saving and loading, other are fast but make big file sizes
    string rec_codec[4] = {"prores_ks","rawvideo","libx264","hap"};
    string rec_bitrate[4] = {"800k","800k","800k","800k"};
    string rec_outFormat[4] = {"yuv420p","yuyv422","yuv420p","rgba"};
    
    ofParameter<int> recSettingIndex;
    
    
    ofxPanel gui_pylon;
    
    ofParameter<bool> autoExposure;
    bool old_autoExposure;
    ofParameter<float> exposureValue;
    float old_exposureValue;
    
    ofParameter<bool> autoGain;
    bool old_autoGain;
    ofParameter<float> gainValue;
    float old_gainValue;
    
    ofParameter<float> gammaValue;
    float old_gammaValue;
    
    ofParameter<int> lightSourcePreset;
    float old_lightSourcePreset;
    
    
    string oldest_fileName;
    string past_fileNames[10];
    int past_fileName_index = 0;
    
    float logTimer;
    //    cv::Mat outputMat;
    
    // Adjust value to make it comply with range and increment passed.
    //
    // The parameter's minimum and maximum are always considered as valid values.
    // If the increment is larger than one, the returned value will be: min + (n * inc).
    // If the value doesn't meet these criteria, it will be rounded down to ensure compliance.
    int64_t Adjust(int64_t val, int64_t minimum, int64_t maximum, int64_t inc){
        
        // Check the input parameters.
        if (inc <= 0)
        {
            // Negative increments are invalid.
            throw LOGICAL_ERROR_EXCEPTION("Unexpected increment %d", inc);
        }
        if (minimum > maximum)
        {
            // Minimum must not be bigger than or equal to the maximum.
            throw LOGICAL_ERROR_EXCEPTION("minimum bigger than maximum.");
        }
        
        // Check the lower bound.
        if (val < minimum)
        {
            return minimum;
        }
        
        // Check the upper bound.
        if (val > maximum)
        {
            return maximum;
        }
        
        // Check the increment.
        if (inc == 1)
        {
            // Special case: all values are valid.
            return val;
        }
        else
        {
            // The value must be min + (n * inc).
            // Due to the integer division, the value will be rounded down.
            return minimum + ( ((val - minimum) / inc) * inc );
        }
    }
    
    threaded_baslerCam(){
        stop();
        waitForThread(false);
    }
    
    void setup(string _path, string _temporaryPath){
        
        localRecordingsFolder = _path;
        localTempRecordingsFolder = _temporaryPath;
        ofDirectory dir;
        ofLog()<<dir.listDir(localRecordingsFolder);
        dir.sort();
        
        if(dir.size() < MAX_BUFFER_NUM) ofLog()<<"!!! make sure "<<localRecordingsFolder<<" has at least"<<MAX_BUFFER_NUM<<" .mov files !!!";
        
         for(int i =0; i<MAX_BUFFER_NUM; i++){
             past_fileNames[i] = dir.getName(i);
             past_fileName_index++;
             past_fileName_index %= MAX_BUFFER_NUM;
         }
        
        
        //----gui
        
        parameters_video.setName("video");
        parameters_video.add(deviceID.set("deviceID",0,0,2));
        parameters_video.add(camFrameRate.set("camFrameRate",30,1,60));
        
        
        parameters_video.add(bFlip.set("flip",false));
        
        parameters_rec.setName("rec");
        parameters_rec.add(recSettingIndex.set("recSettingIndex",0,0,3));
        parameters_rec.add(recDuration.set("recDuration",3,0.5,10));
        parameters_rec.add(recFPS.set("recFPS",30,1,90));
        parameters_rec.add(minRecFrames.set("minRecFrames",39,0,MAX_BUFFER_SIZE));
        parameters_rec.add(maxRecFrames.set("maxRecFrames",93,1,MAX_BUFFER_SIZE));
        parameters_rec.add(minRecPause.set("minRecPause",0,0,10));
        parameters_rec.add(endTrim.set("endTrim",22,0,120));
        
        //-----------GUI --------
        int gui_x = 500;
        int gui_y = 20;
        int gui_w = 210;
        int gui_h = 350;
        
        gui_pylon.setup("pylonCam");
        gui_pylon.setName("pylonCam");
        gui_pylon.setPosition(gui_x,gui_y);
        gui_pylon.setDefaultHeaderBackgroundColor(ofColor(255,0,0));
        
        gui_pylon.add(autoExposure.set("autoExposure",false));
        gui_pylon.add(exposureValue.set("exposure",0.01,0.0001,0.05));
        
        gui_pylon.add(autoGain.set("autoGain",false));
        gui_pylon.add(gainValue.set("gain",0,0,1));
        gui_pylon.add(gammaValue.set("gamma",0,0,1));
        gui_pylon.add(lightSourcePreset.set("lightSourcePreset",0,0,3));
        
        gui_pylon.loadFromFile("gui_pylon.xml");
        
        old_autoExposure = autoExposure;
        old_exposureValue = exposureValue; //-1;
        
        old_autoGain = autoGain;
        old_gainValue = gainValue; //-1;
        old_gammaValue = gammaValue;
        old_lightSourcePreset = lightSourcePreset;
        
        
        //--recorder
        fileExt = ".mov"; // ffmpeg uses the extension to determine the container type. run 'ffmpeg -formats' to see supported formats
        
        // override the default codecs if you like
        // run 'ffmpeg -codecs' to find out what your implementation supports (or -formats on some older versions)
        //ffmpeg -i test.mp4 -vcodec hap -format hap_q out.mov
        /*
         vidRecorder.setVideoCodec("mpeg4"); //prores_ks"); //Vidvox Hap");//
         vidRecorder.setVideoBitrate("800k");//"20000k");//"
         */
        
        cout<<"check if HAP is listed at end when running this terminal script ffmpeg -encoders | grep hap"<<endl;
        cout<<"install HAP codec by upgrading fffmpeg and/or brew install ffmpeg --with-snappy"<<endl;
        
        /*
         what works
         
         //slow in saving. 3sec recroding at 120fps, results in 20 sec file about 160 MB
         #define CAM_WIDTH 1280
         #define CAM_HEIGHT 720
         
         #define CAM_WIDTH  1920
         #define CAM_HEIGHT 1200
         Apple ProRes
         vidRecorder.setVideoCodec("prores_ks"); //Vidvox Hap"); //prores_ks"); //Vidvox Hap");//
         vidRecorder.setVideoBitrate("800k");
         
         //fast in saving. 3sec recroding at 120fps, results in 13 sec file about 2.3 GB
         #define CAM_WIDTH  1920
         #define CAM_HEIGHT 1080
         vidRecorder.setVideoCodec("rawvideo");
         vidRecorder.setVideoBitrate("800k");
         
         //fast in saving. 3sec recording at 120fps, results in 20 sec file about 1 GB
         #define CAM_WIDTH  960
         #define CAM_HEIGHT 600
         vidRecorder.setVideoCodec("rawvideo");
         vidRecorder.setVideoBitrate("800k");
         vidRecorder.setOutputPixelFormat("yuyv422");
         
         //medium fast encoding, 3cec rec at 120fps, results in 10 sec file 1.1MB small
         #define CAM_WIDTH  1920
         #define CAM_HEIGHT 1200
         vidRecorder.setVideoCodec("libx264");
         vidRecorder.setVideoBitrate("800k");//"9000k");//
         vidRecorder.setOutputPixelFormat("yuv420p");
         
         */
        //libx264rgb
        //        vidRecorder.setVideoCodec("Vidvox Hap"); //Vidvox Hap"); //prores_ks"); //Vidvox Hap");//
        
        vidRecorder.setFfmpegLocation(ofToDataPath("ffmpeg/bin/ffmpeg"));
        vidRecorder.setVideoCodec(rec_codec[recSettingIndex]);
        vidRecorder.setVideoBitrate(rec_bitrate[recSettingIndex]);
#ifdef USE_MONO8
        vidRecorder.setPixelFormat("gray");
#else
        vidRecorder.setPixelFormat("rgb24");
#endif
        vidRecorder.setOutputPixelFormat(rec_outFormat[recSettingIndex]);
        //        vidRecorder.setPixelFormat("rgba");
        //                  vidRecorder.setOutputPixelFormat("yuv420p");
        
        //    vidRecorder.setAudioCodec("mp3");
        //    vidRecorder.setAudioBitrate("192k");
        
        
        ofAddListener(vidRecorder.outputFileCompleteEvent, this, &threaded_baslerCam::recordingComplete);
        
        //        bDoneWithLastRecording = true;
        bRecording = false;
        bRecordingDone = true;
        
        //-----camera
#ifdef USE_MONO8
        pixCam.allocate(CAM_WIDTH,CAM_HEIGHT,OF_PIXELS_GRAY); //OF_PIXELS_GRAY
#else
        pixCam.allocate(CAM_WIDTH,CAM_HEIGHT,OF_PIXELS_RGB); //OF_PIXELS_GRAY
#endif
        texCam.allocate(pixCam);
        
        //MARK::basler camera setup
        
        int exitCode = 0;
        
        // Before using any pylon methods, the pylon runtime must be initialized.
        //        PylonInitialize();
        PylonAutoInitTerm autoInitTerm;
        
        try
        {
            // Allow all the names in the namespace GenApi to be used without qualification.
            using namespace GenApi;
            
            // Create an instant camera object with the first camera device found.
            //            CInstantCamera camera( CTlFactory::GetInstance().CreateFirstDevice());
            camera.Attach(CTlFactory::GetInstance().CreateFirstDevice());
            // Print the model name of the camera.
            cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;
            
            INodeMap&  nodemap = camera.GetNodeMap();
            
            // Open the camera for accessing the parameters.
            camera.Open();
            
            // Get camera device information.
            cout << "Camera Device Information" << endl
            << "=========================" << endl;
            cout << "Vendor           : "
            << CStringPtr( nodemap.GetNode( "DeviceVendorName") )->GetValue() << endl;
            cout << "Model            : "
            << CStringPtr( nodemap.GetNode( "DeviceModelName") )->GetValue() << endl;
            cout << "Firmware version : "
            << CStringPtr( nodemap.GetNode( "DeviceFirmwareVersion") )->GetValue() << endl << endl;
            
            // Camera settings.
            cout << "Camera Device Settings" << endl
            << "======================" << endl;
            
#ifdef USE_MONO8
            // Set pixel format to Mono8 if available.
            if ( GenApi::IsAvailable( camera.PixelFormat.GetEntry(PixelFormat_Mono8)))
            {
                camera.PixelFormat.SetValue(PixelFormat_Mono8);
                cout << "New PixelFormat  : " << camera.PixelFormat.ToString() << " (" << camera.PixelFormat.GetValue() << ")" << endl;
            }
#else
            if ( GenApi::IsAvailable( camera.PixelFormat.GetEntry(PixelFormat_BayerBG8)))
            {
                camera.PixelFormat.SetValue(PixelFormat_BayerBG8);
                cout << "New PixelFormat  : " << camera.PixelFormat.ToString() << " (" << camera.PixelFormat.GetValue() << ")" << endl;
            }
#endif
            
            
            // Set the AOI:
            
            // Get the integer nodes describing the AOI.
            CIntegerPtr offsetX( nodemap.GetNode( "OffsetX"));
            CIntegerPtr offsetY( nodemap.GetNode( "OffsetY"));
            CIntegerPtr width( nodemap.GetNode( "Width"));
            CIntegerPtr height( nodemap.GetNode( "Height"));
            
            
            
            
            // Some properties have restrictions. Use GetInc/GetMin/GetMax to make sure you set a valid value.
            int64_t newWidth = 960;
            newWidth = Adjust(newWidth, width->GetMin(), width->GetMax(), width->GetInc());
            
            int64_t newHeight = 600;
            newHeight = Adjust(newHeight, height->GetMin(), height->GetMax(), height->GetInc());
            
            width->SetValue(newWidth);
            height->SetValue(newHeight);
            
            // On some cameras the offsets are read-only,
            // so we check whether we can write a value. Otherwise, we would get an exception.
            // GenApi has some convenience predicates to check this easily.
            if ( IsWritable( offsetX))
            {
                offsetX->SetValue(offsetX->GetMin() + (offsetX->GetMax() - offsetX->GetMin() ) / 2);
            }
            if ( IsWritable( offsetY))
            {
                offsetY->SetValue(offsetY->GetMin() + (offsetY->GetMax() - offsetY->GetMin() ) / 2);
            }
            
            cout << "OffsetX          : " << offsetX->GetValue() << endl;
            cout << "OffsetY          : " << offsetY->GetValue() << endl;
            
            cout << "Width            : " << width->GetValue() << endl;
            cout << "Height           : " << height->GetValue() << endl;
            
            
            // Set the acquisition mode to Continuous
            camera.AcquisitionMode.SetValue(AcquisitionMode_Continuous);
            // Set the timed exposure mode
            camera.ExposureMode.SetValue(ExposureMode_Timed);
            // Set the exposure time
            //            camera.ExposureTime.SetValue(10000.0);
            camera.ExposureTime.SetValue(10000.0); //3247.0); //10000.0);
            // Set the frame rate
            camera.AcquisitionFrameRate.SetValue(100.0); //300); //100.0);
            // Enable free run mode by setting the trigger mode to Off
            camera.TriggerMode.SetValue(TriggerMode_Off);
            // Start frame capture
            camera.AcquisitionStart.Execute();
            
            ////            camera.AutoFunctionProfile.SetValue(AutoFunctionProfile_AntiFlicker60Hz);
            //            camera.AutoFunctionProfile.SetValue(AutoFunctionProfile_MinimizeGain);
            //            camera.GainAuto.SetValue(GainAuto_Continuous);
            //            camera.ExposureAuto.SetValue(ExposureAuto_Continuous);
            
            // Start the grabbing of c_countOfImagesToGrab images.
            // The camera device is parameterized with a default configuration which
            // sets up free-running continuous acquisition.
            camera.StartGrabbing(); // c_countOfImagesToGrab);
            
            //        // This smart pointer will receive the grab result data.
            //        CGrabResultPtr ptrGrabResult;
            
            // Camera.StopGrabbing() is called automatically by the RetrieveResult() method
            // when c_countOfImagesToGrab images have been retrieved.
            
        }
        catch (const GenericException &e)
        {
            // Error handling.
            cerr << "An exception occurred." << endl
            << e.GetDescription() << endl;
            exitCode = 1;
        }
        
        // Second the xxx` must be parameterized.
        
#ifdef USE_MONO8
        converter.OutputPixelFormat = PixelType_Mono8; //PixelType_RGB8packed; //PixelType_RGB8planar; //PixelType_BayerRG8
#else
        converter.OutputPixelFormat = PixelType_RGB8packed;
#endif
        converter.OutputBitAlignment = OutputBitAlignment_MsbAligned;
        ofLog()<<"done camera setup";
        start();
    }
    
    void init(){
        
        //        start();
    }
    
    void start(){
        startThread();
    }
    void exit(){
        
    }
    void stop(){
        
        
        ofLog()<<"Stop the grabbing.";
        try
        {
            camera.StopGrabbing();
        }
        catch (const GenericException & e)
        {
            cerr << "Failed to set the AOI width. Reason: "
            << e.GetDescription() << endl;
        }
        
        ofLog()<<"close the recorder.";
        try
        {
            ofRemoveListener(vidRecorder.outputFileCompleteEvent, this, &threaded_baslerCam::recordingComplete);
            vidRecorder.close();
            vidRecorder.stopThread();
        }
        catch (const GenericException & e)
        {
            cerr << "Failed to set the AOI width. Reason: "
            << e.GetDescription() << endl;
        }
        //        camera.close(); //after reading or writing paramters to camera
        
        sleep(100);
        
        //    if (camera.IsGrabbing() == false){
        ofLog()<<"PylonTerminate";
        //        PylonTerminate();
        //    }
        
        std::unique_lock<std::mutex> lck(mutex);
        stopThread();
        condition.notify_all();
    }
    
    void threadedFunction(){
        
        while(isThreadRunning()) {
            
            
            
            threadFrameNum++;
            //            ofLog()<<"threadFrameNum "<<threadFrameNum;
            
            
            if ( camera.IsGrabbing()) {
                //                std::unique_lock<std::mutex> lock(mutex);
                
                // Wait for an image and then retrieve it. A timeout of 5000 ms is used.
                camera.RetrieveResult( -1, ptrGrabResult, TimeoutHandling_ThrowException);
                // Image grabbed successfully?
                if (ptrGrabResult->GrabSucceeded()){
                    cameraTimer.tick();
                    
                    if(ofGetElapsedTimef() - logTimer > 10){
                        logTimer = ofGetElapsedTimef();
                        ofBuffer buf;
                        buf.set(ofGetTimestampString());
                        ofBufferToFile("runLogs/heartbeat.txt", buf);
                        ofLog()<<"write to runLogs/heartbeat.txt";

                    }
                    //                    ofLog()<<"cameraTimer.getFramerate() "<<cameraTimer.getFramerate();
                    // Access the image data.
                    //                    cout << "SizeX: " << ptrGrabResult->GetWidth() << endl;
                    //                    cout << "SizeY: " << ptrGrabResult->GetHeight() << endl;
                    const uint8_t *pImageBuffer = (uint8_t *) ptrGrabResult->GetBuffer();
                    //                    cout << "Gray value of first pixel: " << (uint32_t) pImageBuffer[0] << endl << endl;
                    
                    
                    // Now we can check if conversion is required.
                    if ( converter.ImageHasDestinationFormat( ptrGrabResult))
                    {
//                        ofLog()<<"No conversion is needed. It can be skipped for saving processing";
                        // time.
                        //                ShowImage( ptrGrabResult, "Grabbed image.");
                        //                        texCam.loadData(pImageBuffer,ptrGrabResult->GetWidth(),ptrGrabResult->GetHeight(),GL_RGB); //GL_LUMINANCE);
                        //                        pixCam.allocate(ptrGrabResult->GetWidth(),ptrGrabResult->GetHeight(),OF_IMAGE_COLOR);
                        
                        //
#ifdef USE_MONO8
                        pixCam.setFromExternalPixels((unsigned char *)pImageBuffer, CAM_WIDTH, CAM_HEIGHT, 1);
#else
                        pixCam.setFromExternalPixels((unsigned char *)pImageBuffer, CAM_WIDTH, CAM_HEIGHT, 3);
#endif
                    }
                    else
                    {
                        //                        ofLog()<<" Conversion is needed.";
                        //                ShowImage( ptrGrabResult, "Grabbed image.");
                        // Create a target image
                        CPylonImage targetImage;
                        converter.Convert( targetImage, ptrGrabResult);
                        const uint8_t *pImageBuffer2 = (uint8_t *) targetImage.GetBuffer();
                        
                        //                        texCam.loadData(pImageBuffer2,ptrGrabResult->GetWidth(),ptrGrabResult->GetHeight(),GL_RGB); //GL_LUMINANCE);
#ifdef USE_MONO8
                        pixCam.setFromExternalPixels((unsigned char *)pImageBuffer2, CAM_WIDTH, CAM_HEIGHT, 1);
#else
                        pixCam.setFromExternalPixels((unsigned char *)pImageBuffer2, CAM_WIDTH, CAM_HEIGHT, 3);
#endif
                    }
                    
                    pixCam.mirror(bFlip,false); //bMirror handled by ofApp
                    
                    if(bRecording){
                        bool success = vidRecorder.addFrame(pixCam);
                        if (!success) {
                            ofLogWarning("This frame was not added!");
                        }
                        
                        float temp_dur = ofGetElapsedTimef() - recTimer;
                        if(temp_dur >= recDuration ){
                            
                            bRecording = false;
                            
                            //                            bRecordingDone = true;
                            ofLog()<<"stop video recording after "<<temp_dur<<" seconds";
                            ofLog()<<"ofGetElapsedTimef() "<<ofGetElapsedTimef();
                            //                        outputVideo.release();
                            vidRecorder.close();
                            
                        }
                    }
                    
                }else{
                    cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
                }
                
                //                 condition.wait(lock);
            }//end if ( camera.IsGrabbing())
            
            // Check if the video recorder encountered any error while writing video frame or audio smaples.
            if (vidRecorder.hasVideoError()) {
                ofLogWarning("The video recorder failed to write some frames!");
            }
            
            //           yield();
            
            
        }//end  while(isThreadRunning())
    }//end  void threadedFunction()
    
    void update(){
        
        ofLog()<<"cam update()";
        std::unique_lock<std::mutex> lock(mutex);
        
        threadedFunction();
        
        //        toOf(outputMat,pixCam);
        ofLog()<<"pixCam "<<pixCam.getWidth();
        texCam.loadData(pixCam);
        
        
        condition.notify_all();
        
    }
    
    void updateNoLock(){
        // we don't lock here so we will see
        // tearing as the thread will update
        // the pixels while we upload them to the texture
        
        //        checkGui();
        if(pixCam.isAllocated()){
            //            ofLog()<<"pixCam "<<pixCam.size();
            texCam.loadData(pixCam);
        }
        condition.notify_all();
    }
    
    void drawLiveVideo(int _x = 0, int _y = 0){
        ofSetColor(255);
        
        if(texCam.isAllocated()){
            texCam.draw(_x,_y);
        }
        //        outputImg.draw(_x + CAM_WIDTH,_y);
        
        ofNoFill();
        ofSetLineWidth(1);
        ofDrawRectangle(_x, _y, CAM_WIDTH, CAM_HEIGHT);
        
        
    }
    
    void drawLiveVideoDebug(int _x = 0, int _y = 0){
        //        ofSetColor(255);
        //        cam.draw(0,0,320,240);
        ofSetColor(255);
        if(texCam.isAllocated()){
            texCam.draw(_x,_y);
        }
        //        drawMat(outputMat, _x, _y);
        
        ofPushStyle();
        ofNoFill();
        ofSetLineWidth(2);
        ofDrawRectangle(roiRect);
        ofDrawRectangle(_x, _y, CAM_WIDTH, CAM_HEIGHT);
        ofPopStyle();
        
        stringstream ss;
        ss << "video queue size: " << vidRecorder.getVideoQueueSize() << endl
        << "FPS: " << ofGetFrameRate() << endl;
        
        ofSetColor(0,0,0,100);
        ofDrawRectangle(0, 0, 260, 75);
        ofSetColor(255, 255, 255);
        ofDrawBitmapString(ss.str(),_x, _y);
        
        if(bRecording){
            ofSetColor(255, 0, 0);
            ofDrawCircle(ofGetWidth() - 20, 20, 15);
        }
        
        //        arial.drawString("cam fps "+ofToString((int)cameraTimer.getFramerate()), _x, _y);
        //        arial.drawString("cam fps "+ofToString((int)cameraTimer.getFramerate()), _x+100, _y);
        
        //        ofDrawBitmapString("cam fps "+ofToString((int)cameraTimer.getFramerate()), _x+100, _y);
        ofDrawBitmapString("rec codec "+ofToString(rec_codec[recSettingIndex])+ ", bitrate "+ofToString(rec_bitrate[recSettingIndex])+", out format "+ofToString(rec_outFormat[recSettingIndex]), _x+100, _y-100);
        
        
    }
    
    
    void triggerNewRecording(){
        
        ofLog()<<"----------------------triggerNewRecording()----------------------";
        
        //        bRecording = !bRecording;
        bRecording = true;
        
        ofLog()<<"vidRecorder.isInitialized() "<<vidRecorder.isInitialized();
        
        if(!vidRecorder.isInitialized()) {
            std::unique_lock<std::mutex> lock(mutex);
            
            //important for recTimer to be set before calling vidRecorder.setup
            bRecordingDone = false;
            recTimer = ofGetElapsedTimef();
            ofLog()<<"recTimer "<<recTimer;
            
            current_fileName = ofGetTimestampString()+fileExt;
            string filePath = ofToDataPath(localRecordingsFolder+current_fileName);
            string temp_filePath = ofToDataPath(localTempRecordingsFolder+current_fileName);

            vidRecorder.setVideoCodec(rec_codec[recSettingIndex]);
            vidRecorder.setVideoBitrate(rec_bitrate[recSettingIndex]);
#ifdef USE_MONO8
            vidRecorder.setPixelFormat("gray");
#else
            vidRecorder.setPixelFormat("rgb24");
#endif
            vidRecorder.setOutputPixelFormat(rec_outFormat[recSettingIndex]);
            
            
            ofLog()<<"start recording for: "<<filePath;
            //            vidRecorder.setup(fileName+ofGetTimestampString()+fileExt, vidGrabber.getWidth(), vidGrabber.getHeight(), 30, sampleRate, channels);
            
            vidRecorder.setup(filePath, CAM_WIDTH, CAM_HEIGHT, recFPS); // no audio
//             vidRecorder.setup(temp_filePath, CAM_WIDTH, CAM_HEIGHT, recFPS); // no audio
            
            //            vidRecorder.setup(fileName+ofGetTimestampString()+fileExt, 0,0,0, sampleRate, channels); // no video
            //          vidRecorder.setupCustomOutput(vidGrabber.getWidth(), vidGrabber.getHeight(), 30, sampleRate, channels, "-vcodec mpeg4 -b 1600k -acodec mp2 -ab 128k -f mpegts udp://localhost:1234"); // for custom ffmpeg output string (streaming, etc)
            
            
            
            //                fileName = "outcpp_"+ofGetTimestampString()+".avi";
            
            
//            rec_counter++;
//            if(rec_counter > 10){
//                oldest_fileName = past_fileNames[past_fileNames_index];
//            }
            oldest_fileName = past_fileNames[past_fileName_index];
            past_fileNames[past_fileName_index] = current_fileName;
            past_fileName_index ++;
            past_fileName_index %= 10;
            
            // Start recording
            vidRecorder.start();
            
            condition.notify_all();
        }
        //        else if(!bRecording && vidRecorder.isInitialized()) {
        //            vidRecorder.setPaused(true);
        //        }
        //        else if(bRecording && vidRecorder.isInitialized()) {
        //            vidRecorder.setPaused(false);
        //        }
        ofLog()<<"vidRecorder.isInitialized() "<<vidRecorder.isInitialized();
        
        
    }
    
    void checkGui(){
        if(old_autoExposure != autoExposure){
            old_autoExposure =  autoExposure;
            if(autoExposure == false) camera.ExposureAuto.SetValue(ExposureAuto_Off);
            else  camera.ExposureAuto.SetValue(ExposureAuto_Continuous);
            //ExposureAuto_Once
        }
        if(old_exposureValue != exposureValue){
            old_exposureValue = exposureValue;
            
            double temp_exposure_f = ofMap(exposureValue,0,0.99,camera.ExposureTime.GetMin(),camera.ExposureTime.GetMax(),true);
            int64_t temp_exposure_i = Adjust(temp_exposure_f, camera.ExposureTime.GetMin(), camera.ExposureTime.GetMax(), camera.ExposureTime.GetInc());
            camera.ExposureTime.SetValue(camera.ExposureTime.GetMin() + temp_exposure_i);
        }
        
        if(old_autoGain != autoGain){
            old_autoExposure = autoGain;
            if(autoGain == false) camera.GainAuto.SetValue(GainAuto_Off);
            else  camera.GainAuto.SetValue(GainAuto_Continuous);
            //GainAuto_Once
        }
        if(old_gainValue != gainValue){
            old_gainValue = gainValue;
            
            double temp_gain_f = ofMap(gainValue,0,0.99,camera.Gain.GetMin(),camera.Gain.GetMax(),true);
            //            int64_t temp_gain_i = Adjust(temp_gain_f, camera.Gain.GetMin(), camera.Gain.GetMax(), camera.Gain.GetInc());
            temp_gain_f = ofClamp(temp_gain_f,0,12);
            camera.Gain.SetValue(camera.Gain.GetMin() + temp_gain_f);
        }
        
        if(old_gammaValue != gammaValue){
            old_gammaValue = gammaValue;
            
            double temp_gamma_f = ofMap(gammaValue,0,0.99,camera.Gamma.GetMin(),camera.Gamma.GetMax(),true);
            //            int64_t temp_gain_i = Adjust(temp_gain_f, camera.Gain.GetMin(), camera.Gain.GetMax(), camera.Gain.GetInc());
            camera.Gamma.SetValue(camera.Gamma.GetMin() + temp_gamma_f);
        }
        if(old_lightSourcePreset != lightSourcePreset){
            old_lightSourcePreset = lightSourcePreset;
            if(lightSourcePreset == 0) camera.LightSourcePreset.SetValue(LightSourcePreset_Off);
            else if(lightSourcePreset == 1) camera.LightSourcePreset.SetValue(LightSourcePreset_Daylight5000K);
            else if(lightSourcePreset == 2) camera.LightSourcePreset.SetValue(LightSourcePreset_Daylight6500K);
            else if(lightSourcePreset == 3) camera.LightSourcePreset.SetValue(LightSourcePreset_Tungsten2800K);
        }
        
    }
    
    void saveGui(){
        gui_pylon.saveToFile("gui_pylon.xml");
    }
    void drawGui(){
        gui_pylon.draw();
    }
    
    int getThreadFrameNum(){
        return threadFrameNum;
    }
    
    ////--------------------------------------------------------------
    void recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args){
        cout << "The recoded video file is now complete." << endl;
        bRecordingDone = true;

        
//        ofDirectory dir;
//        ofLog()<<dir.listDir(localRecordingsFolder);
//        dir.sort();
        
//        for(int i =0; i<dir.size(); i++){
//            ofLog()<<dir.getName(i);
//        }
//        if(dir.size() < 9){
//            ofLog()<<"only "<<dir.size()<<" files present. do not delete anything";
//            return;
//        }
        
        /*
        string _dst = ofToDataPath(localRecordingsFolder+current_fileName,true);
        string _src = ofToDataPath(localTempRecordingsFolder+current_fileName,true);
        
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
        
        string oldest_filePath = localRecordingsFolder+oldest_fileName; //dir.size()-1); // "2018-04-24-22-39-52-061.mov";
        
        if( remove(oldest_filePath.c_str()) != 0 ){
            ofLog()<<"Error deleting file "<<oldest_filePath;
        }
        else{
            ofLog()<<"File deleted "<<oldest_filePath;
        }
    }
    
protected:
    std::condition_variable condition;
    
    ofPixels pixCam;
    ofTexture texCam;
    
    int threadFrameNum = 0;
    
    
    float recTimer;
};

#endif /* threaded_baslerCam_h */
