#include "Sound.h"


Sound::Sound(void)
{
	channel = 0;
	pos = 0;
	unsigned int length = 0;
	isPlaying = false;
	isPaused  = false;
	result = FMOD::System_Create(&ssystem);
    ERRCHECK(result);

    result = ssystem->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
    } else {
		result = ssystem->init(32, FMOD_INIT_NORMAL, 0);
		ERRCHECK(result);
		result = ssystem->createStream("media/background.mp3",FMOD_HARDWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &sound);
		ERRCHECK(result);

		result = sound->getLength(&length, FMOD_TIMEUNIT_MS);
		ERRCHECK(result);
		pos = randomui(0,length); 
	}
}


void Sound::play(){
	if (!isPlaying){
	 result = ssystem->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
	 ERRCHECK(result);
	 printf("RANDOM PLAYBACK POS: %ui\n", pos);
	 result = channel->setPosition(pos, FMOD_TIMEUNIT_MS);
	 ERRCHECK(result);
	} 
	if (isPaused){
		channel->getPaused(&isPaused);
        channel->setPaused(!isPaused);
		isPaused = false;
	}

	isPlaying = true;
}
void Sound::pause(){
	if (!isPaused){
		channel->getPaused(&isPaused);
        channel->setPaused(!isPaused);
		isPaused = true;
	}
}
void Sound::stop(){
	isPlaying = false;
	isPaused  = false;
	result = channel->stop();
    ERRCHECK(result);
}

void Sound::update(){
	ssystem->update();
}

Sound::~Sound(void)
{
	result = sound->release();
    ERRCHECK(result);

    result = ssystem->close();
    ERRCHECK(result);
    result = ssystem->release();
    ERRCHECK(result);
}
