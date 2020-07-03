
//--------------------------------------------------------------
//
//  CS134 - Game Development
//  Please document/comment all of your work !
//  Have Fun !!
//
//  Kevin Smith   10-20-19
//
//  Student Name:   < Rayyan Khan >
//  Date: <date of last version>


#include "ofApp.h"
#include "Util.h"
#include <glm/gtx/intersect.hpp>
#include <iostream>
#include <algorithm>
#include <vector>






//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup(){

    //Boolean set up
    bWireframe = false;
    bDisplayPoints = false;
    bAltKeyDown = false;
    bCtrlKeyDown = false;
    bLanderLoaded = false;
    bTerrainSelected = true;
    collision = false;
    forwardCol= false;
    backCol = false;
    leftCol = false;
    rightCol = false;
    win = false;
    crash = false;
    astro = false;
    gameStart = false;
    startScreen = true;
    restitution = 0.5;
    checkCol = true;
   
    TreeNode node1;
    
    

    //Set up for all cameras
    cam.setDistance(10);
    cam.setNearClip(.1);
    cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
    ofSetVerticalSync(true);
    cam.disableMouseInput();
    ofEnableSmoothing();
    ofEnableDepthTest();
   
    
    //Set up camera
    trackingCam.setPosition(130,30,130);
    trackingCam.setNearClip(.1);
    onboardCam.lookAt(glm::vec3(lander.getPosition().x,lander.getPosition().y, lander.getPosition().z));
    onboardCam.setNearClip(.1);
    testCam.setNearClip(.1);
    landingCam.setNearClip(.1);
    top.setPosition(0, 25, 0);
    top.lookAt(glm::vec3(0, 0, 0));
    top.setNearClip(.1);
    top.setFov(65.5);   // approx equivalent to 28mm in 35mm format
    theCam = &trackingCam;

    //Set up lights
    keyLight.setup();
    //keyLight.enable();
    keyLight.setAreaLight(1, 1);
    keyLight.setAmbientColor(ofFloatColor(0.3, 0.3, 0.3));
    keyLight.setDiffuseColor(ofFloatColor(0, 205, 0));
    keyLight.setSpecularColor(ofFloatColor(0, 205, 0));
    keyLight.rotate(45, ofVec3f(0, 1, 0));
    keyLight.rotate(-45, ofVec3f(1, 0, 0));
    keyLight.setPosition(10, 10, 10);

    fillLight.setup();
    fillLight.enable();
    fillLight.setSpotlight();
    fillLight.setScale(.05);
    fillLight.setSpotlightCutOff(15);
    fillLight.setAttenuation(2, .001, .001);
    fillLight.setAmbientColor(ofFloatColor(0.3, 0.3, 0.3));
    fillLight.setDiffuseColor(ofFloatColor(0, 25, 0));
    fillLight.setSpecularColor(ofFloatColor(0,25, 0));
    fillLight.rotate(-10, ofVec3f(1, 0, 0));
    fillLight.rotate(-45, ofVec3f(0, 1, 0));
    fillLight.setPosition(0, 0, 0);

    
    
    dynamicLight.setup();
    dynamicLight.enable();
    dynamicLight.setSpotlight();
    dynamicLight.setScale(.05);
    dynamicLight.setSpotlightCutOff(10);
    dynamicLight.setAttenuation(.2, .001, .001);
    dynamicLight.setAmbientColor(ofFloatColor(1, 1, 1));
    dynamicLight.setDiffuseColor(ofFloatColor(1, 1, 1));
    dynamicLight.setSpecularColor(ofFloatColor(1, 1, 1));
    dynamicLight.rotate(180, ofVec3f(0, 1, 0));
    dynamicLight.setPosition((ofVec3f) (lander.getPosition(), lander.getPosition() + 10, lander.getPosition()));
    dynamicLight.rotate(90, ofVec3f(1, 0, 0));
    
    
        //load shader
       ofDisableArbTex();
        
        //load sounds
        thrust.load("thrust.mp3");
        astro1.load("astro.mp3");
       
        // load textures
        if (!ofLoadImage(particleTex, "images/dot.png")) {
            cout << "Particle Texture File: images/dot.png not found" << endl;
            ofExit();
        }

        // load the shader
    #ifdef TARGET_OPENGLES
        shader.load("shaders_gles/shader");
    #else
        shader.load("shaders/shader");
    #endif

    ofSetVerticalSync(true);
    ofEnableSmoothing();
    ofEnableDepthTest();
    // setup rudimentary lighting
    
    initLightingAndMaterials();
    backgroundImage.load("images/starfield-plain.jpg");
    start1.load("start.jpg");

    //load terrain and model
    terrain.loadModel("geo/moon-houdini.obj");
    terrain.setScaleNormalization(false);
    if (lander.loadModel("geo/lander.obj")) {
        lander.setScaleNormalization(false);
       // lander.setScale(.5, .5, .5);
        //lander.setRotation(0, -180, 1, 0, 0);
        lander.setPosition(0, 0, 0);
        landerBox = meshBounds(lander.getMesh(0));
        bLanderLoaded = true;
    }
    else {
        cout << "Error: Can't load model" << "geo/lander.obj" << endl;
        ofExit(0);
    }
    
    //create bounding box
    boundingBox = meshBounds(terrain.getMesh(0));

    
    // create KdTree for terrain
    kdtree.create(terrain.getMesh(0), 10);
    
    
    //set up and create gui
    gui.setup();
    gui.add(levels.setup("level", 1, 1, 50));
    
    
    //set up particle system
    player = new Particle();
    sys = new ParticleSystem();
    player->position = ofVec3f(0,10,0);
   // player->position = ofVec3f(100,10,100);
    player->lifespan = 100000;
    sys->add(*player);
    
    
   
    //forces needed for space ship movement
    thrustForce = new ThrustForce(ofVec3f(0, 0, 0));
    impForce = new ImpulseForce();
    sys->addForce(thrustForce);
    sys->addForce(impForce);
    sys->addForce(new TurbulenceForce(ofVec3f(-.05, -.05, -.05), ofVec3f(.05, .05, .05)));
    
    
    
    //set up emitter from space ship
    emitter = new ParticleEmitter();
    emitter->setEmitterType(DiscEmitter);
    emitter->setOneShot(true);
    emitter->setGroupSize(5);
    emitter->particleRadius = .001;
    emitter->setVelocity(ofVec3f(0, -5, 0));

    
   
}

