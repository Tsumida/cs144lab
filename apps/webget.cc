#include "socket.hh"

#include <cstdlib>
#include <iostream>
#include <span>
#include <string>

using namespace std;


std::string wrap_content( const string& host, const string& path )
{
  std::string s = "GET " + path + " HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";
  return s;
}

void print_content( const string& content )
{
  cout << "print content:\n\n-----------------\n" << content << "\n-----------------\n" << endl;
}

void get_URL( const string& host, const string& path )
{
  // cerr << "Function called: get_URL(" << host << ", " << path << ")\n";
  // Get content to send.
  auto content = wrap_content( host, path );
  std::string buffer = std::string();

  // init socket.
  std::string servname = "80";
  auto peer_addr = Address( host, servname );
  auto tcp_endpoint = TCPSocket();
  tcp_endpoint.connect( peer_addr );

  // Send content to destination.
  tcp_endpoint.write( content );

  // Got response.
  std::string response_raw = std::string();
  tcp_endpoint.read( buffer );

  response_raw.append( buffer );
  while ( !tcp_endpoint.eof() ) {
    tcp_endpoint.read( buffer );
    response_raw.append( buffer );
  }

  // first rstrip xxx .. hash\r\n into xxx .. hash
  std::string CRLF = "\r\n";
  auto sp_size = CRLF.size() >> 1;
  auto tripped_content = response_raw.substr( 0, response_raw.size() - sp_size );
  auto index = tripped_content.find_last_of( CRLF );

  // extract hash from response
  auto hash = tripped_content.substr( index + sp_size, tripped_content.size() );

  cout << hash << endl;

  tcp_endpoint.close();
}

int main( int argc, char* argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort(); // For sticklers: don't try to access argv[0] if argc <= 0.
    }

    auto args = span( argv, argc );

    // The program takes two command-line arguments: the hostname and "path" part of the URL.
    // Print the usage message unless there are these two arguments (plus the program name
    // itself, so arg count = 3 in total).
    if ( argc != 3 ) {
      cerr << "Usage: " << args.front() << " HOST PATH\n";
      cerr << "\tExample: " << args.front() << " stanford.edu /class/cs144\n";
      return EXIT_FAILURE;
    }

    // Get the command-line arguments.
    const string host { args[1] };
    const string path { args[2] };

    // Call the student-written function.
    get_URL( host, path );
  } catch ( const exception& e ) {
    cerr << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
