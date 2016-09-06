/*
www.convivial.studio
*/

#include "touchObject.h"


//--------------------------------------------------------------
void touchObject::setup() {

	settings.loadFile("settings.xml");
	cout << "angles used:" << endl;
	for (int i = 0;i < settings.getNumTags("angle");i++) {
		cout << settings.getValue("angle", 0, i) << " - ";
		degrees.push_back(settings.getValue("angle", 0, i));
		triangleTracker tri;
		triangleT.push_back(tri);
	}
	cout <<  endl;
	
	//SET RANGE IN PIXEL
	for (int i = 0;i < triangleT.size();i++) {
		if (i == 0) {
			triangleT[i].rangeLow = MARKER_LOW_RANGE;
			triangleT[i].rangeHigh = (degrees[i]+ degrees[i+1])/2;
		}
		else if (i == triangleT.size() - 1) {
			triangleT[i].rangeLow = (degrees[i - 1] + degrees[i]) / 2;
			triangleT[i].rangeHigh = MARKER_HIGH_RANGE;
		}
		else {
			triangleT[i].rangeLow = (degrees[i-1] + degrees[i]) / 2;
			triangleT[i].rangeHigh = (degrees[i] + degrees[i+1]) / 2;
		}
	}

#ifdef USE_OSC
 
  sender.setup(settings.getValue("host", "localhost", 0), settings.getValue("port", 12345, 0));
#endif

}


//--------------------------------------------------------------
void touchObject::update(vector<ofPoint>touchPt,float distanceTriangle=150) {

	vector<trianglePt>tPt;

	for (int i = 0;i < touchPt.size();i++) {

		trianglePt t;
		t.paired = false;
		t.pos = touchPt[i];

		tPt.push_back(t);
	
	}

  //FIND ALL PAIRS
 
  int pairNum = 0;
  for (int i = 0;i < tPt.size();i++) {
	  for (int j = 0;j < tPt.size();j++) {
		  if (ofDist(tPt[i].pos.x, tPt[i].pos.y, tPt[j].pos.x, tPt[j].pos.y)< distanceTriangle && j != i) {

			  if (tPt[i].paired == true && tPt[j].paired == true) {
			  }
			  else {
				  pairNum++;
				  tPt[i].groupBelonging.push_back(pairNum);
				  tPt[j].groupBelonging.push_back(pairNum);
				  tPt[i].paired = true;
				  tPt[j].paired = true;
			  }
		  }
	  }
  }


  //DRAW PAIRS DEBUG
  /*
  for (int i = 0;i < tPt.size();i++) {
	  for (int i2 = 0;i2 < tPt[i].groupBelonging.size();i2++) {
		  for (int j = 0;j < tPt.size();j++) {
			  for (int j2 = 0;j2 < tPt[j].groupBelonging.size();j2++) {
			  }
		  }
	  }
  }
  */

  //GROUP PAIRS INTO POLYGONS
  vector < vector<ofPoint>> poly;
  for (int i = 0;i < tPt.size();i++) {
	  if (tPt[i].groupBelonging.size()>1) { //a point is part of two pairs at least or discarded, note 3 as it's multiplied
		  vector<ofPoint>nT;
		  for (int i2 = 0;i2 < tPt[i].groupBelonging.size();i2++) {
			  for (int j = 0;j < tPt.size();j++) {
				  for (int j2 = 0;j2 < tPt[j].groupBelonging.size();j2++) {
					  if (tPt[i].groupBelonging[i2] == tPt[j].groupBelonging[j2]) {
						  ofLine(tPt[i].pos, tPt[j].pos);
						  nT.push_back(tPt[j].pos);
					  }
				  }
			  }
		  }
		  if (nT.size() > 0) {
			  nT.push_back(tPt[i].pos);
			  poly.push_back(nT);
		  }
	  }
  }

  vector < vector<ofPoint>> polyNoDuplicate;

  for (int i = 0;i < poly.size();i++) {
	  vector<ofPoint> nP;
	  if (poly[i].size()>0) {
		  nP.push_back(poly[i][0]);
		  for (int j = 0;j < poly[i].size();j++) {
			  bool exist = false;
			  for (int k = 0;k < nP.size();k++) {
				  if (nP[k] == poly[i][j]) {
					  exist = true;
				  }
				  if (j == 0) {
					  exist = true;
				  }
			  }
			  if (exist == false) {
				  nP.push_back(poly[i][j]);
			  }
		  }
	  }
	  polyNoDuplicate.push_back(nP);
  }

  for (int i = 0;i < triangleT.size();i++) {
	  triangleT[i].visible = false;
  }

  ofFill();

  for (int i = 0;i < polyNoDuplicate.size();i++) {

	  vector<ofPoint> tTriangle;
	  if (polyNoDuplicate[i].size() == 3) {


		  triangleT[i].pos[0] = polyNoDuplicate[i][0];
		  triangleT[i].pos[1] = polyNoDuplicate[i][1];
		  triangleT[i].pos[2] = polyNoDuplicate[i][2];

		  ofSetColor(ofColor::purple);

		  int topCircle = findTop(triangleT[i].pos);

		  ofPoint average((triangleT[i].pos[0].x + triangleT[i].pos[1].x + triangleT[i].pos[2].x) / 3, (triangleT[i].pos[0].y + triangleT[i].pos[1].y + triangleT[i].pos[2].y) / 3);
		  triangleT[i].center = average;
		  triangleT[i].apexAngle =  abs(findAngleApex(triangleT[i].pos, topCircle));
		  triangleT[i].orientation = findOrientation(triangleT[i].pos, topCircle);
		  triangleT[i].apex = triangleT[i].pos[topCircle];
		  triangleT[i].widthTri =findWidth(triangleT[i].pos, topCircle);
		  triangleT[i].heightTri = findAltitude(triangleT[i].pos, topCircle);

		  for (int j = 0;j < triangleT.size();j++) {
			  if (triangleT[i].apexAngle > triangleT[j].rangeLow &&  triangleT[i].apexAngle < triangleT[j].rangeHigh) {
				  triangleT[i].index = j;
				  triangleT[i].visible = true;
			  }
		  }
	  }
  }

  //TO DO EXTRACT TRIANGLES FROM POLYGONS

#ifdef USE_OSC
  sendOsc();
#endif



}

