#include "stdafx.h"
#include "CppUnitTest.h"
#include "helper.h"
#include "crc16.h"
#include "HsmQueueMsg.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UART_NETTests
{		

	TEST_CLASS(UnitTest1)
	{
		static WCHAR AssertBuffer[256];
	private:
		const LPCWSTR LN_(int l) {
			swprintf_s(AssertBuffer, 256, L"LINE# = %d", l);
			return AssertBuffer;
		}
		static PSLogger logger;
	public:
		TEST_CLASS_INITIALIZE(Init_UnitTest1)
		{
			logger = Logger_New(256, "UART_NET.Tests.log");
		}

		TEST_CLASS_CLEANUP(Cleanup_UnitTest1)
		{
			Logger_Delete(&logger);
		}

		TEST_METHOD(TestMethod_Logger)
		{
			PSSizedBuffer buffer = Logger_ExportBuffer(logger, 0);
			snprintf((char*)(buffer->buffer), buffer->bcLength, "TestMethod_Logger() started.");
			Logger_Flush(logger);

			Logger_Code(logger, 32, (uint8_t*)"TestMethod_Logger() write error code, 32");
			Logger_Info(logger, (uint8_t*)"TestMethod_Logger() ends.");
		}

		TEST_METHOD(TestMethod_CRC16)
		{
			srand((unsigned int)time(NULL));
			SCRCRegisters crc = {
				0,
				DEFAULT_CRC16FP
			};

			PSSizedBuffer byteArray = SizedBuffer_New(128);
			SizedBuffer_FillRand(byteArray);

			CRC16_Puts2(&crc, byteArray);
			Logger_Code(logger, (uint32_t)crc.SR,
				(const uint8_t*)"CRC16 SR after calculation");
			uint16_t CRC16SR_Saved = crc.SR;
			CRC16_Puts(&crc, (const uint8_t*)&CRC16SR_Saved, 2);
			Logger_Code(logger, (uint32_t)crc.SR,
				(const uint8_t*)"CRC16 SR after calculation with CRC16SR_Saved");
			Assert::AreEqual((DWORD)0, (DWORD)crc.SR, LN_(__LINE__));

			byteArray->buffer[0] ^= 0x02;
			crc.SR = 0;
			CRC16_Puts2(&crc, byteArray);
			CRC16_Puts(&crc, (const uint8_t*)&CRC16SR_Saved, 2);
			Assert::AreNotEqual((DWORD)0, (DWORD)crc.SR);
			SizedBuffer_Delete(&byteArray);
		}

		static int CountHsmQueueMsg(PSHsmQueueMsg anchor)
		{
			int count = 0;
			
			for (PSHsmQueueMsg scan = anchor->next; scan != anchor; scan = scan->next)
			{
				count++;
			}
			return count;
		}
		TEST_METHOD(TestMethod_HsmQueueMsg)
		{
			// init list
			char recipient[10];
			PSHsmQueueMsg anchor = HsmQueueMsg_New();
			for (int i = 9; i >= 0; i--)
			{
				PSHsmQueueMsg msg = HsmQueueMsg_New();
				msg->msg = (uint8_t)i;
				msg->recipient = (recipient + i);
				HsmQueueMsg_Insert(anchor, msg);
			}

			PSHsmQueueMsg msg = HsmQueueMsg_FindRecipient(anchor, (const void*)recipient);

			Assert::AreEqual(msg->recipient, (void*)recipient);
			HsmQueueMsg_Remove(msg);
			HsmQueueMsg_Delete(&msg);
			Assert::AreEqual((const void*)nullptr, (const void*)msg);
			msg = HsmQueueMsg_FindRecipient(anchor, (const void*)recipient);
			Assert::AreEqual((const void*)nullptr, (const void*)msg);

			for (int i = 1; i < 10; i++)
			{
				msg = HsmQueueMsg_FindRecipient(anchor, &recipient[i]);
				Assert::AreEqual(msg->recipient, (void*)&recipient[i]);
				HsmQueueMsg_Remove(msg);
				HsmQueueMsg_Delete(&msg);
			}
			HsmQueueMsg_Delete(&anchor);
		}
	};

	WCHAR UnitTest1::AssertBuffer[];

	PSLogger UnitTest1::logger = NULL;
}