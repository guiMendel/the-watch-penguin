#include "Sound.h"
#include "Resources.h"

using namespace Helper;
using namespace std;

Sound::Sound(GameObject &associatedObject, const string fileName, bool playOnStart) : Component(associatedObject), chunkPath(fileName), playOnStart(playOnStart) {}

void Sound::Play(const int times)
{
  Mix_Chunk &sound = Resources::GetSound(chunkPath);

  // Play and memorize channel
  channel = Mix_PlayChannel(-1, &sound, times - 1);
}

void Sound::Stop()
{
  // Catch no channel registered
  if (channel < 0)
    return;

  // Stop the channel that was previously used
  Mix_HaltChannel(channel);

  // Resets channel
  channel = -1;
}

void Sound::Start()
{
  if (playOnStart)
    Play();
}