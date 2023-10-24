#include <cstddef>
#include <deque>
#include <iostream>
#include <stdexcept>

#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity )
  : capacity_( capacity ), byte_written( 0 ), byte_popped( 0 ), is_stream_closed( false ), has_err( false )
{
  this->buffer = std::string();
  this->buffer.resize( static_cast<std::size_t>( capacity ) );
  this->buffer.clear();
}

// Push data to stream, but only as much as available capacity allows.
// Example: for writer with cap = 2 and push "cat", so only "ca" is buffered.
void Writer::push( string data )
{
  // Your code here.
  if ( this->is_closed() ) {
    return;
  }

  auto incr = min( this->available_capacity(), data.size() );
  this->byte_written += incr;
  this->buffer.append( data.substr( 0, incr ) );
}

void Writer::close()
{
  // Your code here.
  this->is_stream_closed = true;
}

void Writer::set_error()
{
  // Your code here.
  this->has_err = true;
}

bool Writer::is_closed() const
{
  // Your code here.
  return this->is_stream_closed;
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  if ( this->buffer.size() > this->capacity_ ) {
    return 0;
  }
  return this->capacity_ - this->buffer.size();
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return this->byte_written;
}

// string_view for unify both const char* and const string&
string_view Reader::peek() const
{
  // note: consider its empty
  if ( this->buffer.empty() ) {
    return { "" };
  }

  return { &this->buffer.front(), 1 };
}

bool Reader::is_finished() const
{
  // Your code here.
  return this->is_stream_closed && this->buffer.empty();
}

bool Reader::has_error() const
{
  // Your code here.
  return this->has_err;
}

void Reader::pop( uint64_t len )
{
  if ( this->is_stream_closed ) {
    return;
  }

  auto size = this->buffer.size();
  auto incr = min( len, size );
  this->byte_popped += incr;

  if ( len >= size ) {
    this->buffer.clear();
    this->buffer.resize( this->capacity_ );
    return;
  }

  // pop      [0, len)
  // reserve  [len, size)
  this->buffer = this->buffer.substr( len, size );
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return this->buffer.size();
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return this->byte_popped;
}
