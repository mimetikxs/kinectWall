#include "ofApp.h"

using namespace ofxCv;
using namespace cv;


// testing helper to color gui
void setPanelColor(ofxPanel& panel, ofColor bright){
    ofColor mid(bright);
    ofColor dark(bright);
    mid.setBrightness(100);
    dark.setBrightness(30);
    panel.setDefaultHeaderBackgroundColor(bright);
    panel.setDefaultFillColor(mid);
    panel.setDefaultBackgroundColor(dark);
}


ofApp::ofApp(){
}


void ofApp::exit(){
    layout.saveLayout(PATH_LAYOUT);
//    presetsPhysics.save();
//    presetsContours.save();
    osc.saveOscMapping();
    saveHomography();
}

//--------------------------------------------------------------
void ofApp::setup(){
    
     TIME_SAMPLE_SET_FRAMERATE(30.0f);
    
    ofSetFrameRate(30);
    ofSetVerticalSync(true);
    //ofSetLogLevel(OF_LOG_VERBOSE);
    //ofEnableAntiAliasing();
    ofBackground(20, 20, 20);
	ofSetLogLevel(ofxOpenNI::LOG_NAME,OF_LOG_VERBOSE);
	ofSetLogLevel(ofxOpenNICapture::LOG_NAME,OF_LOG_VERBOSE);
    
    // kinect
	bool live = false;
	if(live){
		kinect.setupFromXML("openni/config/ofxopenni_config.xml",true);
	}else{
		kinect.setupFromRecording("recording_tania.oni", true);
	}
    kinect.setUseTexture(false);
    kinect.enableCalibratedRGBDepth();
    
    // viewports (unscaled)
    viewportProjector.set(0,0, PROJECTOR_W, PROJECTOR_H);
    viewporKinect.set(PROJECTOR_W+80,80, KINECT_W, KINECT_H);
    
    // kinect threshold
    stepKinect = 2;
    pixelsThresh.allocate(KINECT_W / stepKinect, KINECT_H / stepKinect, 1);
    
    // warped (projector downsampled)
    // used to calculate optical flow
    wrapedScale = 0.25;
    
    // layers
    fboPhysics.allocate(viewportProjector.width, viewportProjector.height, GL_RGB32F);
    fboPhysics.begin();
    ofClear(0);
    fboPhysics.end();
    
    fboContours.allocate(viewportProjector.width, viewportProjector.height, GL_RGBA);
    fboContours.begin();
    ofClear(0);
    fboContours.end();
    
    // physics
    ofPoint screenCenter(PROJECTOR_W/2,PROJECTOR_H/2);
    emitterBlob.setName("blob");
    emitterBlob.position = screenCenter;
    
    // store for mouse interaction
    physicsActors.push_back(&emitterBlob);
    pickedActor = NULL;

    physics.setup(viewportProjector);
    physics.addForceField(&forceField);
    physics.addEmitter(&emitterBlob);
    
    physicsRender.setup(&physics.getParticleData());
    
    // gui
    buildGui();
    
    // homography
    homographySrc.setup(viewporKinect, "source");
    homographyDst.setup(viewportProjector, "destination");
    loadHomography();
    HomographyHelper::findHomography(homographySrc.getCorners(), homographyDst.getCorners(), homography);
    
    
    // plane to bind textures to
    ofPlanePrimitive plane(1,1, 2,2); // plane coords are 0-1
    planeMesh = plane.getMesh();
    // now move the vertices so topleft is at 0,0 and botomright at 1,1
    for(int i=0; i<planeMesh.getNumVertices(); i++){
        ofVec3f tmp = planeMesh.getVertex(i);
        tmp += 0.5;
        planeMesh.setVertex(i, tmp);
    }
    
    // flag
    bRoiDirty = true;
    bAddFlow = true;
    doCv = false;
}


