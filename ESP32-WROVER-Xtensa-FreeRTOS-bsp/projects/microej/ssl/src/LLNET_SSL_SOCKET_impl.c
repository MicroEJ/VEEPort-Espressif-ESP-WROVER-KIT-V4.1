/*
 * C
 *
 * Copyright 2018-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief LLNET_SSL_SOCKET implementation over mbedtls.
 * @author MicroEJ Developer Team
 * @version 2.1.5
 * @date 20 December 2021
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif
#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#if defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#endif
#include "LLNET_Common.h"
#include "LLNET_CONSTANTS.h"
#include "LLNET_CHANNEL_impl.h"
#include "LLNET_SSL_ERRORS.h"
#include "LLNET_SSL_utils_mbedtls.h"
#include "LLNET_SSL_SOCKET_impl.h"
#include "LLNET_SSL_CONSTANTS.h"
#include <stdio.h>

#ifdef __cplusplus
	extern "C" {
#endif

/* external functions */
extern int32_t LLNET_SSL_TranslateReturnCode(int32_t mbedtls_error);

typedef struct
{
	mbedtls_ssl_context   ssl_context;
	uint32_t              closeNotifyFlag;
} ssl_socket ;

#if defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
/* Global variable for entropy in drbg context */
mbedtls_ctr_drbg_context ctr_drbg;
static mbedtls_entropy_context entropy;

/* personalization string for the drbg */
const char *DRBG_PERS = "mbed TLS microEJ client";
#endif

/* static functions */
static int32_t LLNET_SSL_SOCKET_IMPL_initialHandShake(int32_t sslID, int32_t fd, uint8_t retry);
static int32_t asyncOperationWrapper(int32_t fd, SELECT_Operation operation, uint8_t retry);

int32_t LLNET_SSL_SOCKET_IMPL_initialize(void){
	LLNET_SSL_DEBUG_TRACE("%s()\n", __func__);

#if defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    int ret;
    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                      (const unsigned char *) DRBG_PERS,
                      sizeof (DRBG_PERS))) != 0) {
    	LLNET_SSL_DEBUG_MBEDTLS_TRACE("mbedtls_crt_drbg_init", ret);
    	return LLNET_SSL_TranslateReturnCode(ret);
    }
#endif

    return J_SSL_NO_ERROR;
}

int32_t LLNET_SSL_SOCKET_IMPL_create(int32_t contextID, int32_t fd, uint8_t* hostname, int32_t hostnameLen, uint8_t autoclose, uint8_t useClientMode, uint8_t needClientAuth, uint8_t retry){
	LLNET_SSL_DEBUG_TRACE("%s(context=%d, fd=%d, autoclose=%d, useClientMode=%d, needClientAuth=%d, retry=%d)\n", __func__, (int)contextID, (int)fd, autoclose, useClientMode, needClientAuth, retry);

	mbedtls_ssl_config* conf = (mbedtls_ssl_config*)(contextID);
	ssl_socket* ssl_ctx = (ssl_socket*)mbedtls_calloc(1, sizeof(ssl_socket));

	if ( conf != NULL && ssl_ctx != NULL)
	{
		/* It is possible to disable authentication by passing
		 * MBEDTLS_SSL_VERIFY_NONE in the call to mbedtls_ssl_conf_authmode()
		 */
		if (useClientMode || needClientAuth) {
			mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_REQUIRED);
		} else {
			mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_NONE);
		}

		mbedtls_ssl_init(&(ssl_ctx->ssl_context));
		if ((NULL != hostname) && (hostnameLen > 0)) {
			// Check if the given host is an IP address or is actually a host name.
			struct sockaddr_in hostAddr;
			int is_valid_addr = inet_pton(AF_INET,(const char*) hostname, &(hostAddr.sin_addr));
			if(0 == is_valid_addr) {
				mbedtls_ssl_set_hostname(&(ssl_ctx->ssl_context), (char*)hostname);
			}
		}
		ssl_ctx->closeNotifyFlag = 0;

		int ret = 0;
		if ((ret = mbedtls_ssl_setup(&(ssl_ctx->ssl_context), conf)) != 0) {
		    mbedtls_ssl_free(&(ssl_ctx->ssl_context));
		    mbedtls_free((void*)ssl_ctx);
			LLNET_SSL_DEBUG_MBEDTLS_TRACE("mbedtls_ssl_setup", ret);
			return J_CREATE_SSL_ERROR; //error
		}

		int* net_socket = (int*)mbedtls_calloc(1, sizeof(int));
		if (net_socket != NULL)
		{
			*(net_socket) = fd;
			mbedtls_ssl_set_bio(&(ssl_ctx->ssl_context), (void*)net_socket, LLNET_SSL_utils_mbedtls_send, LLNET_SSL_utils_mbedtls_recv, NULL );
		}
		else{
		    mbedtls_ssl_free(&(ssl_ctx->ssl_context));
		    mbedtls_free((void*)ssl_ctx);
			return J_CREATE_SSL_ERROR;
		}
	}
	else
		return J_CREATE_SSL_ERROR;

	return (int32_t)ssl_ctx;
}


