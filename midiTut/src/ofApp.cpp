#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    globalAmplitude = 0.7;
    
    frequency.smoother = new ParameterSmoother(100);//set the initial portamento to 100ms
    frequency.targetValue = frequency.currentValue = 200;//set the initial frequency
    
    triangle = WTTriangle(frequency.currentValue);//start the triangle at the current freq
    freqScalar = 1;//pitch wheel starts in the middle (freqScalar range is 0.5-2.0, curved)
    envelope = CTEnvelope();//create the new envelope
    
    midiIn.listPorts();//list all MIDI ports in console
    midiIn.openPort(3);//my device was on port 3, check your console to see which port you need
    midiIn.addListener(this);//add the listener for the MIDI call
    
    ofSoundStreamSetup(2, 0, ATKSettings::sampleRate, ATKSettings::bufferSize, 4);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}
//--------------------------------------------------------------
void ofApp::audioOut(float* buffer, int bufferSize, int nChannels){
    for(int i = 0; i < bufferSize; i++){
        
        frequency.currentValue = frequency.smoother->process(frequency.targetValue);
        triangle.setFreq(frequency.currentValue);//set the freq every sample (necessary since smoothing)
        triangle.process();envelope.process();//progress the oscillator and env by one sample
        float currentSample = triangle.getSample() * envelope.getValue();
        
        currentSample *= globalAmplitude;//scale by global amplitude
        buffer[i*nChannels+0] = currentSample;
        buffer[i*nChannels+1] = currentSample;
    }
}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& message){
    
    if(message.status == MIDI_NOTE_ON){
        mostRecentNote = message.pitch;
       frequency.targetValue = (mtof(mostRecentNote) * freqScalar);
        envelope.setGate(1);
    }else if(message.status == MIDI_NOTE_OFF){
        if(message.pitch == mostRecentNote){
            envelope.setGate(0);
        }
    }else if(message.status == MIDI_PITCH_BEND){
        float octave = ofMap(message.value, 0, MIDI_MAX_BEND, -1.0, 1.0);
        freqScalar = pow(2, octave);
        frequency.targetValue = (mtof(mostRecentNote) * freqScalar);
    }
    
    switch(message.control){
        case 20:// my control number, change to match what you need
            globalAmplitude = message.value/127.f;//scale the CC value from 0-127 to 0.0-1.0
            break;
        case 21:
            //set the portamento time (50-1500)
            frequency.smoother->setTime(ofMap(message.value, 0, 127, 50, 1500));
            break;
        default:
            if(message.status == MIDI_CONTROL_CHANGE){//if message is CC
                cout << message.control << endl;//print control number to console
            }
    }
    
 
}
//--------------------------------------------------------------
void ofApp::exit(){
    ofSoundStreamClose();
}
