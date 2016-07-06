#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "miditextparser.h"
#include "game.h"



using namespace std;

MidiTextParser::MidiTextParser(){
	for(int i=0; i<800; i++){
		noteAr[i].measure = 0;
		noteAr[i].beat = 0;
		noteAr[i].ratio = 0.0f;
		noteAr[i].pitch = 0;
		noteAr[i].collected = 0;
		noteAr[i].rxTrans = 0.0f;
		noteAr[i].ryTrans = 0.0f;
		noteAr[i].rxPos = 0.0f;
		noteAr[i].ryPos = 0.0f;
	}
	minPitch = -1;
	maxPitch = -1;
	myTotal = 0;
	lane = -1;
	populated = false;
}

MidiTextParser::~MidiTextParser(){
	//delete stuff if needed
}

int MidiTextParser::parse(string filename, int laneNum){

	lane = laneNum;
	populated = true;

	//-1.0 to 1.0f
	float tmpXPos = 0.0f;
	float tmpYPos = 0.0f;

	int tmpXBiasCtr = 0;
	int tmpYBiasCtr = 0;
	int pitchStack = 0;
	int pitchRunClamp = 6;

	ifstream infile(filename);
	string line = "";
	int ctr = 0;
	
	float ftemp = 0.0f;
	int itemp = 0;
	string token = "";

	if(infile){
		while(getline(infile, line, ';')){



			noteAr[myTotal].rxTrans = ((float)(rand() % 100) * 0.02f) - 1.0f;
			noteAr[myTotal].ryTrans = ((float)(rand() % 100) * 0.02f) - 1.0f;




			stringstream instring(line);
			while(getline(instring, token, ',')){
				switch(ctr){
				case 0:
					itemp = stoi(token);
					noteAr[myTotal].measure = itemp;
					break;
				case 1:
					itemp = stoi(token);
					noteAr[myTotal].beat = itemp;
					break;
				case 2:
					ftemp = stof(token);
					noteAr[myTotal].ratio = ftemp;
					break;
				case 3:
					itemp = stoi(token);
					noteAr[myTotal].pitch = itemp;


					if(myTotal>0){

						if(noteAr[myTotal].pitch != noteAr[myTotal-1].pitch){
							//kluge representing max time to make it accross grid
							float maxBeatTimeToTravelGrid = 1.875f 
								* ((float)(gGame.getBPM()) / 60.0f);
							float zPosDiff = ( (float)((noteAr[myTotal].measure * 4)
								+ noteAr[myTotal].beat) + noteAr[myTotal].ratio )
								- ( (float)((noteAr[myTotal-1].measure * 4)
								+ noteAr[myTotal-1].beat) + noteAr[myTotal-1].ratio );
							
							float rangeScalar = 1.0f;
							if(zPosDiff < maxBeatTimeToTravelGrid){
								rangeScalar = zPosDiff / maxBeatTimeToTravelGrid;
							}

							float randomizerxVal = ((float)((rand() % 500) + 750) * 0.001f);
							if(randomizerxVal > 1.0f)
								randomizerxVal -= 2.0f;
							float randomizeryVal = ((float)((rand() % 500) + 750) * 0.001f);
							if(randomizeryVal > 1.0f)
								randomizeryVal -= 2.0f;


							randomizerxVal *= rangeScalar;
							randomizeryVal *= rangeScalar;

							/*
							if((tmpXPos>0.0f && randomizerxVal>0.0f)
								|| (tmpXPos<0.0f && randomizerxVal<0.0f)){
								if((rand() % 100) < (int)(abs(tmpXPos) * 100)){
									randomizerxVal *= -1.0f;
								}
							}

							if((tmpYPos>0.0f && randomizeryVal>0.0f)
								|| (tmpYPos<0.0f && randomizeryVal<0.0f)){
								if((rand() % 100) < (int)(abs(tmpYPos) * 100)){
									randomizeryVal *= -1.0f;
								}
							}
							*/
							if(tmpXBiasCtr != 0){
								if(4 < (rand() % abs(tmpXBiasCtr))){
									if(randomizerxVal>0.0f && tmpXBiasCtr>0){
										randomizerxVal *= (-1.0f);
									}
									else if(randomizerxVal<0.0f && tmpXBiasCtr<0){
										randomizerxVal *= (-1.0f);
									}
								}
							}

							if(tmpYBiasCtr != 0){
								if(4 < (rand() % abs(tmpYBiasCtr))){
									if(randomizeryVal>0.0f && tmpYBiasCtr>0){
										randomizeryVal *= (-1.0f);
									}
									else if(randomizeryVal<0.0f && tmpYBiasCtr<0){
										randomizeryVal *= (-1.0f);
									}
								}
							}
							

							float xPosVal = noteAr[myTotal-1].rxPos + randomizerxVal;
							float yPosVal = noteAr[myTotal-1].ryPos + randomizeryVal;

							if((abs(xPosVal)) > 1.0f){
								xPosVal -= (2.0f * randomizerxVal);
							}

							if((abs(yPosVal)) > 1.0f){
								yPosVal -= (2.0f * randomizeryVal);
							}

							noteAr[myTotal].rxPos = xPosVal;
							noteAr[myTotal].ryPos = yPosVal;

							tmpXPos = xPosVal;
							tmpYPos = yPosVal;
							pitchStack = 0;
						}
						else{
							noteAr[myTotal].rxPos = tmpXPos;
							noteAr[myTotal].ryPos = tmpYPos;
							pitchStack++;
							if(pitchStack > pitchRunClamp){
								
								/*
								float zTotalDiff = ( (float)((noteAr[myTotal].measure * 4)
								+ noteAr[myTotal].beat) + noteAr[myTotal].ratio )
								- ( (float)((noteAr[myTotal-12].measure * 4)
								+ noteAr[myTotal-12].beat) + noteAr[myTotal-12].ratio );
								*/

								float randomizerxValp = ((float)((rand() % 1400) + 300) * 0.001f);
								if(randomizerxValp > 1.0f)
									randomizerxValp -= 2.0f;
								float randomizeryValp = ((float)((rand() % 1400) + 300) * 0.001f);
								if(randomizeryValp > 1.0f)
									randomizeryValp -= 2.0f;

								float xPosValp = noteAr[myTotal-pitchRunClamp].rxPos + randomizerxValp;
								float yPosValp = noteAr[myTotal-pitchRunClamp].ryPos + randomizeryValp;

								if((abs(xPosValp)) > 1.0f){
									xPosValp -= (2.0f * randomizerxValp);
								}

								if((abs(yPosValp)) > 1.0f){
									yPosValp -= (2.0f * randomizeryValp);
								}

								for(int q=1; q<(pitchRunClamp + 1); q++){
									noteAr[myTotal-(pitchRunClamp-q)].rxPos 
										+= ((((float)q) / (float)pitchRunClamp) * (xPosValp 
										- noteAr[myTotal-pitchRunClamp].rxPos));
									noteAr[myTotal-(pitchRunClamp-q)].ryPos 
										+= ((((float)q) / (float)pitchRunClamp) * (yPosValp 
										- noteAr[myTotal-pitchRunClamp].ryPos));
								}

								pitchStack = 0;
								tmpXPos = xPosValp;
								tmpYPos = yPosValp;
							}
						}
						
						if(tmpXPos < 0.0f){
							tmpXBiasCtr--;
						}
						else{
							tmpXBiasCtr++;
						}
						if(tmpYPos < 0.0f){
							tmpYBiasCtr--;
						}
						else{
							tmpYBiasCtr++;
						}

					}


					if(minPitch==-1)
						minPitch = itemp;
					if(maxPitch==-1)
						maxPitch = itemp;
					if(itemp<minPitch)
						minPitch = itemp;
					if(itemp>maxPitch)
						maxPitch = itemp;
					break;
				default:break;
				}
				instring.clear();
				if(ctr==3)
					myTotal++;
				ctr++;
				ctr = ctr % 4;
			}
		}
	}

	return 0;
}

