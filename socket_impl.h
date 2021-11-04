#ifndef SOCKET_IMPL_HPP_INCLUDED
#define SOCKET_IMPL_HPP_INCLUDED

/**
 * Type for storing a socket handle.
 * Defined here because it does not exist in non-Windows headers.
 */
typedef int SOCKET;

/**
 * Socket handle representing an invalid socket.
 */
const SOCKET INVALID_SOCKET = -1;

/**
 * Return code representing a socket error.
 */
const int SOCKET_ERROR = -1;

struct Socket
{
    SOCKET sock;
    //sockaddr_in sin;

    /*Socket(const SOCKET &sock = INVALID_SOCKET, const sockaddr_in &sin = sockaddr_in())
        : sock(sock), sin(sin)
    { }*/
};

#endif // SOCKET_IMPL_HPP_INCLUDED
