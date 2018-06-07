#pragma once

#include "MIPS.h"
#include "MemoryFunction.h"
#ifdef AOT_BUILD_CACHE
#include "StdStream.h"
#include <mutex>
#endif

struct AOT_BLOCK_KEY
{
	uint32 crc;
	uint32 begin;
	uint32 end;

	bool operator<(const AOT_BLOCK_KEY& k2) const
	{
		const auto& k1 = (*this);
		if(k1.crc == k2.crc)
		{
			if(k1.begin == k2.begin)
			{
				return k1.end < k2.end;
			}
			else
			{
				return k1.begin < k2.begin;
			}
		}
		else
		{
			return k1.crc < k2.crc;
		}
	}
};
static_assert(sizeof(AOT_BLOCK_KEY) == 0x0C, "AOT_BLOCK_KEY must be 12 bytes long.");

namespace Jitter
{
	class CJitter;
};

class CBasicBlock
{
public:
	CBasicBlock(CMIPS&, uint32 = MIPS_INVALID_PC, uint32 = MIPS_INVALID_PC);
	virtual ~CBasicBlock() = default;
	void Execute();
	void Compile();

	uint32 GetBeginAddress() const;
	uint32 GetEndAddress() const;
	bool IsCompiled() const;
	bool IsEmpty() const;

#ifdef AOT_BUILD_CACHE
	static void SetAotBlockOutputStream(Framework::CStdStream*);
#endif

protected:
	uint32 m_begin;
	uint32 m_end;
	CMIPS& m_context;

	virtual void CompileRange(CMipsJitter*);
	void CompileProlog(CMipsJitter*);

private:
	static void EmptyBlockHandler(CMIPS*);

#ifdef AOT_BUILD_CACHE
	static Framework::CStdStream* m_aotBlockOutputStream;
	static std::mutex m_aotBlockOutputStreamMutex;
#endif

#ifndef AOT_USE_CACHE
	CMemoryFunction m_function;
#else
	void (*m_function)(void*);
#endif
};
