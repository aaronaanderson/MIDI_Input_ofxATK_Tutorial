#pragma once

#include "ofMain.h"
#include "ofxATK.hpp"
#include "ofxMidi.h"

class ofApp : public ofBaseApp, public ofxMidiListener{//inherit from listener class as well

	public:
		void setup();
		void update();
		void draw();
    void exit();
    void audioOut(float* buffer, int bufferSize, int nChannels);

    void newMidiMessage(ofxMidiMessage& message);//inherited from ofxMidiListener
    
    ofxMidiIn midiIn;//used to deal with ports and add listener
    
    SmoothValue frequency;//as smooth value for portamento
    WTTriangle triangle;//basic anti-aliased wave table triangle oscillator
    CTEnvelope envelope;//basic Constant Time Linear Envelope
    int mostRecentNote;//needed to keep track of most recent midi note
    float freqScalar;//used to keep track of pitch bend location
    float globalAmplitude;
};