//--------------------------------------------------------------
void ofApp::refreshRoiWraped(){
    // update roi (these are percentages)
    roiProjetor.x      = left;
    roiProjetor.y      = top;
    roiProjetor.width  = right - left;
    roiProjetor.height = bottom - top;
    
    // projector scaled down
    float w = roiProjetor.width * PROJECTOR_W * wrapedScale;
    float h = roiProjetor.height * PROJECTOR_H * wrapedScale;
    
    // allocate the wraped fbo
    fboWarped.allocate(w, h, GL_RGB);
    texDilatedEroded.allocate(w,h,GL_RGB);
    
    // force field
    forceField.setup(w, h, 1/wrapedScale);
    forceField.position = roiProjetor.position * ofPoint(PROJECTOR_W, PROJECTOR_H);
    texForcefield.allocate(w, h, GL_RGB32F); // debug
    
    
    
    // ~~~~~~~~~~~~~
    // testing fluid
    // the bigger the slower
    // the float value is for resolution
    // the boolean defines when to use 32bit textures
    fluid.allocate(w, h, 0.7, true);
    fluid.setGravity(ofPoint());
    fluid.setUseObstacles(false);
    
    // testing frame differencing
    grey.allocate(w, h, 1);
    imitate(previous, grey);
    imitate(diff, grey);
    
    imitate(floatDiff, grey, CV_32F);
    imitate(floatBuffer, grey, CV_32F);
    
    // used to store the fluid velicities
    fboFluidVel.allocate(w, h, GL_RGB32F);
    
    // this is the buffer for the fluid's color
    fboColor.allocate(w, h);
    // ~~~~~~~~~~~~~
    
    
    
    // ~~~~
    // update transformation
    // scale of the coords from pixels coords to normalized
    ofMatrix4x4 scale;
    scale.makeScaleMatrix(pixelsWraped.getWidth(), pixelsWraped.getHeight(), 1);
    // roi transformation (from normalized to global)
    ofMatrix4x4 roiTransform;
    roiTransform.postMultScale(roiProjetor.width * PROJECTOR_W, roiProjetor.height * PROJECTOR_H, 1);
    roiTransform.postMultTranslate(roiProjetor.x * PROJECTOR_W, roiProjetor.y * PROJECTOR_H, 0);
    //
    transformWarpToProjector.makeIdentityMatrix();
    transformWarpToProjector.postMult(scale.getInverse());  // first transformation: to normalized space
    transformWarpToProjector.postMult(roiTransform);        // second: scale to projector space
    
    bRoiDirty = false;
}


