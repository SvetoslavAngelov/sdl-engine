#include "vec2d.h"

vec2d::vec2d(float a, float b) :
x_{a}, y_{b}
{

} 

// operator overloads
float& vec2d::operator[](int i){
	// almost feels dirty for a 2d vector
	return((&x_)[i]);
}

float const& vec2d::operator[](int i) const{
	return ((&x_)[i]);
}

vec2d& vec2d::operator+=(vec2d const& v){
    x_ += v.x_;
    y_ += v.y_;
    return *this;
}

vec2d& vec2d::operator-=(vec2d const& v){
    x_ -= v.x_;
    y_ -= v.y_;
    return *this;
}

vec2d vec2d::operator+(vec2d const& v) const{
	return {x_ + v.x_, y_ + v.y_};
} 

vec2d vec2d::operator-(vec2d const& v) const{
	return {x_ - v.x_, y_ - v.y_};
}

vec2d& vec2d::operator*=(float s){
   x_ *= s;
   y_ *= s;
   return *this;
}

vec2d& vec2d::operator/=(float s){
    s = 1.f / s;
    x_ *= s;
    y_ *= s;
    return *this;
}

vec2d vec2d::operator*(float s) const{
	return {x_ * s, y_ * s};
}

vec2d vec2d::operator/(float s) const{
	s = 1.f/s;
	return {x_ * s, y_ * s};
}

bool vec2d::operator==(vec2d const& other) const{
	return (x_ == other.x_ && y_ == other.y_);
}

bool vec2d::operator!=(vec2d const& other) const{
	return !(x_ == other.x_ && y_ == other.y_);
}

// methods
bool vec2d::isZeroVector() const{
	return (x_ == 0.f && y_ == 0.f);
}
