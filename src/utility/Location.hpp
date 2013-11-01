#ifndef SARAH_LOCATION_H
#define SARAH_LOCATION_H

#include <iosfwd>

#include "File.hpp"

namespace sarah {

// Represents a location in a source file.
struct Location {
  Location() 
    : file(nullptr), line(0), column(0) { }
  
  Location(const File& f, int l, int c) 
    : file(&f), line(l), column(c) { }
  
  Location(const File& f) 
    : Location(f, 1, 1) { }

  const File* file;
  int line;
  int column;
};

template<typename C, typename T>
  std::basic_ostream<C, T>&
  operator<<(std::basic_ostream<C, T>& os, const Location& l) {
    if (l.file)
      os << l.file->path;
    else
      os << "<unknow-file>";
    return os << ':' << l.line << ':' << l.column;
  }

} // namespace sarah

#endif
