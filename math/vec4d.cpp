#include "vec4d.h"
#include <math.h> 

vec4d::vec4d(float a, float b, float c, float d) :
    x_{ a }, y_{ b }, z_{ c }, w_{ d }
{
    
}

float& vec4d::operator[](int i){
    return ((&x_)[i]);
}

float const& vec4d::operator[](int i) const{
    return ((&x_)[i]);
}

vec4d& vec4d::operator+=(vec4d const& v){
	x_ += v.x_;
	y_ += v.y_;
	z_ += v.z_;
    	w_ += v.w_;

    	return *this;
}
vec4d& vec4d::operator-=(vec4d const& v){
    	x_ -= v.x_;
	y_ -= v.y_;
	z_ -= v.z_;
    	w_ -= v.w_;

    	return *this;
}

vec4d vec4d::operator+(vec4d const& v) const{
    return {x_ + v.x_, y_ + v.y_, z_ + v.z_, w_ + v.w_};
}

vec4d vec4d::operator-(vec4d const& v) const{
    return {x_ - v.x_, y_ - v.y_, z_ - v.z_, w_ - v.w_};
}

vec4d& vec4d::operator*=(float s){
    	x_ *= s;
	y_ *= s;
	z_ *= s;
    	w_ *= s;

    	return *this;
}

vec4d& vec4d::operator/=(float s){
	s = 1.f / s; 
    	x_ *= s;
	y_ *= s;
	z_ *= s;
    	w_ *= s;

    	return *this;
}

vec4d vec4d::operator*(float s) const{
    return {x_ * s, y_ * s, z_ * s, w_ * s};
}

vec4d vec4d::operator/(float s) const{
    s = 1.f / s; 
    return {x_ * s, y_ * s, z_ * s, w_ * s};
}

bool vec4d::operator==(vec4d const& other) const{
    return (x_ == other.x_ && y_ == other.y_ && z_ == other.z_ && w_ == other.w_);
}

bool vec4d::operator!=(vec4d const& other) const{
    return !(*this == other);
}

// methods
bool vec4d::isZeroVector() const{
    return (x_ == 0.f && y_ == 0.f && z_ == 0.f && w_ == 0.f);
}

float vec4d::magnitude() const{
    float s = (x_ * x_) + (y_ * y_) + (z_ * z_) + (w_ * w_); 
    return (float)sqrt(s);
}

vec4d vec4d::getNormal() const{
    return !isZeroVector() ? (*this / magnitude()) : vec4d{ 0.f, 0.f, 0.f, 0.f};
}

vec4d vec4d::getUnsafeNormal() const{
    return vec4d{ *this / magnitude() };
}
