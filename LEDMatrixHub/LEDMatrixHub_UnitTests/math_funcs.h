#ifndef _MATH_FUNCS_
#define _MATH_FUNCS_

#define PI 3.14159265358979323846

inline double scale_func(double x, double scale) { 
    return (cos((PI * x)/scale + PI) + 1)/2; 
}

inline double scale_func_bound(double x, double scale, double lower_bound, double upper_bound) {
    return (scale_func(x, scale) * (upper_bound - lower_bound)) + lower_bound;
}

#endif //_MATH_FUNCS_
