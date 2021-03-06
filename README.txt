fast-cam-record-micro-enable
============================
fastCamRecord.exe repetitively records movies from microEnableIV frame grabber, and writes the movie data to disk concurrently.


Hardware
========
This program works with the following frame grabber:


microEnable IV AD4-CL (http://www.silicon-software.de/en/microenable4.html) 


This program was developed and tested with this fast speed camera:


OPTRONIS CL600X2-FULL-M (http://www.stemmer-imaging.co.uk/en/products/article/10802) 


This is a program for 64-bit Windows (> 7).


How to install
==============
The Windows Installer is available from my website: http://www.brain-recording.science


How to use the program
======================
1. Change the settings of the camera accoridngly.
2. Open the command prompt, and go to the directory the program is installed (C:\Program Files\LSENS_EPFL\ if not changed during installation)<sup>2</sup>.
3. fastCamRecord.exe accepts the following arguments.

	1. nrOfPicturesToGrab: The number of frames in each movie.
	
	2. width: Pixel number. Has to be a multiple of 32?
	
	3. height: Pixel number. Has to be a multiple of 32?
	
	4. timeout: The program waits for data from the camera for the specified time in seconds.
	
	5. iterationNum: The number of sweeps.
	
	6. folderName: The absolute path of the folder where the movies are stored. This folder has to exist.
	
	7. subfolderName: Subfolders are created in folderName.

4. Run the program with the arguments.
   e.g., fastCamRecord 1000 512 480 60 3 C:\Users\someone\Desktop\ sweep
5. If this software is useful to you, we'd appreciate it if you could acknowledge my work with
the URL of this repo (https://github.com/tarokiritani/fast-cam-record-micro-enable).

Note 1: You could write a .bat file, or run this program from another program ("dos" in matlab, or "subprocess.call()" in python).

Note 2: The installation does not touch the PATH environmental variable. 


How to program
==============
This program is written, and built in Microsoft Visual Studio Enterprise 2015.