/************************************************************
************************************************************/
#include "ofMain.h"
#include "ofApp.h"

/******************************
******************************/
int main( int argc, char** argv ){
	/********************
	********************/
	//Use ofGLFWWindowSettings for more options like multi-monitor fullscreen
	ofGLWindowSettings settings;
	settings.setSize(1024, 768);
	settings.windowMode = OF_WINDOW; //can also be OF_FULLSCREEN
	
	auto window = ofCreateWindow(settings);
	
	/********************
	********************/
	int input_device_id = 0;
	
	printf("---------------------------------\n");
	printf("> parameters\n");
	printf("\t-d      input_device_id (0)\n");
	printf("---------------------------------\n");
	
	for(int i = 1; i < argc; i++){
		if( strcmp(argv[i], "-d") == 0 ){
			if(i+1 < argc) { input_device_id = atoi(argv[i+1]); i++; }
		}
	}
	
	if( (input_device_id < 0) || (2 <= input_device_id) ) input_device_id = 0;
	
	/********************
	********************/
	ofRunApp(window, make_shared<ofApp>( input_device_id ));
	ofRunMainLoop();
}
