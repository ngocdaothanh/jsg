Directory structure
===================

::

  app                             Game source code
    main.coffee
    foo.coffee
  scripts
    app                           Compiled from "app" above
      main.js
      foo.js
    jsg
  assets
    scripts                       In dev mode, symlink to "scripts" above,
  src                             delete this symlink when release

Compile CoffeeScript files in bare mode:

::

  coffee -cbwo scripts/app/scripts app
  coffee -cbwo scripts/jsg jsg/src
