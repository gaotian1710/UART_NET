#include "stdafx.h"
#include "HsmQueueMsg.h"
#include "hsm.h"
#include "helper.h"
#define _OvenToaster_c
#include "OvenToaster.h"

SHsmQueueMsg events[] = {
	{ NULL, NULL, NULL, NULL, NULL, (uint32_t)EvMsg_Enter },
	{ NULL, NULL, NULL, NULL, NULL, (uint32_t)EvMsg_OpenDoor },
	{ NULL, NULL, NULL, NULL, NULL, (uint32_t)EvMsg_CloseDoor },
	{ NULL, NULL, NULL, NULL, NULL, (uint32_t)EvMsg_SetupStart },
	{ NULL, NULL, NULL, NULL, NULL, (uint32_t)EvMsg_ClearSetup },
	{ NULL, NULL, NULL, NULL, NULL, (uint32_t)EvMsg_TimerTick }
};

PSHsm hsmToaster = NULL;
SOvenToasterContext globalContext = {
	DoorState_Close,
	HeaterState_Off,
	TimerState_Off,
	0, 0
};

VOID CALLBACK TimerCallback_(PVOID hsm_, BOOLEAN notUsed)
{
	PSHsm hsm = (PSHsm)hsm_;
	Hsm_Lock(hsm);
	Hsm_HandleEvent(&events[EvMsg_TimerTick], hsm);
	Hsm_Unlock(hsm);
}

STimer timerContext = { 0 };

// State machine state definitions
const SHsmState ST_Default = { AH_None, AH_None, SH_None, "ST_Default" };
const SHsmState ST_Top = { AH_None, AH_None, SH_Top, "ST_Top" };
const SHsmState ST_DoorOpened = { AH_DoorOpened_Enter, AH_None, SH_DoorOpened, "ST_DoorOpened" };
const SHsmState ST_DoorClosed = { AH_DoorClosed_Enter, AH_None, SH_DoorClosed, "ST_DoorClosed" };
const SHsmState ST_Heating = { AH_Heating_Enter, AH_Heating_Exit, SH_Heating, "ST_Heating" };
const SHsmState ST_Idle = { AH_None, AH_None, SH_Idle, "ST_Idle" };
const SHsmState ST_Ready = { AH_Ready_Enter, AH_None, SH_Ready, "ST_Ready" };
const SHsmState ST_NotReady = { AH_NotReady_Enter, AH_None, SH_NotReady, "ST_NotReady" };


UN_ERROR InitGlobal(LPCSTR logFilename)
{
	UN_ERROR result = UN_ERROR_SUCCESS;
	do {
		logger = Logger_New(256, logFilename);
		hsmToaster = Hsm_New(4);
		hsmToaster->context = (void*)&globalContext;
		Hsm_Push(hsmToaster, &ST_Top);
		timerContext.TimerCallback = TimerCallback_;
		timerContext.pvHsmToaster = hsmToaster;
	} while (false);
	return result;
}

UN_ERROR CleanupGlobal()
{
	UN_ERROR result = UN_ERROR_SUCCESS;
	do {
		Hsm_Delete(&hsmToaster);
		Logger_Delete(&logger);
	} while (false);
	return result;
}

UN_ERROR Timer(PSTimer timer_, TimerState ts)
{
	UN_ERROR result = UN_ERROR_SUCCESS;
	do {
		switch (ts)
		{
		case TimerState_On:
			if (timer_->hTimer == NULL)
			{
				if (!CreateTimerQueueTimer(
					&(timer_->hTimer), NULL, // timer handle, timer queue handle
					TimerCallback_, timer_->pvHsmToaster, // callback method and its parameter
					1000, 1000, WT_EXECUTEDEFAULT // due time, period, and flag specifying execution mode
				))
				{
					result = (UN_ERROR)GetLastError(); break;
				}
				globalContext.ts = ts;
			}
			else
			{
				result = UN_ERROR_INVALID_OPERATION;
			}
			break;
		case TimerState_Off:
			if (timer_->hTimer)
			{
				if (!DeleteTimerQueueTimer(NULL, timer_->hTimer, NULL))
				{
					if (ERROR_IO_PENDING == (result = (UN_ERROR)GetLastError()))
					{
						result = UN_ERROR_SUCCESS;
					}
					else
					{
						break;
					}
				}
				globalContext.ts = ts;
				timer_->hTimer = NULL;
			}
			else
			{
				result = UN_ERROR_INVALID_OPERATION;
			}
			break;
		}
	} while (false);
	return result;
}