int32_t LLNET_SSL_SOCKET_IMPL_close(int32_t sslID, int32_t fd, uint8_t autoclose, uint8_t retry){
	LLNET_SSL_DEBUG_TRACE("%s(ssl=%d, fd=%d, autoclose=%d, retry=%d)\n", __func__, (int)sslID, (int)fd, autoclose, retry);

	ssl_socket* ssl_ctx = (ssl_socket*)(sslID);

	if (ssl_ctx == NULL)
		return J_CREATE_SSL_ERROR;

	if(!retry){
		//set non-blocking mode
		bool was_non_blocking = is_socket_non_blocking(fd);
		int32_t res = 0;
		if(was_non_blocking == false){
			res = set_socket_non_blocking(fd, true);
		}

		if(res == 0){
			//send close notify
			int ret = mbedtls_ssl_close_notify(&(ssl_ctx->ssl_context));
		    if (ret < 0) {
		        if (ret != MBEDTLS_ERR_SSL_WANT_READ &&
		            ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
		        	LLNET_SSL_DEBUG_MBEDTLS_TRACE("mbedtls_ssl_close_notify", ret);
		        }
		    }

			//reset non-blocking mode
		    if(was_non_blocking == false){
				res = set_socket_non_blocking(fd, false);
				if(res != 0){
					return J_CREATE_SSL_ERROR;
				}
		    }
		} else {
			return J_CREATE_SSL_ERROR;
		}
	}

	if(!autoclose){
		//the close of the underlying socket is not requested
		//read close_notify alert to clear input stream
		int8_t buffer[1];
		int32_t res = LLNET_SSL_SOCKET_IMPL_read(sslID, fd, buffer, 0, 1, retry);

		if(res == J_NATIVE_CODE_BLOCKED_WITHOUT_RESULT || (res >= 0 && !(ssl_ctx->closeNotifyFlag))){
			//read operation does not failed and the close notify alert is not yet received
			return J_NATIVE_CODE_BLOCKED_WITHOUT_RESULT;
		}
	}

	LLNET_SSL_DEBUG_TRACE("%s(ssl=%d, fd=%d, autoclose=%d, retry=%d) free SSL\n", __func__, (int)sslID, (int)fd, (int)autoclose, (int)retry);

	return J_SSL_NO_ERROR;
}


int32_t LLNET_SSL_SOCKET_IMPL_freeSSL(int32_t sslID, uint8_t retry){
	LLNET_SSL_DEBUG_TRACE("%s(ssl=%d, retry=%d)\n", __func__, (int)sslID, retry);

	ssl_socket* ssl_ctx = (ssl_socket*)(sslID);

	if (ssl_ctx == NULL)
		return J_CREATE_SSL_ERROR;

	//free BIO
	if (ssl_ctx->ssl_context.p_bio) {
		mbedtls_free(ssl_ctx->ssl_context.p_bio);
	}

	//free SSL
    mbedtls_ssl_free(&(ssl_ctx->ssl_context));
    mbedtls_free((void*)ssl_ctx);

	return J_SSL_NO_ERROR;
}

int32_t LLNET_SSL_SOCKET_IMPL_initialClientHandShake(int32_t sslID, int32_t fd, uint8_t retry){
	LLNET_SSL_DEBUG_TRACE("%s(ssl=%d, fd=%d, retry=%d)\n", __func__, (int)sslID, (int)fd, retry);

	return LLNET_SSL_SOCKET_IMPL_initialHandShake(sslID, fd, retry);
}

int32_t LLNET_SSL_SOCKET_IMPL_initialServerHandShake(int32_t sslID, int32_t fd, uint8_t retry){
	LLNET_SSL_DEBUG_TRACE("%s(ssl=%d, fd=%d, retry=%d)\n", __func__, (int)sslID, (int)fd, retry);

	return LLNET_SSL_SOCKET_IMPL_initialHandShake(sslID, fd, retry);
}

