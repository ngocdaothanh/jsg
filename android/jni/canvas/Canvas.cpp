
//
// Canvas.cc
//
// Copyright (c) 2010 LearnBoost <tj@learnboost.com>
//

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <node_buffer.h>

#include "Canvas.h"
#include "CanvasRenderingContext2d.h"
#include "closure.h"

#include "Image.h"
#include "ImageData.h"
#include "PixelArray.h"
#include "CanvasGradient.h"
#include "CanvasPattern.h"

//------------------------------------------------------------------------------

void
Canvas::InitializeNative(Handle<ObjectTemplate> target) {
  Canvas::Initialize(target);
  Image::Initialize(target);
  ImageData::Initialize(target);
  PixelArray::Initialize(target);
  Context2d::Initialize(target);
  Gradient::Initialize(target);
  Pattern::Initialize(target);
  target->Set(String::New("cairoVersion"), String::New(cairo_version_string()));
}

#include "js/image.js.h"
#include "js/context2d.js.h"
#include "js/canvas.js.h"

void
Canvas::LoadScript() {
  node::Run(image_js,     image_js_len,     "image.js");
  node::Run(context2d_js, context2d_js_len, "context2d.js");
  node::Run(canvas_js,    canvas_js_len,    "canvas.js");
}

//------------------------------------------------------------------------------

Persistent<FunctionTemplate> Canvas::constructor;

/*
 * Initialize Canvas.
 */

void
Canvas::Initialize(Handle<ObjectTemplate> target) {
  HandleScope scope;

  // Constructor
  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(Canvas::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("Canvas"));

  // Prototype
  Local<ObjectTemplate> proto = constructor->PrototypeTemplate();
  NODE_SET_PROTOTYPE_METHOD(constructor, "toBuffer", ToBuffer);
  proto->SetAccessor(String::NewSymbol("width"), GetWidth);
  proto->SetAccessor(String::NewSymbol("height"), GetHeight);
  target->Set(String::NewSymbol("Canvas"), constructor);
}

/*
 * Initialize a Canvas with the given width and height.
 */

Handle<Value>
Canvas::New(const Arguments &args) {
  HandleScope scope;
  int width = 0, height = 0;
  if (args[0]->IsNumber()) width = args[0]->Uint32Value();
  if (args[1]->IsNumber()) height = args[1]->Uint32Value();
  Canvas *canvas = new Canvas(width, height);
  canvas->Wrap(args.This());
  return args.This();
}

/*
 * Get width.
 */

Handle<Value>
Canvas::GetWidth(Local<String> prop, const AccessorInfo &info) {
  HandleScope scope;
  Canvas *canvas = ObjectWrap::Unwrap<Canvas>(info.This());
  return scope.Close(Number::New(canvas->width));
}

/*
 * Get height.
 */

Handle<Value>
Canvas::GetHeight(Local<String> prop, const AccessorInfo &info) {
  HandleScope scope;
  Canvas *canvas = ObjectWrap::Unwrap<Canvas>(info.This());
  return scope.Close(Number::New(canvas->height));
}

/*
 * Canvas::ToBuffer callback.
 */

static cairo_status_t
toBuffer(void *c, const uint8_t *data, unsigned len) {
  closure_t *closure = (closure_t *) c;

  // Olaf: grow buffer
  if (closure->len + len > closure->max_len) {
    uint8_t *data;
    unsigned max = closure->max_len;

    // round to the nearest multiple of 1024 bytes
    max = (closure->max_len + len + 1023) & ~1023;

    data = (uint8_t *) realloc(closure->data, max);
    if (!data) return CAIRO_STATUS_NO_MEMORY;
    closure->data = data;
    closure->max_len = max;
  }

  memcpy(closure->data + closure->len, data, len);
  closure->len += len;

  return CAIRO_STATUS_SUCCESS;
}

/*
 * Convert PNG data to a node::Buffer, sync.
 */

Handle<Value>
Canvas::ToBuffer(const Arguments &args) {
  HandleScope scope;
  cairo_status_t status;
  Canvas *canvas = ObjectWrap::Unwrap<Canvas>(args.This());

  closure_t closure;
  status = closure_init(&closure, canvas);

  // ensure closure is ok
  if (status) {
    closure_destroy(&closure);
    return Canvas::Error(status);
  }

  TryCatch try_catch;
  status = cairo_surface_write_to_png_stream(canvas->surface(), toBuffer, &closure);

  if (try_catch.HasCaught()) {
    closure_destroy(&closure);
    return try_catch.ReThrow();
  } else if (status) {
    closure_destroy(&closure);
    return ThrowException(Canvas::Error(status));
  } else {
    Buffer *buf = Buffer::New(closure.len);
    memcpy(Buffer::Data(buf), closure.data, closure.len);
    closure_destroy(&closure);
    return buf->handle_;
  }
}

/*
 * Initialize cairo surface.
 */

Canvas::Canvas(int w, int h) {
  width = w;
  height = h;
  _surface = NULL;

  _surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h);
  assert(_surface);
  V8::AdjustAmountOfExternalAllocatedMemory(4 * w * h);
}

/*
 * Destroy cairo surface.
 */

Canvas::~Canvas() {
  cairo_surface_destroy(_surface);
}


/*
 * Construct an Error from the given cairo status.
 */

Local<Value>
Canvas::Error(cairo_status_t status) {
  return Exception::Error(String::New(cairo_status_to_string(status)));
}