//--------------------------------------------------------------
void ofApp::update(){
    if(bRoiDirty){
        refreshRoiWraped();
    }
    
	kinect.update();
    
    // get thresholded
    ofShortPixels& depth = kinect.getDepthRawPixels();
    int h = depth.getHeight();
    int w = depth.getWidth();
    int i = 0;  // thresh pixels index
    for(int y=0; y<h; y+=stepKinect){
        int yw = y * w;
        for(int x=0; x<w; x+=stepKinect){
            unsigned short & depthVal = depth[ x + yw ];
            pixelsThresh[i] = (depthVal>thresMin && depthVal<thresMax) ? 255 : 0;
            i++;
        }
    }
    texThresh.loadData(pixelsThresh);
    
    // find homography
    if(bShowHomography){
        HomographyHelper::findHomography(homographySrc.getCorners(), homographyDst.getCorners(), homography);
    }
    
    // warped thresholded image
    fboWarped.begin();
        ofClear(0);
    
        ofPushMatrix();
        ofScale(viewportProjector.getWidth() * wrapedScale,
                viewportProjector.getHeight() * wrapedScale);
        ofTranslate(-roiProjetor.getPositionRef());
        ofMultMatrix(homography);
    
        ofEnableNormalizedTexCoords();
        texThresh.bind();
        planeMesh.draw();
        texThresh.unbind();
        ofDisableNormalizedTexCoords();
    
        ofPopMatrix();
    fboWarped.end();
    
    TS_START("readWrapedFbo");
    //fboWarped.readToPixels(pixelsWraped);
    reader.readToPixels(fboWarped, pixelsWraped);
    TS_STOP("readWrapedFbo");

    // contours and center
    contourFinder.findContours(pixelsWraped);
    
    // polyline render
    fboContours.begin();
        ofClear(0);
        ofPushMatrix();
        ofMultMatrix(transformWarpToProjector);
        contourFinder.drawFill();
        ofPopMatrix();
    fboContours.end();
    
    
    // optical flow
    unsigned long long current = ofGetElapsedTimeMillis();
    //if(current - last > 33.){ // update optical flow at ~30fps
        flow.calculateFlow(pixelsWraped);
        last = current;
    flow.update();
  
    
    
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // testing fluid:
    ofFloatPixels & flowPixels = flow.getFlowPixelsRef();
    pixelsVel.setFromPixels(flowPixels.getPixels(), flowPixels.getWidth(), flowPixels.getHeight(), flowPixels.getNumChannels());
    for(int i=0; i<pixelsVel.size(); i++){
        // cout << test[i] << endl;
        // when realocating (change of size) we can get NaN values from the flow
        // we need to set the value to a float if we don't wan't this error to spread
        if (isnan(pixelsVel[i])) {
            pixelsVel[i] = 0.5f;
        }else{
            // remap flow velocity value range from [-1.0 .. 1.0] to [0.0 .. 1.0]
            pixelsVel[i] *= 0.5;
            pixelsVel[i] += 0.5;
        }
    }
    texVel.loadData(pixelsVel, GL_RG);
    
    // frame differencing
    copyGray(pixelsWraped, grey);
    absdiff(previous, grey, diff);
    diff.update();
    copy(grey, previous);   // like ofSetPixels, but more concise and cross-toolkit

    // erosion dilation (cpu)
    ofxCv::imitate(pixDilated, diff);
    ofxCv::imitate(pixEroded, diff);
    erode(diff.getPixelsRef(), pixEroded);
    dilate(pixEroded, pixDilated);
    texDilatedEroded.loadData(pixDilated);
    
    // char pixels to float
    floatDiff = pixDilated;
    
    // add to the buffer
    for(int i=0; i<floatBuffer.size(); i++){
        floatBuffer[i] *= clearPct;           // slowly fade to zero
        floatBuffer[i] += floatDiff[i];   // accumulate
    }
    texBuffer.loadData(floatBuffer); 
    
    // colorize the blob
    fboColor.begin();
    ofClear(0);
    ofPushStyle();
    //ofSetColor(60, 120, 255); //ofSetColor(20, 80, 255); //ofSetColor(255, 80, 20);
    //int r = ofMap( sin(ofGetElapsedTimef()), -1.0, 1.0, 0, 255);
    //int g = ofMap( 0.5+sin(ofGetElapsedTimef()*1.2), -1.0, 1.0, 0, 255);
    //ofSetColor(r, g, 0);
    ofSetColor(fluidColor);
    texBuffer.draw(0,0, fboColor.getWidth(), fboColor.getHeight());
    ofPopStyle();
    fboColor.end();
    
    fluid.setGravity(ofPoint(0,-gravity));
    fluid.dissipation = disipation;
    fluid.velocityDissipation = velDisipation;
    fluid.addColor(fboColor.getTextureReference(), inkPct);
    fluid.addVelocity(texVel, velPct);
    fluid.update();
    
    // force field (updated with fluid velocity)
    
    if(doCv){
        fboFluidVel.begin();
        ofClear(0);
        fluid.drawVelocity(0,0,fboFluidVel.getWidth(), fboFluidVel.getHeight());
        fboFluidVel.end();
        
        // TODO: optimize, this is really slow!
        TS_START("read_fluid_vel");
        pixelsFluidVel.allocate(fboFluidVel.getWidth(), fboFluidVel.getHeight(), 2);
        fboFluidVel.readToPixels(pixelsFluidVel);
        //reader2.readToPixels(fboFluidVel, pixelsFluidVel); // does not work with floatPixels or float textures :(
        TS_STOP("read_fluid_vel");
        
        TS_START("fluid_vel_pixels_to_float");
        test.allocate(fboFluidVel.getWidth(), fboFluidVel.getHeight(), 2);
        float* pixels = test.getPixels();
        int j = 0;
        for(int i=0; i<pixelsFluidVel.size(); i+=3, j+=2){
            pixels[j]   = pixelsFluidVel[i];
            pixels[j+1] = pixelsFluidVel[i+1];
        }
        TS_STOP("fluid_vel_pixels_to_float");
        
        
        forceField.addFlow(test);
        texForcefield.loadData(forceField.getFieldPixels(), GL_RG);
    }
    
    // physics
    
    // update point emmiter
    // only emmit on areas with movement
    updateBlobPoints(floatBuffer, 2, minThresh, maxThresh);
    
    physics.update();
    physicsRender.update();
    
    // draw physics
    ofDisableDepthTest();
    fboPhysics.begin();
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        ofFill();
        ofSetColor(0,0,0, fadeAmnt);
        ofRect(0,0,fboPhysics.getWidth(),fboPhysics.getHeight());
        ofNoFill();
        ofSetColor(255,255,255);
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        if(physics.enabled){
            physicsRender.draw();
        }
        ofDisableBlendMode();
    fboPhysics.end();
}


