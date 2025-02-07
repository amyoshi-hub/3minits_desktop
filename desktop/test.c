/* TinyWM is written by Nick Welch <mack@incise.org>, 2005.
 *
 * This software is in the public domain
 * and is provided AS IS, with NO WARRANTY. */

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))


void raise_window(Display *dpy, Window window){
	XRaiseWindow(dpy, window);

	Atom wm_state = XInternAtom(dpy, "_NET_WM_STATE", False);
	Atom fullscreen = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);

    	XEvent xev = {0};
    	xev.xclient.type = ClientMessage;
    	xev.xclient.message_type = wm_state;
    	xev.xclient.window = window;
    	xev.xclient.format = 32;
    	xev.xclient.data.l[0] = 0;
    	xev.xclient.data.l[1] = fullscreen;
    	xev.xclient.data.l[2] = 0;

    XSendEvent(dpy, DefaultRootWindow(dpy), False,
               SubstructureNotifyMask | SubstructureRedirectMask,
               &xev);
}

int main()
{
    Display * dpy;
    Window root, current_focus = None;
    int revert;
    XWindowAttributes attr;
    XButtonEvent start;
    XEvent ev;
    Window *children;
    unsigned int num_children = 0;
    int current_index = -1;

    if(!(dpy = XOpenDisplay(0x0))) return 1;

    root = DefaultRootWindow(dpy);

    XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("Tab")), Mod1Mask, root,
            True, GrabModeAsync, GrabModeAsync);
    XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("F4")), Mod1Mask, root,
	    True, GrabModeAsync, GrabModeAsync);

    XGrabButton(dpy, 1, AnyModifier, root, True, ButtonPressMask, GrabModeAsync,
            GrabModeAsync, None, None);
    XGrabButton(dpy, 3, AnyModifier, root, True, ButtonPressMask, GrabModeAsync,
            GrabModeAsync, None, None);

    for(;;)
    {
        XNextEvent(dpy, &ev);
        if(ev.type == KeyPress && ev.xkey.keycode == XKeysymToKeycode(dpy, XStringToKeysym("Tab"))){
	    if(XQueryTree(dpy, root, &root, &current_focus, &children, &num_children) && num_children > 0){
	   	current_index = (current_index + 1) % num_children; 
		current_focus = children[current_index];
		raise_window(dpy,children[current_index]);
		XSetInputFocus(dpy, children[current_index], RevertToPointerRoot, CurrentTime);
	    }
	    if(children) XFree(children);
            //XRaiseWindow(dpy, ev.xkey.subwindow);
	}
	else if(ev.type == KeyPress && ev.xkey.keycode == XKeysymToKeycode(dpy, XStringToKeysym("F4"))){
		if(current_focus != None){
			XDestroyWindow(dpy, current_focus);	
		}	
	}
        else if(ev.type == ButtonPress && ev.xbutton.subwindow != None)
        {
	    raise_window(dpy, ev.xbutton.subwindow);
            XGrabPointer(dpy, ev.xbutton.subwindow, True,
                    PointerMotionMask|ButtonReleaseMask, GrabModeAsync,
                    GrabModeAsync, None, None, CurrentTime);
            XGetWindowAttributes(dpy, ev.xbutton.subwindow, &attr);
            start = ev.xbutton;
        }
        else if(ev.type == MotionNotify && ev.xmotion.subwindow != None)
        {
            int xdiff, ydiff;
            xdiff = ev.xbutton.x_root - start.x_root;
            ydiff = ev.xbutton.y_root - start.y_root;

	    if(start.button == 1){
		    XMoveWindow(dpy, ev.xmotion.window,
				attr.x + xdiff, attr.y + ydiff);
	    }else if(start.button == 3){
	   	XResizeWindow(dpy, ev.xmotion.window,
				MAX(1, attr.width + xdiff), MAX(1, attr.height + ydiff));
	    }
        }
        else if(ev.type == ButtonRelease)
            XUngrabPointer(dpy, CurrentTime);
	XFlush(dpy);
    }
}

