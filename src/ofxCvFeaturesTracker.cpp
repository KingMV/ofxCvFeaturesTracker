//
//  ofxCvFeaturesTracker.cpp
//  markerless_AR
//
//  Created by kikko on 07/11/13.
//
//

#include "ofxCvFeaturesTracker.h"

using namespace ofxCv;
using namespace cv;

void ofxCvFeaturesTracker::setup(ofxCv::Calibration _calibration){
    calibration = _calibration;
    tracker.setup();
	found = false;
}

void ofxCvFeaturesTracker::add(ofBaseHasPixels & img){
    tracker.add(toCv(img));
}

void ofxCvFeaturesTracker::update(ofBaseHasPixels & frame){
    
    int prevMillis = ofGetElapsedTimeMillis();
    
    found = tracker.find(toCv(frame));
    
    if(found){
        Mat rvec, tvec;
        Mat cameraMatrix = calibration.getDistortedIntrinsics().getCameraMatrix();
        tracker.getPose(cameraMatrix, calibration.getDistCoeffs(), rvec, tvec);
        modelMatrix = makeMatrix(rvec, tvec);
    }
    
    updateTime = ofGetElapsedTimeMillis()-prevMillis;
}

void ofxCvFeaturesTracker::draw(){
    
    ofPushStyle();
    
    ofSetColor(ofColor::red);
    drawQueryPoints();
    drawImgKeyPoints();
    
    if(isFound()){
        ofSetColor(ofColor::white);
        drawMatches();
        ofSetColor(ofColor::yellow);
        drawQuad();
    }
    
    ofPopStyle();
}

#pragma mark - Private

void ofxCvFeaturesTracker::drawImgKeyPoints(){
    int length = 2;
    ofPushMatrix();
    ofTranslate(640, 0);
    std::vector<cv::KeyPoint> keyPts = getPatternKeyPoints();
    for (auto & pt : keyPts) {
        ofLine(pt.pt.x-length, pt.pt.y, pt.pt.x+length, pt.pt.y);
        ofLine(pt.pt.x, pt.pt.y-length, pt.pt.x, pt.pt.y+length);
    }
    ofPopMatrix();
}

void ofxCvFeaturesTracker::drawQueryPoints(){
    int length = 2;
    std::vector<cv::KeyPoint> keyPts = getQueryKeyPoints();
    for (auto & pt : keyPts) {
        ofLine(pt.pt.x-length, pt.pt.y, pt.pt.x+length, pt.pt.y);
        ofLine(pt.pt.x, pt.pt.y-length, pt.pt.x, pt.pt.y+length);
    }
}

void ofxCvFeaturesTracker::drawMatches(){
    std::vector<cv::KeyPoint> pKeyPts = getPatternKeyPoints();
    std::vector<cv::KeyPoint> keyPts = getQueryKeyPoints();
    std::vector<cv::DMatch> matches = getMatches();
    cv::Point2f in, out;
    for (auto & m : matches) {
        in = keyPts[m.queryIdx].pt;
        out = pKeyPts[m.trainIdx].pt;
        ofLine(in.x, in.y, out.x+640, out.y);
    }
}

void ofxCvFeaturesTracker::drawQuad(){
    int i;
    const auto & pts = getQuad();
    for (i=0; i<pts.size()-1; i++){
        const Point2f & p = pts[i];
        const Point2f & pn = pts[i+1];
        ofLine(p.x, p.y, pn.x, pn.y);
    }
    ofLine(pts[i].x, pts[i].y, pts[0].x, pts[0].y);
}