//--------------------------------------------------------------
// incrementally update scene (animation)
//
void ofApp::update() {
      // timer for game
      time = ofGetElapsedTimef();
    
      //created to optimize program
      landerBoundMax = landerBounds.max();
      landerBoundMin = landerBounds.min();
    
    //if the game starts load all the features in
    if(gameStart)
    {
      
    // Gets current position of particle.
   float x = sys->particles[0].position.x;
   float y = sys->particles[0].position.y;
   float z = sys->particles[0].position.z;

   // Sets the lander & emitters positions to particle position.
   lander.setPosition(x, y, z);
   emitter->setPosition(ofVec3f(x, y, z));
   
    //update movement and find velocity
    sys->update();
    player->integrate();
    velocity = sys->particles[0].velocity;
    
    
   //update emitter
     emitter->update();
    if (thrusterOn) {
        emitter->start();
  }
    else {
      emitter->stop();
 }

        // setting up ray collision for altitude detection
        Vector3 landerPos = Vector3(lander.getPosition().x,lander.getPosition().y,lander.getPosition().z);
        Ray ray2 = Ray(Vector3(landerPos),Vector3(landerPos.x(), landerPos.y()-1000 ,landerPos.z()));
        TreeNode checkNode;
        if (kdtree.intersect(ray2, kdtree.root, checkNode)) {
            altitude = glm::length(kdtree.mesh.getVertex(checkNode.points[0]) - glm::vec3(lander.getPosition()));
        }

    ofVec3f landerPos1 = lander.getPosition();
  
    //Cameras and lights set up
    onboardCam.setPosition(glm::vec3(lander.getPosition().x,lander.getPosition().y+5, lander.getPosition().z));
    trackingCam.lookAt(landerPos1);
    testCam.setPosition(glm::vec3(lander.getPosition().x,lander.getPosition().y, lander.getPosition().z));
    testCam.lookAt(landerPos1);
   
    landingCam.setPosition(glm::vec3(lander.getPosition().x-10,lander.getPosition().y, lander.getPosition().z));
    landingCam.lookAt(landerPos1);
        
    dynamicLight.setPosition((ofVec3f)(lander.getPosition(), lander.getPosition() + 10, lander.getPosition()));
    dynamicLight.setPosition(landerPos1);
    frameRate =ofGetFrameRate();


   // checkCollision();
        if(checkCol){
    if (sys->particles[0].velocity.y < 0.5) {
        checkCollision();
    }

    if (sys->particles[0].velocity.x < -0.5) {
        checkFrontCollision();
    }
    if (sys->particles[0].velocity.x > 0.5) {
        checkBackCollision();
    }

    if (sys->particles[0].velocity.z < -0.5) {
        checkLeftCollision();
    }

    if (sys->particles[0].velocity.z > 0.5) {
        checkRightCollision();
    }

        //if you dont win in 2 minutes the game is over
        if(time > 240)
        {
            gameFinish = true;
        }
        
         checkWin();
    
          
   //if game finish the crash boolean is set to draw
    if(gameFinish)
    {
        crash = true;
    }
    
    }
    }
}
  
