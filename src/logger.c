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
          "id=%llu remote=%s:%d protocol=%s cipher=%s sni=%s\n",
      client->id,
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
      "connecting to backend id=%llu balance=%s host=%s port=%d\n",
      client->id,
      balance,
      back->host,
      back->port);
}


void kill_backend(bud_trace_client_t* client, bud_trace_backend_t* back) {
  fprintf(
      stderr,
      "killed backend id=%llu host=%s port=%d\n",
      client->id,
      back->host,
      back->port);
}


void revive_backend(bud_trace_client_t* client, bud_trace_backend_t* back) {
  fprintf(
      stderr,
      "revived backend host=%s port=%d\n",
      client->id,
      back->host,
      back->port);
}


void retry(bud_trace_client_t* client) {
  fprintf(
      stderr,
      "backend still dead, retrying id=%llu\n",
      client->id);
}

BUD_TRACE_MODULE = {
  .handshake = handshake,
  .backend_connect = backend_connect,
  .kill_backend = kill_backend,
  .revive_backend = revive_backend,
  .retry = retry
};
