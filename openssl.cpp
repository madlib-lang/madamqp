/**
 *  OpenSSL.cpp
 *
 *  Implementation file for the openssl.h header file
 *
 *  @copyright 2018 Copernica BV
 */

/**
 *  Dependencies
 */
#include "openssl.h"
#include "function.h"
#include "amqpcpp/openssl.h"

/**
 *  The handle to access openssl (the result of dlopen("openssl.so"))
 *  By default we set this to RTLD_DEFAULT, so that AMQP-CPP checks the internal process space
 */
static void *handle = RTLD_DEFAULT;

static auto _SSL_CTX_new = SSL_CTX_new;
static auto _SSL_read = SSL_read;
static auto _SSL_write = SSL_write;
static auto _SSL_set_fd = SSL_set_fd;
static auto _SSL_pending = SSL_pending;
static auto _SSL_CTX_free = SSL_CTX_free;
static auto _SSL_free = SSL_free;
static auto _SSL_new = SSL_new;
static auto _TLS_client_method = TLS_client_method;
static auto _SSL_CTX_ctrl = SSL_CTX_ctrl;
static auto _SSL_CTX_set_default_verify_paths = SSL_CTX_set_default_verify_paths;
static auto _SSL_set_connect_state = SSL_set_connect_state;
static auto _SSL_ctrl = SSL_ctrl;
static auto _SSL_do_handshake = SSL_do_handshake;
static auto _SSL_get_error = SSL_get_error;
static auto _SSL_shutdown = SSL_shutdown;
static auto _ERR_clear_error = ERR_clear_error;
static auto _SSL_up_ref = SSL_up_ref;
static auto _SSL_get_shutdown = SSL_get_shutdown;
static auto _SSL_use_certificate_file = SSL_use_certificate_file;
static auto _ERR_print_errors_cb = ERR_print_errors_cb;

/**
 *  Just the AMQP namespace
 */