//--------------------------------------------------------------
void touchObject::draw() {

  for (int i = 0;i < triangleT.size();i++) {

	  if (triangleT[i].visible ) {


		  			  ofSetColor(ofColor::blue);
		  			  ofDrawTriangle(triangleT[i].pos[0], triangleT[i].pos[1], triangleT[i].pos[2]);


		ofSetColor(ofColor::green);
		ofPoint dirNorm = triangleT[i].apex - triangleT[i].center ;
		dirNorm.normalize();
		  
		ofPoint scaledPt = triangleT[i].apex+(dirNorm.scale(80));
		ofPoint scaledPtText = triangleT[i].apex + (dirNorm.scale(40));
		ofDrawArrow(triangleT[i].center, scaledPt,10);
		ofDrawBitmapStringHighlight(ofToString(triangleT[i].index), scaledPtText);

	  }
  }
}

//--------------------------------------------------------------
void touchObject::sendOsc() {
	for (int i = 0;i < triangleT.size();i++) {
		if (triangleT[i].visible) {
		
			ofxOscMessage m;
			m.setAddress("/object/");
			m.addStringArg("  index: ");
			m.addIntArg(triangleT[i].index);			
			m.addStringArg("  x: ");
			m.addFloatArg(triangleT[i].center.x);
			m.addStringArg("  y: ");
			m.addFloatArg(triangleT[i].center.y);
			m.addStringArg("  w: ");
			m.addIntArg(triangleT[i].widthTri);
			m.addStringArg("  h: ");
			m.addIntArg(triangleT[i].heightTri);
			m.addStringArg("  angle: ");
			m.addIntArg(triangleT[i].orientation);

			sender.sendMessage(m, false);
		}
	}
}

vector<ofPoint> touchObject::getCenter()
{
	vector<ofPoint>c;
	for (int i = 0;i < triangleT.size();i++) {
		if (triangleT[i].visible) {
			c.push_back(triangleT[i].center);
		}
	}
	return c;
}

vector<float> touchObject::getWidth()
{
	vector<float>c;
	for (int i = 0;i < triangleT.size();i++) {
		if (triangleT[i].visible) {
			c.push_back(triangleT[i].widthTri);
		}
	}
	return c;
}

