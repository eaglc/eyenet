

#include "eye_core.h"


static u_char *eye_sprintf_num(u_char *buf, u_char *last, uint64_t ui64,
    u_char zero, eye_uint_t hexadecimal, eye_uint_t width);


void eye_strlow(u_char *dst, u_char *src, size_t n)
{
	while (n) {
		*dst = eye_tolower(*src);
		dst++;
		src++;
		n--;
	}
}

size_t eye_strnlen(u_char *p, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++) {

		if (p[i] == '\0') {
			return i;
		}
	}

	return n;
}


u_char *eye_cpystrn(u_char *dst, u_char *src, size_t n)
{
	if (n == 0) {
		return dst;
	}

	while (--n) {
		*dst = *src;

		if (*dst == '\0') {
			return dst;
		}

		dst++;
		src++;
	}

	*dst = '\0';

	return dst;
}

u_char *eye_pstrdup(eye_pool_t *pool, eye_str_t *src)
{
	u_char *dst;

	dst = eye_palloc(pool, src->len);
	if (dst == NULL) {
		return NULL;
	}

	eye_memcpy(dst, src, src->len);

	return dst;
}


/*
 * supported formats:
 *    %[0][width][x][X]O        off_t
 *    %[0][width]T              time_t
 *    %[0][width][u][x|X]z      ssize_t/size_t
 *    %[0][width][u][x|X]d      int/u_int
 *    %[0][width][u][x|X]l      long
 *    %[0][width|m][u][x|X]i    eye_int_t/eye_uint_t
 *    %[0][width][u][x|X]D      int32_t/uint32_t
 *    %[0][width][u][x|X]L      int64_t/uint64_t
 *    %[0][width|m][u][x|X]A    eye_atomic_int_t/eye_atomic_uint_t
 *    %[0][width][.width]f      double, max valid number fits to %18.15f
 *    %P                        eye_pid_t
 *    %M                        eye_msec_t
 *    %r                        rlim_t
 *    %p                        void *
 *    %V                        eye_str_t *
 *    %v                        eye_variable_value_t *
 *    %s                        null-terminated string
 *    %*s                       length and string
 *    %Z                        '\0'
 *    %N                        '\n'
 *    %c                        char
 *    %%                        %
 *
 *  reserved:
 *    %t                        ptrdiff_t
 *    %S                        null-terminated wchar string
 *    %C                        wchar
 */


u_char *eye_sprintf(u_char *buf, const char *fmt, ...)
{
	u_char	*p;
	va_list	 args;

	va_start(args, fmt);
	p = eye_vslprintf(buf, (void *)-1, fmt, args);
	va_end(args);

	return p;
}


u_char *eye_snprintf(u_char *buf, size_t max, const char *fmt, ...)
{
	u_char	*p;
	va_list	 args;

	va_start(args, fmt);
	p = eye_vslprintf(buf, buf + max, fmt, args);
	va_end(args);

	return p;
}


u_char *eye_slprintf(u_char *buf, u_char *last, const char *fmt, ...)
{
	u_char	*p;
	va_list	 args;

	va_start(args, fmt);
	p = eye_vslprintf(buf, last, fmt, args);
	va_end(args);

	return p;
}

