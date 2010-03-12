#include "viewer.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <assert.h>
#include "appwindow.hpp"


using namespace std;

Viewer::Viewer()
{
	
	// By default turn double buffer on
	doubleBuffer = true;

	Glib::RefPtr<Gdk::GL::Config> glconfig;
	
	// Ask for an OpenGL Setup with
	//  - red, green and blue component colour
	//  - a depth buffer to avoid things overlapping wrongly
	//  - double-buffered rendering to avoid tearing/flickering
	//	- Multisample rendering to smooth out edges
	glconfig = Gdk::GL::Config::create(	Gdk::GL::MODE_RGB |
										Gdk::GL::MODE_DEPTH |
										Gdk::GL::MODE_DOUBLE |
										Gdk::GL::MODE_MULTISAMPLE );
	if (glconfig == 0) {
	  // If we can't get this configuration, die
	  abort();
	}

	// Accept the configuration
	set_gl_capability(glconfig);

	// Register the fact that we want to receive these events
	add_events(	Gdk::BUTTON1_MOTION_MASK	|
				Gdk::BUTTON2_MOTION_MASK    |
				Gdk::BUTTON3_MOTION_MASK    |
				Gdk::BUTTON_PRESS_MASK      | 
				Gdk::BUTTON_RELEASE_MASK    |
				Gdk::KEY_PRESS_MASK 		|
				Gdk::VISIBILITY_NOTIFY_MASK);
	
/*	Polygon *test = new Polygon("test");
	std::vector<Polygon::MovablePoint> vertices;
	Polygon::MovablePoint vertex;
	vertex.loopMotion = false;
	vertex.currentPos = Point3D(0, 0, 0);
	vertices.push_back(vertex);
	vertex.currentPos = Point3D(0, 1, 0);
	vertices.push_back(vertex);
	vertex.currentPos = Point3D(1, 1, 0);
	vertices.push_back(vertex);	
	vertex.currentPos = Point3D(1, 0, 0);
	vertices.push_back(vertex);	
	
	test->vertices = vertices;
	objects.push_back(test);*/
	
	string name;
	double xCoord, yCoord, zCoord;
	int numVertices;
	ifstream partsFile("parts.txt");
	if (partsFile.is_open() )
	{
		while (! partsFile.eof() )
		{
			getline (partsFile, name);
			Polygon *poly = new Polygon(name);
			partsFile >> numVertices;
			for (int i = 0;i<numVertices;i++)
			{
				partsFile >> xCoord;
				partsFile >> yCoord;
				partsFile >> zCoord;
				Point3D vertex(xCoord, yCoord, zCoord);
				Polygon::MovablePoint movablePt;
				movablePt.currentPos = vertex;
				movablePt.loopMotion = false;
				poly->vertices.push_back(movablePt);
			}
			objects.push_back(poly);
						cerr << name << ", " << numVertices << ", " << xCoord << ", " << yCoord << ", " << zCoord << "\n";			
			getline (partsFile, name);
		}
	}
	
	animTimer = Glib::signal_timeout().connect(sigc::mem_fun(*this, &Viewer::step), 100);
}

Viewer::~Viewer()
{

}

void Viewer::invalidate()
{
  //Force a rerender
  Gtk::Allocation allocation = get_allocation();
  get_window()->invalidate_rect( allocation, false);
  
}

void Viewer::on_realize()
{
	// Do some OpenGL setup.
	// First, let the base class do whatever it needs to
	Gtk::GL::DrawingArea::on_realize();
	
	Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();
	
	if (!gldrawable)
		return;
	
	if (!gldrawable->gl_begin(get_gl_context()))
		return;
	
	// Just enable depth testing and set the background colour.
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.7, 0.7, 1.0, 0.0);
	glEnable (GL_TEXTURE_2D);
	
	gldrawable->gl_end();
}

