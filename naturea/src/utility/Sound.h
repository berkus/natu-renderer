#pragma once
#include "globals.h"

#include "FMOD/inc/fmod.hpp"
#include "FMOD/inc/fmod_errors.h"




class Sound
{
public:

	Sound(void);
	~Sound(void);
	
	bool isPlaying;
	bool isPaused;

	void play();
	void pause();
	void stop();
	void update();
	FMOD::System     *ssystem;
    FMOD::Sound      *sound;
    FMOD::Channel    *channel;
    FMOD_RESULT       result;
    unsigned int      version;
	unsigned int	  pos;
	void ERRCHECK(FMOD_RESULT result)
	{
		if (result != FMOD_OK)
		{
			printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
			system("PAUSE");
			exit(-1);
		}
	};

};