u_char *eye_vslprintf(u_char *buf, u_char *last, const char *fmt, va_list args)
{
    u_char                *p, zero;
    int                    d;
    double                 f;
    size_t                 len, slen;
    int64_t                i64;
    uint64_t               ui64, frac;
    eye_msec_t             ms;
    eye_uint_t             width, sign, hex, max_width, frac_width, scale, n;
    eye_str_t             *v;
    //eye_variable_value_t  *vv;

    while (*fmt && buf < last) {

        /*
         * "buf < last" means that we could copy at least one character:
         * the plain character, "%%", "%c", and minus without the checking
         */

        if (*fmt == '%') {

            i64 = 0;
            ui64 = 0;

            zero = (u_char) ((*++fmt == '0') ? '0' : ' ');
            width = 0;
            sign = 1;
            hex = 0;
            max_width = 0;
            frac_width = 0;
            slen = (size_t) -1;

            while (*fmt >= '0' && *fmt <= '9') {
                width = width * 10 + (*fmt++ - '0');
            }


            for ( ;; ) {
                switch (*fmt) {

                case 'u':
                    sign = 0;
                    fmt++;
                    continue;

                case 'm':
                    max_width = 1;
                    fmt++;
                    continue;

                case 'X':
                    hex = 2;
                    sign = 0;
                    fmt++;
                    continue;

                case 'x':
                    hex = 1;
                    sign = 0;
                    fmt++;
                    continue;

                case '.':
                    fmt++;

                    while (*fmt >= '0' && *fmt <= '9') {
                        frac_width = frac_width * 10 + (*fmt++ - '0');
                    }

                    break;

                case '*':
                    slen = va_arg(args, size_t);
                    fmt++;
                    continue;

                default:
                    break;
                }

                break;
            }


            switch (*fmt) {

            case 'V':
                v = va_arg(args, eye_str_t *);

                len = eye_min(((size_t) (last - buf)), v->len);
                buf = eye_cpymem(buf, v->data, len);
                fmt++;

                continue;

            // case 'v':
            //     vv = va_arg(args, eye_variable_value_t *);

            //     len = eye_min(((size_t) (last - buf)), vv->len);
            //     buf = eye_cpymem(buf, vv->data, len);
            //     fmt++;

            //     continue;

            case 's':
                p = va_arg(args, u_char *);

                if (slen == (size_t) -1) {
                    while (*p && buf < last) {
                        *buf++ = *p++;
                    }

                } else {
                    len = eye_min(((size_t) (last - buf)), slen);
                    buf = eye_cpymem(buf, p, len);
                }

                fmt++;

                continue;

            case 'O':
                i64 = (int64_t) va_arg(args, off_t);
                sign = 1;
                break;

            case 'P':
                i64 = (int64_t) va_arg(args, eye_pid_t);
                sign = 1;
                break;

            case 'T':
                i64 = (int64_t) va_arg(args, time_t);
                sign = 1;
                break;

            case 'M':
                ms = (eye_msec_t) va_arg(args, eye_msec_t);
                if ((eye_msec_int_t) ms == -1) {
                    sign = 1;
                    i64 = -1;
                } else {
                    sign = 0;
                    ui64 = (uint64_t) ms;
                }
                break;

            case 'z':
                if (sign) {
                    i64 = (int64_t) va_arg(args, ssize_t);
                } else {
                    ui64 = (uint64_t) va_arg(args, size_t);
                }
                break;

            case 'i':
                if (sign) {
                    i64 = (int64_t) va_arg(args, eye_int_t);
                } else {
                    ui64 = (uint64_t) va_arg(args, eye_uint_t);
                }

                if (max_width) {
                    width = EYE_INT_T_LEN;
                }

                break;

            case 'd':
                if (sign) {
                    i64 = (int64_t) va_arg(args, int);
                } else {
                    ui64 = (uint64_t) va_arg(args, u_int);
                }
                break;

            case 'l':
                if (sign) {
                    i64 = (int64_t) va_arg(args, long);
                } else {
                    ui64 = (uint64_t) va_arg(args, u_long);
                }
                break;

            case 'D':
                if (sign) {
                    i64 = (int64_t) va_arg(args, int32_t);
                } else {
                    ui64 = (uint64_t) va_arg(args, uint32_t);
                }
                break;

            case 'L':
                if (sign) {
                    i64 = va_arg(args, int64_t);
                } else {
                    ui64 = va_arg(args, uint64_t);
                }
                break;

            case 'A':
                if (sign) {
                    i64 = (int64_t) va_arg(args, eye_atomic_int_t);
                } else {
                    ui64 = (uint64_t) va_arg(args, eye_atomic_uint_t);
                }

                if (max_width) {
                    width = EYE_ATOMIC_T_LEN;
                }

                break;

            case 'f':
                f = va_arg(args, double);

                if (f < 0) {
                    *buf++ = '-';
                    f = -f;
                }

                ui64 = (int64_t) f;
                frac = 0;

                if (frac_width) {

                    scale = 1;
                    for (n = frac_width; n; n--) {
                        scale *= 10;
                    }

                    frac = (uint64_t) ((f - (double) ui64) * scale + 0.5);

                    if (frac == scale) {
                        ui64++;
                        frac = 0;
                    }
                }

                buf = eye_sprintf_num(buf, last, ui64, zero, 0, width);

                if (frac_width) {
                    if (buf < last) {
                        *buf++ = '.';
                    }

                    buf = eye_sprintf_num(buf, last, frac, '0', 0, frac_width);
                }

                fmt++;

                continue;

#if !(EYE_WIN32)
            case 'r':
                i64 = (int64_t) va_arg(args, rlim_t);
                sign = 1;
                break;
#endif

            case 'p':
                ui64 = (uintptr_t) va_arg(args, void *);
                hex = 2;
                sign = 0;
                zero = '0';
                width = 2 * sizeof(void *);
                break;

            case 'c':
                d = va_arg(args, int);
                *buf++ = (u_char) (d & 0xff);
                fmt++;

                continue;

            case 'Z':
                *buf++ = '\0';
                fmt++;

                continue;

            case 'N':
#if (EYE_WIN32)
                *buf++ = CR;
                if (buf < last) {
                    *buf++ = LF;
                }
#else
                *buf++ = LF;
#endif
                fmt++;

                continue;

            case '%':
                *buf++ = '%';
                fmt++;

                continue;

            default:
                *buf++ = *fmt++;

                continue;
            }

            if (sign) {
                if (i64 < 0) {
                    *buf++ = '-';
                    ui64 = (uint64_t) -i64;

                } else {
                    ui64 = (uint64_t) i64;
                }
            }

            buf = eye_sprintf_num(buf, last, ui64, zero, hex, width);

            fmt++;

        } else {
            *buf++ = *fmt++;
        }
    }

    return buf;
}