vector<float> touchObject::getHeight()
{
	vector<float>c;
	for (int i = 0;i < triangleT.size();i++) {
		if (triangleT[i].visible) {
			c.push_back(triangleT[i].heightTri);
		}
	}
	return c;
}

vector<float> touchObject::getAngle()
{
	vector<float>c;
	for (int i = 0;i < triangleT.size();i++) {
		if (triangleT[i].visible) {
			c.push_back(triangleT[i].orientation);
		}
	}
	return c;
}

vector<int> touchObject::getIndex()
{
	vector<int>c;
	for (int i = 0;i < triangleT.size();i++) {
		if (triangleT[i].visible) {
			c.push_back(triangleT[i].index);
		}
	}
	return c;
}

//--------------------------------------------------------------
int  touchObject::findTop(ofPoint pt[3]) {

	float dist[3];
	dist[0] = ofDist(pt[0].x, pt[0].y, pt[1].x, pt[1].y);
	dist[1] = ofDist(pt[1].x, pt[1].y, pt[2].x, pt[2].y);
	dist[2] = ofDist(pt[0].x, pt[0].y, pt[2].x, pt[2].y);

	float dist0to1 = dist[0];
	float dist1to2 = dist[1];
	float dist0to2 = dist[2];

	float diff01m02 = abs(dist0to1 - dist0to2);
	float diff02m12 = abs(dist0to2 - dist1to2);
	float diff01m12 = abs(dist0to1 - dist1to2);

	if (diff01m02 < diff02m12 && diff01m02 < diff01m12) {
		return 0;
	}
	else if (diff01m12<diff01m02 && diff01m12<diff02m12) {
		return 1;
	}
	else if (diff02m12<diff01m02 && diff02m12<diff01m12) {
		return 2;
	}

}

//--------------------------------------------------------------
float  touchObject::findOrientation(ofPoint pt[3], int top) {

	ofPoint a;
	ofPoint b;
	ofPoint c;
	b = pt[top];
	if (top == 0) {
		a = pt[1];
		c = pt[2];
	}
	else if (top == 1) {
		a = pt[0];
		c = pt[2];
	}
	else if (top == 2) {
		a = pt[0];
		c = pt[1];
	}

	ofPoint middlePt(ofLerp(a.x, c.x, 0.5), ofLerp(a.y, c.y, 0.5));

	ofPoint diff = b - middlePt;

	diff.normalize();

	return 	ofRadToDeg(atan2(diff.x, diff.y))*-1;

}

//--------------------------------------------------------------
float  touchObject::findAltitude(ofPoint pt[3], int top) {

	ofPoint a;
	ofPoint b;
	ofPoint c;
	b = pt[top];
	if (top == 0) {
		a = pt[1];
		c = pt[2];
	}
	else if (top == 1) {
		a = pt[0];
		c = pt[2];
	}
	else if (top == 2) {
		a = pt[0];
		c = pt[1];
	}

	ofPoint middlePt(ofLerp(a.x,c.x, 0.5), ofLerp(a.y, c.y, 0.5));

	return 	ofDist(middlePt.x, middlePt.y, b.x, b.y);
}

//--------------------------------------------------------------
float  touchObject::findAngleApex(ofPoint pt[3], int top) {

		ofPoint a;
		ofPoint b;
		ofPoint c;
		b = pt[top];
		if (top == 0) {
			a = pt[1];
			c = pt[2];
		}
		else if (top == 1) {
			a = pt[0];
			c = pt[2];
		}
		else if (top == 2) {
			a = pt[0];
			c = pt[1];
		}

	ofPoint ab = { b.x - a.x, b.y - a.y };
	ofPoint cb = { b.x - c.x, b.y - c.y };

	float dot = (ab.x * cb.x + ab.y * cb.y); // dot product
	float cross = (ab.x * cb.y - ab.y * cb.x); // cross product

	float alpha = atan2(cross, dot);

	return (int)floor(alpha * 180. / PI + 0.5);

}

//--------------------------------------------------------------
float  touchObject::findWidth(ofPoint pt[3], int top) {

	ofPoint a;
	ofPoint b;
	ofPoint c;
	b = pt[top];
	if (top == 0) {

		a = pt[1];
		c = pt[2];

	}
	else if (top == 1) {

		a = pt[0];
		c = pt[2];

	}
	else if (top == 2) {

		a = pt[0];
		c = pt[1];

	}

	return ofDist(a.x, a.y, c.x, c.y);

}