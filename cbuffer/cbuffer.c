/**
 * circle buffer implements
 *
 * bekars <bekars@gmail.com>
 *
 * History:
 *	090817	bekars	implement cbuffer put/get function.
 *
 */

#include "cbuffer.h"
#include "public.h"

/**
 * cbuffer_init - allocates a new circle buffer using a preallocated buffer
 * @buffer: the preallocated buffer to be used.
 * @size: the size of the internal buffer, this have to be a power of 2.
 *
 */
struct cbuffer *
cbuffer_init(unsigned char *buffer, unsigned int size)
{
	struct cbuffer *cb;

	/* size must be a power of 2 */
	assert(!(size & (size - 1)));

	cb = MALLOC(sizeof(struct cbuffer));
	if (!cb)
		return NULL;

	cb->buffer = buffer;
	cb->size = size;
	cb->in = cb->out = 0;
	pthread_mutex_init(&cb->lock, NULL);
	pthread_cond_init(&cb->cond_empty, NULL);
	pthread_cond_init(&cb->cond_full, NULL);

	return cb;
}

/**
 * cbuffer_alloc - allocates a new circle buffer and its internal buffer
 * @size: the size of the internal buffer to be allocated.
 *
 */
struct cbuffer *
cbuffer_alloc(unsigned int size)
{
	unsigned char *buffer;
	struct cbuffer *cb;

	buffer = MALLOC(size);
	if (!buffer)
		return NULL;

	cb = cbuffer_init(buffer, size);

	if (!cb) {
		FREE(buffer, size);
	}

	return cb;
}

/**
 * cbuffer_free - frees the circle buffer 
 * @cb: the circle buffer to be freed.
 */
void 
cbuffer_free(struct cbuffer *cb)
{
	pthread_mutex_destroy(&cb->lock);
	pthread_cond_destroy(&cb->cond_empty);
	pthread_cond_destroy(&cb->cond_full);

	FREE(cb->buffer, cb->size);
	FREE(cb, sizeof(struct cbuffer));
}

/**
 * __cbuffer_put - puts some data into the circle buffer, no locking version
 * @cb: the fifo to be used.
 * @buffer: the data to be added.
 * @len: the length of the data to be added.
 *
 */
unsigned int 
__cbuffer_put(struct cbuffer *cb, unsigned char *buffer, unsigned int len)
{
	unsigned int ll;
	unsigned int clen;

	clen = min(len, cb->size - cb->in + cb->out);

	while (!clen) {
		pthread_cond_wait(&cb->cond_empty, &cb->lock);
		clen = min(len, cb->size - cb->in + cb->out);
	}

	/* first put the data starting from fifo->in to buffer end */
	ll = min(clen, cb->size - (cb->in & (cb->size - 1)));
	memcpy(cb->buffer + (cb->in & (cb->size - 1)), buffer, ll);

	/* then put the rest (if any) at the beginning of the buffer */
	memcpy(cb->buffer, buffer + ll, clen - ll);

	cb->in += clen;

	pthread_cond_signal(&cb->cond_full);

	return clen;
}

/**
 * __cbuffer_get - gets some data from the circle buffer, no locking version
 * @fifo: the fifo to be used.
 * @buffer: where the data must be copied.
 * @len: the size of the destination buffer.
 *
 */
unsigned int 
__cbuffer_get(struct cbuffer *cb, unsigned char *buffer, unsigned int len)
{
	unsigned int ll;
	unsigned int clen;

	clen = min(len, cb->in - cb->out);

	while (!clen) {
		pthread_cond_wait(&cb->cond_full, &cb->lock);
		clen = min(len, cb->in - cb->out);
	}

	/* first get the data from fifo->out until the end of the buffer */
	ll = min(clen, cb->size - (cb->out & (cb->size - 1)));
	memcpy(buffer, cb->buffer + (cb->out & (cb->size - 1)), ll);

	/* then get the rest (if any) from the beginning of the buffer */
	memcpy(buffer + ll, cb->buffer, clen - ll);

	cb->out += clen;

	pthread_cond_signal(&cb->cond_empty);

	return clen;
}