static u_char *
eye_sprintf_num(u_char *buf, u_char *last, uint64_t ui64, u_char zero,
    eye_uint_t hexadecimal, eye_uint_t width)
{
    u_char         *p, temp[EYE_INT64_LEN + 1];
                       /*
                        * we need temp[EYE_INT64_LEN] only,
                        * but icc issues the warning
                        */
    size_t          len;
    uint32_t        ui32;
    static u_char   hex[] = "0123456789abcdef";
    static u_char   HEX[] = "0123456789ABCDEF";

    p = temp + EYE_INT64_LEN;

    if (hexadecimal == 0) {

        if (ui64 <= (uint64_t) EYE_MAX_UINT32_VALUE) {

            /*
             * To divide 64-bit numbers and to find remainders
             * on the x86 platform gcc and icc call the libc functions
             * [u]divdi3() and [u]moddi3(), they call another function
             * in its turn.  On FreeBSD it is the qdivrem() function,
             * its source code is about 170 lines of the code.
             * The glibc counterpart is about 150 lines of the code.
             *
             * For 32-bit numbers and some divisors gcc and icc use
             * a inlined multiplication and shifts.  For example,
             * unsigned "i32 / 10" is compiled to
             *
             *     (i32 * 0xCCCCCCCD) >> 35
             */

            ui32 = (uint32_t) ui64;

            do {
                *--p = (u_char) (ui32 % 10 + '0');
            } while (ui32 /= 10);

        } else {
            do {
                *--p = (u_char) (ui64 % 10 + '0');
            } while (ui64 /= 10);
        }

    } else if (hexadecimal == 1) {

        do {

            /* the "(uint32_t)" cast disables the BCC's warning */
            *--p = hex[(uint32_t) (ui64 & 0xf)];

        } while (ui64 >>= 4);

    } else { /* hexadecimal == 2 */

        do {

            /* the "(uint32_t)" cast disables the BCC's warning */
            *--p = HEX[(uint32_t) (ui64 & 0xf)];

        } while (ui64 >>= 4);
    }

    /* zero or space padding */

    len = (temp + EYE_INT64_LEN) - p;

    while (len++ < width && buf < last) {
        *buf++ = zero;
    }

    /* number safe copy */

    len = (temp + EYE_INT64_LEN) - p;

    if (buf + len > last) {
        len = last - buf;
    }

    return eye_cpymem(buf, p, len);
}