//--------------------------------------------------------------
void ofApp::draw() {
   
    //if the game starts set the start screen
  if (startScreen) {
    ofBackground(ofColor::black);
    start1.draw(0, 0);
  }
    
  //Start the game and draw everything
  if (gameStart) {
    ofBackground(ofColor::black);
    
    //Start the camera
    theCam ->begin();
      
    //draw the system
    emitter -> draw();

    //draw terrain and model
    ofPushMatrix();
    if (bWireframe) { // wireframe mode  (include axis)
      ofDisableLighting();
      ofSetColor(ofColor::slateGray);
      terrain.drawWireframe();
      if (bLanderLoaded) {
        lander.drawWireframe();
        if (!bTerrainSelected) drawAxis(lander.getPosition());
      }
      if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
    } else {
      ofEnableLighting(); // shaded mode
      terrain.drawFaces();

      if (bLanderLoaded) {
        lander.drawFaces();
        if (!bTerrainSelected) drawAxis(lander.getPosition());

        ofVec3f min = lander.getSceneMin() + lander.getPosition();
        ofVec3f max = lander.getSceneMax() + lander.getPosition();

        Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

        landerBounds = bounds;

        // set color of bounding box based on selection status
        //
        if (bLanderSelected) ofSetColor(ofColor::red);
        else ofSetColor(ofColor::white);

        //drawBox(bounds);
        ofVec3f min1 = terrain.getSceneMin() / 45;
        ofVec3f max1 = terrain.getSceneMax() / 45;
        Box bounds2 = Box(Vector3(min1.x, min1.y, min1.z), Vector3(max1.x, max1.y, max1.z));
        ofSetColor(ofColor::red);
        drawBox(bounds2);
        ofSetColor(ofColor::white);
      }
      if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));

      if (bDisplayPoints) { // display points as an option
        glPointSize(3);
        ofSetColor(ofColor::green);
        terrain.drawVertices();
      }

      // highlight selected point (draw sphere around selected point)
      //
      if (bPointSelected) {
        ofSetColor(ofColor::blue);
        ofDrawSphere(selectedPoint, .1);
      }

      ofNoFill();
      ofSetColor(ofColor::white);

      //kdtree.draw(5,0);

      // if node selected draw a red box
      if (nodeSelected) {
        ofSetColor(ofColor::red);
        drawBox(smallestNode.box);
      }

      //Load shaders and shade particles
      loadVbo();
      glDepthMask(GL_FALSE);
      ofSetColor(ofColor::orange);
      ofEnableBlendMode(OF_BLENDMODE_ADD);
      ofEnablePointSprites();

      shader.begin();
      particleTex.bind();
      vbo.draw(GL_POINTS, 0, (int) emitter-> sys-> particles.size());
      particleTex.unbind();
      shader.end();
      ofDisablePointSprites();
      ofDisableBlendMode();
      ofEnableAlphaBlending();
      glDepthMask(GL_TRUE);

      theCam->end();

      //draw the bit maps
      ofSetColor(ofColor::white);
      string altText = "Altitude: " + std::to_string(altitude);
      string fR = "Framerate: " + std::to_string(frameRate);
      string time2 = "Time: " + std::to_string(time);
      ofDrawBitmapString(altText, 10, 15);
      ofDrawBitmapString(time2, 40, 50);
      ofDrawBitmapString(fR, 20, 30);
      string l = "WIN";
      string b = "GAME OVER,You Lose";
      if (gameFinish) {
        if (win) {
          ofDrawBitmapString(l, 60, 70);

        } else {
          ofDrawBitmapString(b, 60, 70);
        }

      }

    }

  }
}
//

// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

    ofPushMatrix();
    ofTranslate(location);

    ofSetLineWidth(1.0);

    // X Axis
    ofSetColor(ofColor(255, 0, 0));
    ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
    

    // Y Axis
    ofSetColor(ofColor(0, 255, 0));
    ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

    // Z Axis
    ofSetColor(ofColor(0, 0, 255));
    ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

    ofPopMatrix();
}


void ofApp::keyPressed(int key) {
  float angle = 5.0;
  int numRotation = lander.getNumRotations();
  switch (key) {
  case 'C':
  case 'c':
    if (cam.getMouseInputEnabled()) cam.disableMouseInput();
    else cam.enableMouseInput();
    break;
  case 'F':
  case 'f':
    ofToggleFullscreen();
    break;
  case 'H':
  case ' ':
    gameStart = true;
    startScreen = false;
    break;
  case 'r':
   theCam = & cam;
    break;
  case 't':
    setCameraTarget();
    break;
  case 'u':
    theCam = & landingCam;
    break;
  case 'v':
    togglePointsDisplay();
    break;
  case 'V':
    drawTree = true;
    break;
    //  case 'w':
    toggleWireframeMode();
    //    break;
  case OF_KEY_ALT:
    cam.enableMouseInput();
    bAltKeyDown = true;
    break;
  case OF_KEY_CONTROL:
    bCtrlKeyDown = true;
    break;
  case OF_KEY_SHIFT:
    break;
  case OF_KEY_DEL:
    break;
  case OF_KEY_F1:
    break;
  case OF_KEY_F2:
    theCam = & top;
    break;
  case 'p':
    theCam = & trackingCam;
    astro = false;
    astro1.stop();
    break;
  case 'y':
    theCam = & onboardCam;
    astro = true;
    if (!gameFinish) {
      astro1.play();
    }

    break;
  case 'm':
    theCam = & testCam;
    astro = false;
    astro1.stop();
    break;
  case 'd':
    lander.setRotation(numRotation, -angle, 0, 1, 0);
    radian = glm::radians(angle);
    break;
  case 'a':
    lander.setRotation(numRotation, angle, 0, 1, 0);
    radian = glm::radians(angle);
    break;
  case 'w':
    impForce -> set(ofVec3f(0, 0, 0));
    thrustForce -> set(ofVec3f(0, 1, 0));
    thrust.play();
    thrusterOn = true;
    break;
  case 's':
    impForce -> set(ofVec3f(0, 0, 0));
    thrustForce -> set(ofVec3f(0, -1, 0));
    emitter -> stop();
    thrust.play();
    thrusterOn = true;
    //  thrusterOn = true;
    break;
  case OF_KEY_UP:
    impForce -> set(ofVec3f(0, 0, 0));
    if (!forwardCol &!gameFinish) {
      thrustForce -> set(ofVec3f(-sin(radian), 0, -cos(radian)));
         thrust.play();
         thrusterOn = true;
    }

    break;
  case OF_KEY_DOWN:
    impForce -> set(ofVec3f(0, 0, 0));
    if (!backCol &!gameFinish) {
      thrustForce -> set(ofVec3f(sin(radian), 0, cos(radian)));
       thrusterOn = true;
        thrust.play();
    }
    break;
  case OF_KEY_LEFT:
    impForce -> set(ofVec3f(0, 0, 0));
    if (!leftCol &!gameFinish) {
      thrustForce -> set(ofVec3f(-cos(radian), 0, sin(radian)));
        thrust.play();
        thrusterOn = true;
    }
    break;
  case OF_KEY_RIGHT:
    impForce -> set(ofVec3f(0, 0, 0));
    if (!rightCol &!gameFinish) {
      thrustForce -> set(ofVec3f(cos(radian), 0, -sin(radian)));
      thrust.play();
      thrusterOn = true;
    }
    break;

  default:
    break;
  }

}

