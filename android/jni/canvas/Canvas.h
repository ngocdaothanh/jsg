
//
// Canvas.h
//
// Copyright (c) 2010 LearnBoost <tj@learnboost.com>
//

#ifndef __NODE_CANVAS_H__
#define __NODE_CANVAS_H__

#include <node_object_wrap.h>
#include <cairo.h>

using namespace v8;
using namespace node;

/*
 * Maxmimum states per context.
 * TODO: remove/resize
 */

#ifndef CANVAS_MAX_STATES
#define CANVAS_MAX_STATES 64
#endif

/*
 * Canvas.
 */

class Canvas: public node::ObjectWrap {
  public:
    int width;
    int height;

    static void InitializeNative(Handle<ObjectTemplate> target);
    static void LoadScript();

    static Persistent<FunctionTemplate> constructor;
    static void Initialize(Handle<ObjectTemplate> target);
    static Handle<Value> New(const Arguments &args);
    static Handle<Value> ToBuffer(const Arguments &args);
    static Handle<Value> GetWidth(Local<String> prop, const AccessorInfo &info);
    static Handle<Value> GetHeight(Local<String> prop, const AccessorInfo &info);
    static Local<Value> Error(cairo_status_t status);

    inline cairo_surface_t *surface(){ return _surface; }
    inline uint8_t *data(){ return cairo_image_surface_get_data(_surface); }
    inline int stride(){ return cairo_image_surface_get_stride(_surface); }
    Canvas(int width, int height);
    ~Canvas();

  private:
    cairo_surface_t *_surface;
};

#endif
