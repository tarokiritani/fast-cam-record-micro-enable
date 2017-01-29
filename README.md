fastVideoRecord
===============
fastVideoRecord.exe repetitively records movies from microEnableIV frame grabber, and write the movie data to disk concurrently.

Hardware
========
This program works with the following frame grabber and camera.


Frame grabber: microEnable IV AD4-CL (http://www.silicon-software.de/en/microenable4.html) 


Camera: OPTRONIS CL600X2-FULL-M (http://www.stemmer-imaging.co.uk/en/products/article/10802) 


Installation
============
It might be necessary to first install Visual C++ Redistributable (http://www.microsoft.com/en-us/download/details.aspx?id=30679).

It is also necessary to install the driver for the frame grabber. This is included
in the driver CD you get from the company.

How to use the program
======================
1. fastVideoRecord takes the following arguments.

	1. nrOfPicturesToGrab: The number of frames in each movie.
	
	2. width: Pixel number. Has to be a multiple of 32?
	
	3. height: Pixel number. Has to be a multiple of 32?
	
	4. timeout: The program waits for data from the camera for the specified time in seconds.
	
	5. iterationNum: The number of sweeps.
	
	6. folderName: The absolute path of the folder where the movies are stored. This folder has to exist.
	
	7. subfolderName: Subfolders are created in folderName.

2. Change the settings of the camera accoridngly.
3. Run the program from the command line or .bat file.
	e.g., fastVideoRecord 1000 512 480 60 1 "C:\Users\someone\Desktop\test\\" "sweep"
4. If this software is useful to you, we'd appreciate it if you acknowledged my work with
the URL (https://github.com/tarokiritani/videoSweep)


How to program
==============
This program is written in Microsoft Visual Studio Professional 2012.
