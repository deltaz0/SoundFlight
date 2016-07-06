/*
 *
 * Hayden Barton
 * February 15, 2014
 *
 * Timer.h
 *
 */

#pragma once

#include <string>
#include <windows.h>

#define BEATS_PER_MEASURE 4   // Using a 4:4 song structure

class Timer {


protected:
  int m_nBPM;         // Beats Per Minute
  int m_nStartTime;   // NOTE: time stored in milliseconds

	float m_fBeatFraction;

  float m_fBPS;       // Beats Per Second

  struct musicTimer {

    int m_nMeasure;
    int m_nBeat;

  };

  typedef struct musicTimer MusicTimer;

  MusicTimer m_tTime;

  bool m_bCountdown;
  int m_nCountdownBeats;
  int m_nCountdownRemain;


public:
  Timer(int p_nBPM);    // Constructor

  int getBeat();
  int getMeasure();
  int getBPM();
  float getBPS();
  float getBeatTime();
  std::string getTimeString();
  char* getTimeStringC();
  void setBPM(int p_nBPM);
  void startTimer();
  void updateTimer();
	float getBeatFraction();

  void startCountdown();
  bool isCountdown() { return m_bCountdown; }
  int getCountdownRemain() { return m_nCountdownRemain; }

};