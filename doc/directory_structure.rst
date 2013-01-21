Directory structure
===================

::

  coffee               CoffeeScript source code in (if you write game in CoffeeScript)
    main.coffee
    foo.coffee
  scripts
    app                Compiled from "coffee" above
      main.js
      foo.js
    jsg
      [JSG .js files]
    covas
      [Covas .js files]
  assets
    scripts            Symlink to "scripts" above in during development.
                       Delete this symlink when release.
    images
  src                  Java source code

Compile CoffeeScript files in bare mode:

::

  coffee -cbwo scripts/app/scripts app
  coffee -cbwo scripts/jsg jsg/src
