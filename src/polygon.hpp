#ifndef CS488_POLYGON_HPP
#define CS488_POLYGON_HPP

#include "algebra.hpp"
#include <iostream>
#include <string>
#include <deque>
#include <vector>

class Polygon {
public:
	struct MovablePoint {
		std::deque<Point3D> motionPath;
		Point3D currentPos;
		bool loopMotion;
	};
	
	Polygon(std::string val);
	Polygon(std::string val, std::vector<MovablePoint> points);
	virtual ~Polygon();
		
	std::vector<MovablePoint> vertices;

protected:
	std::string name;

	
};

#endif
