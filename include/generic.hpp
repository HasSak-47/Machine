#ifndef __GENERIC_HPP__
#define __GENERIC_HPP__

#include <stdint.h>

#define make_typedef(p)\
typedef uint##p##_t u##p;\
typedef int##p##_t i##p;

make_typedef(8)
make_typedef(16)
make_typedef(32)
make_typedef(64)

typedef float  f32;
typedef double f64;

#define let const auto
#define mut auto

#include <string>
#include <vector>

template<typename T>
using Vec = std::vector<T>;

typedef std::string String;

#endif

