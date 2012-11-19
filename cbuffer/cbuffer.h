/**
 * circle buffer implements
 *
 * bekars <bekars@gmail.com>
 *
 * History:
 *	090817	bekars	implement cbuffer put/get function.
 *
 */

#ifndef __CIRCLE_BUF_H__
#define __CIRCLE_BUF_H__ 1

#include <pthread.h>

struct cbuffer {
	unsigned char *buffer;		/* the buffer holding the data */
	unsigned int size;		/* the size of the allocated buffer */
	unsigned int in;		/* data is added at offset (in % size) */
	unsigned int out;		/* data is extracted from off. (out % size) */
	pthread_mutex_t lock;		/* protects concurrent modifications */
	pthread_cond_t cond_empty;	/* buffer empty condition */
	pthread_cond_t cond_full;	/* buffer full condition */
};

extern struct cbuffer *cbuffer_init(unsigned char *buffer, unsigned int size);
extern struct cbuffer *cbuffer_alloc(unsigned int size);
extern void cbuffer_free(struct cbuffer *cb);
extern unsigned int __cbuffer_put(struct cbuffer *cb, unsigned char *buffer, unsigned int len);
extern unsigned int __cbuffer_get(struct cbuffer *cb, unsigned char *buffer, unsigned int len);

/**
 * __cbuffer_reset - removes the entire buffer contents, no locking version
 * @cb: the circle buffer to be emptied.
 **/
static inline void 
__cbuffer_reset(struct cbuffer *cb)
{
	cb->in = cb->out = 0;
}

/**
 * cbuffer_reset - removes the entire buffer contents
 * @cb: the circle buffer to be emptied.
 **/
static inline void 
cbuffer_reset(struct cbuffer *cb)
{
	pthread_mutex_lock(&cb->lock);

	__cbuffer_reset(cb);

	pthread_mutex_unlock(&cb->lock);
}

/**
 * cbuffer_put - puts some data into the circle buffer
 * @cb: the circle buffer to be used.
 * @buffer: the data to be added.
 * @len: the length of the data to be added.
 **/
static inline unsigned int 
cbuffer_put(struct cbuffer *cb, unsigned char *buffer, unsigned int len)
{
	unsigned int ret;

	pthread_mutex_lock(&cb->lock);

	ret = __cbuffer_put(cb, buffer, len);

	pthread_mutex_unlock(&cb->lock);

	return ret;
}

/**
 * cbuffer_get - gets some data from the circle buffer
 * @cb: the fifo to be used.
 * @buffer: where the data must be copied.
 * @len: the size of the destination buffer.
 **/
static inline unsigned int 
cbuffer_get(struct cbuffer *cb, unsigned char *buffer, unsigned int len)
{
	unsigned int ret;

	pthread_mutex_lock(&cb->lock);

	ret = __cbuffer_get(cb, buffer, len);

	/**
	 * optimization: if the FIFO is empty, set the indices to 0
	 * so we don't wrap the next time
	 **/
	if (cb->in == cb->out) {
		cb->in = cb->out = 0;
	}

	pthread_mutex_unlock(&cb->lock);

	return ret;
}

/**
 * __cbuffer_len - returns the number of bytes available in the circle buffer, no locking version
 * @cb: the circle buffer to be used.
 **/
static inline unsigned int 
__cbuffer_len(struct cbuffer *cb)
{
	return cb->in - cb->out;
}

/**
 * cbuffer_len - returns the number of bytes available in the circle buffer 
 * @cb: the circle buffer to be used.
 **/
static inline unsigned int 
cbuffer_len(struct cbuffer *cb)
{
	unsigned int ret;

	pthread_mutex_lock(&cb->lock);

	ret = __cbuffer_len(cb);

	pthread_mutex_unlock(&cb->lock);

	return ret;
}

#endif /* __CIRCLE_BUF_H__ */

