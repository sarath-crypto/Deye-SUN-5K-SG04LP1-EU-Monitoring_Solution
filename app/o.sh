g++ -c motiondetector.cpp `pkg-config --cflags --libs opencv4` -Wno-psabi  -ldl -lm -g 
g++ -c scanner.cpp        `pkg-config --cflags --libs opencv4` -Wno-psabi  -ldl -lm -g 
g++ -c syscam.cpp         `pkg-config --cflags --libs opencv4` -llccv -Wno-psabi  -ldl -lm -g
g++ -c global.cpp 	   -g 
g++ -c amg8833.cpp 	   `pkg-config --cflags --libs opencv4` -Wno-psabi  -ldl -lm -g 
g++ -c bmp180.cpp 	   -g
g++ -c tcpc.cpp 	   -g