namespace AMQP {

/**
 *  Function to set the openssl handle
 *  @param  ptr
 */
void openssl(void *ptr)
{
    // store handle
    handle = ptr;
}

/**
 *  Begin of AMQP::OpenSSL namespace
 */
namespace OpenSSL {

/**
 *  Is the openssl library loaded?
 *  @return bool
 */
bool valid() {
  return true;
}

/**
 *  Get the SSL_METHOD for outgoing connections
 *  @return SSL_METHOD *
 */
const SSL_METHOD *TLS_client_method() {
  return _TLS_client_method();
}

/**
 *  Create new SSL context
 *  @param  method  SSL_METHOD can be of the following types: TLS_method(), TLS_server_method(), TLS_client_method()
 *  @return         pointer to object   
 */
SSL_CTX *SSL_CTX_new(const SSL_METHOD *method) {
  return _SSL_CTX_new(method);
}

/**
 *  Read data from an ssl socket
 *  @param  ssl     ssl structure
 *  @param  buf     buffer to read into
 *  @param  num     size of buffer
 *  @return int     number of bytes read
 */
int SSL_read(SSL *ssl, void *buf, int num) {
  return _SSL_read(ssl, buf, num);
}

/**
 *  Write data to an ssl socket
 *  @param  ssl     ssl structure
 *  @param  buf     buffer to write
 *  @param  num     size of buffer
 *  @return int     number of bytes written
 */
int SSL_write(SSL *ssl, const void *buf, int num) {
  return _SSL_write(ssl, buf, num);
}

/**
 *  Connect the SSL object with a file descriptor
 *  @param  ssl     SSL object
 *  @param  fd      file descriptor
 *  @return int     wether the operation succeeded or not   
 */
int SSL_set_fd(SSL *ssl, int fd) {
  return _SSL_set_fd(ssl, fd);
}

/**
 *  The number of bytes availabe in the ssl struct that have been read
 *  from the socket, but that have not been returned the SSL_read()
 *  @param  ssl     SSL object
 *  @return int     number of bytes
 */
int SSL_pending(const SSL *ssl) {
  return _SSL_pending(ssl);
}

/**
 *  Free an allocated ssl context
 *  @param  ctx
 */
void SSL_CTX_free(SSL_CTX *ctx) {
  return _SSL_CTX_free(ctx);
}

/**
 *  Free an allocated SSL structure
 *  @param  ssl     SSL object to be freed
 *  @return int     wether the operation succeeded or not   
 */
void SSL_free(SSL *ssl) {
  return _SSL_free(ssl);
}

/**
 *  Create a new SSL structure for a connection
 *  @param  ctx     SSL context object
 *  @return SSL     the created SSL oject based on th context   
 */
SSL *SSL_new(SSL_CTX *ctx) {
  return _SSL_new(ctx);
}

/**
 *  Increment refcount for a ssl structure
 *  @param  ctx     SSL structure
 *  @return int     1 for success, 0 for failure   
 */
int SSL_up_ref(SSL *ssl) {
  return _SSL_up_ref(ssl);
}

/**
 *  Shut down a TLS/SSL shut down
 *  @param  ssl     SSL object to terminate
 *  @return int     returns diagnostic values   
 */
int SSL_shutdown(SSL *ssl) {
  return _SSL_shutdown(ssl);
}

/**
 *  Prepare SSL object to work in client or server mode
 *  @param  ssl     SSL object to set connect state on              
 */
void SSL_set_connect_state(SSL *ssl) {
  return _SSL_set_connect_state(ssl);
}

/**
 *  Perform a TLS/SSL handshake
 *  @param  ssl     SSL object
 *  @return int     returns diagnostic values   
 */
int SSL_do_handshake(SSL *ssl) {
  return _SSL_do_handshake(ssl);
}

/**
 *  Obtain shutdown statue for TLS/SSL I/O operation
 *  @param  ssl     SSL object
 *  @return int     returns error values
 */
int SSL_get_shutdown(const SSL *ssl) {
  return _SSL_get_shutdown(ssl);
}

/**
 *  Obtain result code for TLS/SSL I/O operation
 *  @param  ssl     SSL object
 *  @param  ret     the returned diagnostic value of SSL calls
 *  @return int     returns error values
 */
int SSL_get_error(const SSL *ssl, int ret) {
  return _SSL_get_error(ssl, ret);
}

/**
 *  Internal handling function for a ssl context
 *  @param  ssl     ssl context
 *  @param  cmd     command 
 *  @param  larg    first arg
 *  @param  parg    second arg
 *  @return long
 */
long SSL_ctrl(SSL *ssl, int cmd, long larg, void *parg) {
  return _SSL_ctrl(ssl, cmd, larg, parg);
}

/**
 *  Set the certificate file to be used by the connection
 *  @param  ssl     ssl structure
 *  @param  file    filename
 *  @param  type    type of file
 *  @return int
 */
int SSL_use_certificate_file(SSL *ssl, const char *file, int type) {
  return _SSL_use_certificate_file(ssl, file, type);
}

/**
 *  Control the SSL context
 *  @param  ctx
 *  @param  cmd
 *  @param  larg
 *  @param  parg
 *  @return long
 */
long SSL_CTX_ctrl(SSL_CTX *ctx, int cmd, long larg, void *parg) {
  return _SSL_CTX_ctrl(ctx, cmd, larg, parg);
}

/**
 *  Specify that the default location from which CA certificates are loaded
 *  should be used.
 *  @param  ctx
 */
int SSL_CTX_set_default_verify_paths(SSL_CTX *ctx) {
  return _SSL_CTX_set_default_verify_paths(ctx);
}

/**
 *  Clear the SSL error queue
 *  @return void
 */
void ERR_clear_error() {
  _ERR_clear_error();
}

/**
 *  Print errors via a callback
 *  @param  cb
 *  @param  u
 */
void ERR_print_errors_cb(int (*cb)(const char *str, size_t len, void *u), void *u) {
  _ERR_print_errors_cb(cb, u);
}

/**
 *  End of namespace
 */
}}
