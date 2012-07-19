#include "media_player.h"
#include "jsg.h"

using namespace v8;

//------------------------------------------------------------------------------

Persistent<FunctionTemplate> MediaPlayer::constructor;

void MediaPlayer::InitializeNative(Handle<ObjectTemplate> target)
{
  HandleScope scope;

  // Constructor
  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("MediaPlayer"));

  // Prototype
  Local<ObjectTemplate> proto = constructor->PrototypeTemplate();
  proto->Set(String::NewSymbol("play"), FunctionTemplate::New(Play));
  proto->Set(String::NewSymbol("playLoop"), FunctionTemplate::New(PlayLoop));
  proto->Set(String::NewSymbol("pause"), FunctionTemplate::New(Pause));
  proto->Set(String::NewSymbol("stop"), FunctionTemplate::New(Stop));
  target->Set(String::NewSymbol("MediaPlayer"), constructor);
}

//------------------------------------------------------------------------------

Handle<Value> MediaPlayer::New(const Arguments& args)
{
  HandleScope scope;
  String::Utf8Value src(args[0]);
  MediaPlayer* mediaPlayer = new MediaPlayer(*src);
  mediaPlayer->Wrap(args.This());
  return args.This();
}

Handle<Value> MediaPlayer::Play(const Arguments& args) {
  HandleScope scope;
  MediaPlayer* mediaPlayer = ObjectWrap::Unwrap<MediaPlayer>(args.This());
  mediaPlayer->play();
  return Undefined();
}

Handle<Value> MediaPlayer::PlayLoop(const Arguments& args) {
  HandleScope scope;
  MediaPlayer* mediaPlayer = ObjectWrap::Unwrap<MediaPlayer>(args.This());
  mediaPlayer->playLoop();
  return Undefined();
}

Handle<Value> MediaPlayer::Pause(const Arguments& args) {
  HandleScope scope;
  MediaPlayer* mediaPlayer = ObjectWrap::Unwrap<MediaPlayer>(args.This());
  mediaPlayer->pause();
  return Undefined();
}

Handle<Value> MediaPlayer::Stop(const Arguments& args) {
  HandleScope scope;
  MediaPlayer* mediaPlayer = ObjectWrap::Unwrap<MediaPlayer>(args.This());
  mediaPlayer->stop();
  return Undefined();
}

//------------------------------------------------------------------------------

MediaPlayer::MediaPlayer(const char* src)
{
  JNIEnv* env = JSG::getJNIEnv();

  static jclass    klass = NULL;
  static jmethodID mid   = NULL;
  if (!klass) {
    klass = JSG::findClassAndMakeGlobal("js/g/MediaPlayer");
    mid   = env->GetMethodID(klass, "<init>", "(Ljava/lang/String;)V");
  }

  // Will not be released because it's just a wrapper
  // Releasing will cause crash
  jstring jsrc = env->NewStringUTF(src);

  jmediaPlayer = JSG::refLocal2Global(env->NewObject(klass, mid, jsrc));
  env->DeleteLocalRef(jsrc);
}

MediaPlayer::~MediaPlayer()
{
  JNIEnv* env = JSG::getJNIEnv();
  env->DeleteGlobalRef(jmediaPlayer);
}

void MediaPlayer::play()
{
  JNIEnv* env = JSG::getJNIEnv();

  static jclass    klass = NULL;
  static jmethodID mid   = NULL;
  if (!klass) {
    klass = JSG::findClassAndMakeGlobal("js/g/MediaPlayer");
    mid   = env->GetMethodID(klass, "play", "()V");
  }

  env->CallVoidMethod(jmediaPlayer, mid);
}

void MediaPlayer::playLoop()
{
  JNIEnv* env = JSG::getJNIEnv();

  static jclass    klass = NULL;
  static jmethodID mid   = NULL;
  if (!klass) {
    klass = JSG::findClassAndMakeGlobal("js/g/MediaPlayer");
    mid   = env->GetMethodID(klass, "playLoop", "()V");
  }

  env->CallVoidMethod(jmediaPlayer, mid);
}

void MediaPlayer::pause()
{
  JNIEnv* env = JSG::getJNIEnv();

  static jclass    klass = NULL;
  static jmethodID mid   = NULL;
  if (!klass) {
    klass = JSG::findClassAndMakeGlobal("js/g/MediaPlayer");
    mid   = env->GetMethodID(klass, "pause", "()V");
  }

  env->CallVoidMethod(jmediaPlayer, mid);
}

void MediaPlayer::stop()
{
  JNIEnv* env = JSG::getJNIEnv();

  static jclass    klass = NULL;
  static jmethodID mid   = NULL;
  if (!klass) {
    klass = JSG::findClassAndMakeGlobal("js/g/MediaPlayer");
    mid   = env->GetMethodID(klass, "stop", "()V");
  }

  env->CallVoidMethod(jmediaPlayer, mid);
}
