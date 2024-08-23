#include "audio.h"
#include <ESP8266Audio.h>
#include "AudioGeneratorMIDI.h"
#include "AudioFileSourceFunction.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2SNoDAC.h"
#include <ESP8266SAM.h>
#include "core/mykeyboard.h"


#if defined(HAS_NS4168_SPKR)

bool playAudioFile(FS* fs, String filepath) {
  
  AudioFileSource* source = new AudioFileSourceFS(*fs, filepath.c_str());
  if(!source) return false;
  
  AudioOutputI2S* audioout = new AudioOutputI2S();  // https://github.com/earlephilhower/ESP8266Audio/blob/master/src/AudioOutputI2S.cpp#L32
  audioout->SetPinout(BCLK, WCLK, DOUT);

  AudioGenerator* generator = NULL;

  // switch on extension
  filepath.toLowerCase(); // case-insensitive match
  if (filepath.endsWith(".txt") || filepath.endsWith(".rtttl"))
    generator = new AudioGeneratorRTTTL();
  if (filepath.endsWith(".wav")) 
    generator = new AudioGeneratorWAV();
  if (filepath.endsWith(".mod")) 
    generator = new AudioGeneratorMOD();
  if (filepath.endsWith(".opus")) 
    generator = new AudioGeneratorOpus();
  if (filepath.endsWith(".mp3")) {
    generator = new AudioGeneratorMP3();
    source = new AudioFileSourceID3(source);
  }
  /* 2FIX: compilation issues 
  if(filepath.endsWith(".mid"))  {
    // need to load a soundfont
    AudioFileSource* sf2 = NULL;
    if(setupSdCard()) sf2 = new AudioFileSourceFS(SD, "1mgm.sf2");  // TODO: make configurable
    if(!sf2) sf2 = new AudioFileSourceLittleFS(LittleFS, "1mgm.sf2");  // TODO: make configurable
    if(!sf2) return false;  // a soundfount was not found
    AudioGeneratorMIDI* midi = new AudioGeneratorMIDI();
    midi->SetSoundfont(sf2);
    generator = midi;
  } */
    
  if (generator && source && audioout) {
    Serial.println("Start audio");
    generator->begin(source, audioout);
    while (generator->isRunning()) {
      if (!generator->loop() || checkAnyKeyPress() ) generator->stop();
    }
    audioout->stop();
    source->close();
    Serial.println("Stop audio");

    delete generator;
    delete source;
    delete audioout;
    
    return true;
  }
  // else    
  return false;  // init error
}

bool playAudioRTTTLString(String song) {
  // derived from https://github.com/earlephilhower/ESP8266Audio/blob/master/examples/PlayRTTTLToI2SDAC/PlayRTTTLToI2SDAC.ino
  
  song.trim();
  if(song=="") return false;
  
  AudioOutputI2S* audioout = new AudioOutputI2S();
  audioout->SetPinout(BCLK, WCLK, DOUT);
  
  AudioGenerator* generator = new AudioGeneratorRTTTL();
  
  AudioFileSource* source = new AudioFileSourcePROGMEM( song.c_str(), song.length() );
    
  if (generator && source && audioout) {
    Serial.println("Start audio");
    generator->begin(source, audioout);
    while (generator->isRunning()) {
      if (!generator->loop() || checkAnyKeyPress() ) generator->stop();
    }
    audioout->stop();
    source->close();
    Serial.println("Stop audio");

    delete generator;
    delete source;
    delete audioout;
    
    return true;
  }
  // else
  return false;    // init error
}

bool tts(String text){
  text.trim();
  if(text=="") return false;
  
  AudioOutputI2S* audioout = new AudioOutputI2S();
  audioout->SetPinout(BCLK, WCLK, DOUT);
  
  // https://github.com/earlephilhower/ESP8266SAM/blob/master/examples/Speak/Speak.ino
  audioout->begin();
  ESP8266SAM *sam = new ESP8266SAM;
  sam->Say(audioout, text.c_str());
  delete sam;
  return true;
}


