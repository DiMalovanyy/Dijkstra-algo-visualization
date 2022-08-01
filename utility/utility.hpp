#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "wx/gdicmn.h"
#include <utility>

inline wxPoint toWxPoint(std::pair<size_t, size_t> point) { return wxPoint(point.first, point.second); }
inline std::pair<size_t, size_t> fromWxPoint(wxPoint point) { return {point.x, point.y}; }


#endif
