#include <cstddef>
#include <cstdint>
#include <deque>
#include <iostream>
#include <stdexcept>
#include <sys/types.h>

#include "byte_stream.hh"

using namespace std;

// =====================================================================================
void print_state(Reader* r) 
{
  std::cout << "reader r.byte_buffered=" << r->bytes_buffered() << endl;
  std::cout << "reader r.byte_poped=" << r->bytes_popped() << endl;
}

inline uint64_t get_buffered_num(uint64_t written, uint64_t popped){
  return written - popped;
}

inline bool is_all_popped(uint64_t written, uint64_t popped){
  return written == popped;
}

inline uint64_t next_byte_index(uint64_t popped, uint64_t truncated){
  return popped - truncated;
}

inline bool need_truncate(uint64_t buf_size,  uint64_t buffered_size){
  return buf_size >= 4096 && (buf_size > 8 * buffered_size);
}

std::string truncate(uint64_t cap, std::string& buf, uint64_t popped, uint64_t truncated){
  std::string new_buf;
  new_buf.resize(cap);
  new_buf.clear();

  uint64_t offset = popped - truncated;
  new_buf.append(buf.substr(offset, buf.size() - offset));
  return new_buf;
}
// =====================================================================================

ByteStream::ByteStream( uint64_t capacity )
  : capacity_( capacity ), 
  byte_written( 0 ), 
  byte_popped( 0 ), 
  is_stream_closed( false ), 
  has_err( false ),
  truncated (0)
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
  if ( this->is_closed()) {
    return;
  }

  auto n = this->available_capacity();
  auto s = data.size();
  this->byte_written +=  min( n, s );
  if (n >= s){
    this->buffer.append(data);
  }else{
    this->buffer.append( data.substr( 0, n ) );
  }

  if (need_truncate(this->buffer.size(), 
  get_buffered_num(this->byte_written, this->byte_popped))){
    this->buffer = truncate(this->capacity_, this->buffer, this->byte_popped, this->truncated);
    this->truncated = this->byte_popped;
  }
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
  return this->capacity_ - get_buffered_num(this->byte_written, this->byte_popped);
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return this->byte_written;
}

// string_view for unify both const char* and const string&
string_view Reader::peek() const
{
  auto index = next_byte_index(this->bytes_popped(), this->truncated);
  return { &this->buffer[index], 1 };
}

bool Reader::is_finished() const
{
  // Your code here.
  return this->is_stream_closed && is_all_popped(this->byte_written, this->byte_popped);
}

bool Reader::has_error() const
{
  // Your code here.
  return this->has_err;
}

void Reader::pop( uint64_t len )
{
  auto size = this->bytes_buffered();
  auto incr = min( len, size );
  this->byte_popped += incr;
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return this->byte_written - this->byte_popped;
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return this->byte_popped;
}