bool Viewer::on_expose_event(GdkEventExpose* event)
{
	Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();
	
	if (!gldrawable) return false;

	if (!gldrawable->gl_begin(get_gl_context()))
		return false;

	// Decide which buffer to write to
	if (doubleBuffer)
		glDrawBuffer(GL_BACK);	
	else
		glDrawBuffer(GL_FRONT);
		
			
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Modify the current projection matrix so that we move the 
	// camera away from the origin.  We'll draw the game at the
	// origin, and we need to back up to see it.

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glTranslated(0.0, 0.0, -40.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// set up lighting (if necessary)
	// Followed the tutorial found http://www.falloutsoftware.com/tutorials/gl/gl8.htm
	// to implement lighting
	
	// Initialize lighting settings
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_LIGHTING);
	
	// Create one light source
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	// Define properties of light 
	float ambientLight0[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	float diffuseLight0[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float specularLight0[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	float position0[] = { 5.0f, 0.0f, 0.0f, 1.0f };	
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight0);
	glLightfv(GL_LIGHT0, GL_POSITION, position0);
	
	// Scale and rotate the scene

	
	// Increment rotation angles for next render

	// You'll be drawing unit cubes, so the game will have width
	// 10 and height 24 (game = 20, stripe = 4).  Let's translate
	// the game so that we can draw it starting at (0,0) but have
	// it appear centered in the window.
	glTranslated(-5.0, -12.0, 0.0);
	
	// Draw shit
	glLineWidth (2);
	for (int i = 0;i<objects.size();i++)
	{
		glBegin(GL_LINE_LOOP);
		for (int j = 0;j<objects[i]->vertices.size();j++)
		{
			glVertex3d(objects[i]->vertices[j].currentPos[0], objects[i]->vertices[j].currentPos[1], objects[i]->vertices[j].currentPos[2]);
		}
		glEnd();
	}
	
 	// We pushed a matrix onto the PROJECTION stack earlier, we 
	// need to pop it.

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// Swap the contents of the front and back buffers so we see what we
	// just drew. This should only be done if double buffering is enabled.
	if (doubleBuffer)
		gldrawable->swap_buffers();
	else
		glFlush();	
		
	gldrawable->gl_end();

	return true;
}

bool Viewer::on_configure_event(GdkEventConfigure* event)
{
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable) return false;
  
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  // Set up perspective projection, using current size and aspect
  // ratio of display

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, event->width, event->height);
  gluPerspective(40.0, (GLfloat)event->width/(GLfloat)event->height, 0.1, 1000.0);

  // Reset to modelview matrix mode
  
  glMatrixMode(GL_MODELVIEW);

  gldrawable->gl_end();

  return true;
}

bool Viewer::on_button_press_event(GdkEventButton* event)
{
	startPos[0] = event->x;
	startPos[1] = event->y;
	mouseDownPos[0] = event->x;
	mouseDownPos[1] = event->y;
	
	// Set our appropriate flags to true
	if (event->button == 1)
		mouseB1Down = true;	
	if (event->button == 2)
		mouseB2Down = true;
	if (event->button == 3)
		mouseB3Down = true;

	return true;
}

bool Viewer::on_button_release_event(GdkEventButton* event)
{
	// Set the appropriate flags to true and false
	if (event->button == 1)
	{
		mouseB1Down = false;
	}	
	if (event->button == 2)
	{
		mouseB2Down = false;
	}
		
	if (event->button == 3)
	{
		mouseB3Down = false;
	}
		
	invalidate();
	return true;
}

bool Viewer::on_motion_notify_event(GdkEventMotion* event)
{
	double x2x1;
	
	// Store the position of the cursor
	startPos[0] = event->x;
	startPos[1] = event->y;
	invalidate();
	return true;
}