// Event hander/action handler definitions
void AH_None(PSHsm hsm)
{
}

SHsmCmd SH_None(PSHsm hsm)
{
	SHsmCmd cmd = { &ST_Default, Hsm_stay };
	return cmd;
}

SHsmCmd SH_Top(PSHsm hsm)
{
	SHsmCmd cmd = { 0 };
	PSOvenToasterContext toasterContext = (PSOvenToasterContext)(hsm->context);
	switch (hsm->rxMsg->msg)
	{
	case EvMsg_Enter:
		cmd.tt = Hsm_child;
		cmd.nextState = (toasterContext->ds == DoorState_Close) ? &ST_DoorClosed : &ST_DoorOpened;
		break;
	default:
		break;
	}
	return cmd;
}

SHsmCmd SH_DoorClosed(PSHsm hsm)
{
	SHsmCmd cmd = { 0 };
	PSOvenToasterContext toasterContext = (PSOvenToasterContext)(hsm->context);
	switch (hsm->rxMsg->msg)
	{
	case EvMsg_Enter:
	case EvMsg_CloseDoor:
		cmd.tt = Hsm_child;
		cmd.nextState = (toasterContext->timerPeriod == toasterContext->elapsedTime) ?
			&ST_Idle : &ST_Heating;
		break;
	case EvMsg_OpenDoor:
		cmd.tt = Hsm_sibling;
		cmd.nextState = &ST_DoorOpened;
		break;
	default:
		break;
	}
	return cmd;
}

SHsmCmd SH_DoorOpened(PSHsm hsm)
{
	SHsmCmd cmd = { 0 };
	PSOvenToasterContext toasterContext = (PSOvenToasterContext)(hsm->context);
	switch (hsm->rxMsg->msg)
	{
	case  EvMsg_Enter:
	case EvMsg_OpenDoor:
		cmd.tt = Hsm_child;
		cmd.nextState = (toasterContext->timerPeriod == toasterContext->elapsedTime) ?
			&ST_NotReady : &ST_Ready;
		break;
	case EvMsg_CloseDoor:
		cmd.tt = Hsm_sibling;
		cmd.nextState = &ST_DoorClosed;
		break;
	default:

		break;
	}
	return cmd;
}

SHsmCmd SH_Heating(PSHsm hsm)
{
	SHsmCmd cmd = { &ST_Heating, Hsm_stay };
	PSOvenToasterContext toasterContext = (PSOvenToasterContext)(hsm->context);
	switch (hsm->rxMsg->msg)
	{
	case EvMsg_Enter:
		break;
	case EvMsg_SetupStart:
		toasterContext->timerPeriod = (uint32_t)(hsm->rxMsg->context);
		toasterContext->elapsedTime = 0;
		break;
	case EvMsg_TimerTick:
		toasterContext->elapsedTime++;
		if (toasterContext->elapsedTime == toasterContext->timerPeriod)
		{
			cmd.nextState = &ST_Idle; cmd.tt = Hsm_sibling;
		}
		break;
	case EvMsg_ClearSetup:
		toasterContext->elapsedTime = toasterContext->timerPeriod = 0;
		cmd.nextState = &ST_Idle; cmd.tt = Hsm_sibling;
		break;
	case EvMsg_OpenDoor:
		Timer(&timerContext, TimerState_Off);
		cmd.nextState = NULL; cmd.tt = Hsm_parent;
		break;
	}
	return cmd;
}

SHsmCmd SH_Idle(PSHsm hsm)
{
	SHsmCmd cmd = { &ST_Idle, Hsm_stay };
	PSOvenToasterContext toasterContext = (PSOvenToasterContext)(hsm->context);
	switch (hsm->rxMsg->msg)
	{
	case EvMsg_SetupStart:
		toasterContext->timerPeriod = (uint32_t)(hsm->rxMsg->context);
		toasterContext->elapsedTime = 0;
		cmd.nextState = &ST_Heating; cmd.tt = Hsm_sibling;
		printf("\t\t+++ SH_Idle() handled EvMsg_SetupStart\n");
		break;
	case EvMsg_OpenDoor:
		toasterContext->ds = DoorState_Open;
		cmd.nextState = NULL; cmd.tt = Hsm_parent;
		printf("\t\t+++ SH_Idle() handled EvMsg_OpenDoor\n");
		break;
	}
	return cmd;
}

