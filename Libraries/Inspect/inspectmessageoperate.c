/**
  *********************************************************************************************************
  * @file    inspectmessageoperate.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-04-29
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "inspectmessageoperate.h"
#include "platform_config.h"
#include "platform_map.h"
#include "delay.h"
#include "usart.h"

Inspect_SwapSpotStatusTypeDef		InspectSpotStatus;

/********************************************Is Full******************************************************/
/**********************************************************************************************************
 @Function			bool Inspect_Message_SpotStatusisFull(void)
 @Description			Inspect_Message_SpotStatusisFull
 @Input				void
 @Return				true							: 已满
					false						: 未满
**********************************************************************************************************/
bool Inspect_Message_SpotStatusisFull(void)
{
	bool MessageState;
	
	if ((InspectSpotStatus.Rear + 1) % INSPECT_SPOT_STATUS_PACK_NUM == InspectSpotStatus.Front) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/********************************************Is Empty*****************************************************/
/**********************************************************************************************************
 @Function			bool Inspect_Message_SpotStatusisEmpty(void)
 @Description			Inspect_Message_SpotStatusisEmpty
 @Input				void
 @Return				true							: 已空
					false						: 未空
**********************************************************************************************************/
bool Inspect_Message_SpotStatusisEmpty(void)
{
	bool MessageState;
	
	if (InspectSpotStatus.Front == InspectSpotStatus.Rear) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/******************************************** Enqueue ****************************************************/
/**********************************************************************************************************
 @Function			void Inspect_Message_SpotStatusEnqueue(SpotStatusTypedef dataBuf)
 @Description			Inspect_Message_SpotStatusEnqueue
 @Input				dataBuf	 		 				: 需写入数据
 @Return				void
**********************************************************************************************************/
void Inspect_Message_SpotStatusEnqueue(SpotStatusTypedef dataBuf)
{
	u8 rearold = 0;
	
	if (Inspect_Message_SpotStatusisFull() == true) {																//队列已满
		rearold = InspectSpotStatus.Rear;
		InspectSpotStatus.Rear = (InspectSpotStatus.Rear + 1) % INSPECT_SPOT_STATUS_PACK_NUM;							//队尾偏移1
		
		memset((u8 *)&InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear], 0x0, sizeof(InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear]));
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].spot_status = dataBuf.spot_status;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].spot_count = InspectSpotStatus.SpotStatus[rearold].spot_count;
		
		if (dataBuf.spot_count != 0) {																		//状态改变
			InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].spot_count += dataBuf.spot_count;
			TCFG_SystemData.SpotStatusCount = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].spot_count;
			TCFG_EEPROM_SetStatusCount(TCFG_SystemData.SpotStatusCount);
			TCFG_EEPROM_SetEventTime(dataBuf.unixTime);
		}
		else {																							//状态未变
			InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].spot_status |= 0x02;
		}
		
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lData.X_Now = dataBuf.qmc5883lData.X_Now;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lData.Y_Now = dataBuf.qmc5883lData.Y_Now;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lData.Z_Now = dataBuf.qmc5883lData.Z_Now;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lData.X_Back = dataBuf.qmc5883lData.X_Back;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lData.Y_Back = dataBuf.qmc5883lData.Y_Back;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lData.Z_Back = dataBuf.qmc5883lData.Z_Back;
		
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lDiff.BackVal_Diff = dataBuf.qmc5883lDiff.BackVal_Diff;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lDiff.GraduaVal_Diff = dataBuf.qmc5883lDiff.GraduaVal_Diff;
		
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.NotargetNum = dataBuf.radarData.NotargetNum;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.CoverNum = dataBuf.radarData.CoverNum;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.DismagNum = dataBuf.radarData.DismagNum;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.DisVal = dataBuf.radarData.DisVal;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.MagVal = dataBuf.radarData.MagVal;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.Diff = dataBuf.radarData.Diff;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.Diff_v2 = dataBuf.radarData.Diff_v2;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.timedomain_dif = dataBuf.radarData.timedomain_dif;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.timedomain_square_dif = dataBuf.radarData.timedomain_square_dif;
		
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].timeCounter = dataBuf.timeCounter;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].unixTime = dataBuf.unixTime;
		
		InspectSpotStatus.Front = (InspectSpotStatus.Front + 1) % INSPECT_SPOT_STATUS_PACK_NUM;							//队头偏移1
	}
	else {																								//队列未满
		rearold = InspectSpotStatus.Rear;
		InspectSpotStatus.Rear = (InspectSpotStatus.Rear + 1) % INSPECT_SPOT_STATUS_PACK_NUM;							//队尾偏移1
		
		memset((u8 *)&InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear], 0x0, sizeof(InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear]));
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].spot_status = dataBuf.spot_status;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].spot_count = InspectSpotStatus.SpotStatus[rearold].spot_count;
		
		if (dataBuf.spot_count != 0) {																		//状态改变
			InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].spot_count += dataBuf.spot_count;
			TCFG_SystemData.SpotStatusCount = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].spot_count;
			TCFG_EEPROM_SetStatusCount(TCFG_SystemData.SpotStatusCount);
			TCFG_EEPROM_SetEventTime(dataBuf.unixTime);
		}
		else {																							//状态未变
			InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].spot_status |= 0x02;
		}
		
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lData.X_Now = dataBuf.qmc5883lData.X_Now;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lData.Y_Now = dataBuf.qmc5883lData.Y_Now;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lData.Z_Now = dataBuf.qmc5883lData.Z_Now;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lData.X_Back = dataBuf.qmc5883lData.X_Back;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lData.Y_Back = dataBuf.qmc5883lData.Y_Back;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lData.Z_Back = dataBuf.qmc5883lData.Z_Back;
		
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lDiff.BackVal_Diff = dataBuf.qmc5883lDiff.BackVal_Diff;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lDiff.GraduaVal_Diff = dataBuf.qmc5883lDiff.GraduaVal_Diff;
		
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.NotargetNum = dataBuf.radarData.NotargetNum;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.CoverNum = dataBuf.radarData.CoverNum;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.DismagNum = dataBuf.radarData.DismagNum;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.DisVal = dataBuf.radarData.DisVal;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.MagVal = dataBuf.radarData.MagVal;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.Diff = dataBuf.radarData.Diff;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.Diff_v2 = dataBuf.radarData.Diff_v2;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.timedomain_dif = dataBuf.radarData.timedomain_dif;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.timedomain_square_dif = dataBuf.radarData.timedomain_square_dif;
		
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].timeCounter = dataBuf.timeCounter;
		InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].unixTime = dataBuf.unixTime;
	}
}

