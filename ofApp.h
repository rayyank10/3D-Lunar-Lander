#pragma once

#include "ofMain.h"

#include  "ofxAssimpModelLoader.h"

#include "box.h"

#include "ray.h"

#include "KdTree.h"

#include "ofxGui.h"

#include "ParticleSystem.h"

#include "ParticleEmitter.h"

class ofApp: public ofBaseApp {

  public: void setup();
  void update();
  void draw();

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
  void drawAxis(ofVec3f);
  void initLightingAndMaterials();
  void savePicture();
  void toggleWireframeMode();
  void togglePointsDisplay();
  void toggleSelectTerrain();
  void setCameraTarget();
  glm::vec3 getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm);
  void drawBox(const Box & box);
  Box meshBounds(const ofMesh & );
  void checkCollision();
  void checkFrontCollision();
  void checkBackCollision();
  void checkLeftCollision();
  void checkRightCollision();
  void checkWin();
  void loadVbo();

  //Camera Set up
  ofEasyCam cam;
  ofCamera top;
  ofCamera trackingCam;
  ofCamera onboardCam;
  ofCamera testCam;
  ofCamera * theCam;
  ofCamera landingCam;
  
  //models and lights
  ofxAssimpModelLoader terrain,
  lander;
  ofLight light;
  ofLight keyLight,
  rimLight,
  fillLight,
  dynamicLight;

  //all the boxes
  Box boundingBox;
  Box landerBounds;
  Box landerBox;
  Box box;
  Box bounds2;

  //all the images
  ofImage backgroundImage;
  ofImage start1;

  //booleans
  bool collision;
  bool bAltKeyDown;
  bool bCtrlKeyDown;
  bool bWireframe;
  bool bDisplayPoints;
  bool bPointSelected;
  bool col;
  bool win;
  bool crash;
  bool forwardCol;
  bool backCol;
  bool leftCol;
  bool rightCol;
  bool astro;
  bool startScreen;
  bool gameStart;
  bool gameFinish;
  bool timeUp;
  bool bLanderLoaded = false;
  bool bTerrainSelected;
  bool bLanderSelected = false;
  bool drawTree = false;
  bool nodeSelected = false;
    
    bool checkCol;

  //vectors
  ofVec3f selectedPoint;
  ofVec3f intersectPoint;
  ofVec3f lastMouse;
  glm::vec3 mouseDownPos;
  ofVec3f contactPoint;
  ofVec3f contactPointFront;
  ofVec3f contactPointBack;
  ofVec3f contactPointRight;
  ofVec3f contactPointLeft;
  ofVec3f velocity;
  Vector3 landerBoundMax;
  Vector3 landerBoundMin;

  const float selectionRange = 4.0;
 
    
  //gui
  ofxIntSlider levels;
  ofxPanel gui;

  //time
  ofTime start;
  ofTime finish;

  //all tree related objects
  KdTree kdtree;
  TreeNode root;
  TreeNode node1;
  TreeNode smallestNode;

  //particles and forces
  Particle * player;
  ThrustForce * thrustForce;
  ImpulseForce * impForce;

  // The system where forces added.
  ParticleSystem * sys;
  ParticleEmitter * emitter;

  //restitution
  float restitution;

  // Tracks rotation angle.
  int angle;
  int radian;
  bool thrusterOn = false;
  float altitude;
  float frameRate;
  float time;

  //textures
  ofTexture particleTex;
  ofVbo vbo;
  ofShader shader;

  //Sounds
  ofSoundPlayer thrust;
  ofSoundPlayer astro1;

};
