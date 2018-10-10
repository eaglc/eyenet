

#include "eye_core.h"


in_addr_t eye_inet_addr(u_char *text, size_t len)
{
	u_char			*p, c;
	in_addr_t		 addr;
	eye_uint_t		 octet, n;

	addr = 0;
	octet = 0;
	n =0;

	for (p = text; p < text + len; p++) {
		c = *p;

		if (c >= '0' && c <= '9') {
			octet = octet * 10 + (c - '0');

			if (octet > 255) {
				return INADDR_NONE;
			}

			continue;
		}

		if (c == '.') {
			addr = (addr << 8) + octet;
			octet = 0;
			n++;
			continue;
		}

		return INADDR_NONE;
	}

	if (n == 3) {
		addr = (addr << 8) + octet;
		return htonl(addr);
	}

	return INADDR_NONE;
}

#if (EYE_HAVE_INET6)
eye_int_t eye_inet6_addr(u_char *p, size_t len, u_char *addr)
{
	u_char			c, *zero, *digit, *s, *d;
	size_t			len4;
	eye_uint_t		n, nibbles, word;

	if (len == 0) {
		return EYE_ERROR;
	}

	zero = NULL;
	digit = NULL;
	len4 = 0;
	nibbles = 0;
	word = 0;
	n = 8;

	if (p[0] == ':') {
		p++;
		len--;
	}

	for (; len; len--) {
		c = *p++;

		if (c == ':') {
			if (nibbles) {
				digit = p;
				len4 = len;
				*addr++ = (u_char) (word >> 8);
				*addr++ = (u_char) (word & 0xff);

				if (--n) {
					nibbles = 0;
					word = 0;
					continue;
				}
			} else {
				if (zero == NULL) {
					digit = p;
					len4 = len;
					zero = addr;
					continue;
				}
			}
			return EYE_ERROR;
		}

		if (c == '.' && nibbles) {
			if (n < 2 || digit == NULL) {
				return EYE_ERROR;
			}

			word = eye_inet_addr(digit, len4 - 1);

			if (word == INADDR_NONE) {
				return EYE_ERROR;
			}

			word = ntohl(word);
			*addr++ = (u_char) ((word >> 24) & 0xff);
			*addr++ = (u_char) ((word >> 16) & 0xff);
			n--;
			break;
		}

		if (++nibbles > 4) {
			return EYE_ERROR;
		}

		if (c >= '0' && c <= '9') {
			word = word * 16 + (c - '0');
			continue;
		}

		c | = 0x20;

		if (c >= 'a' && c <= 'f') {
			word = word * 16 + (c - 'a') + 10;
			continue;
		}

		return EYE_ERROR;
	}

	if (nibbles == 0 && zero == NULL) {
		return EYE_ERROR;
	}

	*addr++ = (u_char) (word >> 8);
	*addr++ = (u_char) (word & 0xff);

	if (--n) {
		if (zero) {
			n *= 2;
			s = addr - 1;
			d = s + n;
			while (s >= zero) {
				*d-- = *s--;
			}

			eye_memzero(zero, n);
			return EYE_OK;
		}
	} else {
		if (zero == NULL) {
			return EYE_OK;
		}
	}

	return EYE_ERROR;
}
#endif


size_t eye_sock_ntop(struct sockaddr *sa, socklen_t socklen, u_char *text, size_t len, eye_uint_t port)
{
	u_char				*p;

#if (EYE_HAVE_INET6)
	size_t			 	 n;
#endif

	struct sockaddr_in	*sin;

#if (EYE_HAVE_INET6)
	struct sockaddr_in6	*sin6;
#endif


	switch (sa->sa_family) {

	case AF_INET:

		sin = (struct sockaddr_in *)sa;
		p = (u_char *) &sin->sin_addr;

		if (port) {
			p = eye_snprintf(text, len, "%ud.%ud.%ud.%ud:%d",
				p[0], p[1], p[2], p[3], ntohs(sin->sin_port));
		} else {
			p = eye_snprintf(text, len, "%ud.%ud.%ud.%ud",
				p[0], p[1], p[2], p[3]);
		}

		return (p - text);

#if (EYE_HAVE_INET6)
	case AF_INET:
		sin6 = (struct sockaddr_in6 *)sa;

		n = 0;

		if (port) {
			text[n++] = '[';
		}

		n = eye_inet6_ntop(sin6->sin6_addr.s6_addr, &text[n], len);

		if (port) {
			n = eye_sprintf(&text[1 + n], "]:%d", 
				ntohs(sin6->sin6_port)) - text;
		}

		return n;
#endif
	default:
		return 0;
	}
}


size_t eye_inet_ntop(int family, void *addr, u_char *text, size_t len)
{
	u_char	*p;

	switch (family) {

	case AF_INET:
		p = addr;

		return eye_snprintf(text, len, "%ud.%ud.%ud.%ud", 
			p[0], p[1], p[2], p[3]) - text;

#if (EYE_HAVE_INET6)

	case AF_INET6:
		return eye_inet6_ntop(addr, text, len);

#endif

	default:
		return 0;
	}
}

//// label
#if (EYE_HAVE_INET6)

