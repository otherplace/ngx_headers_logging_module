/*
 * ngx request, response headers log module
 * <otherplace@gmail.com>
 */
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <nginx.h>

static ngx_int_t ngx_header_log_init(ngx_conf_t *cf);
static ngx_int_t ngx_request_header_log_handler(ngx_http_request_t *r);

static ngx_http_module_t  ngx_headers_log_module_ctx = {
	NULL,				/* preconfiguration */
	ngx_header_log_init,		/* postconfiguration */
	NULL,				/* create main configuration */
	NULL,				/* init main configuration */
	NULL,				/* create server configuration */
	NULL,				/* merge server configuration */
	NULL,				/* create location configuration */
	NULL				/* merge location configuration */
};

ngx_module_t  ngx_headers_logging_module = {
	NGX_MODULE_V1,
	&ngx_headers_log_module_ctx,	/* module context */
	NULL,	/* module directives */
	NGX_HTTP_MODULE,			/* module type */
	NULL,					/* init master */
	NULL,	/* init module */
	NULL,	/* init process */
	NULL,					/* init thread */
	NULL,					/* exit thread */
	NULL,		/* exit process */
	NULL,			/* exit master */
	NGX_MODULE_V1_PADDING
};

static ngx_int_t ngx_header_log_init(ngx_conf_t *cf)
{
	ngx_http_handler_pt *h;
	ngx_http_core_main_conf_t *cmcf;
	
	cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_core_module);
	if( !cmcf )
	{
		return NGX_ERROR;
	}
	h = ngx_array_push(&cmcf->phases[NGX_HTTP_POST_READ_PHASE].handlers);
	if( !h )
	{
		return NGX_ERROR;
	}
	*h = ngx_request_header_log_handler;

	return NGX_OK; 

}

static ngx_int_t ngx_request_header_log_handler(ngx_http_request_t *r)
{
	ngx_http_core_loc_conf_t *clcf;
	ssize_t header_len = 0;
	ngx_table_elt_t *th;
	ngx_list_part_t *part;
	ngx_uint_t i;
	ngx_str_t logbuf;
	u_char *last;

	clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);
	if( !clcf )
		return NGX_ERROR;
	if( clcf->request_header_logging > 0 )
	{
		/* calculate request message length for header logging */
		header_len += r->request_line.len;
		header_len += sizeof(CRLF) - 1;
		part = &r->headers_in.headers.part;
		th = part->elts;

		for (i = 0; /* void */; i++) {
			if (i >= part->nelts) {
				if (part->next == NULL) {
					break;
				}

				part = part->next;
				th = part->elts;
				i = 0;
			}
			header_len += th[i].key.len;
			header_len += sizeof(": ") - 1;
			header_len += th[i].value.len;
			header_len += sizeof(CRLF) - 1;
		}	
		/* request message regenerate for header logging */
		logbuf.data = ngx_palloc(r->pool, header_len);
		logbuf.len = header_len;
		last = ngx_sprintf(logbuf.data, "%*s%s", r->request_line.len, r->request_line.data, CRLF);

		part = &r->headers_in.headers.part;
		th = part->elts;
		for (i = 0; /* void */; i++) {
			if (i >= part->nelts) {
				if (part->next == NULL) {
					break;
				}

				part = part->next;
				th = part->elts;
				i = 0;
			}
			last = ngx_sprintf(last, "%V: %V%s", &th[i].key, &th[i].value, CRLF);
		}	
		last = ngx_sprintf(last, "%s", CRLF);
		ngx_log_error(NGX_LOG_INFO, r->connection->log, 0,
				"%s, \"%V\"", __func__, &logbuf);
		ngx_pfree(r->pool, logbuf.data);

	}
	return NGX_OK;
}

