#pragma once
#include <vector>

using std::vector;

extern const long double EPS;

// Убираем нули с конца
void trim(vector<long double>& p);

// Значение многочлена в точке x (схема Горнера)
long double evalPolynomial(vector<long double> p, long double x);

// Производная
vector<long double> deriv(vector<long double>& p);

// Порядок нуля в точке A: сколько раз P(A)=0, P'(A)=0, ... подряд
int orderZero(vector<long double>& p, long double A);

// k-я производная
vector<long double> derivK(vector<long double> p, int k);

// Степень
int degree(vector<long double>& p);