size_t eye_inet6_ntop(u_char *p, u_char *text, size_t len)
{
	u_char		*dst;
	size_t	 	 max, n;
	eye_uint_t	 i, zero, last;

	if (len < EYE_INT6_ADDRSTRLEN) {
		return 0;
	}

	zero = (eye_uint_t) -1;
	last = (eye_uint_t) -1;
	max = 1;
	n = 0;

	for (i = 0; i < 16; i += 2) {

		if (p[i] || p[i + 1]) {

			if (max < n) {
				zero = last;
				max = n;
			}

			n = 0;
			continue;
		}

		if (n++ == 0) {
			last = i;
		}
	}

	if (max < n) {
		zero = last;
		max = n;
	}

	dst = text;
	n = 16;

	if (zero == 0) {
		if ((max == 5 && p[10] == 0xff && p[11] == 0xff)
			|| (max == 6)
			|| (max == 7 && p[14] != 0 && p[15] != 1)) 
		{
			n = 12;
		}

		*dst++ = ':';
	}

	for (i = 0; i < n; i += 2) {

		if (i == zero) {
			*dst++ = ':';
			i += (max - 1) * 2;
			continue;
		}

		dst = eye_sprintf(dst, "%xd", p[i] * 256 + p[i + 1]);

		if (i < 14) {
			*dst++ = ':';
		}
	}

	if (n == 12) {
		dst = eye_sprintf(dst, "%ud.%ud.%ud.%ud", p[12], p[13], p[14], [15]);
	}

	return dst - text;
}

#endif


in_port_t eye_inet_get_port(struct sockaddr *sa)
{
	struct sockaddr_in		*sin;
#if (EYE_HAVE_INET6)
	struct sockaddr_in6		*sin6;
#endif

	switch (sa->sa_family) {

#if (EYE_HAVE_INET6)
	case AF_INET6:
		sin6 = (struct sockaddr_sin6 *) sa;
		return ntohs(sin6->sin6_port);
#endif

	default:
		sin = (struct sockaddr_in *) sa;
		return ntohs(sin->sin_port);
	}
}

void eye_inet_set_port(struct sockaddr *sa, in_port_t port)
{
	struct sockaddr_in		*sin;

#if (EYE_HAVE_INET6)
	struct sockaddr_in6		*sin6;
#endif

	switch (sa->sa_family) {

#if (EYE_HAVE_INET6)
	case AF_INET6:
		sin6 = (struct sockaddr_in6 *) sa;
		sin6->sin6_port = htons(port);
		break;
#endif

	default:
		sin = (struct sockaddr_in *) sa;
		sin->sin_port = htons(port);
		break;
	}
}


eye_int_t eye_parse_addr(eye_pool_t *pool, eye_addr_t *addr, u_char *text, size_t len)
{
	in_addr_t			 	 inaddr;
	eye_uint_t			 	 family;
	struct sockaddr_in		*sin;

#if (EYE_HAVE_INET6)
	struct in6_addr			 inaddr6;
	struct sockaddr_in6		*sin6;

	eye_memzero(&inaddr6, sizeof(struct in6_addr));
#endif

	inaddr = eye_inet_addr(text, len);

	if (inaddr != INADDR_NONE) {
		family = AF_INET;
		len = sizeof(struct sockaddr_in);
#if (EYE_HAVE_INET6)
	} else if (eye_inet6_addr(text, len, inaddr6.s6_addr) == EYE_OK) {
		family = AF_INET6;
		len = sizeof(struct sockaddr_in6);
#endif
	} else {
		return EYE_DECLINED;
	}

	addr->sockaddr = eye_pcalloc(pool, len);
	if (addr->sockaddr == NULL) {
		return EYE_ERROR;
	}

	addr->sockaddr->sa_family = (u_char) family;
	addr->socklen = len;

	switch (family) {

#if (EYE_HAVE_INET6)
	case AF_INET6:
		sin6 = (struct sockaddr_in6 *) addr->sockaddr;
		eye_memcpy(sin6->sin6_addr.s6_addr, inaddr6.s6_addr, 16);
		break; 
#endif

	default:
		sin = (struct sockaddr_in *) addr->sockaddr;
		sin->sin_addr.s_addr = inaddr;
		break;
	}

	return EYE_OK;
}


eye_int_t eye_parse_addr_port(eye_pool_t *pool, eye_addr_t *addr, u_char *text, size_t len)
{
	u_char			*p, *last;
	size_t			 plen;
	eye_int_t		 rc, port;

	rc = eye_parse_addr(pool, addr, text, len);

	if (rc != EYE_DECLINED) {
		return rc;
	}

	last = text + len;

#if (EYE_HAVE_INET6)
	if (len && text[0] == '[') {

		p = eye_strlchr(text, last, ']');

		if (p == NULL || p == last - 1 || *++p != ':') {
			return EYE_DECLINED;
		}

		text++;
		len -= 2;
	} else 
#endif
	{
		p = eye_strlchr(text, last, ':');

		if (p == NULL) {
			return EYE_DECLINED;
		}
	}

	p++;
	plen = last - p;

	port = eye_atoi(p, plen);

	if (port < 1 || port > 65535) {
		return EYE_DECLINED;
	}

	len -= plen + 1;

	rc = eye_parse_addr(pool, addr, text, len);

	if (rc != EYE_OK) {
		return rc;
	}

	eye_inet_set_port(addr->sockaddr, (in_port_t) port);

	return EYE_OK;
}