void ofApp::toggleWireframeMode() {
    bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain() {
    bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
    bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

    switch (key) {
    case 'w' :
           thrusterOn = false;
            thrustForce->set(ofVec3f(0, 0, 0));
             emitter->stop();
           case 's':
             thrustForce->set(ofVec3f(0, 0, 0));
            thrusterOn = false;
                  break;
                  case OF_KEY_UP:
            thrusterOn = false;
             thrustForce->set(ofVec3f(0, 0, 0));
                      break;
                  case OF_KEY_DOWN:
             thrustForce->set(ofVec3f(0, 0, 0));
           thrusterOn = false;
                      break;
                  case OF_KEY_LEFT:
             thrustForce->set(ofVec3f(0, 0, 0));
            thrusterOn = false;
                      break;
                  case OF_KEY_RIGHT:
             thrustForce->set(ofVec3f(0, 0, 0));
             thrusterOn = false;
                      break;
           case 'd' :
           thrusterOn = false;
       case OF_KEY_ALT:
           cam.disableMouseInput();
           bAltKeyDown = false;
           break;
       case OF_KEY_CONTROL:
           bCtrlKeyDown = false;
           break;
       case OF_KEY_SHIFT:
           break;
       default:
           break;
    }
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

    

}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

    // if moving camera, don't allow mouse interaction
    //
    if (cam.getMouseInputEnabled()) return;
    
    
    //

    glm::vec3 p =  theCam->screenToWorld(glm::vec3(mouseX, mouseY, 0));
    glm::vec3 rayDir = glm::normalize(p - theCam->getPosition());

    //create ray for the mouse
    Ray r1;
    r1 = Ray(Vector3(p.x, p.y, p.z), Vector3(rayDir.x, rayDir.y, rayDir.z));

    
   
  //if kd.intersect returns true set kdtree to true.
  start = ofGetCurrentTime();
  nodeSelected = kdtree.intersect(r1, kdtree.root, smallestNode);
  finish = ofGetCurrentTime();;

   
    // compute bounds
    //
    if (bLanderLoaded) {
        glm::vec3 origin = cam.getPosition();
        glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
        glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
        glm::vec3 min = lander.getSceneMin() + lander.getPosition();
        glm::vec3 max = lander.getSceneMax() + lander.getPosition();
        Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
        mouseDownPos = getMousePointOnPlane(lander.getPosition(), cam.getZAxis());
        lastMouse = mouseDownPos;
    
   
    if (bounds.intersect(Ray(Vector3(p.x, p.y, p.z), Vector3(rayDir.x, rayDir.y, rayDir.z)), 0, 1000)) {
        bLanderSelected = true;
    }
    else {
        bLanderSelected = false;
    }

}
}

//draw a box from a "Box" class
//
void ofApp::drawBox(const Box &box) {
    Vector3 min = box.parameters[0];
    Vector3 max = box.parameters[1];
    Vector3 size = max - min;
    Vector3 center = size / 2 + min;
    ofVec3f p = ofVec3f(center.x(), center.y(), center.z());
    float w = size.x();
    float h = size.y();
    float d = size.z();
    ofDrawBox(p, w, h, d);
}

// return a Mesh Bounding Box for the entire Mesh
//
Box ofApp::meshBounds(const ofMesh & mesh) {
    int n = mesh.getNumVertices();
    ofVec3f v = mesh.getVertex(0);
    ofVec3f max = v;
    ofVec3f min = v;
    for (int i = 1; i < n; i++) {
        ofVec3f v = mesh.getVertex(i);

        if (v.x > max.x) max.x = v.x;
        else if (v.x < min.x) min.x = v.x;

        if (v.y > max.y) max.y = v.y;
        else if (v.y < min.y) min.y = v.y;

        if (v.z > max.z) max.z = v.z;
        else if (v.z < min.z) min.z = v.z;
    }
    return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
}



