/************************************************************
ofxMidi
	https://github.com/danomatika/ofxMidi
	
	note
		MIDI input messages can be handled in one of two ways: direct or queued
		
		direct
			Subclass ofxMidiListener if you want to receive messages directly on the
			MIDI thread. Messages should be manually buffered for use on the GUI thread.
			This is faster as it is not limited by the draw framerate but you have to be
			careful of memory access.
		
		queued
			Manually handle messages using hasWaitingMessages()/getNextMessage().
			Received messages are thread safe.
		
		queued message passing is enabled by default and is disabled when an
		ofxMidiListener is added
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "ofxMidi.h" // sj

/************************************************************
************************************************************/
#define ERROR_MSG(); printf("Error in %s:%d\n", __FILE__, __LINE__);

/************************************************************
************************************************************/

// class ofApp : public ofBaseApp{
class ofApp : public ofBaseApp, public ofxMidiListener { // sj
private:
	/****************************************
	****************************************/
	ofxMidiIn midi_in_;
	
	// std::vector<ofxMidiMessage> midi_messages_;	// received messages
	std::queue<ofxMidiMessage> midi_messages_;		// received messages
	std::size_t kMaxMessages_ = 10;					// max number of messages to keep track of
	
	ofxMidiOut midi_out_;
	
	int input_device_id_;
	
public:
	/****************************************
	****************************************/
	/********************
	********************/
	void newMidiMessage(ofxMidiMessage &message) override;
	void ReceiveMidi_MainThread();
	void SendMidi_AllNoteOff();
	
	/********************
	********************/
	ofApp(int input_device_id);
	~ofApp();
	
	void setup() override;
	void update() override;
	void draw() override;
	void exit() override;

	void keyPressed(int key) override;
	void keyReleased(int key) override;
	void mouseMoved(int x, int y ) override;
	void mouseDragged(int x, int y, int button) override;
	void mousePressed(int x, int y, int button) override;
	void mouseReleased(int x, int y, int button) override;
	void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
	void mouseEntered(int x, int y) override;
	void mouseExited(int x, int y) override;
	void windowResized(int w, int h) override;
	void dragEvent(ofDragInfo dragInfo) override;
	void gotMessage(ofMessage msg) override;
	
};