eye_int_t eye_strcasecmp(u_char *s1, u_char *s2)
{
	eye_uint_t c1, c2;

	for ( ;; ) {
		c1 = (eye_uint_t) *s1++;
		c2 = (eye_uint_t) *s2++;

		c1 = (c1 >= 'A' && c1 <= 'Z') ? (c1 | 0x20) : c1;
		c2 = (c2 >= 'A' && c2 <= 'Z') ? (c2 | 0x20) : c2;

		if (c1 == c2) {

			if (c1) {
				continue;
			}

			return 0;
		}

		return c1 - c2;
	}
}


eye_int_t eye_strncasecmp(u_char *s1, u_char *s2, size_t n)
{
	eye_uint_t c1, c2;

	while (n) {
		c1 = (eye_uint_t) *s1++;
		c2 = (eye_uint_t) *s2++;

		c1 = (c1 >= 'A' && c1 <= 'Z') ? (c1 | 0x20) : c1;
		c2 = (c2 >= 'A' && c2 <= 'Z') ? (c2 | 0x20) : c2;

		if (c1 == c2) {

			if (c1) {
				n--;
				continue;
			}

			return 0;
		}

		return c1 - c2;
	}

	return 0;
}


u_char *eye_strnstr(u_char *s1, char *s2, size_t len)
{
	u_char	c1, c2;
	size_t	n;

	c2 = *(u_char *) s2++;

	n = eye_strlen(s2);

	do {
		do {
			if (len-- == 0) {
				return NULL;
			}

			c1 = *s1++;

			if (c1 == 0) {
				return NULL;
			}

		} while (c1 != c2);

		if (n > len) {
			return NULL;
		}
	} while (eye_strncmp(s1, (u_char *) s2, n) != 0);

	return --s1;
}


u_char *eye_strstrn(u_char *s1, char *s2, size_t n)
{
	u_char	c1, c2;

	c2 = *(u_char *)s2++;

	do {
		do {
			c1 = *s1++;

			if (c1 == 0) {
				return NULL;
			}

		} while (c1 != c2);

	} while (eye_strncmp(s1, (u_char *) s1, n) != 0);

	return --s1;
}


u_char *eye_strcasestrn(u_char *s1, char *s2, size_t n)
{
	eye_uint_t	c1, c2;

	c2 = (eye_uint_t) *s2++;
	c2 = (c2 >= 'A' && c2 <= 'Z') ? (c2 | 0x20) : c2;

	do {
		do {
			c1 = (eye_uint_t) *s1++;

			if (c1 == 0) {
				return NULL;
			}

			c1 = (c1 >= 'A' && c1 <= 'Z') ? (c1 | 0x20) : c1;

		} while (c1 != c2);

	} while (eye_strncasecmp(s1, (u_char *) s2, n) != 0);

	return --s1;
}


u_char *eye_strlcasestrn(u_char *s1, u_char *last, u_char *s2, size_t n)
{
	eye_uint_t	c1, c2;

	c2 = (eye_uint_t) *s2++;
	c2 = (c2 >= 'A' && c2 <= 'Z') ? (c2 | 0x20) : c2;
	last -= n;

	do {
		do {
			if (s1 >= last) {
				return NULL;
			}

			c1 = (eye_uint_t) *s1++;

			c1 = (c1 >= 'A' && c1 <= 'Z') ? (c1 | 0x20) : c1;

		} while (c1 != c2);

	} while (eye_strncasecmp(s1, s2, n) != 0);

	return --s1;
}


eye_int_t eye_atoi(u_char *line, size_t n)
{
	eye_int_t value, cutoff, cutlim;

	if (n == 0) {
		return EYE_ERROR;
	}

	cutoff = EYE_MAX_INT_T_VALUE / 10;
	cutlim = EYE_MAX_INT_T_VALUE % 10;

	for (value = 0; n--; line++) {
		if (*line < '0' || *line > '9') {
			return EYE_ERROR;
		}

		if (value >= cutoff && (value > cutoff || *line - '0' > cutlim)) {
			return EYE_ERROR;
		}

		value = value * 10 + (*line - '0');
	}

	return value;
}


