//
//RingBuffer.c
//

#include "RingBuffer.h"

//#define FILL_STATE(P) ((P)->writePosition - (P)->readPosition))

static inline uint32_t FillState(RingBuffer_t const * const ptr2Buffer)
{
  return ptr2Buffer->writePosition - ptr2Buffer->readPosition;
}

void InitRingBuffer (RingBuffer_t * ptr2Buffer)
{
  ptr2Buffer->writePosition = 0;
  ptr2Buffer->readPosition = 0;
}

RingBufferError_t RingBufferRead(RingBuffer_t * const ptr2Buffer, char * ptr2Chr)
{
  if ( FillState(ptr2Buffer) == 0U )
  {
    return BUFFER_EMPTY;
  } else
  {
    *ptr2Chr = ptr2Buffer->chBuffer[ptr2Buffer->readPosition++ & BUFFER_MASK];
    return NO_ERROR;
  }
}

RingBufferError_t RingBufferWrite(RingBuffer_t * const ptr2Buffer, char const theChr)
{
  if( FillState(ptr2Buffer) < BUFFER_DIM )
  {
    ptr2Buffer->chBuffer[ptr2Buffer->writePosition++ & BUFFER_MASK] = theChr;

    return BUFFER_FULL;
  } else
  {
    return NO_ERROR;
  }
}

uint32_t GetRingBufferCurrentFillState(RingBuffer_t const * const ptr2Buffer)
{
  return FillState(ptr2Buffer);
}

