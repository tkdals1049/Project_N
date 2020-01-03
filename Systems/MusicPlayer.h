#pragma once

class MusicPlayer
{
public:
	MusicPlayer();
	~MusicPlayer();

	bool SetSong(string file);

	void Start();
	void Stop();
private:
	void LoadingOgg(string file);
	void LoadingWav(string file);

	DSBUFFERDESC  dsbd;
	IDirectSound8* dsound8 = 0;
	IDirectSoundBuffer8* DsBuffer;
	IDirectSoundBuffer*  buffer;
};