ngx_headers_logging_module
==========================

HTTP request headers and HTTP response headers log module for nginx.

Configuration directives
========================
ngx_request_headers_logging
---------------------------
* **syntax**: `ngx_request_headers_logging on|off`
* **default**: `none`
* **context**: `http`, `server`, `location`, `if`

HTTP request headers logging to ngx's error log as `INFO` log level.

ngx_response_headers_logging
---------------------------
* **syntax**: `ngx_response_headers_logging on|off`
* **default**: `none`
* **context**: `http`, `server`, `location`, `if`

HTTP response headers logging to ngx's error log as `INFO` log level.

How to compile
==============

	$ cd nginx-1.x.x/
	$ patch -p0 < <path of this module>/nginx-1.6.x.patch
	$ ./configure --add-module=<path of this module> 