//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    if(cam.getMouseInputEnabled()) return;
    
    if(bLanderSelected)
    {
    
        glm::vec3 landerPos = lander.getPosition();
        glm::vec3 mousePos = getMousePointOnPlane(landerPos, cam.getZAxis());
        glm::vec3 delta = mousePos - lastMouse;
        landerPos += delta;
        lander.setPosition(landerPos.x, landerPos.y, landerPos.z);
        lastMouse = mousePos;
        
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
    bLanderSelected = false;
}



// Set the camera to use the selected point as it's new target
//
void ofApp::setCameraTarget() {

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
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

    static float ambient[] =
    { .5f, .5f, .5, 1.0f };
    static float diffuse[] =
    { 1.0f, 1.0f, 1.0f, 1.0f };

    static float position[] =
    {5.0, 5.0, 5.0, 0.0 };

    static float lmodel_ambient[] =
    { 1.0f, 1.0f, 1.0f, 1.0f };

    static float lmodel_twoside[] =
    { GL_TRUE };


    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, position);


    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
//    glEnable(GL_LIGHT1);
    glShadeModel(GL_SMOOTH);




}

void ofApp::savePicture() {
    ofImage picture;
    picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
    picture.save("screenshot.png");
    cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {
    if (lander.loadModel(dragInfo.files[0])) {
        bLanderLoaded = true;
        lander.setScaleNormalization(false);
    //    lander.setScale(.5, .5, .5);
        lander.setPosition(0, 0, 0);
//        lander.setRotation(1, 180, 1, 0, 0);

        glm::vec3 origin = theCam->getPosition();
        glm::vec3 camAxis = theCam->getZAxis();
        glm::vec3 mouseWorld = theCam->screenToWorld(glm::vec3(mouseX, mouseY, 0));
        glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
        float distance;

        bool hit = glm::intersectRayPlane(origin, mouseDir, glm::vec3(0, 0, 0), camAxis, distance);
        if (hit) {
            // find the point of intersection on the plane using the distance
            // We use the parameteric line or vector representation of a line to compute
            //
            // p' = p + s * dir;
            //
            glm::vec3 intersectPoint = origin + distance * mouseDir;

            // Now position the lander's origin at that intersection point
            //
            glm::vec3 min = lander.getSceneMin();
            glm::vec3 max = lander.getSceneMax();
            float offset = (max.y - min.y) / 2.0;
            lander.setPosition(intersectPoint.x, intersectPoint.y - offset, intersectPoint.z);

            // set up bounding box for lander while we are at it
            //
            landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
            
        }
    }
    

}


//  intersect the mouse ray with the plane normal to the camera
//  return intersection point.   (package code above into function)
//
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm) {
    // Setup our rays
    //
    glm::vec3 origin = theCam->getPosition();
    glm::vec3 camAxis = theCam->getZAxis();
    glm::vec3 mouseWorld = theCam->screenToWorld(glm::vec3(mouseX, mouseY, 0));
    glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
    float distance;

    bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);

    if (hit) {
        // find the point of intersection on the plane using the distance
        // We use the parameteric line or vector representation of a line to compute
        //
        // p' = p + s * dir;
        //
        glm::vec3 intersectPoint = origin + distance * mouseDir;
        
        return intersectPoint;
    }
    else return glm::vec3(0, 0, 0);
}
                     

void ofApp::checkCollision() {
    //check the middle of the bottom of the box to make sure you land properly
    Vector3 center = landerBounds.center();
    contactPoint = ofVec3f(center.x(), center.y() - landerBounds.height()/2, center.z()) + lander.getPosition();
    
    
    TreeNode node;
    kdtree.intersect(contactPoint, kdtree.root, node);
    if(node.points.size()>0)
    {
        collision = true;
        col = true;
        ofVec3f normal = kdtree.mesh.getNormal(node.points.at(0));
        ofVec3f v = ofGetFrameRate() * velocity;
        ofVec3f force = (restitution + 1) * (-v.dot(normal) * normal);
        impForce->set(force);
       
        //cout<<"hit"<<endl;
    }
    else
        
    {
        collision = false;
        col = false;
    }
    
    }
    


