#include "stdafx.h"
#include "HsmQueueMsg.h"
#include "hsm.h"


PSHsmState State_New()
{
	return (PSHsmState)UN_Alloc(sizeof(SHsmState));
}

void State_Delete(PSHsmState * p)
{
	if (*p)
	{
		UN_Free(*p);
		*p = NULL;
	}
}

PSHsm Hsm_New(uint8_t stackSize)
{
	PSHsm p = UN_Alloc(sizeof(SHsm) + sizeof(SHsmState) * stackSize);
	InitializeCriticalSection(&(p->cs));
	p->context = NULL;
	p->rxMsg = NULL;
	p->current = -1;
	p->size = stackSize;
	return p;
}

void Hsm_Delete(PSHsm * p)
{
	if (*p)
	{
		DeleteCriticalSection(&((*p)->cs));
		UN_Free(*p);
		*p = NULL;
	}
}

void Hsm_Lock(PSHsm p)
{
	EnterCriticalSection(&(p->cs));
}

void Hsm_Unlock(PSHsm p)
{
	LeaveCriticalSection(&(p->cs));
}

UN_ERROR Hsm_Push(PSHsm p, PCSHsmState state)
{
	// check if the stack pointer points the stack top and return error.
	if ((p->size - 1) == (uint8_t)(p->current)) return UN_ERROR_INSUFFICIENT_BUFFER;

	// if the stack has more space, ...
	p->stack[++(p->current)] = state;
	return UN_ERROR_SUCCESS;
}

PCSHsmState Hsm_Pop(PSHsm p)
{
	if (p->current >= 0) return p->stack[p->current--];
	else return NULL;
}

UN_ERROR Hsm_HandleEvent(const PSHsmQueueMsg msg, PSHsm hsm)
{
	SHsmCmd cmd;
	UN_ERROR result = Hsm_stay;
	hsm->rxMsg = msg;
	do {
		cmd = hsm->stack[hsm->current]->state_(hsm);
		//printf("SH result: tt=0x%08x, next state=%s\n", cmd.tt, cmd.nextState ? cmd.nextState->name : "(null)");
		switch (cmd.tt)
		{
		case Hsm_child:
		{
			if (Hsm_Push(hsm, cmd.nextState))
			{
				// error handler
				result = (UN_ERROR)Hsm_error;
			}
			else
			{
				hsm->stack[hsm->current]->enter_(hsm);
			}
			break;
		}
		case Hsm_parent:
		{
			hsm->stack[hsm->current]->exit_(hsm);
			if (Hsm_Pop(hsm) == NULL)
			{
				// error handler
				result = (UN_ERROR)Hsm_error;
			}
			break;
		}
		case Hsm_sibling:
		{
			hsm->stack[hsm->current]->exit_(hsm);
			if (Hsm_Pop(hsm) == NULL)
			{
				// error handler
				result = (UN_ERROR)Hsm_error;
			}
			else
			{
				if (Hsm_Push(hsm, cmd.nextState))
				{
					// error handler
					result = (UN_ERROR)Hsm_error;
				}
				else
				{
					hsm->stack[hsm->current]->enter_(hsm);
				}
			}
			break;
		}
		default:
			break;
		}
	} while (cmd.tt != Hsm_stay && cmd.tt != Hsm_error);
	//printf("Hsm_HandleEvent() exiting with result = 0x%08x\n", result);
	return result;
}