int32_t LLNET_SSL_SOCKET_IMPL_read(int32_t sslID, int32_t fd, int8_t* buf, int32_t off, int32_t len, uint8_t retry){
	LLNET_SSL_DEBUG_TRACE("%s(ssl=%d, fd=%d, offset=%d, length=%d, retry=%d)\n", __func__, (int)sslID, (int)fd, (int)off, (int)len, retry);

	//set non-blocking mode
	bool was_non_blocking = is_socket_non_blocking(fd);
	int32_t res;
	if(was_non_blocking == false){
		res = set_socket_non_blocking(fd, true);
		if(res != 0){
			return J_SOCKET_ERROR;
		}
	}

	ssl_socket* ssl_ctx = (ssl_socket*)(sslID);
	if (NULL == ssl_ctx)
		return J_CREATE_SSL_ERROR;

    /* Read data out of the socket */
    int ret = mbedtls_ssl_read(&(ssl_ctx->ssl_context), (unsigned char *) buf + off, len);
    if (ret < 0) {
    	if (MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY == ret){
    		ssl_ctx->closeNotifyFlag = 1;
    		//end-of-file
    		return J_EOF;
    	}
    	else if (MBEDTLS_ERR_SSL_WANT_READ != ret && MBEDTLS_ERR_SSL_WANT_WRITE != ret) {
    		LLNET_SSL_DEBUG_MBEDTLS_TRACE("mbedtls_ssl_read", ret);
        }
    }

	LLNET_SSL_DEBUG_TRACE("%s READ (ssl=%d, fd=%d, offset=%d, length=%d, retry=%d) bytes read=%d\n", __func__, (int)sslID, (int)fd, (int)off, (int)len, (int)retry, (int)ret);

	//restore original flags
	if(was_non_blocking == false){
		res = set_socket_non_blocking(fd, false);
		if(res != 0){
			return J_SOCKET_ERROR;
		}
	}

	if(ret < 0){
		if(ret == MBEDTLS_ERR_SSL_WANT_READ){
			 return asyncOperationWrapper(fd, SELECT_READ, retry);
		}
		else if(ret == MBEDTLS_ERR_SSL_WANT_WRITE){
			//read operation can also cause write operation when the peer requests a re-negotiation
			 return asyncOperationWrapper(fd, SELECT_WRITE, retry);
		}
		return LLNET_SSL_TranslateReturnCode(ret);
	}

	return ret;
}

int32_t LLNET_SSL_SOCKET_IMPL_write(int32_t sslID, int32_t fd, int8_t* buf, int32_t off, int32_t len, uint8_t retry){
	LLNET_SSL_DEBUG_TRACE("%s(ssl=%d, fd=%d, offset=%d, length=%d, retry=%d)\n", __func__, (int)sslID, (int)fd, (int)off, (int)len, retry);

	//set non-blocking mode
	bool was_non_blocking = is_socket_non_blocking(fd);
	int32_t res;
	if(was_non_blocking == false){
		res = set_socket_non_blocking(fd, true);
		if(res != 0){
			return J_SOCKET_ERROR;
		}
	}

	ssl_socket* ssl_ctx = (ssl_socket*)(sslID);
	if (NULL == ssl_ctx)
		return J_CREATE_SSL_ERROR;

    int ret = mbedtls_ssl_write(&(ssl_ctx->ssl_context), (const unsigned char *) buf + off, len);
    if (ret < 0) {
    	if (MBEDTLS_ERR_NET_CONN_RESET == ret){
    		/* Set ret to 0 for re*/
    		return J_CONNECTION_RESET;
    	}
    	else if (MBEDTLS_ERR_SSL_WANT_READ != ret &&
    			MBEDTLS_ERR_SSL_WANT_WRITE != ret) {
        	LLNET_SSL_DEBUG_MBEDTLS_TRACE("mbedtls_ssl_write", ret);
        }
    }

	LLNET_SSL_DEBUG_TRACE("%s WRITE (ssl=%d, fd=%d, offset=%d, length=%d, retry=%d) written bytes=%d\n", __func__, (int)sslID, (int)fd, (int)off, (int)len, (int)retry, (int)ret);

	//restore original flags
	if(was_non_blocking == false){
		res = set_socket_non_blocking(fd, false);
		if(res != 0){
			return J_SOCKET_ERROR;
		}
	}

	if(ret < 0){
		if(ret == MBEDTLS_ERR_SSL_WANT_READ){
			//write operation can also cause read operation when the peer requests a re-negotiation
			 return asyncOperationWrapper(fd, SELECT_READ, retry);
		}
		else if(ret == MBEDTLS_ERR_SSL_WANT_WRITE){
			 return asyncOperationWrapper(fd, SELECT_WRITE, retry);
		}
		return LLNET_SSL_TranslateReturnCode(ret);
	}

	return ret;
}