/******************************************** Dequeue ****************************************************/
/**********************************************************************************************************
 @Function			bool Inspect_Message_SpotStatusDequeue(SpotStatusTypedef* dataBuf)
 @Description			Inspect_Message_SpotStatusDequeue
 @Input				dataBuf	 		 				: 需读出数据地址
 @Return				true								: 未空
					false							: 已空
**********************************************************************************************************/
bool Inspect_Message_SpotStatusDequeue(SpotStatusTypedef* dataBuf)
{
	bool MessageState;
	unsigned char front;
	
	if (Inspect_Message_SpotStatusisEmpty() == true) {															//队列已空
		MessageState = false;
	}
	else {																								//队列未空
		front = (InspectSpotStatus.Front + 1) % INSPECT_SPOT_STATUS_PACK_NUM;											//队头偏移1
		
		dataBuf->spot_status = InspectSpotStatus.SpotStatus[front].spot_status;
		dataBuf->spot_count = InspectSpotStatus.SpotStatus[front].spot_count;
		
		dataBuf->qmc5883lData.X_Now = InspectSpotStatus.SpotStatus[front].qmc5883lData.X_Now;
		dataBuf->qmc5883lData.Y_Now = InspectSpotStatus.SpotStatus[front].qmc5883lData.Y_Now;
		dataBuf->qmc5883lData.Z_Now = InspectSpotStatus.SpotStatus[front].qmc5883lData.Z_Now;
		dataBuf->qmc5883lData.X_Back = InspectSpotStatus.SpotStatus[front].qmc5883lData.X_Back;
		dataBuf->qmc5883lData.Y_Back = InspectSpotStatus.SpotStatus[front].qmc5883lData.Y_Back;
		dataBuf->qmc5883lData.Z_Back = InspectSpotStatus.SpotStatus[front].qmc5883lData.Z_Back;
		
		dataBuf->qmc5883lDiff.BackVal_Diff = InspectSpotStatus.SpotStatus[front].qmc5883lDiff.BackVal_Diff;
		dataBuf->qmc5883lDiff.GraduaVal_Diff = InspectSpotStatus.SpotStatus[front].qmc5883lDiff.GraduaVal_Diff;
		
		dataBuf->radarData.NotargetNum = InspectSpotStatus.SpotStatus[front].radarData.NotargetNum;
		dataBuf->radarData.CoverNum = InspectSpotStatus.SpotStatus[front].radarData.CoverNum;
		dataBuf->radarData.DismagNum = InspectSpotStatus.SpotStatus[front].radarData.DismagNum;
		dataBuf->radarData.DisVal = InspectSpotStatus.SpotStatus[front].radarData.DisVal;
		dataBuf->radarData.MagVal = InspectSpotStatus.SpotStatus[front].radarData.MagVal;
		dataBuf->radarData.Diff = InspectSpotStatus.SpotStatus[front].radarData.Diff;
		dataBuf->radarData.Diff_v2 = InspectSpotStatus.SpotStatus[front].radarData.Diff_v2;
		dataBuf->radarData.timedomain_dif = InspectSpotStatus.SpotStatus[front].radarData.timedomain_dif;
		dataBuf->radarData.timedomain_square_dif = InspectSpotStatus.SpotStatus[front].radarData.timedomain_square_dif;
		
		dataBuf->timeCounter = InspectSpotStatus.SpotStatus[front].timeCounter;
		dataBuf->unixTime = InspectSpotStatus.SpotStatus[front].unixTime;
		
		MessageState = true;
	}
	
	return MessageState;
}

