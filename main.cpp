#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <list>
#include <random>
#include <experimental/random>
#include <map>
#include <any>
#include <vector>

using namespace std;

Display *dis;
int screen;
Window win;
GC gc;

int WindowX = 100; // Actual size = WindowX * ParticleSize
int CenterX = WindowX / 2; // center for x
int WindowY = 100; // Actual size = WindowY * ParticleSize
int CenterY = WindowX / 2; // center for y
int ParticleSize = 8; // particles width / height

map<int, map<int, int>> emptyMap(int w, int h) {
    map<int, map<int, int>> oMap = {};
    for(int x = 0; x < w; x++) {
        oMap[x] = {};
        for(int y = 0; y < h; y++) {
            oMap[x][y] = 0;
        }
    }

    return oMap;
}

map<int, map<int, int>> randMap(int w, int h) {
    map<int, map<int, int>> oMap = {};
    for(int x = 0; x < w; x++) {
        oMap[x] = {};
        for(int y = 0; y < h; y++) {
            oMap[x][y] = experimental::randint(0,1);
        }
    }

    return oMap;
}

map<int, map<int, int>> curMap = randMap(WindowX, WindowY);

map<int, map<int, int>> nextMap = emptyMap(WindowX, WindowY);

int getNeighbors(int x, int y) {
    int total = 0;
    vector<vector<int>> neighbors = {
        {x + 1, y},
        {x - 1, y},
        {x, y + 1},
        {x, y - 1},
        {x + 1, y + 1},
        {x + 1, y - 1},
        {x - 1, y + 1},
        {x - 1, y - 1}
    };

    for (auto const& neighbor : neighbors) {
        try {
            total += curMap[neighbor[0]][neighbor[1]];
        } catch (std::exception) {

        }
    }

    return total;
}

int doParticle(int x, int y) {
    int neighbors = getNeighbors(x, y);
    if (curMap[x][y]) {
        if (neighbors == 2 || neighbors == 3) {
            return 1;
        } else {
            return 0;
        }
    } else {
        if (neighbors == 3) {
            return 1;
        } else {
            return 0;
        }
    }
}

void generation(int w, int h) {
    nextMap = emptyMap(WindowX, WindowY);

    for (int x = 0; x < w; x++) {
        for(int y = 0; y < h; y++) {
            nextMap[x][y] = doParticle(x, y);
        }
    }

    curMap = nextMap;
}

void draw(int w, int h) {
    for (int x = 0; x < w; x++) {
        for(int y = 0; y < h; y++) {
            if (curMap[x][y]) {
                XDrawRectangle(dis, win, gc, x*ParticleSize, y*ParticleSize, ParticleSize, ParticleSize);
            }
        }
    }
}

void init_x();
void redraw();
void close_x();

int main () {
	init_x();

	while(1) {		

		redraw();

        generation(WindowX, WindowY);
        draw(WindowX, WindowY);

		XFlush(dis);
	}
}

void init_x() {       
	unsigned long black,white;

	dis=XOpenDisplay((char *)0);
   	screen=DefaultScreen(dis);
	black=BlackPixel(dis, screen),
	white=WhitePixel(dis, screen);
   	win=XCreateSimpleWindow(dis,DefaultRootWindow(dis),0,0,	
		WindowX*ParticleSize, WindowY*ParticleSize, 5, white, black);
	XSetStandardProperties(dis, win, "Conway's GOL","Conway's GOL",None,NULL,0,NULL);
	XSelectInput(dis, win, ExposureMask|ButtonPressMask|KeyPressMask);
        gc=XCreateGC(dis, win, 0,0);        
	XSetBackground(dis, gc, black);
	XSetForeground(dis, gc, white);
	XClearWindow(dis, win);
	XMapRaised(dis, win);
};

void close_x() {
	XFreeGC(dis, gc);
	XDestroyWindow(dis,win);
	XCloseDisplay(dis);	
	exit(1);				
};

void redraw() {
	XClearWindow(dis, win);
};