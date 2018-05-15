#ifndef __INSPECT_MESSAGE_OPERATE_H
#define   __INSPECT_MESSAGE_OPERATE_H

#include "sys.h"
#include "tmesh_algorithm.h"

#define INSPECT_SPOT_STATUS_PACK_NUM		5

/* Inspect Spot Status */
typedef struct
{
	unsigned char						Front;
	unsigned char						Rear;
	SpotStatusTypedef					SpotStatus[INSPECT_SPOT_STATUS_PACK_NUM];
}Inspect_SwapSpotStatusTypeDef;

extern Inspect_SwapSpotStatusTypeDef		InspectSpotStatus;

bool Inspect_Message_SpotStatusisFull(void);

bool Inspect_Message_SpotStatusisEmpty(void);

void Inspect_Message_SpotStatusEnqueue(SpotStatusTypedef dataBuf);

bool Inspect_Message_SpotStatusDequeue(SpotStatusTypedef* dataBuf);

void Inspect_Message_SpotStatusDequeueEx(SpotStatusTypedef* dataBuf);

bool Inspect_Message_SpotStatusOffSet(void);

unsigned char Inspect_Message_SpotStatusRear(void);

#endif