//--------------------------------------------------------------
void ofApp::erode(ofPixels& _src, ofPixels& _dst){
    if(erodeRadius > 0){
        //erosion (MORPH_RECT, MORPH_CROSS, MORPH_ELLIPSE)
        int size = ofxCv::forceOdd(erodeRadius);
        Mat element = getStructuringElement(MORPH_RECT,
                                            cv::Size( 2*size+1, 2*size+1 ),
                                            cv::Point( size, size ) );
        Mat srcMat = toCv(_src);
        Mat dstMat = toCv(_dst);
        cv::erode( srcMat, dstMat, element );
    }else{
        // bypass
        copy(_src, _dst);
    }
}


//--------------------------------------------------------------
void ofApp::dilate(ofPixels &_src, ofPixels &_dst){
    if(dilateRadius > 0){
        int size = ofxCv::forceOdd(dilateRadius);
        Mat element = getStructuringElement(MORPH_RECT,
                                            cv::Size( 2*size+1, 2*size+1 ),
                                            cv::Point( size, size ) );
        Mat srcMat = toCv(_src);
        Mat dstMat = toCv(_dst);
        cv::dilate(srcMat, dstMat, element);
    }else{
        // bypass
        copy(_src, _dst);
    }
}


//--------------------------------------------------------------
void ofApp::updateBlobPoints(ofPixels pixels, int step, int min, int max){
    emitterBlob.clear();
    int w = pixels.getWidth();
    int h = pixels.getHeight();
    int channels = pixels.getNumChannels();
    for(int x=0; x<w; x+=step){
        for(int y=0; y<h; y+=step){
            int i = (x + y * w) * channels;
            if(pixels[i] > min  && pixels[i] < max){
                emitterBlob.setPoint(ofPoint(x,y) * transformWarpToProjector);
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    // physics
    fboPhysics.draw(viewportProjector);
    
    // fluid
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    // projector scaled down
    float x = roiProjetor.x * PROJECTOR_W;
    float y = roiProjetor.y * PROJECTOR_H;
    float w = roiProjetor.width * PROJECTOR_W;
    float h = roiProjetor.height * PROJECTOR_H;
    fluid.draw(x, y, w, h);
    //ofDisableBlendMode();
    
    // contour
    //ofEnableBlendMode(OF_BLENDMODE_ADD);
    fboContours.draw(0,0);
    ofDisableBlendMode();
    
    drawDebug();
    
    if(bDrawGui){
        drawGui();
    }
}


//--------------------------------------------------------------
void ofApp::drawDebug(){
    if(bDrawDebugPhysics){
        ofNoFill();
        ofPushStyle();
        for (int i=0; i<physicsActors.size(); i++) {
            physicsActors[i]->draw();
        }
        ofPopStyle();
        // attractor and emitter draw strings that mess up the blend mode
        // to correct this we need to disable after drawing them
        ofDisableBlendMode();
    }
    
    // maps
    if(bDrawDebugMaps){
        float padding = 20;
        ofPoint pos(padding, padding);
        fboWarped.draw(pos);
        pos.x += fboWarped.getWidth() + padding;
        //texDilatedEroded.draw(pos);
        //diff.draw(pos);
        texBuffer.draw(pos);
        pos.x += diff.getWidth() + padding;
        texForcefield.draw(pos);
        //fboFluidVel.draw(pos);
    }
    
    
    ofPushMatrix();
    ofMultMatrix(transformWarpToProjector);
    
    // projector warped preview
    if(bShowHomography){
        fboWarped.draw(0,0);
    }
    
    // contours
    if(bDrawDebugContours){
        ofPushStyle();
        ofNoFill();
        ofSetColor(0,255,255);
        contourFinder.drawLine();
        contourFinder.drawCenter();
        ofPopStyle();
    }
    
    ofPopMatrix();
    
    // force field
    if(bDrawDebugFieldVectors){
        ofPushStyle();
        ofSetColor(255);
        forceField.drawVectors();
        ofPopStyle();
    }
    
    // homgraphy windows
    if(bShowHomography){
        // kinect input preview
        texThresh.draw(viewporKinect);
        homographySrc.draw();
        homographyDst.draw();
    }
    
    // roi
    if(bDrawDebugRois){
        ofPushStyle();
        ofFill();
        ofSetColor(ofColor(0,255,255, 100));
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        
        ofPushMatrix();
        ofScale(viewportProjector.getWidth(), viewportProjector.getHeight());
        ofRect(roiProjetor);
        ofPopMatrix();
        ofDisableBlendMode();
        ofPopStyle();
    }
    
    //
    ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()),PROJECTOR_W+20,20);
    ofDrawBitmapStringHighlight(ofToString(physics.getParticleData().countAlive) + "/" + ofToString(physics.getParticleData().count),PROJECTOR_W+20,40);
    ofDrawBitmapStringHighlight((doCv)?"(d) cv enabled":"(d) cv disabled",PROJECTOR_W+20,60);
}


//--------------------------------------------------------------
void ofApp::drawGui(){
    panelManager.draw();
    if(bShowPanelDebug) panelDebug.draw();
    if(bShowPanelPhysics) panelPhysics.draw();
    if(bShowPanelFlow) panelFlow.draw();
    if(bShowPanelField) panelForceField.draw();
    if(bShowPanelContours) panelContours.draw();
    if(bShowPanelInput) panelInput.draw();
    if(bShowPanelLayer) panelLayer.draw();

    panelFluid.draw();
    panelFrameDiff.draw();
    
    presetsPhysics.draw();
    presetsContours.draw();
    presetsFluid.draw();
}


//--------------------------------------------------------------
void ofApp::buildGui(){
    ofxPanel::setDefaultWidth(250);
    
    // NOTE: when saving, all guiPanels have "settings.xml" by default
    // setting the file name on the constructors aims to prevent accidental overriding
    
    groupRoi.setName("roi");
    groupRoi.add(top.set("top.", 0, 0, 1));
    groupRoi.add(bottom.set("bottom.", 1, 0, 1));
    groupRoi.add(left.set("left.", 0, 0, 1));
    groupRoi.add(right.set("right.", 1, 0, 1));
    btnApplyRoi.setup("apply roi");
    btnApplyRoi.addListener(this, &ofApp::applyRoi);
    groupBlobCleaning.setName("blob cleaning");
    groupBlobCleaning.add(thresMin.set("thresh min", 1000, 1000, 8000));
    groupBlobCleaning.add(thresMax.set("thresh max", 3000, 2000, 8000));
    groupBlobCleaning.add(erodeRadius.set("erosion radius", 0, 0, 5));
    groupBlobCleaning.add(dilateRadius.set("dilation radius", 0, 0, 5));
    panelInput.setup("input", "panelInput.xml");
    panelInput.add(groupRoi);
    panelInput.add(&btnApplyRoi);
    panelInput.add(groupBlobCleaning);
    
    // optical flow
    panelFlow.setup("optical flow", "panelFlow.xml");
    panelFlow.add(flow.parameters);
    //panelFlow.add(frameSkip.set("time skip", 2, 1, 30)); // TODO: rename sampleMilis
    
    // force field
    panelForceField.setup("force field", "panelForceField.xml");
    panelForceField.add(forceField.parameters);
    
    // contours
    panelContours.setup("contours", "panelContours.xml");
    panelContours.add(contourFinder.parameters);
    
    // physics
    panelPhysics.setup("physics", "panelPhysics.xml");
    groupPhysics.setName("physics");
    groupPhysics.add(physics.parameters);
    for(int i=0; i<physicsActors.size(); i++){
        groupPhysics.add(physicsActors[i]->parameters);
    }
    groupPhysics.add(physicsRender.parameters);
    panelPhysics.add(groupPhysics);
    panelPhysics.getGroup("physics").minimizeAll();
    
    // frame diff
    panelFrameDiff.setup("frame diff", "panelFrameDiff.xml");
    panelFrameDiff.add(clearPct.set("clear pct", 0.85, 0.0, 1.0));
    panelFrameDiff.add(minThresh.set("thresh min", 0, 0, 255));
    panelFrameDiff.add(maxThresh.set("thresh max", 255, 0, 255));
    
    // fluid
    panelFluid.setup("fluid", "panelFluid.xml");
    panelFluid.add(inkPct.set("ink pct", 0.1, 0.0, 1.0));
    panelFluid.add(velPct.set("vel pct", 0.1, 0.0, 1.0));
    panelFluid.add(disipation.set("disipation", 0.0, 0.0, 1.0));
    panelFluid.add(velDisipation.set("vel disipation", 0.0, 0.0, 1.0));
    panelFluid.add(gravity.set("gravity", 0.01, 0.0, 5.0));
    panelFluid.add(fluidColor.set("color", ofColor(255,100,0), ofColor(0,0,0), ofColor(255,255,255)));
    
    // layer (viewport)
    panelLayer.setup("layer", "panelLayer.xml");
    panelLayer.add(fadeAmnt.set("trails", 200.0, 0.0, 255.0));
    
    // debug
    panelDebug.setup("debug", "panelDebug.xml");
    panelDebug.add(bDrawDebugPhysics.set("physics", false));
    panelDebug.add(bDrawDebugContours.set("contours", false));
    panelDebug.add(bDrawDebugMaps.set("optical flow maps", false));
    panelDebug.add(bDrawDebugFieldVectors.set("force field vectors", false));
    panelDebug.add(bDrawDebugRois.set("roi", false));
    
    // manager
    panelManager.setup("panel manager", "panelManager.xml");
    panelManager.add(bShowPanelDebug.set("debug", false));
    panelManager.add(bShowPanelPhysics.set("physics", false));
    panelManager.add(bShowPanelFlow.set("optical flow", false));
    panelManager.add(bShowPanelField.set("force field", false));
    panelManager.add(bShowPanelContours.set("contours", false));
    panelManager.add(bShowPanelInput.set("input", false));
    panelManager.add(bShowPanelLayer.set("layer", false));
    panelManager.add(bShowHomography.set("homography", false));
    bShowHomography.addListener(this, &ofApp::onShowHomographyChange);
    
    // ~~~~~~~~~~~~~~
    // presets
    
    // presets physics
    // group all relevant parameters
    groupPhysicsPreset.setName("physics parameters");
    groupPhysicsPreset.add(forceField.parameters);
    groupPhysicsPreset.add(groupPhysics);
    groupPhysicsPreset.add(fadeAmnt);
    
    presetsPhysics.setup("presets physics", 10);
    presetsPhysics.setParameters(&groupPhysicsPreset);
    presetsPhysics.load();
    
    // presets contours
    groupContoursPreset.setName("polyline presets");
    groupContoursPreset.add(contourFinder.parameters);
    
    presetsContours.setup("presets polylines", 5);
    presetsContours.setParameters(&groupContoursPreset);
    presetsContours.load();
    
    // presets fluid
    groupFluidPreset.setName("fluid parameters");
    groupFluidPreset.add(inkPct);
    groupFluidPreset.add(velPct);
    groupFluidPreset.add(disipation);
    groupFluidPreset.add(velDisipation);
    groupFluidPreset.add(gravity);
    groupFluidPreset.add(fluidColor);
    
    presetsFluid.setup("presets fluid", 10);
    presetsFluid.setParameters(&groupFluidPreset);
    presetsFluid.load();
    
    // ~~~~~~~~~~~~~~
    // testing:
    // load  saved parameters
    // TODO: remove the panels and parameters that are in a preset panel
    // otherwise they'll override the preset's values
    panelInput.loadFromFile("panelInput.xml");
    panelFlow.loadFromFile("panelFlow.xml");
    panelForceField.loadFromFile("panelForceField.xml");
    panelContours.loadFromFile("panelContours.xml");
    panelFluid.loadFromFile("panelFluid.xml");
    // ~~~~~~~~~~~~~~
    
    // add panels to layout manager
    layout.addPanel(&panelInput);
    layout.addPanel(&panelFlow);
    layout.addPanel(&panelForceField);
    layout.addPanel(&panelContours);
    layout.addPanel(&panelPhysics);
    layout.addPanel(&panelLayer);
    layout.addPanel(&panelDebug);
    layout.addPanel(&panelManager);
    layout.addPanel(&panelFrameDiff);
    layout.addPanel(&panelFluid);
    layout.addPanel(&presetsPhysics);
    layout.addPanel(&presetsContours);
    layout.addPanel(&presetsFluid);
    layout.loadLayout(PATH_LAYOUT);
    
    bDrawGui = true;
    
    
    // osc control
    osc.setup(8000);
    // add the panels to be controlled
    osc.addPanel(&panelPhysics);
    osc.addPanel(&panelForceField);
    osc.addPanel(&panelContours);
    osc.addPanel(presetsPhysics.getPanel());
    osc.addPanel(presetsContours.getPanel());
    osc.addPanel(presetsFluid.getPanel());
    //
    osc.loadOscMapping();
}


//--------------------------------------------------------------
void ofApp::applyRoi() {
    bRoiDirty = true;
}


//--------------------------------------------------------------
void ofApp::onShowHomographyChange(bool& show){
    if(show){
        homographyDst.enableMouse();
        homographySrc.enableMouse();
    }else{
        homographyDst.disableMouse();
        homographySrc.disableMouse();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 'd':
            doCv = !doCv;
            break;
        case 'g':
            bDrawGui = !bDrawGui;
            break;
        case 'f':
            ofToggleFullscreen();
            break;
        case 'x':
            //physics.dumpPositions();
            break;
        case 'c':
            homographySrc.disableMouse();
            homographySrc.enableMouse();
            homographyDst.disableMouse();
            homographyDst.enableMouse();
            break;
        case 's':
            presetsPhysics.save();
            presetsContours.save();
            presetsFluid.save();
            osc.saveOscMapping();
            saveHomography();
            break;
        case 'm':
        case 'M':
            osc.toggleMappingMode();
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if(pickedActor){
        pickedActor->position = ofPoint(x,y);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    ofVec2f mouse(x,y);
    for (int i=0; i<physicsActors.size(); i++) {
        float dist = physicsActors[i]->position.get().distance(mouse);
        if(dist < 30){
            pickedActor = physicsActors[i];
            break;
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    pickedActor = NULL;
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}




void ofApp::loadHomography(){
    ofXml XML;
    
    if(XML.load("homography.xml")){
    
    
    //~~~~~~
    // src
    
    XML.setTo("//src");
    
    //if(XML.exists("point")){
    XML.setTo("point[0]");
    
    int cornerIndex = 0;
    do{
//        string addressName = XML.getValue<string>("address");
//        string controlName = XML.getValue<string>("param");
//        
//        // check if we've loaded a valid mapping
//        // the parameter name loaded from the xml must match
//        // the a control names added through addPanel()
//        if(controls.find(controlName) == controls.end()) {
//            cout << "[ alert ] OscController: loaded control [" << controlName << "] hasen't been added" << endl;
//        }else{
//            addressToName[addressName] = controlName;
//        }
        float x = XML.getValue<float>("x");
        float y = XML.getValue<float>("y");
        
        homographySrc.getCorners()[cornerIndex].set(x, y);
        cornerIndex++;
        
        //cout << "~~~~~~~" << x << " " << y << endl;
    }
    while( XML.setToSibling() ); // go to the next link
    //}
    
    
    
    //~~~~~~
    // dst
    
    XML.reset();
    
    XML.setTo("//dst");
    
    XML.setTo("point[0]");
    
    cornerIndex = 0;
    
    do{
        float x = XML.getValue<float>("x");
        float y = XML.getValue<float>("y");
        float z = XML.getValue<float>("z");
        
        homographyDst.getCorners()[cornerIndex].set(x, y);
        cornerIndex++;
        
        cout << "~~~~~~~" << x << " " << y << endl;
    }
    while( XML.setToSibling() ); // go to the next link
        
    }
}



void ofApp::saveHomography(){
    int num_points = 0;
    
    ofXml XML;
    XML.addChild("map");
    XML.setTo("map");
    
    
    XML.addChild("src");
    
    for(int i=0; i<homographySrc.getCorners().size(); i++){
        ofPoint point = homographySrc.getCorners()[i];
        
        XML.setTo("//src");
        XML.addChild("point");
        XML.setTo("point[" + ofToString(num_points) + "]");
        XML.addValue("x", point.x);
        XML.addValue("y", point.y);
        XML.addValue("z", point.z);
        num_points++;
    }
    
    XML.reset();
    
    XML.addChild("dst");
    
    num_points = 0;
    
    for(int i=0; i<homographyDst.getCorners().size(); i++){
        ofPoint point = homographyDst.getCorners()[i];
        
        XML.setTo("//dst");
        XML.addChild("point");
        XML.setTo("point[" + ofToString(num_points) + "]");
        XML.addValue("x", point.x);
        XML.addValue("y", point.y);
        XML.addValue("z", point.z);
        num_points++;
    }
    
    
    XML.save("homography.xml");
}
