#pragma once

struct vec3d{

	// constructors
	vec3d() = default; 

	vec3d(float a, float b, float c);
	vec3d(vec3d const& v) = default;

	// operator overloads
	// Unchecked access; index must be between 0 and 2.
	float& operator[](int i);
	float const& operator[](int i) const; 

	vec3d& operator=(vec3d const& v) = default;

	vec3d& operator+=(vec3d const& v);
	vec3d& operator-=(vec3d const& v);
	vec3d operator+(vec3d const& v) const; 
	vec3d operator-(vec3d const& v) const; 

	vec3d& operator*=(float s);
	vec3d& operator/=(float s);
	vec3d operator*(float s) const; 
	vec3d operator/(float s) const;

	bool operator==(vec3d const& other) const;
	bool operator!=(vec3d const& other) const; 

	// methods
	bool isZeroVector() const; 
	float magnitude() const; 
	vec3d getNormal() const; 
	// Requires a nonzero vector.
	vec3d getUnsafeNormal() const; 

	// member variables
	float x_ = 0.0f, y_ = 0.0f, z_ = 0.0f;
};
