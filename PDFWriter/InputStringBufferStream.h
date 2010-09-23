#pragma once

#include "IByteReader.h"

#include <sstream>
#include <string>

using namespace std;

class InputStringBufferStream : public IByteReader
{
public:
	InputStringBufferStream(stringbuf* inBufferToReadFrom);
	~InputStringBufferStream(void);

	// IByteReader implementation
	virtual LongBufferSizeType Read(Byte* inBuffer,LongBufferSizeType inBufferSize);
	virtual bool NotEnded();
	virtual void Skip(LongBufferSizeType inSkipSize);
	virtual void SetPosition(LongFilePositionType inOffsetFromStart);

private:
	stringbuf* mBufferToReadFrom;


};
