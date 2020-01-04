#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include "gui_library.h"
int main() {

    Display *d;
    int s;
    Window w;
    GC gc;
    XEvent e;

    /* open connection with the server */
    d=XOpenDisplay(NULL);
    if(d==NULL) {
        printf("Cannot open display\n");
        exit(1);
    }
    s=DefaultScreen(d);
    XColor color_cell;
    color_cell.flags = DoRed | DoGreen | DoBlue;
    color_cell.red = 10000;
    color_cell.green = 40000;
    color_cell.blue = 60000;
    w=XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, 1000, 1000, 1,
                          WhitePixel(d, s), BlackPixel(d, s));
    Atom delWindow = XInternAtom( d, "WM_DELETE_WINDOW", 0 );
    XSetWMProtocols(d , w, &delWindow, 1);
    XSelectInput(d, w, ExposureMask | KeyPressMask);
    gc = XCreateGC(d,w,NULL,NULL);

    XMapWindow(d, w);
    menuPage mainPage = menuPage();
    mainPage.setTitle("Main Page");
    menuOption testOption = menuOption("Hello World.");
    menuOption testOption2 = menuOption("Option2.");
    float value = 5.12;
    editorLabel Param1 = editorLabel("Factor: ",250,250,0,&value);
    mainPage.addWidget(&testOption);
    mainPage.addWidget(&testOption2);
    mainPage.addWidget(&Param1);
    Renderer renderer = Renderer(d,&w,&gc);
    renderer.addPage(&mainPage);
    while(1) {
        XNextEvent(d, &e);
        /* draw or redraw the window */
        if(e.type==Expose) {
            //This is where the drawing or rendering happens
            renderer.render();
        }
        if(e.xkey.keycode == 30)//30 = "u" key
        {
            renderer.moveSelectedItem(DIR_UP);

        }
        if(e.xkey.keycode == 40)//40 = "d" key
        {
            renderer.moveSelectedItem(DIR_DOWN);

        }
        if(e.xkey.keycode == 36)//36 = enter key
        {
            renderer.selectCurrentOption();
        }
        if(e.xkey.keycode == 56)//56 = 'b' key
        {
            renderer.back();
        }
        // Handle Windows Close Event
        if(e.type==ClientMessage)
            break;
    }

    /* destroy our window */
    XDestroyWindow(d, w);

    /* close connection to server */
    XCloseDisplay(d);
    return 0;
}
