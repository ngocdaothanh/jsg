Workflow
========

Assets

Development mode
----------------

cd assets
ln -s ../scripts

Production mode
---------------

cd assets
rm scripts

ruby tools/2c.rb scripts > jni/jsg_assets_2c.h
#define JSG_PROD
