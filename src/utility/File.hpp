#ifndef SARAH_FILE_H
#define SARAH_FILE_H

#include <iosfwd>
#include <string>

namespace sarah {

// The File class represents the text of an opened file. The file is
// opened and read as soon as it is initalized, and the text is retained
// as long as the object lives.
struct File {
  using iterator = std::string::iterator;
  using const_iterator = std::string::const_iterator;

  File(std::istream& is)
    : path(), text(read(is)) { }

  File(const std::string& p)
    : path(p), text(read(p)) { }

  // Text iterators
  iterator       begin()       { return text.begin(); }
  const_iterator begin() const { return text.begin(); }

  iterator       end()       { return text.end(); }
  const_iterator end() const { return text.end(); }

  std::string path;
  std::string text;

private:
  static std::string read(const std::string&);
  static std::string read(std::istream& is);
};

} // namespace sarah

#endif
