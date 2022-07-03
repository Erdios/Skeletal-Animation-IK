Environment and Library:
	+ Qt5.15.2 with openGL extension in Visual Studio 2019
	+ Eigen library
	+ c++ 11

How to use:
	+ Press  "Display BVH file" button to open an existed .bvh file.
	+ Press "Create Animation" to display an draggable figure.
	+ In the draggable mode:
		- tick "damped" box to use damped mode and can drag the slider under that box to change the value of lambda
			- the dragging movement control will be better with "damped"
		- click "Add Frame" button to add the current pose into your new .bvh file.
		- enter frame time in the text box next to the "Add Frame" button can change the frame time (can only be 0 - 1. if inputting invalid value, will use 0.5)
		- click "Generate New BVH File" button to generate a new .bvh file with your input in the previous steps.