#include "stdafx.h"
#include "HsmQueueMsg.h"

PSHsmQueueMsg HsmQueueMsg_New()
{
	PSHsmQueueMsg p = (PSHsmQueueMsg)UN_Alloc(sizeof(SHsmQueueMsg));
	p->next = p->prev = p;
	p->context = p->sender = p->recipient = NULL;
	p->msg = 0;
	return p;
}

void HsmQueueMsg_Delete(PSHsmQueueMsg * p)
{
	if (*p)
	{
		UN_Free(*p);
		*p = NULL;
	}
}

void HsmQueueMsg_Insert(PSHsmQueueMsg anchor, PSHsmQueueMsg msg)
{
	PSHsmQueueMsg current1st = anchor->next;
	msg->prev = anchor;
	msg->next = current1st;
	current1st->prev = msg;
	anchor->next = msg;
}

PSHsmQueueMsg HsmQueueMsg_FindRecipient(PSHsmQueueMsg anchor, const void * recipient)
{
	PSHsmQueueMsg found = NULL, scanner = anchor->next;
	for (; scanner != anchor; scanner = scanner->next)
	{
		if (scanner->recipient == recipient)
		{
			found = scanner;
			break;
		}
	}
	return found;
}

void HsmQueueMsg_Remove(PSHsmQueueMsg msg)
{
	PSHsmQueueMsg prev = msg->prev, next = msg->next;
	prev->next = next;
	next->prev = prev;
	msg->next = msg->prev = NULL;
}

void HsmQueueMsg_Reset(PSHsmQueueMsg msg)
{
	msg->next = msg->prev = msg;
	msg->context = msg->sender = msg->recipient = NULL;
}

