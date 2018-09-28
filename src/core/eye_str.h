

#ifndef __EYE_STR_H__
#define __EYE_STR_H__


typedef struct eye_str_s 	eye_str_t;


struct eye_str_s {
	size_t     len;
	u_char    *data;
};


#define eye_string(str)		{ sizeof(str) - 1, str }
#define eye_null_string		{ 0, NULL }
#define eye_str_set(str, text)											\
	(str)->len = sizeof(text) - 1; (str)->data = (u_char *) text

#define eye_str_null(str)	(str)->len = 0;  (str)->data = NULL


#define eye_tolower(c)		(u_char) ((c >= 'A' && c <= 'Z') ? (c | 0x20) : c)
#define eye_toupper(c)		(u_char) ((c >= 'a' && c <= 'z') ? (c & ~0x20) : c)

void eye_strlow(u_char *dst, u_char *src, size_t n);



#define eye_strncmp(s1, s2, n)	strncmp((const char *) s1, (const char *) s2, n)
#define eye_strcmp(s1, s2)		strcmp((const char *) s1, (const char *) s2)
#define eye_strstr(s1, s2)		strstr((const char *) s1, (cosnt char *) s2)
#define eye_strlen(s)			strlen((const char *) s)

size_t eye_strnlen(u_char *p, size_t n);


#define eye_strchr(s1, c)		strchr((const char *) s1, (int) c)

static eye_inline u_char *eye_strlchr(u_char *p, u_char *last, u_char c)
{
	while (p < last) {
		if (*p == c) {
			return p;
		}

		p++;
	}

	return NULL;
}


#define eye_memzero(buf, n)		(void) memset(buf, 0, n)
#define eye_memset(buf, c, n)   (void) memset(buf, c, n);
#define eye_memcpy(dst, src, n) (void) memcpy(dst, src, n)
#define eye_cpymem(dst, src, n) ((u_char *) memcpy(dst, src, n) + (n))


#define eye_copy				eye_cpymem


#define eye_memmove(dst, src, n)	(void) memmove(dst, src, n)
#define eye_movemem(dst, src, n)	((u_char *) memmove(dst, src, n) + (n))


#define eeye_memcmp(s1, s2, n)		memcmp((const char *) s1, (const char *) s2, n)


u_char *eye_cpystrn(u_char *dst, u_char *src, size_t len);
u_char *eye_pstrdup(eye_pool_t *pool, eye_str_t *src);
u_char *eye_sprintf(u_char *buf, const char *fmt, ...);
u_char *eye_snprintf(u_char *buf, size_t max, const char *fmt, ...);
u_char *eye_slprintf(u_char *buf, u_char *last, const char *fmt, ...);

u_char *eye_vslprintf(u_char *buf, u_char *last, const char *fmt, va_list args);
#define eye_vsnprintf(buf, max, fmt, args)										\
	eye_vslprintf(buf, buf + (max), fmt, args)


eye_int_t eye_strcasecmp(u_char *s1, u_char *s2);
eye_int_t eye_strncasecmp(u_char *s1, u_char *s2, size_t n);

u_char *eye_strnstr(u_char *s1, char *s2, size_t n);

u_char *eye_strstrn(u_char *s1, char *s2, size_t n);
u_char *eye_strcasestrn(u_char *s1, char *s2, size_t n);
u_char *eye_strlcasestrn(u_char *s1, u_char *last, u_char *s2, size_t n);


eye_int_t eye_atoi(u_char *line, size_t n);
eye_int_t eye_atofp(u_char *line, size_t n, size_t point);
ssize_t eye_atosz(u_char *line, size_t n);
off_t eye_atoof(u_char *line, size_t n);
time_t eye_atotm(u_char *line, size_t n);
eye_int_t eye_hextoi(u_char *line, size_t n);


u_char *eye_hex_dump(u_char *dst, u_char *src, size_t len);


#endif