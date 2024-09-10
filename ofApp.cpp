/************************************************************
************************************************************/
#include "ofApp.h"

/************************************************************
************************************************************/

/******************************
******************************/
ofApp::ofApp(int input_device_id)
: input_device_id_(input_device_id)
{
}

/******************************
******************************/
ofApp::~ofApp(){
}

/******************************
******************************/
void ofApp::setup(){
	/********************
	********************/
	ofBackground(255, 255, 255);
	// ofSetBackgroundAuto(true);
	
	ofSetWindowTitle("midi test");
	ofSetVerticalSync(true);
	ofSetFrameRate(30);
	// ofSetWindowShape( static_cast<int>(WindowSize::kWidth), static_cast<int>(WindowSize::kHeight) );
	ofSetEscapeQuitsApp(false);
	
	
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	/********************
	********************/
	ofSeedRandom();
	
	ofEnableAntiAliasing();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA); // OF_BLENDMODE_DISABLED, OF_BLENDMODE_ALPHA, OF_BLENDMODE_ADD
	
	/********************
	********************/
	midi_in_.listInPorts();
	
	std::vector<string> midi_port_name = {
		"Launchpad Mini MK3 LPMiniMK3 MIDI Out",
		"microKEY2-25 Air KEYBOARD",
	};
	
	if(midi_port_name.size() <= input_device_id_)	input_device_id_ = 0;
	
	// if( !midi_in_.openPort(1) ){ // by number
	if( !midi_in_.openPort(midi_port_name[input_device_id_]) ){
		ERROR_MSG();
		std::exit(1);
	}
	
	// don't ignore sysex, timing, & active sense messages,
	// these are ignored by default
	midi_in_.ignoreTypes(false, false, false);
	
	// add ofApp as a listener and enable direct message handling
	// comment this to use queued message handling
	midi_in_.addListener(this); // sj
	
	// print received messages to the console
	// midi_in_.setVerbose(true); // sj
	
	/********************
	********************/
	midi_out_.listOutPorts();
	if( !midi_out_.openPort("Launchpad Mini MK3 LPMiniMK3 MIDI In") ){
		ERROR_MSG();
		std::exit(1);
	}
}

/******************************
******************************/
void ofApp::update(){
	ReceiveMidi_MainThread();
}

/******************************
******************************/
void ofApp::ReceiveMidi_MainThread(){
	if(midi_in_.hasWaitingMessages()) {
		ofxMidiMessage message;
		
		// add the latest message to the message queue
		while(midi_in_.getNextMessage(message)) {
			// midi_messages_.push_back(message);
			midi_messages_.push(message);
		}
		
		// remove any old messages if we have too many
		/*
		while(midi_messages_.size() > kMaxMessages) {
			midi_messages_.erase(midi_messages_.begin());
		}
		*/
		while(kMaxMessages_ < midi_messages_.size()) {
			midi_messages_.pop();
		}
	}
}


