/* Poly1305 implementation written by Andrew Moon,
          https://github.com/floodyberry/poly1305-donna
   License: MIT or public domain.
   Minor adaptations for Cryptokit by Xavier Leroy. */

#include "poly1305-donna.h"

/* auto detect between 32bit / 64bit */
#if defined(__SIZEOF_INT128__) && defined(__LP64__)
#include "poly1305-donna-64.h"
#else
#include "poly1305-donna-32.h"
#endif

void
poly1305_update(poly1305_context *ctx, const unsigned char *m, size_t bytes) {
	poly1305_state_internal_t *st = (poly1305_state_internal_t *)ctx;
	size_t i;

	/* handle leftover */
	if (st->leftover) {
		size_t want = (poly1305_block_size - st->leftover);
		if (want > bytes)
			want = bytes;
		for (i = 0; i < want; i++)
			st->buffer[st->leftover + i] = m[i];
		bytes -= want;
		m += want;
		st->leftover += want;
		if (st->leftover < poly1305_block_size)
			return;
		poly1305_blocks(st, st->buffer, poly1305_block_size);
		st->leftover = 0;
	}

	/* process full blocks */
	if (bytes >= poly1305_block_size) {
		size_t want = (bytes & ~(poly1305_block_size - 1));
		poly1305_blocks(st, m, want);
		m += want;
		bytes -= want;
	}

	/* store leftover */
	if (bytes) {
		for (i = 0; i < bytes; i++)
			st->buffer[st->leftover + i] = m[i];
		st->leftover += bytes;
	}
}