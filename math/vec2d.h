#pragma once

struct vec2d{

	// constructors
	vec2d() = default; 

	vec2d(float a, float b);
	vec2d(vec2d const& v) = default; 

	// operator overloads
	// Unchecked access; index must be between 0 and 1.
	float& operator[](int i);
	float const& operator[](int i) const; 

	vec2d& operator=(vec2d const& v) = default;

	vec2d& operator+=(vec2d const& v);
	vec2d& operator-=(vec2d const& v);
	vec2d operator+(vec2d const& v) const; 
	vec2d operator-(vec2d const& v) const; 

	vec2d& operator*=(float s);
	vec2d& operator/=(float s);
	vec2d operator*(float s) const; 
	vec2d operator/(float s) const; 

	bool operator==(vec2d const& other) const;
	bool operator!=(vec2d const& other) const;

	// methods
	bool isZeroVector() const; 

	// member variables
	float x_ = 0.0f, y_ = 0.0f;
};
