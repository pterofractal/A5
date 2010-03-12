#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include "algebra.hpp"
#include "polygon.hpp"
#include <gtkmm.h>
#include <gtkglmm.h>
#include <vector>

// The "main" OpenGL widget
class Viewer : public Gtk::GL::DrawingArea {
public:
	Viewer();
	virtual ~Viewer();
	enum DrawMode {
		WIRE,
		FACE,
		MULTICOLOURED
	};
	

		
	// A useful function that forces this widget to rerender. If you
	// want to render a new frame, do not call on_expose_event
	// directly. Instead call this, which will cause an on_expose_event
	// call when the time is right.
	void invalidate();
	void setDrawMode(DrawMode newDrawMode);
	
	void toggleBuffer();

		
	virtual bool on_key_press_event( GdkEventKey *ev );
 	bool step ();
	void lerp (Polygon::MovablePoint *pt, Point3D endPos, int steps = 100, bool loop = false);

protected:

	// Events we implement
	// Note that we could use gtkmm's "signals and slots" mechanism
	// instead, but for many classes there's a convenient member
	// function one just needs to define that'll be called with the
	// event.

	// Called when GL is first initialized
	virtual void on_realize();
	// Called when our window needs to be redrawn
	virtual bool on_expose_event(GdkEventExpose* event);
	// Called when the window is resized
	virtual bool on_configure_event(GdkEventConfigure* event);
	// Called when a mouse button is pressed
	virtual bool on_button_press_event(GdkEventButton* event);
	// Called when a mouse button is released
	virtual bool on_button_release_event(GdkEventButton* event);
	// Called when the mouse moves
	virtual bool on_motion_notify_event(GdkEventMotion* event);
	


private:

	void drawCube(int y, int x, int colourId, GLenum mode, bool multiColour = false);
	
	// Flags to denote which mouse buttons are down
	bool mouseB1Down, mouseB2Down, mouseB3Down;
	
	
	Point2D startPos, mouseDownPos, startScalePos, endScalePos;
	
	// Flag that determines when to use doubleBuffer
	bool doubleBuffer;
	
	// Timer used to call the tick method
	sigc::connection animTimer;
	
	
	// Lighting flag
	bool lightingFlag;
		
	// Label widgets
	Gtk::Label *scoreLabel, *linesClearedLabel;
	
	// List of objects
	std::vector<Polygon *> objects;
};

#endif
