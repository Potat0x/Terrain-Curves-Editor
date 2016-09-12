# Terrain-Curves-Editor v1.1
Terrain editor for 2D games based on Bezier curves.

##How to use

###Control

| Key|Action|
| ------------- |:----------------------------------:|
| N             | add point                          |
| Y             | add curve                          |
| X, Del        | delete pressed point                |
| Mouse Middle  | set point as node                   |
| Mouse Left    | set point as selected               |
| Mouse Right   | set point as pressed                |
| F1            | set all points as pressed           |
| F2            | set all points as selected          |
| Space         | create curve                        |
| Hold LShift + Mouse| select/press array             |
| Hold LCtrl         | select/press multiple points   |
| Ctrl + C/V         | copy/paste                     |
| Ctrl + Z/Shift+Z   | undo/redo                      |
When points are selected, you can move them.
When points are pressed, you can delete and copy them or create a curve.

###Creating curve
Press Y - new curve will be spawned.

or

set two points as pressed and set one as node, then press space.

##Using terrain in your game
When terrain is ready go File -> Export

Curves segments will be saved to txt file.

>x1 y1 x2 y2    //first segment

>x2 y2 x3 y3    //second

>x3 y3 x4 y 4   //third

Now you must load segments from file. If you need help go examples\example_loading.cpp

##Download
v1.0 http://goo.gl/nx9Nog

v1.1 http://goo.gl/U1tPeC