eye_int_t eye_atofp(u_char *line, size_t n, size_t point)
{
	eye_int_t	value, cutoff, cutlim;
	eye_uint_t	dot;

	if (n == 0) {
		return EYE_ERROR;
	}

	cutoff = EYE_MAX_INT_T_VALUE / 10;
	cutlim = EYE_MAX_INT_T_VALUE % 10;

	dot = 0;

	for (value = 0; n--; line++) {
		if (point == 0) {
			return EYE_ERROR;
		}

		if (*line == '.') {
			if (dot) {
				return EYE_ERROR;
			}

			dot = 1;
			continue;
		}

		if (*line < '0' || *line > '9') {
			return EYE_ERROR;
		}

		if (value >= cutoff && (value > cutoff || *line - '0' > cutlim)) {
			return EYE_ERROR;
		}

		value = value * 10 + (*line - '0');
		point -= dot;
	}

	while (point--) {
		if (value > cutoff) {
			return EYE_ERROR;
		}

		value = value * 10;
	}

	return value;
}


ssize_t eye_atosz(u_char *line, size_t n)
{
	ssize_t		value, cutoff, cutlim;

	if (n == 0) {
		return EYE_ERROR;
	}

	cutoff = EYE_MAX_SIZE_T_VALUE / 10;
	cutlim = EYE_MAX_SIZE_T_VALUE % 10;

	for (value = 0; n--; line++) {
		if (*line < '0' || *line > '9') {
			return EYE_ERROR;
		}

		if (value >= cutoff && (value > cutoff || *line - '0' > cutlim)) {
			return EYE_ERROR;
		}

		value = value * 10 + (*line - '0');
	}

	return value;
}


off_t eye_atoof(u_char *line, size_t n)
{
	off_t	value, cutoff, cutlim;

	if (n == 0) {
		return EYE_ERROR;
	} 

	cutoff = EYE_MAX_OFF_T_VALUE / 10;
	cutlim = EYE_MAX_OFF_T_VALUE % 10;

	for (value = 0; n--; line++) {
		if (*line < '0' || *line > '9') {
			return EYE_ERROR;
		}

		if (value >= cutoff && (value > cutoff || *line - '0' > cutlim)) {
			return EYE_ERROR;
		}

		value = value * 10 + (*line - '0');
	}

	return value;
}


time_t eye_atotm(u_char *line, size_t n)
{
	time_t	value, cutoff, cutlim;

	if (n == 0) {
		return EYE_ERROR;
	} 

	cutoff = EYE_MAX_TIME_T_VALUE / 10;
	cutlim = EYE_MAX_TIME_T_VALUE % 10;

	for (value = 0; n--; line++) {
		if (*line < '0' || *line > '9') {
			return EYE_ERROR;
		}

		if (value >= cutoff && (value > cutoff || *line - '0' > cutlim)) {
			return EYE_ERROR;
		}

		value = value * 10 + (*line - '0');
	}

	return value;
}


eye_int_t eye_hextoi(u_char *line, size_t n)
{
	u_char		c, ch;
	eye_int_t	value, cutoff;

	if (n == 0) {
		return EYE_ERROR;
	}

	cutoff = EYE_MAX_INT_T_VALUE / 16;

	for (value = 0; n--; line++) {
		if (value > cutoff) {
			return EYE_ERROR;
		}

		ch = *line;

		if (ch >= '0' && ch <= '9') {
			value = value * 16 + (ch - '0');
			continue;
		}

		c = (u_char) (ch | 0x20);

		if (c >= 'a' && c <= 'f') {
			value = value * 16 + (c - 'a' + 10);
			continue;
		}

		return EYE_ERROR;
	}

	return value;
}


u_char *eye_hex_dump(u_char *dst, u_char *src, size_t len)
{
    static u_char  hex[] = "0123456789abcdef";

    while (len--) {
        *dst++ = hex[*src >> 4];
        *dst++ = hex[*src++ & 0xf];
    }

    return dst;
}

