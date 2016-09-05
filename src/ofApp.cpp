/*
www.convivial.studio
*/

#include "ofApp.h"
#include "ofxWin8Touch.h"

//--------------------------------------------------------------
void ofApp::setup() {


	
  ofxWin8TouchSetup();
  ofRegisterTouchEvents(this);

  touchObj.setup();

  gui.setup("gui");
  gui.add(distanceTriangle.setup("distance triangle", 140, 1, 400));
  gui.add(errorToleranceTriangle.setup("errorToleranceTriangle", 0.2, 0.0, 1.0));
  gui.add(drawDebugB.setup("drawDebugB", true));
  gui.add(fullScreenB.setup("fullScreenB", false));
  gui.loadFromFile("gui.xml");

  
}


//--------------------------------------------------------------
void ofApp::update() {

	vector<ofPoint>pts;
	for (auto &i : touchMap) {
		auto &touch = i.second;

		pts.push_back(ofPoint(touch.x, touch.y));

	}
	touchObj.update(pts, distanceTriangle);


	if (fullScreenB) {
		fullScreenB = false;
		ofToggleFullscreen();
	}

}

//--------------------------------------------------------------
void ofApp::draw() {

	ofSetBackgroundColor(20, 20, 20);

	if(drawDebugB)touchObj.draw();
	
	ofSetColor(255, 255, 255);
	gui.draw();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'f') {
		ofToggleFullscreen();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
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
void ofApp::windowResized(int w, int h){
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
}

void ofApp::touchDown(ofTouchEventArgs & touch) {
	touchMap[touch.id] = touch;
}

//--------------------------------------------------------------
void ofApp::touchMoved(ofTouchEventArgs & touch) {
	touchMap[touch.id] = touch;
}

//--------------------------------------------------------------
void ofApp::touchUp(ofTouchEventArgs & touch) {
	touchMap.erase(touch.id);
}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(ofTouchEventArgs & touch) {

}

//--------------------------------------------------------------
void ofApp::touchCancelled(ofTouchEventArgs & touch) {
	touchMap.erase(touch.id);
}


void ofApp::exit() {
	gui.saveToFile("gui.xml");
}