bool isAudioFile(String filepath) {
    
    return filepath.endsWith(".opus") || filepath.endsWith(".rtttl") || 
        filepath.endsWith(".wav") || filepath.endsWith(".mod") || filepath.endsWith(".mp3") ;
}


void playTone(unsigned int frequency, unsigned long duration, short waveType)
{
  // derived from https://github.com/earlephilhower/ESP8266Audio/blob/master/examples/PlayWAVFromFunction/PlayWAVFromFunction.ino
  
  if(frequency==0 || duration==0) return;
  
  float hz = frequency;

  AudioGeneratorWAV* wav;
  AudioFileSourceFunction* file;
  AudioOutputI2S* out = new AudioOutputI2S();
  out->SetPinout(BCLK, WCLK, DOUT);
  
  file = new AudioFileSourceFunction( duration/1000.0);  // , 1, 44100
  //
  // you can set (sec, channels, hz, bit/sample) but you should care about
  // the trade-off between performance and the audio quality
  //
  // file = new AudioFileSourceFunction(sec, channels, hz, bit/sample);
  // channels   : default = 1
  // hz         : default = 8000 (8000, 11025, 22050, 44100, 48000, etc.)
  // bit/sample : default = 16 (8, 16, 32)

  // ===== set your sound function =====
  
  if(waveType==0) { // square
    file->addAudioGenerators([&](const float time) {
      float v = ( sin(hz * time) >= 0 ) ? 1.0f : -1.0f;;  // generate square wave
      v *= 0.1;                           // scale
      return v;
    });
  }  
  else if(waveType==1) { // sine
    file->addAudioGenerators([&](const float time) {
      float v = sin(TWO_PI * hz * time);  // generate sine wave
      v *= fmod(time, 1.f);               // change linear
      v *= 0.1;                           // scale
      return v;
    });
  }
  // TODO: more wave types: triangle, sawtooth
  //
  // sound function should have one argument(float) and one return(float)
  // param  : float (current time [sec] of the song)
  // return : float (the amplitude of sound which varies from -1.f to +1.f)

  wav = new AudioGeneratorWAV();
  wav->begin(file, out);
  
  while (wav->isRunning()) {
    if (!wav->loop() || checkAnyKeyPress()) wav->stop();
  }
  
  delete file;
  delete wav;
  delete out;
}

#endif


void _tone(unsigned int frequency, unsigned long duration) {
#if defined(BUZZ_PIN)
  tone(BUZZ_PIN, frequency, duration);
#elif defined(HAS_NS4168_SPKR)
  //  alt. implementation using the speaker
  playTone(frequency, duration, 0);
#endif
}

void playDTMF(String input) {
    
  const int DTMF_frequencies[][2] = {
    {697, 1209},  // 1
    {697, 1336},  // 2
    {697, 1477},  // 3
    {770, 1209},  // 4
    {770, 1336},  // 5
    {770, 1477},  // 6
    {852, 1209},  // 7
    {852, 1336},  // 8
    {852, 1477},  // 9
    {941, 1336},  // 0
    {941, 1209},  // *
    {941, 1477}   // #
  };

  for (int i = 0; i < input.length(); i++) {
      char key = input.charAt(i);
      int index;

      // Determine the index based on the key pressed
      switch (key) {
        case '1': index = 0; break;
        case '2': index = 1; break;
        case '3': index = 2; break;
        case '4': index = 3; break;
        case '5': index = 4; break;
        case '6': index = 5; break;
        case '7': index = 6; break;
        case '8': index = 7; break;
        case '9': index = 8; break;
        case '0': index = 9; break;
        case '*': index = 10; break;
        case '#': index = 11; break;
        default: continue; // Invalid key, skip to the next character
      }

      // Play the two tones sequentially using _tone function
      _tone(DTMF_frequencies[index][0], 300);
      _tone(DTMF_frequencies[index][1], 300);

      delay(50); // Short delay between tones
  }
}