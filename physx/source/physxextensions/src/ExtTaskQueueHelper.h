// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2008-2025 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#ifndef EXT_TASK_QUEUE_HELPER_H
#define EXT_TASK_QUEUE_HELPER_H

#include "task/PxTask.h"
#include "ExtSharedQueueEntryPool.h"

namespace physx
{

#define EXT_TASK_QUEUE_ENTRY_POOL_SIZE 128
#define EXT_TASK_QUEUE_ENTRY_HIGH_PRIORITY_POOL_SIZE 32

namespace Ext
{
	class TaskQueueHelper
	{
		SharedQueueEntryPool<>	mQueueEntryPool;
		PxSList					mJobList;

		SharedQueueEntryPool<>	mHighPriorityQueueEntryPool;
		PxSList					mHighPriorityJobList;

	public:

		TaskQueueHelper() : mQueueEntryPool(EXT_TASK_QUEUE_ENTRY_POOL_SIZE, "QueueEntryPool"),
							mHighPriorityQueueEntryPool(EXT_TASK_QUEUE_ENTRY_HIGH_PRIORITY_POOL_SIZE, "HighPriorityQueueEntryPool")
		{}

		PX_FORCE_INLINE	bool	tryAcceptJobToQueue(PxBaseTask& task)
		{
			if(task.isHighPriority())
			{
				SharedQueueEntry* entry = mHighPriorityQueueEntryPool.getEntry(&task);
				if(entry)
				{
					mHighPriorityJobList.push(*entry);
					return true;
				}
			}

			SharedQueueEntry* entry = mQueueEntryPool.getEntry(&task);
			if(entry)
			{
				mJobList.push(*entry);
				return true;
			}
			else
			{
				return false;	// PT: we never actually reach this
			}
		}

		template<const bool highPriorityT>
		PxBaseTask* fetchTask()
		{
			SharedQueueEntry* entry = highPriorityT ? static_cast<SharedQueueEntry*>(mHighPriorityJobList.pop()) : static_cast<SharedQueueEntry*>(mJobList.pop());
			if(entry)
			{
				PxBaseTask* task = reinterpret_cast<PxBaseTask*>(entry->mObjectRef);
				if(highPriorityT)
					mHighPriorityQueueEntryPool.putEntry(*entry);
				else
					mQueueEntryPool.putEntry(*entry);
				return task;
			}
			return NULL;
		}
	};

} // namespace Ext

}

#endif