void Viewer::drawCube(int y, int x, int colourId, GLenum mode, bool multiColour)
{
	if (mode == GL_LINE_LOOP)
		glLineWidth (2);
	
	double r, g, b;
	r = 0;
	g = 0;
	b = 0;
	switch (colourId)
	{
		case 0:	// blue
			r = 0.514;
			g = 0.839;
			b = 0.965;
			break;              
		case 1:	// purple       
			r = 0.553;          
			g = 0.6;            
			b = 0.796;          
			break;              
		case 2: // orange       
			r = 0.988;          
			g = 0.627;          
			b = 0.373;          
			break;              
		case 3:	// green        
			r = 0.69;           
			g = 0.835;          
			b = 0.529;          
			break;              
		case 4:	// red          
			r = 1.00;           
			g = 0.453;          
			b = 0.339;          
			break;              
		case 5:	// pink         
			r = 0.949;          
			g = 0.388;          
			b = 0.639;          
			break;              
		case 6:	// yellow       
			r = 1;              
			g = 0.792;          
			b = 0.204;          
			break;
		case 7:	// black
			r = 0;
			g = r;
			b = g;
			break;
		default:
			return;
	}
	
	double innerXMin = 0;
	double innerYMin = 0;
	double innerXMax = 1;
	double innerYMax = 1;
	double zMax = 1;
	double zMin = 0;
	
	// Front face
	glNormal3d(1, 0, 0);
		
	glColor3d(r, g, b);
	glBegin(mode);
		glVertex3d(innerXMin + x, innerYMin + y, zMax);
		glVertex3d(innerXMax + x, innerYMin + y, zMax);
		glVertex3d(innerXMax + x, innerYMax + y, zMax);
		glVertex3d(innerXMin + x, innerYMax + y, zMax);
	glEnd();
	
	// top face
	glNormal3d(0, 1, 0);
	
	if (multiColour)
		glColor3d(g, r, b);

	glBegin(mode);
		glVertex3d(innerXMin + x, innerYMax + y, zMin);
		glVertex3d(innerXMax + x, innerYMax + y, zMin);
		glVertex3d(innerXMax + x, innerYMax + y, zMax);
		glVertex3d(innerXMin + x, innerYMax + y, zMax);
	glEnd();
	
	// left face
	glNormal3d(0, 0, 1);
	
	if (multiColour)
		glColor3d(b, g, r);

	glBegin(mode);
		glVertex3d(innerXMin + x, innerYMin + y, zMin);
		glVertex3d(innerXMin + x, innerYMax + y, zMin);
		glVertex3d(innerXMin + x, innerYMax + y, zMax);
		glVertex3d(innerXMin + x, innerYMin + y, zMax);
	glEnd();
	
	// bottom face
	glNormal3d(0, 1, 0);
	
	if (multiColour)
		glColor3d(r, b, g);	

	glBegin(mode);
		glVertex3d(innerXMin + x, innerYMin + y, zMin);
		glVertex3d(innerXMax + x, innerYMin + y, zMin);
		glVertex3d(innerXMax + x, innerYMin + y, zMax);
		glVertex3d(innerXMin + x, innerYMin + y, zMax);
	glEnd();
	
	// right face
	glNormal3d(0, 0, 1);
	
	if (multiColour)
		glColor3d(b, r, g);
	
	glBegin(mode);
		glVertex3d(innerXMax + x, innerYMin + y, zMin);
		glVertex3d(innerXMax + x, innerYMax + y, zMin);
		glVertex3d(innerXMax + x, innerYMax + y, zMax);
		glVertex3d(innerXMax + x, innerYMin + y, zMax);
	glEnd();
	
	// Back of front face
	glNormal3d(1, 0, 0);

	if (multiColour)
		glColor3d(g, b, r);

	glBegin(mode);
		glVertex3d(innerXMin + x, innerYMin + y, zMin);
		glVertex3d(innerXMax + x, innerYMin + y, zMin);
		glVertex3d(innerXMax + x, innerYMax + y, zMin);
		glVertex3d(innerXMin + x, innerYMax + y, zMin);
	glEnd();
}

void Viewer::toggleBuffer() 
{ 
	// Toggle the value of doubleBuffer
	doubleBuffer = !doubleBuffer;
	invalidate();
}

bool Viewer::on_key_press_event( GdkEventKey *ev )
{
	if (ev->keyval == GDK_z)
	{
		animTimer.disconnect();
		
		animTimer = Glib::signal_timeout().connect(sigc::mem_fun(*this, &Viewer::step), 50);
	}
//	invalidate();
	return true;
}

// Linear interpolation from pt to endPos
void Viewer::lerp (Polygon::MovablePoint *pt, Point3D endPos, int steps, bool loop)
{
	Point3D currPos = pt->currentPos;
	
	// If a motionpath already exists delete it?
	if (pt->motionPath.size() > 1)
		currPos = pt->motionPath.back();
		
	// The amount we need to step through
	Vector3D changeInPos = endPos - currPos;
	
	// Divide the changeInPos by the number of steps. This tells us how much to increment in each step
	changeInPos = (1.0 / steps) * changeInPos;
	
	// Generate points
	for (int i = 1;i<steps;i++)
	{
		pt->motionPath.push_back(currPos + i * changeInPos);
	}
	
	// Set whether to loop the animation or not
	pt->loopMotion = loop;
}

bool Viewer::step ()
{
	// Iterate through each object
	for (int i = 0;i<objects.size();i++)
	{
		// Iterate through each vertex
		for (int j = 0;j<objects[i]->vertices.size();j++)
		{
			// If there are no points in the motion path move onto the next object
			if (objects[i]->vertices[j].motionPath.size() < 1)
				continue;
				
			// Set the current position to the next step in the motionPath deque
			objects[i]->vertices[j].currentPos = objects[i]->vertices[j].motionPath[0];
			
			// If the animation should be looped push the front to the end of the deque
			if (objects[i]->vertices[j].loopMotion)
				 objects[i]->vertices[j].motionPath.push_back(objects[i]->vertices[j].motionPath[0]);
			
			// Remove the frame from the deque
			objects[i]->vertices[j].motionPath.pop_front();
		}
	}
	
	// Render screen
	invalidate();
	return true;
}
