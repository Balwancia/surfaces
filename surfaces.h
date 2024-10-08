#ifndef SURFACES_H
#define SURFACES_H

#include "real.h"
#include <ostream>
#include <functional>
#include <cmath>

class Point {
public:
    const Real x, y;
    Point(Real x, Real y) : x(x), y(y) {}
    friend std::ostream& operator<<(std::ostream& out, const Point& p){
        out << p.x << ' ' << p.y;
        return out;
    }
};

using Surface = std::function<Real(Point)>;

inline Surface plain() { return [=] ([[maybe_unused]] Point p) { return 0; }; };
inline Surface slope() { return [=] (Point p) { return p.x; }; };
inline Surface steps(Real s = 1) {
    return [=] (Point p) {return s <= 0 ? 0 : std::floor(p.x / s); };
};
inline Surface checker(Real s = 1) {
    return [=] (Point p) {
        return s <= 0 ? 0 : 
        std::fmod(std::abs(std::floor(p.x / s)) + std::abs(std::floor(p.y / s) + 1), 2);
    };
};
inline Surface sqr() { return [=] (Point p) { return p.x * p.x; }; };
inline Surface sin_wave() { return [=] (Point p) { return std::sin(p.x); }; };
inline Surface cos_wave() { return [=] (Point p) { return std::cos(p.x); }; };
inline Surface rings(Real s = 1) {
    return [=] (Point p) {
        return s <= 0 ? 0 : p.x == 0 && p.y == 0 ? 1 : 
        std::fmod((std::ceil(std::sqrt(p.x * p.x + p.y * p.y) / s)), 2);
    };
};

inline Surface ellipse(Real a = 1, Real b = 1) {
    return [=] (Point p) {
        return a <=0 || b <= 0 ? 0 :
        p.x * p.x / (a * a) + p.y * p.y / (b * b) <= 1 ? 1 : 0;
    };
};


inline Surface rectangle(Real a = 1, Real b = 1) {
    return [=] (Point p) {
        return a <=0 || b <= 0 ? 0 :
        p.x <= std::abs(b) && -std::abs(b) <= p.x && p.y <= std::abs(a) && -std::abs(a) <= p.y ? 1 : 0;
    };
};

inline Surface stripes(Real s = 1) {
    return [=] (Point p) {
        return s <= 0 ? 0 : std::abs(std::fmod(std::ceil(p.x / s), 2));
    };
};

inline Surface rotate(const Surface& f, Real deg) {
    return [f, rad = deg * std::numbers::pi / 180] (Point p) {
        // return deg >= 0 ? 
        return f(Point(p.x * std::cos(-rad) - p.y * std::sin(-rad), 
            p.x * std::sin(-rad) + p.y * std::cos(-rad))) ;
        // f(Point(p.x * std::cos(-rad) + p.y * std::sin(-rad), 
            // - p.x * std::sin(-rad) + p.y * std::cos(-rad)));
    };
};

inline Surface translate(const Surface& f, Point v) {
    return [=] (Point p) { return f(Point(p.x + std::abs(v.x), p.y + std::abs(v.y))); };
}

inline Surface scale(const Surface& f, Point s) {
    return [=] (Point p) { return f(Point(p.x / s.x, p.y / s.y)); };
};

inline Surface invert(const Surface& f) {
    return [=] (Point p) { return f(Point(p.y, p.x)); };
};

inline Surface flip(const Surface& f) {
    return [=] (Point p) { return f(Point(-p.x, p.y)); };
};

inline Surface mul(const Surface& f, Real c) {
    return [=] (Point p) { return f(p) * c; };
};

inline Surface add(const Surface& f, Real c) {
    return [=] (Point p) { return f(p) + c; };
};

template<typename H, typename... F>
inline auto evaluate(const H& h, const F&... f) {
    return [=] ([[maybe_unused]] Point p) { return h(f(p)...); };
};

inline auto compose() {
    return std::identity();
}

template<typename F>
inline auto compose(const F& f) {
    return [=] (auto... args) { return f(args...); };
};

template<typename F, typename F1, typename... Fn>
inline auto compose(const F& f, const F1& f1, const Fn&... fs) {
    return compose( [=] (auto... args) { return f1(f(args...)); }, fs...);
};

#endif