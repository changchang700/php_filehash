/* filehash extension for PHP */

#ifndef PHP_FILEHASH_H
# define PHP_FILEHASH_H

extern zend_module_entry filehash_module_entry;
# define phpext_filehash_ptr &filehash_module_entry

# define PHP_FILEHASH_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_FILEHASH)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_FILEHASH_H */

