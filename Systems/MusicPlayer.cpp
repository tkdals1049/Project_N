#include "stdafx.h"
#include "MusicPlayer.h"

MusicPlayer::MusicPlayer()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	buffer = NULL;
	DsBuffer = NULL;

	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	DSBUFFERDESC bufferDesc;
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFormat.cbSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	HRESULT hr = DirectSoundCreate8(0, &dsound8, 0);
	assert(SUCCEEDED(hr));
	hr = dsound8->SetCooperativeLevel(desc.Handle, DSSCL_NORMAL);
	assert(SUCCEEDED(hr));
}

MusicPlayer::~MusicPlayer()
{
	SAFE_RELEASE(buffer);
	SAFE_RELEASE(dsound8);
}

//음악파일 형식에 따른 함수 분류
bool MusicPlayer::SetSong(string file)
{
	if (file.find("ogg") != string::npos)
	{
		LoadingOgg(file);
		return true;
	}
	else if (file.find("wav") != string::npos)
	{
		LoadingWav(file);
		return true;
	}
	return false;
}

//Vorbis를 이용한 OGG파일 불러오기
void MusicPlayer::LoadingOgg(string file)
{
	OggVorbis_File vf;
	FILE * File;
	fopen_s(&File, file.c_str(), "rb");
	int result = ov_open(File, &vf, NULL, 0);
	vorbis_info* vi;
	vi = ov_info(&vf, -1);

	int SizeData = vi->channels * 16 * (int)ov_pcm_total(&vf, -1) / 8; char* pData;

	pData = new char[SizeData]; int bitstream;
	long Read = 0L;
	long Write = 0L;
	while (1) {

		Write = ov_read(&vf, pData + Read, SizeData - Read, 0, 2, 1, &bitstream);

		if (Write == 0) // 파일의 끝

			break;
		if (SizeData - Read >= 0) // 저장할 데이터가 아직 더 남아있음
			Read += Write;
		else // 디코딩 실패
			break;

	}

	WAVEFORMATEX* pWaveFormat;
	pWaveFormat = new WAVEFORMATEX;
	pWaveFormat->wFormatTag = WAVE_FORMAT_PCM;
	pWaveFormat->nChannels = vi->channels;
	pWaveFormat->nSamplesPerSec = vi->rate;
	pWaveFormat->nAvgBytesPerSec = vi->channels*vi->rate * 16 / 8;
	pWaveFormat->nBlockAlign = vi->channels * 16 / 8;
	pWaveFormat->wBitsPerSample = 16;
	pWaveFormat->cbSize = 0;
	
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE;
	dsbd.dwBufferBytes = SizeData;  // 위에서 구한 값.
	dsbd.lpwfxFormat = pWaveFormat;

	ZeroMemory(&DsBuffer, sizeof(IDirectSoundBuffer8));
	ZeroMemory(&buffer, sizeof(IDirectSoundBuffer));
	HRESULT hr = dsound8->CreateSoundBuffer(&dsbd, &buffer, NULL);
	assert(SUCCEEDED(hr));

	hr = buffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&DsBuffer);
	assert(SUCCEEDED(hr));

	VOID* pBuff1 = NULL;
	VOID* pBuff2 = NULL;
	DWORD dwLength;
	DWORD dwLength2;

	DsBuffer->Lock(0, dsbd.dwBufferBytes, &pBuff1, &dwLength, &pBuff2, &dwLength2, 0L);
	memcpy(pBuff1, pData, dwLength);
	memcpy(pBuff2, (pData + dwLength), dwLength2);
	DsBuffer->Unlock(pBuff1, dwLength, pBuff2, dwLength2);

	pBuff1 = pBuff2 = NULL;
	delete pWaveFormat;
	delete pData;
	ov_clear(&vf);
	fclose(File);
}

//음악파일을 통한 버퍼 생성
void MusicPlayer::LoadingWav(string file)
{
	HMMIO   hFile; // File Handle

	//CreateFile
	hFile = mmioOpen((LPWSTR)String::StringToWString(file).c_str(), NULL, MMIO_READ);//wave파일을 연다.

	//Chunk 청크 구조체, 문자열로 색인을 인식해서 WaveFormat 및 버퍼선언정보를 읽어온다.
	MMCKINFO    pParent;
	memset(&pParent, 0, sizeof(pParent));
	pParent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	mmioDescend(hFile, &pParent, NULL, MMIO_FINDRIFF);

	MMCKINFO    pChild;
	memset(&pChild, 0, sizeof(pChild));
	pChild.ckid = mmioFOURCC('f', 'm', 't', ' ');
	mmioDescend(hFile, &pChild, &pParent, MMIO_FINDCHUNK);

	WAVEFORMATEX    wft;
	memset(&wft, 0, sizeof(wft));
	mmioRead(hFile, (char*)&wft, sizeof(wft));

	mmioAscend(hFile, &pChild, 0);
	pChild.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmioDescend(hFile, &pChild, &pParent, MMIO_FINDCHUNK);

	// 생성할 버퍼의 선언정보. FLAG DSBCAPS_CTRLVOLUME 를 설정해야 사운드 조절이 가능하다.
	DSBUFFERDESC    BuffInfo;
	memset(&BuffInfo, 0, sizeof(DSBUFFERDESC));
	BuffInfo.dwBufferBytes = pChild.cksize;
	BuffInfo.dwSize = sizeof(DSBUFFERDESC);
	BuffInfo.dwFlags = DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE | DSBCAPS_CTRLVOLUME;
	BuffInfo.lpwfxFormat = &wft;

	ZeroMemory(&DsBuffer, sizeof(IDirectSoundBuffer8));
	ZeroMemory(&buffer, sizeof(IDirectSoundBuffer));

	HRESULT hr = dsound8->CreateSoundBuffer(&BuffInfo, &buffer, NULL);
	assert(SUCCEEDED(hr));

	hr = buffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&DsBuffer);
	assert(SUCCEEDED(hr));

	void* pWrite1 = NULL;
	void* pWrite2 = NULL;
	DWORD dwlength1, dwlength2;

	DsBuffer->Lock(0, pChild.cksize, &pWrite1, &dwlength1, &pWrite2, &dwlength2, 0L);

	if (pWrite1 > 0)mmioRead(hFile, (char*)pWrite1, dwlength1);
	if (pWrite2 > 0)mmioRead(hFile, (char*)pWrite2, dwlength2);

	DsBuffer->Unlock(pWrite1, dwlength1, pWrite2, dwlength2);

	mmioClose(hFile, 0);
}

void MusicPlayer::Start()
{
	DsBuffer->Play(0, 0, 0);
}

void MusicPlayer::Stop()
{
	DsBuffer->Stop();
}
