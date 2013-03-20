/*
 * SoundPlay.cpp
 *
 *  Created on: Mar 17, 2013
 *      Author: ckchan
 */

#include "../header/SoundPlay.h"

SoundPlay::SoundPlay() {
	// TODO Auto-generated constructor stub

}

SoundPlay::~SoundPlay() {
	// TODO Auto-generated destructor stub
}

void SoundPlay::inputFile(char* input_name) {

	libvlc_instance_t *inst;
	libvlc_media_player_t *mp;
	libvlc_media_t *m;

	// load the vlc engine
	inst = libvlc_new(0, NULL);

	// create a new item
	m = libvlc_media_new_path(inst, input_name);

	// create a media play playing environment
	mp = libvlc_media_player_new_from_media(m);

	// no need to keep the media now
	libvlc_media_release(m);

	// play the media_player
	libvlc_media_player_play(mp);

	sleep(2);

	// stop playing
	libvlc_media_player_stop(mp);

	// free the media_player
	libvlc_media_player_release(mp);

	libvlc_release(inst);
}


