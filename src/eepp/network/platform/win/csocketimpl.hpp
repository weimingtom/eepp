#ifndef EE_NETWORKCSOCKETIMPL_WIN_HPP
#define EE_NETWORKCSOCKETIMPL_WIN_HPP

#include <eepp/declares.hpp>

#if EE_PLATFORM == EE_PLATFORM_WIN

#ifdef _WIN32_WINDOWS
	#undef _WIN32_WINDOWS
#endif
#ifdef _WIN32_WINNT
	#undef _WIN32_WINNT
#endif
#define _WIN32_WINDOWS 0x0501
#define _WIN32_WINNT   0x0501
#include <eepp/network/csocket.hpp>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace EE { namespace Network { namespace Private {

/** @brief Helper class implementing all the non-portable socket stuff; this is the Windows version */
class cSocketImpl {
	public :
		// Types
		typedef socklen_t AddrLength;

		/** @brief  Create an internal sockaddr_in address
		**  @param address Target address
		**  @param port	Target port
		**  @return sockaddr_in ready to be used by socket functions */
		static sockaddr_in CreateAddress(Uint32 address, unsigned short port);

		/** @brief  Return the value of the invalid socket
		**  @return Special value of the invalid socket */
		static SocketHandle InvalidSocket();

		/** @brief  Close and destroy a socket
		**  @param sock Handle of the socket to close */
		static void Close(SocketHandle sock);

		/** @brief  Set a socket as blocking or non-blocking
		**  @param sock  Handle of the socket
		**  @param block New blocking state of the socket */
		static void SetBlocking(SocketHandle sock, bool block);

		/** Get the last socket error status
		**  @return Status corresponding to the last socket error */
		static cSocket::Status GetErrorStatus();
};

}}}

#endif

#endif // EE_NETWORKCSOCKETIMPL_WIN_HPP
