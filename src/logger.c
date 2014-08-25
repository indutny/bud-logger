#include <stdio.h>

#include "bud/tracing.h"
#include "bud/logger.h"
#include "openssl/ssl.h"

void on_handshake(bud_trace_client_t* client) {
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

  bud_log(
      client->logger,
      kBudLogInfo,
      "new frontend connection "
          "id=%llu remote=%s:%d protocol=%s cipher=%s sni=%s",
      client->id,
      client->host,
      client->port,
      ver,
      SSL_get_cipher_name(client->ssl),
      sni);
}

void on_backend_connect(bud_trace_client_t* client, bud_trace_backend_t* back) {
  const char* balance;

  if (back->sni_match) {
    balance = "sni_match";
  } else {
    switch (back->balance) {
      case kBudTraceBalanceOnFail: balance = "on_fail"; break;
      default: balance = "default"; break;
    }
  }

  bud_log(
      client->logger,
      kBudLogInfo,
      "connecting to backend id=%llu balance=%s host=%s port=%d",
      client->id,
      balance,
      back->host,
      back->port);
}


void on_kill_backend(bud_trace_client_t* client, bud_trace_backend_t* back) {
  bud_log(
      client->logger,
      kBudLogWarning,
      "killed backend id=%llu host=%s port=%d",
      client->id,
      back->host,
      back->port);
}


void on_revive_backend(bud_trace_client_t* client, bud_trace_backend_t* back) {
  bud_log(
      client->logger,
      kBudLogInfo,
      "revived backend id=%llu host=%s port=%d",
      client->id,
      back->host,
      back->port);
}


void on_retry(bud_trace_client_t* client) {
  bud_log(
      client->logger,
      kBudLogWarning,
      "backend still dead, retrying id=%llu",
      client->id);
}


void on_close(bud_trace_client_t* client, bud_error_t err) {
  bud_log(
      client->logger,
      kBudLogInfo,
      "client close id=%llu, reason: %s",
      client->id,
      bud_error_to_str(err));
}


BUD_TRACE_MODULE = {
  .version = BUD_TRACE_VERSION,

  .handshake = on_handshake,
  .backend_connect = on_backend_connect,
  .kill_backend = on_kill_backend,
  .revive_backend = on_revive_backend,
  .retry = on_retry,
  .close = on_close
};
