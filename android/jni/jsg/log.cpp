#include "log.h"

#include <string.h>

using namespace v8;

//------------------------------------------------------------------------------

Persistent<FunctionTemplate> Console::constructor;

void Console::InitializeNative(Handle<ObjectTemplate> target)
{
  HandleScope scope;

  // Constructor
  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("Console"));

  // Prototype
  Local<ObjectTemplate> proto = constructor->PrototypeTemplate();
  proto->Set(String::NewSymbol("log"), FunctionTemplate::New(Log));
  target->Set(String::NewSymbol("Console"), constructor);
}

void Console::LoadScript() {
  node::Run("var console = new Console()", -1, __FILE__);
}

//------------------------------------------------------------------------------

Handle<Value> Console::New(const Arguments& args)
{
  HandleScope scope;

  Console* console = new Console();
  console->Wrap(args.This());

  return args.This();
}

Handle<Value> Console::Log(const Arguments& args) {
  static char buffer[10240];
  memset(buffer, 0, sizeof(buffer));

  bool first = true;
  for (int i = 0; i < args.Length(); i++) {
    HandleScope handle_scope;
    if (first) {
      first = false;
    } else {
      strcat(buffer, " ");
    }
    String::Utf8Value str(args[i]);
    const char* cstr = *str;
    strcat(buffer, cstr);  // FIXME: avoid overflow
  }

  LOGI("%s", buffer);
  return Undefined();
}
