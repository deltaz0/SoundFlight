/*
 *
 * Hayden Barton
 * February 15, 2014
 *
 * Timer.cpp
 *
 */

#include "Timer.h"

// Constructor
Timer::Timer(int p_nBPM) {

  m_nBPM = p_nBPM;
  m_nStartTime = 0;

  // Compute Beats Per Second
  m_fBPS = (float)m_nBPM / (float)60;

  m_bCountdown = false;
  m_nCountdownRemain = m_nCountdownBeats = BEATS_PER_MEASURE;

}

// Returns the current beat
int Timer::getBeat() {

  return m_tTime.m_nBeat;

}

int Timer::getBPM() {
  return m_nBPM;
}

float Timer::getBPS() {
  return m_fBPS;
}

float Timer::getBeatTime() {
  return (1.0f / m_fBPS); // Return the time, in seconds, of one beat
}

// Returns the current measure
int Timer::getMeasure() {

  return m_tTime.m_nMeasure;

}

std::string Timer::getTimeString() {

  std::string t_sTime = "";

  t_sTime = std::to_string(m_tTime.m_nMeasure);
  t_sTime += ":";
  t_sTime += std::to_string(m_tTime.m_nBeat);

  return t_sTime;

}

char* Timer::getTimeStringC() {

  char* t_sTime;

  char buf[256];

	sprintf_s(buf,sizeof(char)*256,"%d:%d:%f", m_tTime.m_nMeasure, m_tTime.m_nBeat, m_fBeatFraction);

  t_sTime = buf;

  return t_sTime;

}

void Timer::setBPM(int p_nBPM) {

  m_nBPM = p_nBPM;

  // Compute Beats Per Second
  m_fBPS = (float)m_nBPM / (float)60;

}

void Timer::startTimer() {

  m_nStartTime = timeGetTime();

  m_tTime.m_nBeat = 1;
  m_tTime.m_nMeasure = 1;

}

void Timer::updateTimer() {

  int t_nTimeElapsed = (timeGetTime() - m_nStartTime);
  float t_fSeconds = (float)t_nTimeElapsed / 1000;        // Convert from milliseconds to seconds

  // Compute total number of beats since beginning
  float t_fBeats = t_fSeconds * m_fBPS;

  if(m_bCountdown) {
    if(m_nCountdownRemain != 0) {
      m_nCountdownRemain = (BEATS_PER_MEASURE - (int)t_fBeats);
    }
    else if(m_nCountdownRemain == 0) {
      m_bCountdown = false;
      // Restart Timer
      startTimer();
    }
  }
  else {
    m_tTime.m_nMeasure = (int)t_fBeats / BEATS_PER_MEASURE;
  	m_tTime.m_nBeat = (int)t_fBeats % BEATS_PER_MEASURE;
		m_fBeatFraction = t_fBeats - (int)t_fBeats;

  	m_tTime.m_nBeat += 1;
  	m_tTime.m_nMeasure += 1;
  }

}

float Timer::getBeatFraction(){
	return m_fBeatFraction;
}

void Timer::startCountdown() {

  m_bCountdown = true;

  m_nCountdownRemain = BEATS_PER_MEASURE;
}
