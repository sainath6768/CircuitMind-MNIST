#include "image_classifier.h"


int main(int argc, char* argv[]) {
    if(argc !=2){
	    LOG(LOGGER_LEVEL_INFO,"please enter the valid port ");
		return 0;
	}
    char *portname = argv[1];
    ImageClassifier(portname);
    return 0;
}
