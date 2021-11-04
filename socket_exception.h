#ifndef SOCKET_EXCEPTION
#define SOCKET_EXCEPTION

#include <string>

class Socket_Exception : public std::exception
{
	protected:
		const char *err;
	public:
		Socket_Exception(const char *e) : err(e) {};
		const char *error()  const noexcept { return err; };
		virtual const char *what() const noexcept { return "Socket_Exception"; }
};

#endif