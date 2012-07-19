Sound
=====

Convert .aif to .wav:
ffmpeg -i inputfile.aif -acodec libmp3lame -ab 128000 -ar 44100 outputfile.wav
