jsg.plugin.billing =
  setPublicKey: (publicKey) ->
    jsg.android.callJava('js.g.plugin.Billing', 'setPublicKey', publicKey)

  purchase: (productId) ->
    jsg.android.callJava('js.g.plugin.Billing', 'purchase', productId)

  purchased: (listener) ->
    @purchaseds ?= []
    @purchaseds.push(listener)

  firePurchased: (productId, isOK) ->
    return if !@purchaseds?
    for listener in @purchaseds
      listener(productId, isOK)
