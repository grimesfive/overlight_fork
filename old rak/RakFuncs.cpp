/*

PROJECT:		mod_sa
LICENSE:		See LICENSE in the top level directory
COPYRIGHT:		Copyright we_sux, BlastHack

mod_sa is available from https://github.com/BlastHackNet/mod_s0beit_sa/

mod_sa is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

mod_sa is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with mod_sa.  If not, see <http://www.gnu.org/licenses/>.

*/

//#include "../main.h"
#include "main.h"

RakFuncs *g_RakFuncs = NULL;

RakFuncs::RakFuncs(void *pRakClientInterface)
{
	pRakClient = (RakClientInterface_ *)pRakClientInterface;
	//pRakClient2 = (RakClientInterface *)pRakClientInterface;
}

bool RakFuncs::RPC(int rpcId, BitStream *bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp)
{
	if (!pRakClient)
		return false;

	return pRakClient->RPC(&rpcId, bitStream, priority, reliability, orderingChannel, shiftTimestamp);//, UNASSIGNED_NETWORK_ID, NULL);//);
}

bool RakFuncs::Send(BitStream *bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	if (!pRakClient)
		return false;
	return pRakClient->Send(bitStream, priority, reliability, orderingChannel);
}

// misc functions
void RakFuncs::SendDeath(uint16_t killerId, uint8_t reason)
{
	BitStream bsDeath;

	bsDeath.Write(reason);
	bsDeath.Write(killerId);
	g_RakFuncs->RPC(RPC_Death, &bsDeath);
}

void RakFuncs::SendPickUp(int pickupId)
{
	BitStream bsPickup;

	bsPickup.Write(pickupId);

	RPC(RPC_PickedUpPickup, &bsPickup);
}

void RakFuncs::RequestClass(int classId)
{
	BitStream bsClass;

	bsClass.Write(classId);
	g_RakFuncs->RPC(RPC_RequestClass, &bsClass);
}

void RakFuncs::SendSCMEvent(int vehicleID, int eventId, int param1, int param2)
{
	BitStream bsScmEvent;

	bsScmEvent.Write(vehicleID);
	bsScmEvent.Write(param1);
	bsScmEvent.Write(param2);
	bsScmEvent.Write(eventId);

	RPC(RPC_SCMEvent, &bsScmEvent);
}

void RakFuncs::SendSpawn(void)
{
	BitStream bsSpawn;

	g_RakFuncs->RPC(RPC_RequestSpawn, &bsSpawn);
	g_RakFuncs->RPC(RPC_Spawn, &bsSpawn);
}


void RakFuncs::SendUnoccupiedSync(stUnoccupiedData *unocSync)
{
	BitStream bsSend;

	bsSend.Write((BYTE)ID_UNOCCUPIED_SYNC);
	bsSend.Write((PCHAR)unocSync, sizeof(stUnoccupiedData));
	Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void RakFuncs::SendUnoccupiedSync(uint16_t sVehicleID, float fRoll[3], float fDirection[3], float fPos[3], float fSpeed[3], float fSpin[3], float fHealth)
{
	stUnoccupiedData unocSync;
	ZeroMemory(&unocSync, sizeof(stUnoccupiedData));

	unocSync.sVehicleID = sVehicleID;
	unocSync.byteSeatID = 1;
	unocSync.fHealth = fHealth;

	vect3_copy(fRoll, unocSync.fRoll);
	vect3_copy(fDirection, unocSync.fDirection);
	vect3_copy(fPos, unocSync.fPosition);
	vect3_copy(fSpeed, unocSync.fMoveSpeed);
	vect3_copy(fSpin, unocSync.fTurnSpeed);

	BitStream bsSend;
	bsSend.Write((BYTE)ID_UNOCCUPIED_SYNC);
	bsSend.Write((PCHAR)&unocSync, sizeof(stUnoccupiedData));
	Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void RakFuncs::SendInCarSyncData(stInCarData *picSync, int iUseCarPos)
{
	if (picSync == NULL)
		return;

	BitStream bsVehicleSync;

	if (iUseCarPos)
	{
		int car_id = (int)(((DWORD)g_Vehicles->pGTA_Vehicle[picSync->sVehicleID]) - (DWORD)pool_vehicle->start) / 2584;
		struct vehicle_info *vinfo = vehicle_info_get(car_id, 0);

		if (!vinfo)
			return;

		float fPos[3];
		vect3_copy(&vinfo->base.matrix[4 * 3], fPos);

		picSync->fPosition[0] = fPos[0];
		picSync->fPosition[1] = fPos[1];
		picSync->fPosition[2] = fPos[2];
	}

	bsVehicleSync.Write((BYTE)ID_VEHICLE_SYNC);
	bsVehicleSync.Write((PCHAR)picSync, sizeof(stInCarData));

	g_RakFuncs->Send(&bsVehicleSync);
}

void RakFuncs::MoveUnoccupiedVehicle(int iVehicleID, float fPos[3], float fSpeed[3])
{
	if (isBadSAMPVehicleID(iVehicleID))
		return;

	vehicle_info *vinfo = getGTAVehicleFromSAMPVehicleID(iVehicleID);
	if (vinfo == NULL)
		return;

	stUnoccupiedData unocSync;
	ZeroMemory(&unocSync, sizeof(stUnoccupiedData));

	unocSync.sVehicleID = iVehicleID;
	unocSync.byteSeatID = 1;
	unocSync.fHealth = vinfo->hitpoints;
	vect3_copy(fPos, unocSync.fPosition);
	vect3_copy(fSpeed, unocSync.fMoveSpeed);
	vect3_copy(vinfo->spin, unocSync.fTurnSpeed);
	vect3_copy(&vinfo->base.matrixStruct->right.X, unocSync.fRoll);
	vect3_copy(&vinfo->base.matrixStruct->up.X, unocSync.fDirection);

	BitStream bsSend;
	bsSend.Write((BYTE)ID_UNOCCUPIED_SYNC);
	bsSend.Write((PCHAR)&unocSync, sizeof(stUnoccupiedData));
	Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void RakFuncs::SetSpeedToVehicle(int iVehicleID, float fSpeed[3])
{
	if (isBadSAMPVehicleID(iVehicleID))
		return;

	vehicle_info *vinfo = getGTAVehicleFromSAMPVehicleID(iVehicleID);
	if (vinfo == NULL)
		return;

	actor_info *self = actor_info_get(ACTOR_SELF, 0);

	stInCarData vehSync;
	ZeroMemory(&vehSync, sizeof(stInCarData));

	vehSync.sVehicleID = iVehicleID;
	vehSync.fVehicleHealth = vinfo->hitpoints;
	vect3_copy(&vinfo->base.matrix[12], vehSync.fPosition);
	vect3_copy(fSpeed, vehSync.fMoveSpeed);
	//float fQuaternion[4];
	//MatrixToQuaternion(vinfo->base.matrix, fQuaternion);
	//vect4_copy(fQuaternion, vehSync.fQuaternion);
	vehSync.bytePlayerHealth = self->hitpoints;
	vehSync.byteArmor = self->armor;
	vehSync.byteCurrentWeapon = g_Players->pLocalPlayer->byteCurrentWeapon;

	BitStream bsSend;
	bsSend.Write((BYTE)ID_VEHICLE_SYNC);
	bsSend.Write((PCHAR)&vehSync, sizeof(stInCarData));
	Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

