#include <stdio.h>

#include "bud/tracing.h"
#include "openssl/ssl.h"

void handshake(bud_trace_client_t* client) {
  const char* sni;
  const char* ver;

  sni = SSL_get_servername(client->ssl, TLSEXT_NAMETYPE_host_name);
  if (sni == NULL)
    sni = "(empty)";

  if (client->ssl->version == TLS1_2_VERSION)
    ver = "tls1.2";
  else if (client->ssl->version == TLS1_1_VERSION)
    ver = "tls1.1";
  else if (client->ssl->version == TLS1_VERSION)
    ver = "tls1.0";
  else
    ver = "ssl3";

  fprintf(
      stderr,
      "new frontend connection "
          "id=%d remote=%s:%d protocol=%s cipher=%s sni=%s\n",
      client->fd,
      client->host,
      client->port,
      ver,
      SSL_get_cipher_name(client->ssl),
      sni);
}

void backend_connect(bud_trace_client_t* client, bud_trace_backend_t* back) {
  const char* balance;

  if (back->sni_match) {
    balance = "sni_match";
  } else {
    switch (back->balance) {
      case kBudTraceBalanceOnFail: balance = "on_fail"; break;
      default: balance = "default"; break;
    }
  }

  fprintf(
      stderr,
      "connecting to backend id=%d balance=%s host=%s port=%d\n",
      client->fd,
      balance,
      back->host,
      back->port);
}

BUD_TRACE_MODULE = {
  .handshake = handshake,
  .backend_connect = (bud_trace_cb_t) backend_connect
};
