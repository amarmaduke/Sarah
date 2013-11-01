
#include <iostream>
#include <fstream>
#include <iterator>
#include <memory>

#include "File.hpp"

namespace sarah {

// Read the contents of the spcified file, returning a string containing
// its contents.
std::string
File::read(const std::string& path)
{
  using namespace std;

  fstream input(path, ios::in);
  input.seekg(0, ios::end);
  int size = input.tellg();
  input.seekg(0, ios::beg);

  unique_ptr<char> buf(new char[size]);
  input.read(buf.get(), size);
  input.close();

  return string(buf.get(), size);
}

// Read from the given input stream.
std::string
File::read(std::istream& is) {
  using namespace std;
  is >> noskipws;
  istream_iterator<char> iter = is;
  istream_iterator<char> end;
  return string(iter, end);  
}

} // namespace sarah 
