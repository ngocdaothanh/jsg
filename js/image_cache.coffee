jsg.imageCache = {}

# Loaded images are cached to save memory
jsg.loadImage = (src, onload, onerror) ->
  img = @imageCache[src]
  if img
    setTimeout((=> onload(img)), 100) if onload?
    return img

  img = new Image
  @imageCache[src] = img
  img.onload = (img) =>
    onload(img) if onload?
  img.onerror = (img) =>
    onerror(img) if onerror?
  img.src = src
  img

jsg.freeImage = (src) ->
  delete @images[src]