int MidiTextParser::getMeasureValue(int index){
	return noteAr[index].measure;
}

int MidiTextParser::getBeatValue(int index){
	return noteAr[index].beat;
}

float MidiTextParser::getRatioValue(int index){
	return noteAr[index].ratio;
}

int MidiTextParser::getPitchValue(int index){
	return noteAr[index].pitch;
}

int MidiTextParser::getNumberOfNotes(){
	return myTotal;
}

int MidiTextParser::getLane(){
	return lane;
}

void MidiTextParser::setCollected(int index, int value){
	noteAr[index].collected = value;
}

int MidiTextParser::getCollected(int index){
	return noteAr[index].collected;
}

float MidiTextParser::getRXTrans(int index){
	return noteAr[index].rxTrans;
}

float MidiTextParser::getRYTrans(int index){
	return noteAr[index].ryTrans;
}

float MidiTextParser::getRXPos(int index){
	return noteAr[index].rxPos;
}

float MidiTextParser::getRYPos(int index){
	return noteAr[index].ryPos;
}

float MidiTextParser::getPitchRatio(int index){
	int tempMaxPitch = -1;
	int tempMinPitch = -1;
	for(int i=0; i<10; i++){
		if(tempMinPitch==-1)
			tempMinPitch = noteAr[index + i].pitch;
		if(tempMaxPitch==-1)
			tempMaxPitch = noteAr[index + i].pitch;
		if(noteAr[index + i].pitch<tempMinPitch)
			tempMinPitch = noteAr[index + i].pitch;
		if(noteAr[index + i].pitch>tempMaxPitch)
			tempMaxPitch = noteAr[index + i].pitch;
	}
	return (((float)(noteAr[index].pitch - tempMinPitch)
		/ (float)(tempMaxPitch - tempMinPitch)) * 2.0f) - 1.0f;
	/*
	return (float)(noteAr[index].pitch - minPitch)
		/ (float)(maxPitch - minPitch);
		*/
}

void MidiTextParser::clearVals(){
	if(populated==true){
		for(int i=0; i<800; i++){
			noteAr[i].measure = 0;
			noteAr[i].beat = 0;
			noteAr[i].ratio = 0.0f;
			noteAr[i].pitch = 0;
			noteAr[i].collected = 0;
			noteAr[i].rxTrans = 0.0f;
			noteAr[i].ryTrans = 0.0f;
			noteAr[i].rxPos = 0.0f;
			noteAr[i].ryPos = 0.0f;
		}
		minPitch = -1;
		maxPitch = -1;
		myTotal = 0;
		lane = -1;
	}
	populated = false;
}