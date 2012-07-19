#ifndef _JSG_AUDIO_H_
#define _JSG_AUDIO_H_

#include <node_object_wrap.h>
#include <jni.h>

class MediaPlayer: public node::ObjectWrap
{
 public:
  static void InitializeNative(v8::Handle<v8::ObjectTemplate> target);

  static v8::Persistent<v8::FunctionTemplate> constructor;
  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> Play(const v8::Arguments& args);
  static v8::Handle<v8::Value> PlayLoop(const v8::Arguments& args);
  static v8::Handle<v8::Value> Pause(const v8::Arguments& args);
  static v8::Handle<v8::Value> Stop(const v8::Arguments& args);

 public:
  MediaPlayer(const char* src);
  ~MediaPlayer();
  void play();
  void playLoop();
  void pause();
  void stop();

 public:
  jobject jmediaPlayer;
};

#endif