SHsmCmd SH_Ready(PSHsm hsm)
{
	SHsmCmd cmd = { &ST_Ready, Hsm_stay };
	PSOvenToasterContext toasterContext = (PSOvenToasterContext)(hsm->context);
	switch (hsm->rxMsg->msg)
	{
	case EvMsg_SetupStart:
		toasterContext->timerPeriod = (uint32_t)(hsm->rxMsg->context);
		toasterContext->elapsedTime = 0;
		break;
	case EvMsg_CloseDoor:
		toasterContext->ds = DoorState_Close;
		cmd.nextState = NULL; cmd.tt = Hsm_parent;
		break;
	case EvMsg_ClearSetup:
		toasterContext->timerPeriod = toasterContext->elapsedTime = 0;
		break;
	}
	return cmd;
}

SHsmCmd SH_NotReady(PSHsm hsm)
{
	SHsmCmd cmd = { &ST_Ready, Hsm_stay };
	PSOvenToasterContext toasterContext = (PSOvenToasterContext)(hsm->context);
	switch (hsm->rxMsg->msg)
	{
	case EvMsg_SetupStart:
		toasterContext->timerPeriod = (uint32_t)(hsm->rxMsg->context);
		toasterContext->elapsedTime = 0;
		cmd.nextState = &ST_Ready; cmd.tt = Hsm_sibling;
		break;
	case EvMsg_CloseDoor:
		toasterContext->ds = DoorState_Close;
		cmd.nextState = NULL; cmd.tt = Hsm_parent;
		break;
	}
	return cmd;
}

void AH_DoorOpened_Enter(PSHsm hsm)
{
	PSOvenToasterContext toasterContext = (PSOvenToasterContext)(hsm->context);
	toasterContext->ds = DoorState_Open;
	printf("\t\t*** AH_DoorOpened_Enter(): Turn light on\n");
}

void AH_DoorClosed_Enter(PSHsm hsm)
{
	PSOvenToasterContext toasterContext = (PSOvenToasterContext)(hsm->context);
	toasterContext->ds = DoorState_Close;
	printf("\t\t*** AH_DoorClosed_Enter(): Turn light off\n");
}

void AH_Heating_Enter(PSHsm hsm)
{
	PSOvenToasterContext toasterContext = (PSOvenToasterContext)(hsm->context);
	toasterContext->hs = HeaterState_On;
	Timer(&timerContext, TimerState_On);
}

void AH_Heating_Exit(PSHsm hsm)
{
	printf("AH_Heating_Exit() entered.\n");
	PSOvenToasterContext toasterContext = (PSOvenToasterContext)(hsm->context);
	toasterContext->hs = HeaterState_Off;
	Timer(&timerContext, TimerState_Off);
}

void AH_Ready_Enter(PSHsm hsm)
{
	PSOvenToasterContext toasterContext = (PSOvenToasterContext)(hsm->context);
}

void AH_NotReady_Enter(PSHsm hsm)
{
	PSOvenToasterContext toasterContext = (PSOvenToasterContext)(hsm->context);
}

void DisplayContext(const SOvenToasterContext * context)
{
	printf("(Door, Timer, Heater) = (%s, %s, %s)\n",
		context->ds == DoorState_Open ? "Open" : "Close",
		context->ts == TimerState_On ? "On" : "Off",
		context->hs == HeaterState_On ? "On" : "Off");
	printf("Elapsed time/Period = %d/%d\n", context->elapsedTime, context->timerPeriod);
	//printf("Context end ===========\n");
}

void DisplayState(const SHsm * hsm)
{
	printf("Hsm begin ========== \n");
	printf("Stack=%d / %d\n", hsm->current, hsm->size);
	printf("Current state = %s\n", hsm->stack[hsm->current]->name);
	if (hsm->rxMsg)
	{
		printf("rxMsg = msg: %d, context: 0x%08x, sender: 0x%08x\n", 
			hsm->rxMsg->msg, 
			(uint32_t)(hsm->rxMsg->context), 
			(uint32_t)(hsm->rxMsg->sender));
	}
	else
	{
		printf("rxMsg = (null)\n");
	}
	DisplayContext((const SOvenToasterContext*)(hsm->context));
	printf("Hsm end ========== \n\n");
}

