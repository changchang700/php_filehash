/* filehash extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_filehash.h"
#include <openssl/sha.h>

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

#define CHUNK_SIZE 2097152 // 2MB
#define SHA1_LENGTH 20

static void StrToHex(const unsigned char* src, size_t len, char* output) {
    for (size_t i = 0; i < len; ++i) {
        snprintf(output + (i * 2), 3, "%02x", src[i]);
    }
}

static void SHA1State(SHA_CTX* ctx, char* output) {
    StrToHex((unsigned char*)&ctx->h0, SHA1_LENGTH, output);
}

static void SHA1Final(SHA_CTX* ctx, char* output) {
    unsigned char tmp[SHA1_LENGTH];
    SHA1_Final(tmp, ctx);
    StrToHex(tmp, SHA1_LENGTH, output);
}

PHP_FUNCTION(file_hash_chunks) {
    char *filename;
    size_t filename_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &filename, &filename_len) == FAILURE) {
        RETURN_NULL();
    }

    FILE *file = fopen(filename, "rb");
    if (!file) {
        php_error_docref(NULL, E_WARNING, "打开文件失败，请检查文件是否存在");
        RETURN_NULL();
    }
	
	fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
	
	const uint64_t total_block = (file_size - 1) / CHUNK_SIZE + 1;

    SHA_CTX ctx;
    SHA1_Init(&ctx);

    size_t read_size;
    unsigned char buffer[CHUNK_SIZE];
    array_init(return_value);
	
	int i = 1;

    while ((read_size = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        SHA1_Update(&ctx, buffer, read_size);
        if (i == total_block) {
            char cumulative_sha1[SHA1_LENGTH * 2 + 1];
            SHA1Final(&ctx, cumulative_sha1);
            add_next_index_string(return_value, cumulative_sha1);
        } else {
			char cumulative_sha1[SHA1_LENGTH * 2 + 1];
            SHA1State(&ctx, cumulative_sha1);
            add_next_index_string(return_value, cumulative_sha1);
        }
		i++;
    }

    fclose(file);
}

PHP_RINIT_FUNCTION(filehash)
{
#if defined(ZTS) && defined(COMPILE_DL_FILEHASH)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}

PHP_MINFO_FUNCTION(filehash)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "filehash support", "enabled");
	php_info_print_table_end();
}

static const zend_function_entry filehash_functions[] = {
	PHP_FE(file_hash_chunks,		NULL)
	PHP_FE_END
};

zend_module_entry filehash_module_entry = {
	STANDARD_MODULE_HEADER,
	"filehash",						/* Extension name */
	filehash_functions,				/* zend_function_entry */
	NULL,							/* PHP_MINIT - Module initialization */
	NULL,							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(filehash),			/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(filehash),			/* PHP_MINFO - Module info */
	PHP_FILEHASH_VERSION,			/* Version */
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_FILEHASH
# ifdef ZTS
	ZEND_TSRMLS_CACHE_DEFINE()
# endif
	ZEND_GET_MODULE(filehash)
#endif