/******************************************** DequeueEx ****************************************************/
/**********************************************************************************************************
 @Function			void Inspect_Message_SpotStatusDequeueEx(SpotStatusTypedef* dataBuf)
 @Description			Inspect_Message_SpotStatusDequeueEx
 @Input				dataBuf	 		 				: 需读出数据地址
 @Return				true								: 未空
					false							: 已空
**********************************************************************************************************/
void Inspect_Message_SpotStatusDequeueEx(SpotStatusTypedef* dataBuf)
{
	dataBuf->spot_status = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].spot_status;
	dataBuf->spot_count = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].spot_count;
	
	dataBuf->qmc5883lData.X_Now = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lData.X_Now;
	dataBuf->qmc5883lData.Y_Now = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lData.Y_Now;
	dataBuf->qmc5883lData.Z_Now = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lData.Z_Now;
	dataBuf->qmc5883lData.X_Back = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lData.X_Back;
	dataBuf->qmc5883lData.Y_Back = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lData.Y_Back;
	dataBuf->qmc5883lData.Z_Back = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lData.Z_Back;
	
	dataBuf->qmc5883lDiff.BackVal_Diff = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lDiff.BackVal_Diff;
	dataBuf->qmc5883lDiff.GraduaVal_Diff = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].qmc5883lDiff.GraduaVal_Diff;
	
	dataBuf->radarData.NotargetNum = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.NotargetNum;
	dataBuf->radarData.CoverNum = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.CoverNum;
	dataBuf->radarData.DismagNum = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.DismagNum;
	dataBuf->radarData.DisVal = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.DisVal;
	dataBuf->radarData.MagVal = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.MagVal;
	dataBuf->radarData.Diff = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.Diff;
	dataBuf->radarData.Diff_v2 = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.Diff_v2;
	dataBuf->radarData.timedomain_dif = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.timedomain_dif;
	dataBuf->radarData.timedomain_square_dif = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].radarData.timedomain_square_dif;
	
	dataBuf->timeCounter = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].timeCounter;
	dataBuf->unixTime = InspectSpotStatus.SpotStatus[InspectSpotStatus.Rear].unixTime;
}

/********************************************* OffSet ****************************************************/
/**********************************************************************************************************
 @Function			bool Inspect_Message_SpotStatusOffSet(void)
 @Description			Inspect_Message_SpotStatusOffSet
 @Input				void
 @Return				true							: 未空
					false						: 已空
**********************************************************************************************************/
bool Inspect_Message_SpotStatusOffSet(void)
{
	bool MessageState;
	
	if (Inspect_Message_SpotStatusisEmpty() == true) {															//队列已空
		MessageState = false;
	}
	else {																								//队列未空
		InspectSpotStatus.Front = (InspectSpotStatus.Front + 1) % INSPECT_SPOT_STATUS_PACK_NUM;
		MessageState = true;
	}
	
	return MessageState;
}

/********************************************* Rear *****************************************************/
/**********************************************************************************************************
 @Function			unsigned char Inspect_Message_SpotStatusRear(void)
 @Description			Inspect_Message_SpotStatusRear
 @Input				void
 @Return				队尾值
**********************************************************************************************************/
unsigned char Inspect_Message_SpotStatusRear(void)
{
	return InspectSpotStatus.Rear;
}

/********************************************** END OF FLEE **********************************************/
