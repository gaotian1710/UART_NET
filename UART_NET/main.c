#include "stdafx.h"
#include "helper.h"
#include "HsmQueueMsg.h"
#include "hsm.h"
#include "OvenToaster.h"

#define TRY_NEW(ptr,msg) \
if (NULL == ptr) { \
	Logger_Code(logger, UN_ERROR_NOT_ENOUGH_MEMORY, msg); break; \
}

#define TRY_NEW_ARRAY(ptr_array,new_method,msg) \
{ \
	for (int __array_index = 0; __array_index < ARRAY_DIM(ptr_array); __array_index++) {\
		TRY_NEW((ptr_array[__array_index] = new_method()),msg); \
	} \
	if (logger->code) break; \
}


int main(int argc, const char* *argv)
{
	UN_ERROR err = UN_ERROR_SUCCESS;
	do {
		err = InitGlobal(LOGFILENAME);
		Logger_Info(logger, (uint8_t*)"main() started.");

		DisplayState(hsmToaster);

		printf("\n--- Hsm_HandleEvent(EvMsg_Enter) = 0x%08x\n",
			err = Hsm_HandleEvent(&events[EvMsg_Enter], hsmToaster));
		DisplayState(hsmToaster);

		printf("\n--- Hsm_HandleEvent(EvMsg_OpenDoor) = 0x%08x\n",
			err = Hsm_HandleEvent(&events[EvMsg_OpenDoor], hsmToaster));
		DisplayState(hsmToaster);

		printf("\n--- Hsm_HandleEvent(EvMsg_CloseDoor) = 0x%08x\n",
			err = Hsm_HandleEvent(&events[EvMsg_CloseDoor], hsmToaster));
		DisplayState(hsmToaster);

		events[EvMsg_SetupStart].context = (void*)7;
		printf("\n--- Hsm_HandleEvent(EvMsg_SetupStart) = 0x%08x\n",
			err = Hsm_HandleEvent(&events[EvMsg_SetupStart], hsmToaster));
		DisplayState(hsmToaster);

		for (int i = 0; i < 15; i++)
		{
			Sleep(1000);
			if (i == 4)
			{
				printf("\n--- Hsm_HandleEvent(EvMsg_OpenDoor) = 0x%08x\n",
					err = Hsm_HandleEvent(&events[EvMsg_OpenDoor], hsmToaster));
			}
			else if (i == 7)
			{
				printf("\n--- Hsm_HandleEvent(EvMsg_CloseDoor) = 0x%08x\n",
					err = Hsm_HandleEvent(&events[EvMsg_CloseDoor], hsmToaster));
			}
			DisplayState(hsmToaster);
		}


	} while (false);
	Logger_Info(logger, (uint8_t*)"main() exits.");
	CleanupGlobal();
	return (int)err;
}