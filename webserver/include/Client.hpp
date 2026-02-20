#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <map>
#include <fstream>
#include <sys/types.h>

enum ClientState {
	STATE_READ_REQUEST,
	STATE_READ_BODY,
	STATE_PROCESS,
	STATE_WRITE_RESPONSE,
	STATE_CLOSED
};

class	Client {
	private:
		int		_fd;
		int		_server_port;
		ClientState	_state;

		// -- BUFFERS --
		std::string	_recv_buffer;
		std::string	_send_buffer;

		// -- REQUEST STORAGE --
		std::string				_method;
		std::string				_path;
		std::string				_query;
		std::string				_version;
		std::map<std::string, std::string>	_headers;
		size_t					_content_length;

		// -- RESPONSE STORAGE --
		std::ifstream	_file_stream;
		size_t		_file_size;
		size_t		_bytes_sent;
		bool		_headers_sent;
	
	public:
		Client(int fd, int port);
		~Client();

		// Getters
		int		getFd() const;
		int		getPort() const;
		ClientState	getState() const;

		// Setters
		void	setState(ClientState s);

		// buffer access (reference allows direct modification)
		std::string&	recvBuffer();
		std::string&	sendBuffer();

		// request parser helpers
		void	parseRequestLine(const std::string& line);
		void	addHeader(const std::string& key, const std::string& value);

		// request data access 
		std::string	getMethod() const;
		std::string	getPath() const;
		bool		hasHeader(const std::string& key) const;
		std::string	getHeader(const std::string& key) const;

		// response helpers
		void			openFile(const std::string& path);
		bool			isFileOpen();
		std::ifstream&		fileStream();
		void			setFileSize(size_t size);
		size_t			getFileSize() const;
		size_t			getBytesSent() const;
		void			addBytesSent(size_t bytes);
		bool			isHeadersSent() const;
		void			setHeadersSent(bool sent);


		// reset for keep-alive (next request on the same connection)
		void	reset();
};

#endif
