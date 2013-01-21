# For storing callback functions to be called from native platform (e.g. Android Java).
class Callback
  constructor: ->
    @id_fun      = {}
    @idGenerator = new IdGenerator

  register: (fun) =>
    id = @idGenerator.next()
    @id_fun[id] = fun
    id

  unregister: (id) =>
    delete @id_fun[id]

  invokeNoUnregister: (id, args...) =>
    @id_fun[id].apply(this, args)

  invoke: (id, args...) =>
    ret = @id_fun[id].apply(this, args)
    @unregister(id)
    ret

# jsg should have been created
jsg.callback = new Callback
