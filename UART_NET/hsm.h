#if !defined(_hsm_h)
#define _hsm_h
#if defined(__cplusplus)
extern "C" {
#endif
	struct _SHsm;
	struct _SHsmState;
	struct _SHsmCmd;

	typedef enum {
		Hsm_stay,
		Hsm_child,
		Hsm_parent,
		Hsm_sibling,
		Hsm_error
	} HsmTransitionType, *PHsmTransitionType;

	/*!
	\brief definition of function pointer to HSM action handler for enter_ or exit_
	*/
	typedef void (*HsmActionHandler)(struct _SHsm *hsm);

	/*!
	\brief definition of function pointer to HSM state handler
	*/
	typedef struct _SHsmCmd (*HsmStateHandler)(struct _SHsm *hsm);

	typedef struct _SHsmState {
		HsmActionHandler enter_;
		HsmActionHandler exit_;
		HsmStateHandler state_;
		LPCSTR name;
	} SHsmState, *PSHsmState;

	typedef const SHsmState *PCSHsmState; // const object of SHsmState

	typedef struct _SHsmCmd {
		PCSHsmState nextState;
		HsmTransitionType tt;
	} SHsmCmd, *PSHsmCmd;

	typedef const SHsmCmd *PCSHsmCmd;

	/*! \struct SHsm
	\brief hierarchical state machine object
	*/
	typedef struct _SHsm {
		PSHsmQueueMsg rxMsg;
		void* context;
		CRITICAL_SECTION cs;
		uint8_t size;
		int8_t  current;
		PCSHsmState stack[0];
	} SHsm, *PSHsm;

	PSHsm Hsm_New(uint8_t stackSize);
	void Hsm_Delete(PSHsm *p);
	void Hsm_Lock(PSHsm p);
	void Hsm_Unlock(PSHsm p);
	UN_ERROR Hsm_Push(PSHsm p, PCSHsmState state);
	PCSHsmState Hsm_Pop(PSHsm p); /*! \< return null or stack top */
	

	UN_ERROR Hsm_HandleEvent(const PSHsmQueueMsg msg, PSHsm hsm);
#if defined(__cplusplus)
}
#endif
#endif