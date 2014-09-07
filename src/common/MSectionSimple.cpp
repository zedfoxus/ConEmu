﻿
/*
Copyright (c) 2009-2014 Maximus5
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the authors may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//#ifdef _DEBUG
//#define USE_LOCK_SECTION
//#endif

#define HIDE_USE_EXCEPTION_INFO
#include <windows.h>
#include "defines.h"
#include "MAssert.h"
#include "MSectionSimple.h"

MSectionLockSimple::MSectionLockSimple()
{
	mp_S = NULL;
	mb_Locked = false;
	#ifdef _DEBUG
	mn_LockTID = mn_LockTick = 0;
	#endif
}

MSectionLockSimple::~MSectionLockSimple()
{
	if (mb_Locked)
	{
		Unlock();
	}
}

BOOL MSectionLockSimple::Lock(CRITICAL_SECTION* apS, DWORD anTimeout/*=-1*/)
{
	if (mb_Locked && (mp_S != apS))
		Unlock();

	mp_S = apS;

	if (!mp_S)
	{
		_ASSERTEX(apS);
		return FALSE;
	}

	_ASSERTEX(!mb_Locked);

	bool bLocked = false;
	DWORD nStartTick = GetTickCount();
	DWORD nDelta;
#if 0
	EnterCriticalSection(apS);

	bLocked = mb_Locked = true;

	nDelta = GetTickCount() - nStartTick;
	if (nDelta >= anTimeout)
	{
		_ASSERTEX(FALSE && "Failed to lock CriticalSection, timeout");
	}
#else
	while (true)
	{
		if (TryEnterCriticalSection(apS))
		{
			bLocked = mb_Locked = true;
			#ifdef _DEBUG
			mn_LockTID = GetCurrentThreadId();
			mn_LockTick = GetTickCount();
			#endif
			break;
		}

		if (anTimeout != (DWORD)-1)
		{
			nDelta = GetTickCount() - nStartTick;
			if (nDelta >= anTimeout)
			{
				_ASSERTEX(FALSE && "Failed to lock CriticalSection, timeout");
				break;
			}
		}

		Sleep(1);
	}
#endif

	return bLocked;
}

void MSectionLockSimple::Unlock()
{
	if (mb_Locked)
	{
		if (mp_S)
			LeaveCriticalSection(mp_S);
		mb_Locked = false;
	}
}

BOOL MSectionLockSimple::isLocked()
{
	return mb_Locked;
}
