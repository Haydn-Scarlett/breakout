#pragma once
class vector2
{
public:
	// construction
	vector2();
	vector2(const vector2& rhs);
	/**
	*  Destructor.
	*/
	~vector2();
	
	// operations
	vector2 operator*(float scalar);
	void normalise();
	float getX();
	float getY();
	void setX(float newX);
	void setY(float newY);
	
private:

	// data
	float x = 0;
	float y = 0;
};