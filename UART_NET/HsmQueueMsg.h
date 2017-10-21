#if !defined(_HsmQueueMsg_h)
#define _HsmQueueMsg_h
#if defined(__cplusplus)
extern "C" {
#endif
	typedef struct _SHsmQueueMsg {
		struct _SHsmQueueMsg *next, *prev;
		void *context, *sender, *recipient;
		uint32_t msg;
	} SHsmQueueMsg, *PSHsmQueueMsg;

	PSHsmQueueMsg HsmQueueMsg_New();
	void HsmQueueMsg_Delete(PSHsmQueueMsg *p);

	void HsmQueueMsg_Insert(PSHsmQueueMsg anchor, PSHsmQueueMsg msg);
	PSHsmQueueMsg HsmQueueMsg_FindRecipient(PSHsmQueueMsg anchor, const void* recipient);
	void HsmQueueMsg_Remove(PSHsmQueueMsg msg);
	void HsmQueueMsg_Reset(PSHsmQueueMsg msg);

#if defined(__cplusplus)
}
#endif
#endif