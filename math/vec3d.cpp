#include "vec3d.h"
#include <math.h>

vec3d::vec3d(float a, float b, float c) :  
x_{a}, y_{b}, z_{c}
{

}

// operator overloads
float& vec3d::operator[](int i){
	return((&x_)[i]);
}

float const& vec3d::operator[](int i) const{
	return((&x_)[i]);
}

vec3d& vec3d::operator+=(vec3d const& v){
	x_ += v.x_;
	y_ += v.y_;
	z_ += v.z_; 

	return *this; 
}

vec3d& vec3d::operator-=(vec3d const& v){
	x_ -= v.x_;
	y_ -= v.y_;
	z_ -= v.z_; 

	return *this; 
}

vec3d vec3d::operator+(vec3d const& v) const{
	return {x_ + v.x_, y_ + v.y_, z_ + v.z_};
} 

vec3d vec3d::operator-(vec3d const& v) const{
	return {x_ - v.x_, y_ - v.y_, z_ - v.z_};
}

vec3d& vec3d::operator*=(float s){
	x_ *= s; 
	y_ *= s; 
	z_ *= s;

	return *this; 
}

vec3d& vec3d::operator/=(float s){
	s = 1.f / s;
	x_ *= s; 
	y_ *= s; 
	z_ *= s;

	return *this;
}

vec3d vec3d::operator*(float s) const{
	return {x_ * s, y_ * s, z_ * s};
}

vec3d vec3d::operator/(float s) const{
	s = 1.f / s; 
	return {x_ * s, y_ * s, z_ * s};
}

bool vec3d::operator==(vec3d const& other) const{
	return (x_ == other.x_ && y_ == other.y_ && z_ == other.z_);
}

bool vec3d::operator!=(vec3d const& other) const{
	return !(x_ == other.x_ && y_ == other.y_ && z_ == other.z_);
} 

// methods
bool vec3d::isZeroVector() const{
	return (x_ == 0.f && y_ == 0.f && z_ == 0.f);
}

float vec3d::magnitude() const{
	float s = (x_ * x_) + (y_ * y_) + (z_ * z_);
	return (float)sqrt(s);
}

vec3d vec3d::getNormal() const{
	return !isZeroVector() ? (*this / magnitude()) : vec3d(0.f, 0.f, 0.f);
}

vec3d vec3d::getUnsafeNormal() const{
	return *this / magnitude();
}
