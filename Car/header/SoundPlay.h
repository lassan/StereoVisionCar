/*
 * SoundPlay.h
 *
 *  Created on: Mar 17, 2013
 *      Author: ckchan
 */

#ifndef SOUNDPLAY_H_
#define SOUNDPLAY_H_
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <vlc/vlc.h>
#include <vlc/libvlc.h>

class SoundPlay {
public:
	SoundPlay();
	void inputFile(char* input_name);
	virtual ~SoundPlay();
private:

};

#endif /* SOUNDPLAY_H_ */