/******************************
******************************/
void ofApp::draw(){
	/********************
	********************/
	std::queue<ofxMidiMessage> midi_messages = midi_messages_; // sj
	
	/********************
	********************/
	ofBackground(210);
	
	for( int i = 0; !midi_messages.empty(); i++ ){
		ofxMidiMessage &message = midi_messages.front();
		// midi_messages.pop();
		
		int x = 10;
		int y = i * 40 + 40;
		
		// draw the last received message contents to the screen,
		// this doesn't print all the data from every status type
		// but you should get the general idea
		stringstream text;
		text << ofxMidiMessage::getStatusString(message.status);
		while(text.str().length() < 16) { // pad status width
			text << " ";
		}
		
		ofSetColor(127);
		if(message.status < MIDI_SYSEX) {
			text << "chan: " << message.channel;
			
			if( (message.status == MIDI_NOTE_ON) || (  message.status == MIDI_NOTE_OFF) ){
				text << "\tpitch: " << message.pitch;
				ofDrawRectangle(x + ofGetWidth()*0.2, y + 12, ofMap(message.pitch, 0, 127, 0, ofGetWidth()*0.2), 10);
				text << "\tvel: " << message.velocity;
				ofDrawRectangle(x + (ofGetWidth()*0.2 * 2), y + 12, ofMap(message.velocity, 0, 127, 0, ofGetWidth()*0.2), 10);
				
			}else if(message.status == MIDI_CONTROL_CHANGE) {
				text << "\tctl: " << message.control;
				ofDrawRectangle(x + ofGetWidth()*0.2, y + 12, ofMap(message.control, 0, 127, 0, ofGetWidth()*0.2), 10);
				text << "\tval: " << message.value;
				ofDrawRectangle(x + ofGetWidth()*0.2 * 2, y + 12, ofMap(message.value, 0, 127, 0, ofGetWidth()*0.2), 10);
			}else if(message.status == MIDI_PROGRAM_CHANGE) {
				text << "\tpgm: " << message.value;
				ofDrawRectangle(x + ofGetWidth()*0.2, y + 12, ofMap(message.value, 0, 127, 0, ofGetWidth()*0.2), 10);
			}else if(message.status == MIDI_PITCH_BEND) {
				text << "\tval: " << message.value;
				ofDrawRectangle(x + ofGetWidth()*0.2, y + 12, ofMap(message.value, 0, MIDI_MAX_BEND, 0, ofGetWidth()*0.2), 10);
			}else if(message.status == MIDI_AFTERTOUCH) {
				text << "\tval: " << message.value;
				ofDrawRectangle(x + ofGetWidth()*0.2, y + 12, ofMap(message.value, 0, 127, 0, ofGetWidth()*0.2), 10);
			}else if(message.status == MIDI_POLY_AFTERTOUCH) {
				text << "\tpitch: " << message.pitch;
				ofDrawRectangle(x + ofGetWidth()*0.2, y + 12, ofMap(message.pitch, 0, 127, 0, ofGetWidth()*0.2), 10);
				text << "\tval: " << message.value;
				ofDrawRectangle(x + ofGetWidth()*0.2 * 2, y + 12, ofMap(message.value, 0, 127, 0, ofGetWidth()*0.2), 10);
			}
			
			text << " "; // pad for delta print
		} else {
			text << message.bytes.size() << " bytes ";
		}
		
		text << "delta: " << message.deltatime;
		ofSetColor(0);
		ofDrawBitmapString(text.str(), x, y);
		text.str(""); // clear
		
		
		midi_messages.pop();
	}
}

/******************************
******************************/
void ofApp::SendMidi_AllNoteOff(){
	/********************
	********************/
	for(int i = 0; i <= 0; i++){
		midi_out_.sendNoteOff(1, i, 127);
	}
}

/******************************
******************************/
void ofApp::exit(){
	/********************
	********************/
	SendMidi_AllNoteOff();
	
	/********************
	********************/
	// clean up
	midi_in_.closePort();
	midi_in_.removeListener(this);
	
	// clean up
	midi_out_.closePort();
}

/******************************
******************************/
void ofApp::keyPressed(int key){
	switch(key) {
		case '?':
			midi_in_.listInPorts();
			midi_out_.listOutPorts();
			break;
			
		case '0':
			// cf. ofxmidi_out_.cpp
			// void ofxmidi_out_::sendNoteOff(int channel, int pitch, int velocity) {
			midi_out_.sendNoteOff(1, 0, 127);
			break;
			
		case '1':
			midi_out_.sendNoteOn(1, 0, 127); // sj : velocity = 0 にしてしまうと、NoteOffになってしまう
			break;
	}
}

/******************************
******************************/
void ofApp::keyReleased(int key){

}

/******************************
direct message handling
note: this is called on the MIDI thread, so copy message contents
******************************/
void ofApp::newMidiMessage(ofxMidiMessage &message) {
	/*
	// add the latest message to the message queue
	midi_messages_.push_back(message);
	
	// remove any old messages if we have too many
	while(midi_messages_.size() > kMaxMessages) {
		midi_messages_.erase(midi_messages_.begin());
	}
	*/
	
	midi_messages_.push(message);
	
	while(kMaxMessages_ < midi_messages_.size()) {
		midi_messages_.pop();
	}
}

/******************************
******************************/
void ofApp::mouseMoved(int x, int y ){

}

/******************************
******************************/
void ofApp::mouseDragged(int x, int y, int button){

}

/******************************
******************************/
void ofApp::mousePressed(int x, int y, int button){

}

/******************************
******************************/
void ofApp::mouseReleased(int x, int y, int button){

}

/******************************
******************************/
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){

}

/******************************
******************************/
void ofApp::mouseEntered(int x, int y){

}

/******************************
******************************/
void ofApp::mouseExited(int x, int y){

}

/******************************
******************************/
void ofApp::windowResized(int w, int h){

}

/******************************
******************************/
void ofApp::gotMessage(ofMessage msg){

}

/******************************
******************************/
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
