#ifndef VECTOR2F_HPP
#define VECTOR2F_HPP
#pragma once

class Vector2f{
	public:
		float _x, _y;
		
		Vector2f(float x, float y): _x{x}, _y{y} {}
		Vector2f(): _x{0}, _y{0} {}

		~Vector2f() {}

		void Zero(){
			_x = 0;
			_y = 0;
		}

		friend Vector2f operator+(const Vector2f& v1, const Vector2f& v2){
			return Vector2f(v1._x + v2._x, v1._y + v2._y);
		}
		friend Vector2f operator-(const Vector2f& v1, const Vector2f& v2){
			return Vector2f(v1._x - v2._x, v1._y - v2._y);
		}
		friend Vector2f operator*(const Vector2f& v1, const Vector2f& v2){
			return Vector2f(v1._x * v2._x, v1._y * v2._y);
		}
		friend Vector2f operator/(const Vector2f& v1, const Vector2f& v2){
			return Vector2f(v1._x / v2._x, v1._y / v2._y);
		}

		void operator+=(const Vector2f& v){
			_x += v._x;
			_y += v._y;
		}
};

#endif
