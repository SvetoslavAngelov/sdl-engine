#pragma once

struct vec4d {

    // constructors
    vec4d() = default;

    vec4d(float a, float b, float c, float d);
    vec4d(vec4d const& v) = default;

    // operator overloads 
	// Unchecked access; index must be between 0 and 3.
	float& operator[](int i);
	float const& operator[](int i) const; 

	vec4d& operator=(vec4d const& v) = default;

	vec4d& operator+=(vec4d const& v);
	vec4d& operator-=(vec4d const& v);
	vec4d operator+(vec4d const& v) const; 
	vec4d operator-(vec4d const& v) const; 

	vec4d& operator*=(float s);
	vec4d& operator/=(float s);
	vec4d operator*(float s) const; 
	vec4d operator/(float s) const;

	bool operator==(vec4d const& other) const;
	bool operator!=(vec4d const& other) const; 

	// methods
	bool isZeroVector() const; 
	float magnitude() const; 
	vec4d getNormal() const; 
	// Requires a nonzero vector.
	vec4d getUnsafeNormal() const; 

    // member variables
    float x_ = 0.0f, y_ = 0.0f, z_ = 0.0f, w_ = 0.0f; 
};
