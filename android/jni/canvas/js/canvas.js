
/*!
 * Canvas - Context2d
 * Copyright (c) 2010 LearnBoost <tj@learnboost.com>
 * MIT Licensed
 */

Canvas.prototype.inspect = function(){
  return '[Canvas ' + this.width + 'x' + this.height + ']';
};

Canvas.prototype.getContext = function(contextId){
  if ('2d' == contextId) {
    var ctx = this._context2d || (this._context2d = new Context2d(this));
    this.context = ctx;
    ctx.canvas = this;
    return ctx;
  }
};

Canvas.prototype.toDataURL = function(type, fn){
  // Default to png
  type = type || 'image/png';

  // Allow callback as first arg
  if ('function' == typeof type) fn = type, type = 'image/png';

  // Throw on non-png
  if ('image/png' != type) throw new Error('currently only image/png is supported');

  var prefix = 'data:' + type + ';base64,';

  if (fn) {
    this.toBuffer(function(err, buf){
      if (err) return fn(err);
      var str = 'data:' + type
      fn(null, prefix + buf.toString('base64'));
    });
  } else {
    return prefix + this.toBuffer().toString('base64');
  }
};
