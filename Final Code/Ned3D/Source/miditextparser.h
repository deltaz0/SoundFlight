#ifndef __MIDITEXTPARSER_H__
#define __MIDITEXTPARSER_H__

#include <string>

using namespace std;

class MidiTextParser {

public:

	MidiTextParser();
	~MidiTextParser();

	struct note{
		int measure;
		int beat;
		float ratio;
		int pitch;
		//0 = not encountered yet
		//1 = collected successfully by passing through
		//2 = missed
		int collected;
		float rxTrans;
		float ryTrans;
		float rxPos;
		float ryPos;
	} noteAr[800];

	int minPitch;
	int maxPitch;

	bool populated;

	int lane;
	int myTotal;

	int parse(string filename, int laneNum);

	int getMeasureValue(int index);
	int getBeatValue(int index);
	float getRatioValue(int index);
	int getPitchValue(int index);
	int getNumberOfNotes();
	int getLane();
	void setCollected(int index, int value);
	int getCollected(int index);
	void clearVals();

	float getRXTrans(int index);
	float getRYTrans(int index);
	float getRXPos(int index);
	float getRYPos(int index);

	float getPitchRatio(int index);

};

#endif