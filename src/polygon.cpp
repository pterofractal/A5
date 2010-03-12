#include "polygon.hpp"

	Polygon::Polygon(std::string val, std::vector<MovablePoint> points)
	:	name(val),
		vertices(points)
	{ }
	
	Polygon::Polygon(std::string val)
	:	name(val)
	{ }
	
 	Polygon::~Polygon()
	{
		
	}

