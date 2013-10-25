
#ifndef SARAH_IOS_HPP
#define SARAH_IOS_HPP

#include <iosfwd>

namespace sarah {

// Returns the base flag of the stream.
//
// TODO: Move this into an I/O facility (in utility) and extend the
// set of ios flags to include binary literals. I'm not sure there is
// a safe way to do this, we may need to provide a wrapper around the
// entire ios facility to make this work.
int stream_base(const std::ios_base& s);

} // namespace sarah

#endif