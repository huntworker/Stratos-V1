//
//RingBuffer.h
//

#ifndef __RINGBUFFER_H__
# define __RINGBUFFER_H__

#include <stdint.h>

# define BUFFER_K               8
# define BUFFER_DIM             (1U << BUFFER_K)
# define BUFFER_MASK            (BUFFER_DIM - 1U)

typedef struct
	{
  char chBuffer[BUFFER_DIM];
  uint32_t writePosition;
  uint32_t readPosition;
} RingBuffer_t;

typedef enum
{
	NO_ERROR,
	BUFFER_EMPTY,
	BUFFER_FULL
} RingBufferError_t;

void InitRingBuffer (RingBuffer_t * ptr2Buffer);

RingBufferError_t RingBufferRead (RingBuffer_t * ptr2Buffer, char * ptr2Chr);

RingBufferError_t RingBufferWrite(RingBuffer_t * ptr2Buffer, char   theChr);

uint32_t GetRingBufferCurrentFillState(RingBuffer_t const * ptr2Buffer);

#endif