//--------------------------------------------------------------


void ofApp::checkFrontCollision() {
    
    contactPointFront = ofVec3f(landerBoundMax.x(), landerBoundMin.y(), landerBoundMax.z());
    TreeNode node;

    kdtree.intersect(contactPointFront, kdtree.root, node);
    if (node.points.size() > 0) {
        forwardCol = true;
        col = true;
        ofVec3f normal = kdtree.mesh.getNormal(node.points.at(0));
        ofVec3f v = ofGetFrameRate() * velocity;
        ofVec3f force = (restitution + 1) * (-v.dot(normal) * normal);
        impForce->set(force);
        
        //cout<<"hit front"<<endl;
    }
    
    else
           
       {
           forwardCol = false;
           col = false;
       }
       
    }
    
    


//--------------------------------------------------------------


void ofApp::checkBackCollision() {
    contactPointBack = ofVec3f(landerBoundMin.x(), landerBoundMin.y(), landerBoundMin.z());
    TreeNode node;
    kdtree.intersect(contactPointBack, kdtree.root, node);
    if (node.points.size() > 0) {
        backCol = true;
        col = true;
        ofVec3f normal = kdtree.mesh.getNormal(node.points.at(0));
        ofVec3f v = ofGetFrameRate() * velocity;
        ofVec3f force = (restitution + 1) * (-v.dot(normal) * normal);
        impForce->set(force);
       
    }
   else
          
      {
          backCol = false;
          col = false;
      }
      

    }
    


//--------------------------------------------------------------


void ofApp::checkLeftCollision() {
    contactPointLeft = ofVec3f(landerBoundMin.x(),landerBoundMin.y(),landerBoundMin.z());
    TreeNode node;
    kdtree.intersect(contactPointLeft, kdtree.root, node);
    if (node.points.size() > 0) {
       leftCol = true;
        col = true;
        ofVec3f normal = kdtree.mesh.getNormal(node.points.at(0));
        ofVec3f v = ofGetFrameRate() * velocity;
        ofVec3f p = (restitution + 1) * (-v.dot(normal)) * normal;
        impForce->set(p);
    }
   else
          
      {
          leftCol = false;
          col = false;
      }
      

    }
    
    


//--------------------------------------------------------------


void ofApp::checkRightCollision() {
    contactPointRight = ofVec3f(landerBoundMax.x(), landerBoundMin.y(), landerBoundMin.z());
        TreeNode node;
       kdtree.intersect(contactPointRight, kdtree.root, node);
       if (node.points.size() > 0) {
          rightCol = true;
           col = true;
           ofVec3f normal = kdtree.mesh.getNormal(node.points.at(0));
           ofVec3f v = ofGetFrameRate() * velocity;
           ofVec3f force = (restitution + 1) * (-v.dot(normal) * normal);
           impForce->set(force);
       }
      else
             
         {
             rightCol = false;
             col = false;
         }
         

       }
    
    //--------------------------------------------------------------


void ofApp::checkWin() {    
    if(timeUp)
    {
        gameFinish = true;
    }
    if(landerBounds.inside(bounds2.center()))
    {
        if(collision)
        {
                win = true;
                gameFinish = true;
        }
    
       
    }
   else
   {
       if(collision)
          {
              gameFinish = true;
              crash = true;
          }
   }
}

void ofApp::loadVbo() {
    if (emitter->sys->particles.size() < 1) return;

    vector<ofVec3f> sizes;
    vector<ofVec3f> points;
    for (int i = 0; i < emitter->sys->particles.size(); i++) {
        points.push_back(emitter->sys->particles[i].position);
        sizes.push_back(ofVec3f(5));
    }
    // upload the data to the vbo
    //
    int total = (int)points.size();
    vbo.clear();
    vbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
    vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
}