int32_t LLNET_SSL_SOCKET_IMPL_available(int32_t sslID, uint8_t retry){
	LLNET_SSL_DEBUG_TRACE("%s(ssl=%d, retry=%d)\n", __func__, (int)sslID, retry);

	ssl_socket* ssl_ctx = (ssl_socket*)(sslID);
	if (NULL == ssl_ctx)
		return J_CREATE_SSL_ERROR;

    int ret = (int) mbedtls_ssl_get_bytes_avail( &(ssl_ctx->ssl_context) );

	if(ret < 0){
		return J_UNKNOWN_ERROR;
	}
	return ret;
}

static int32_t LLNET_SSL_SOCKET_IMPL_initialHandShake(int32_t sslID, int32_t fd, uint8_t retry){
	//set non-blocking mode
	bool was_non_blocking = is_socket_non_blocking(fd);
	int32_t res;
	if(was_non_blocking == false){
		res = set_socket_non_blocking(fd, true);
		if(res != 0){
			return J_SOCKET_ERROR;
		}
	}

	ssl_socket* ssl_ctx = (ssl_socket*)(sslID);
	if (NULL == ssl_ctx)
		return J_CREATE_SSL_ERROR;

    int ret = mbedtls_ssl_handshake(&(ssl_ctx->ssl_context));
    if (ret < 0) {
        if (MBEDTLS_ERR_SSL_WANT_READ != ret &&
        		MBEDTLS_ERR_SSL_WANT_WRITE != ret)
        {
        	LLNET_SSL_DEBUG_MBEDTLS_TRACE("mbedtls_ssl_handshake", ret);

        	/* Send bad cert alert if no cert available */
        	if (MBEDTLS_ERR_SSL_CA_CHAIN_REQUIRED == ret){
				int32_t retAlert = 0;
				if (0 != (retAlert = mbedtls_ssl_send_alert_message( &(ssl_ctx->ssl_context),
								MBEDTLS_SSL_ALERT_LEVEL_FATAL,
								MBEDTLS_SSL_ALERT_MSG_BAD_CERT)))
				{
					printf("ERROR : failed to send alert_message : ret = %d", retAlert);
				}
        	}

        	/* SSL Context unusable */
        	mbedtls_ssl_session_reset(&(ssl_ctx->ssl_context));
        }
    }

    LLNET_SSL_DEBUG_TRACE("%s HandShake (ssl=%d, fd=%d, retry=%d) ret=%d\n", __func__, (int)sslID, (int)fd, (int)retry, (int)ret);

	//restore original flags
    if(false == was_non_blocking){
		res = set_socket_non_blocking(fd, false);
		if(0 != res){
			return J_SOCKET_ERROR;
		}
    }

	if(0 != ret){
		if(MBEDTLS_ERR_SSL_WANT_READ == ret){
			 return asyncOperationWrapper(fd, SELECT_READ, retry);
		}
		else if(MBEDTLS_ERR_SSL_WANT_WRITE == ret){
			 return asyncOperationWrapper(fd, SELECT_WRITE, retry);
		}

		return LLNET_SSL_TranslateReturnCode(ret);
	}

	return ret;
}

static int32_t asyncOperationWrapper(int32_t fd, SELECT_Operation operation, uint8_t retry){
	int32_t ret = asyncOperation(fd, operation, retry);

	if (J_NET_NATIVE_CODE_BLOCKED_WITHOUT_RESULT == ret){
		return J_NATIVE_CODE_BLOCKED_WITHOUT_RESULT;
	} else if (J_ASYNC_BLOCKING_REQUEST_QUEUE_LIMIT_REACHED == ret){
		return J_BLOCKING_QUEUE_LIMIT_REACHED;
	} else if (J_ETIMEDOUT == ret){
		return J_SOCKET_TIMEOUT;
	} else if (J_EUNKNOWN == ret){
		return J_UNKNOWN_ERROR;
	} else {
		return ret;
	}
}

#ifdef __cplusplus
	}
#endif
