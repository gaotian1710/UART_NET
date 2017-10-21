#if !defined(_OvenToaster_h)
#define _OvenToaster_h
#define LOGFILENAME "OVENTOASTER.log"
typedef enum {
	DoorState_Open,
	DoorState_Close
} DoorState;

typedef enum {
	HeaterState_On,
	HeaterState_Off
} HeaterState;

typedef enum {
	TimerState_On,
	TimerState_Off
} TimerState;

typedef struct {
	DoorState ds;
	HeaterState hs;
	TimerState ts;
	uint32_t timerPeriod, elapsedTime;
} SOvenToasterContext, *PSOvenToasterContext;

typedef enum {
	EvMsg_Enter = 0,
	EvMsg_OpenDoor,
	EvMsg_CloseDoor,
	EvMsg_SetupStart,
	EvMsg_ClearSetup,
	EvMsg_TimerTick
} EvMsg;

typedef struct {
	HANDLE hTimer;
	WAITORTIMERCALLBACK TimerCallback;
	void* pvHsmToaster; // HSM object with context, SOvenToasterContenxt
} STimer, *PSTimer;

UN_ERROR InitGlobal(); // initialize global variables
UN_ERROR CleanupGlobal(); // cleanup global variables
UN_ERROR Timer(PSTimer timer_, TimerState ts); // turn timer on/off

#pragma region state handlers
// State/Action handler prototype definitions
void AH_None(PSHsm hsm);
SHsmCmd SH_None(PSHsm hsm);

SHsmCmd SH_Top(PSHsm hsm);

SHsmCmd SH_DoorClosed(PSHsm hsm);

SHsmCmd SH_DoorOpened(PSHsm hsm);

SHsmCmd SH_Heating(PSHsm hsm);

SHsmCmd SH_Idle(PSHsm hsm);

SHsmCmd SH_Ready(PSHsm hsm);

SHsmCmd SH_NotReady(PSHsm hsm);

void AH_DoorOpened_Enter(PSHsm hsm);
void AH_DoorClosed_Enter(PSHsm hsm);

void AH_Heating_Enter(PSHsm hsm);
void AH_Heating_Exit(PSHsm hsm);
void AH_Ready_Enter(PSHsm hsm);
void AH_NotReady_Enter(PSHsm hsm);
#pragma endregion

// helper functions for state/action handlers
void DisplayContext(const SOvenToasterContext* context);
void DisplayState(const SHsm* hsm);

// global variable definitions
#if defined(_OvenToaster_c)
SOvenToasterContext globalContext;
PSHsm hsmToaster;
PSLogger logger;

#else
extern SOvenToasterContext globalContext;
extern PSHsm hsmToaster;
extern PSLogger logger;
extern const SHsmState ST_Default;
extern const SHsmState ST_Top;
extern const SHsmState ST_DoorOpened;
extern const SHsmState ST_DoorClosed;
extern const SHsmState ST_Heating;
extern const SHsmState ST_Idle;
extern const SHsmState ST_Ready;
extern const SHsmState ST_NotReady;
extern SHsmQueueMsg events[];
#endif

#endif