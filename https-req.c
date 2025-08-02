#include <openssl/ssl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

// Error handling
static void assert(_Bool condition, char *message) {
	if (condition) perror(message), exit(-1);
}

#define ERROR "\x1b[31mError:\x1b[m "

// SSL connection
static void try_connection(struct addrinfo *p, SSL *ssl, char *url, int sd) {
	// Tries recursion by another address if necessary
	if (connect(sd, p->ai_addr, p->ai_addrlen) == -1)
		p->ai_next ?
			try_connection(p->ai_next, ssl, url, sd)
		: (
			perror(ERROR"connect"),
			exit(-1)
		);

	// Some weird shit to make it work
	assert(!SSL_set_fd(ssl, sd), ERROR"SSL_set_fd");
	assert(
		!SSL_set_tlsext_host_name(ssl, url),
		ERROR"SSL_set_tlsext_host_name"
	);

	assert(SSL_connect(ssl) < 1, ERROR"SSL_connect");
}

typedef struct { SSL *ssl; int sd; } Req;

// Handles an HTTPS connection/handshake over the domain
Req connect_url(char *url) {
	// Init bullshit required to encript
	SSL *ssl = SSL_new(SSL_CTX_new(TLS_method()));

	assert(!ssl, ERROR"SSL_new");

	// Get domain IP's
	struct addrinfo *result;
	{
		struct addrinfo hints = {
			0,
			AF_UNSPEC,
			SOCK_STREAM
		};
		assert(
			getaddrinfo(url, "443", &hints, &result) != 0,
			ERROR"getaddrinfo"
		);
	}
	int sd = socket(
		result->ai_family,
		result->ai_socktype,
		result->ai_protocol
	);

	assert(sd == -1, ERROR"socket");
	try_connection(result, ssl, url, sd);

	Req x = { ssl, sd };

	return x;
}

// Writes a simple request for data to the domain
void request(SSL *ssl, char *url, char *path) {
	char request[1024];
	snprintf(
		request,
		1024,
		"GET /%s HTTP/1.0\r\nHost: %s\r\n\r\n",
		path,
		url
	);
	size_t writen = 0;
	size_t bytes;
	while ((bytes = SSL_write(ssl, writen + request, 1024 - writen)) > 0)
		writen += bytes;
	assert(bytes < 0, ERROR"SSL_write");
}

// Heap management
static void try_realloc(char **x, size_t len, size_t *max) {
	if (len >= *max) {
		char *p = realloc(*x, *max = len * 3);
		assert(!p, ERROR"realloc");
		*x = p;
	}
}

// Dynamically reads the domain from requested data
char * download(Req *x, char *url, char *path, size_t max) {
	request(x->ssl, url, path);

	char *response = malloc(max ? max : (max = 8192));

	assert(!response, ERROR"malloc");

	{
		size_t readed = 0;
		size_t bytes;
		while ((
			bytes = SSL_read(
				x->ssl,
				readed + response,
				max - readed
			)
		) > 0)
			readed += bytes,
			printf("+ %zd | %zd / %zd\n", bytes, readed, max),
			try_realloc(&response, readed, &max);
		assert(bytes < 0, ERROR"SSL_read");
	}

	return response;
}

// Cleans the structure
void free_req(Req *x) {
	SSL_free(x->ssl);
	close(x->sd);
}

// Handles a new connection and downloads
char * simply_download(char *url, size_t max) {
	Req x = connect_url(url);
	char *source = download(&x, url, "", max);
	free_req(&x);

	return source;
}
