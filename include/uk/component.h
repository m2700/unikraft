#pragma once

#include <uk/config.h>

#define __UK_LIB_COMPONENT(lib) __##lib##_COMPONENT__
#define UK_LIB_COMPONENT(lib) __UK_LIB_COMPONENT(lib)

#define UK_COMPONENT UK_LIB_COMPONENT(__LIBNAME__)