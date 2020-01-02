#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
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
    w=XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, 100, 100, 1,
                          BlackPixel(d, s), WhitePixel(d, s));
    Atom delWindow = XInternAtom( d, "WM_DELETE_WINDOW", 0 );
    XSetWMProtocols(d , w, &delWindow, 1);
    XSelectInput(d, w, ExposureMask | KeyPressMask);
    gc = XCreateGC(d,w,NULL,NULL);
    XColor color_cell;
    color_cell.flags = DoRed | DoGreen | DoBlue;
    color_cell.red = 10000;
    color_cell.green = 40000;
    color_cell.blue = 60000;
    XMapWindow(d, w);
    while(1) {
        XNextEvent(d, &e);
        /* draw or redraw the window */
        if(e.type==Expose) {
            //This is where the drawing or rendering happens
            XSetForeground(d,gc,color_cell.pixel);
            XFillRectangle(d, w, gc, 20, 20, 10, 10);

            XDrawLine(d,w,gc,0,0,120,120);
        }
        /* exit on key press */
        if(e.type==KeyPress)
            break;

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
