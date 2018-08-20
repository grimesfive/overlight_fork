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

#include "main.h"
//#include"../main.h"

bool HookedRakClientInterface::RPC(int* uniqueID, BitStream *parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp)
{
	traceLastFunc("HookedRakClientInterface::RPC(BitStream)");
	if (uniqueID != nullptr)
	{

		if (*uniqueID != RPC_UpdateScoresPingsIPs)
		{
			lastRPC = *uniqueID;
		}
		
		if (!OnSendRPC(*uniqueID, parameters, priority, reliability, orderingChannel, shiftTimestamp))
		{
			return false;
		}

	}
	return g_RakFuncs->GetInterface()->RPC(uniqueID, parameters, priority, reliability, orderingChannel, shiftTimestamp);
}

bool HookedRakClientInterface::Send(BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	traceLastFunc("HookedRakClientInterface::Send(BitStream)");

	if (bitStream != nullptr)
	{
		if (!OnSendPacket(bitStream, priority, reliability, orderingChannel))
			return false;
	}

	//int packetID;
	//bitStream->Read(packetID);

	//if (newcheats.AimCrasher
	//	|| (vehcheats.bVehCheats && !vehcheats.use_unoccupied)
	//	//|| newcheats.DetonatorCrasher
	//	//|| newcheats.StuntCrasher
	//	//|| (vehcheats.vehicle_grabber && vehcheats.use_allvehicles)
	//	|| newcheats.bAFK)
	//	return 1;

	return g_RakFuncs->GetInterface()->Send(bitStream, priority, reliability, orderingChannel);
}


BYTE GetPacketID(Packet *p)
{
	if (p == 0) return 255;

	//if ((unsigned char)p->data[0] == ID_TIMESTAMP) {
	//	assert(p->length > sizeof(unsigned char) + sizeof(unsigned long));
	//	return (unsigned char)p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	//}
	//else {
		return (unsigned char)p->data[0];
	//}
}

BYTE GetPacketID2(Packet2 *p)
{
	if (p == 0) return 255;
	//
	//if ((unsigned char)p->data[0] == ID_TIMESTAMP) {
	//	assert(p->length > sizeof(unsigned char) + sizeof(unsigned long));
	//	return (unsigned char)p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	//}
	//else {
	return (unsigned char)p->data[0];
	//}
}

Packet2* HookedRakClientInterface::Receive(void)
{
	traceLastFunc("HookedRakClientInterface::Receive");
	
	Packet2 *ReceivedPacket = g_RakFuncs->GetInterface()->Receive();

	BYTE ReceivedPacketID = GetPacketID2(ReceivedPacket);
	

	if (ReceivedPacketID == ID_PLAYER_SYNC)
	{
		traceLastFunc("ID_PLAYER_SYNC");
		if (OLCheats->bSilentAim)
		{
			BitStream bsOnFootSync((unsigned char *)ReceivedPacket->data, ReceivedPacket->length, false);
			unsigned short PlayerID;
			bsOnFootSync.IgnoreBits(8);
			bsOnFootSync.Read(PlayerID);
			
			EnemyStats[PlayerID].iPlayerOnFootTick[LastTick::SECOND_LAST] = EnemyStats[PlayerID].iPlayerOnFootTick[LastTick::LAST];
			EnemyStats[PlayerID].iPlayerOnFootTick[LastTick::LAST] = GetTickCount();

			stOnFootData OnFootData;
			memset(&OnFootData, 0, sizeof(stOnFootData));

			bool bLR, bUD;
			bsOnFootSync.Read(bLR);
			if (bLR)
				bsOnFootSync.Read(OnFootData.sLeftRightKeys);
			bsOnFootSync.Read(bUD);
			if (bUD)
				bsOnFootSync.Read(OnFootData.sUpDownKeys);
			bsOnFootSync.Read(OnFootData.sKeys);
			bsOnFootSync.Read(OnFootData.fPosition[0]);
			bsOnFootSync.Read(OnFootData.fPosition[1]);
			bsOnFootSync.Read(OnFootData.fPosition[2]);

			vect3_copy(EnemyStats[PlayerID].fPlayerPos[LastTick::LAST], EnemyStats[PlayerID].fPlayerPos[LastTick::SECOND_LAST]);
			vect3_copy(OnFootData.fPosition, EnemyStats[PlayerID].fPlayerPos[LastTick::LAST]);


		}
	}

	if (ReceivedPacketID == ID_AIM_SYNC)
	{

		traceLastFunc("ID_AIM_SYNC");
		if (bAntiBadAim)
		{
			BitStream bsAimSync((unsigned char *)ReceivedPacket->data, ReceivedPacket->length, false);
			unsigned short PlayerID;

			stAimData AimData;
			bsAimSync.IgnoreBits(8);
			bsAimSync.Read(PlayerID);
			bsAimSync.Read((PCHAR)&AimData, sizeof(stAimData));
			//Log("%d", PlayerID);
			//cheat_state_text("%d", PlayerID);
			bool deSyncAim = false;


			actor_info * player = getGTAPedFromSAMPPlayerID(PlayerID);
			

			//
			if (AimData.byteCamMode == 49)
			{
				cheat_state_text("%s is sending bad CAM MODE.",
					g_Players->pRemotePlayer[PlayerID]->iNameAllocated <= 0xF ? g_Players->pRemotePlayer[PlayerID]->szPlayerName : g_Players->pRemotePlayer[PlayerID]->pszPlayerName);
				ReceivedPacket->data[0] = 0xFF;
			}

			if (player)
			{

				if (!_finite(AimData.vecAimPos[0]) || !_finite(AimData.vecAimPos[1]) || !_finite(AimData.vecAimPos[2]))
				{
					deSyncAim = true;
				}
				else if (vect3_dist(AimData.vecAimPos, &player->base.matrix[12]) > 500.0f)
				{
					deSyncAim = true;
				}
			}
			else
			{
				deSyncAim = true;
			}

			if (deSyncAim)
			{
				cheat_state_text("%s is sending bad Aim.", getPlayerName(PlayerID));
				ReceivedPacket->data[0] = 0xFF;
				//Log("Bad Aim: %s\n Aim pos: %.2f %.2f %.2f\n OnFoot pos: %.2f %.2f %.2f",
				//	getPlayerName(PlayerID),
				//	AimData.vecAimPos[0],
				//	AimData.vecAimPos[1],
				//	AimData.vecAimPos[2],
				//	g_Players->pRemotePlayer[PlayerID]->pPlayerData->onFootData.fPosition[0],
				//	g_Players->pRemotePlayer[PlayerID]->pPlayerData->onFootData.fPosition[1],
				//	g_Players->pRemotePlayer[PlayerID]->pPlayerData->onFootData.fPosition[2]);
			}
		}
	}

	if (ReceivedPacketID == ID_VEHICLE_SYNC)
	{
		traceLastFunc("ID_VEHICLE_SYNC");
		if (bAntiBadVehicle)
		{
			BitStream bsVehicleSync((unsigned char *)ReceivedPacket->data, ReceivedPacket->length, false);
			unsigned short PlayerID;
			stInCarData InCarData;
			bsVehicleSync.IgnoreBits(8);
			bsVehicleSync.Read(PlayerID);
			bsVehicleSync.Read((PCHAR)&InCarData, sizeof(stInCarData));
			bsVehicleSync.Read(InCarData.sVehicleID);

			if (bAntiVehicleTroll && (InCarData.sVehicleID == g_Players->pLocalPlayer->sCurrentVehicleID))
			{
				cheat_state_text("%s tried to controll your car", g_Players->pRemotePlayer[PlayerID]->iNameAllocated <= 0xF ? g_Players->pRemotePlayer[PlayerID]->szPlayerName : g_Players->pRemotePlayer[PlayerID]->pszPlayerName);
				ReceivedPacket->data[0] = 255;
				return ReceivedPacket;
			}

			bool deSyncVeh = false;

			// KEYS
			bsVehicleSync.Read(InCarData.sLeftRightKeys);
			bsVehicleSync.Read(InCarData.sUpDownKeys);
			bsVehicleSync.Read(InCarData.sKeys);
			//
			//// QUATERNIONS
			bsVehicleSync.ReadNormQuat(InCarData.fQuaternion[0],
				InCarData.fQuaternion[1],
				InCarData.fQuaternion[2],
				InCarData.fQuaternion[3]);
			//
			//// POSITION
			bsVehicleSync.Read(InCarData.fPosition[0]);
			bsVehicleSync.Read(InCarData.fPosition[1]);
			bsVehicleSync.Read(InCarData.fPosition[2]);


			if (!_finite(InCarData.fPosition[0]) || !_finite(InCarData.fPosition[1]) || !_finite(InCarData.fPosition[2]))
				deSyncVeh = true;

			if (InCarData.fPosition[0] > 20000.0f || InCarData.fPosition[0] < -20000.0f || InCarData.fPosition[1] > 20000.0f || InCarData.fPosition[1] < -20000.0f || InCarData.fPosition[2] > 20000.0f || InCarData.fPosition[2] < -100.0f)
				deSyncVeh = true;

			if (deSyncVeh)
			{
				cheat_state_text("%s is sending bad vehicle data.", g_Players->pRemotePlayer[PlayerID]->iNameAllocated <= 0xF ? g_Players->pRemotePlayer[PlayerID]->szPlayerName : g_Players->pRemotePlayer[PlayerID]->pszPlayerName);
				ReceivedPacket->data[0] = 255;
			}
		}
	}

	if (ReceivedPacketID == ID_UNOCCUPIED_SYNC )
	{
#ifdef OWNER
		traceLastFunc("ID_UNOCCUPIED_SYNC");

		if (bAntiBadUnoccupied)
		{
			RakNet::BitStream bsUnoccupiedSync((unsigned char *)ReceivedPacket->data, ReceivedPacket->length, false);
			unsigned short PlayerID;

			stUnoccupiedData IncomingUnoccupiedData;
			bsUnoccupiedSync.IgnoreBits(8);
			bsUnoccupiedSync.Read(PlayerID);
			bsUnoccupiedSync.Read((PCHAR)&IncomingUnoccupiedData, sizeof(stUnoccupiedData));//nice


			bsUnoccupiedSync.Read(IncomingUnoccupiedData.sVehicleID);

			bsUnoccupiedSync.Read(IncomingUnoccupiedData.fPosition[0]);
			bsUnoccupiedSync.Read(IncomingUnoccupiedData.fPosition[1]);
			bsUnoccupiedSync.Read(IncomingUnoccupiedData.fPosition[2]);


			bsUnoccupiedSync.Read(IncomingUnoccupiedData.fMoveSpeed[0]);
			bsUnoccupiedSync.Read(IncomingUnoccupiedData.fMoveSpeed[1]);
			bsUnoccupiedSync.Read(IncomingUnoccupiedData.fMoveSpeed[2]);


			bsUnoccupiedSync.Read(IncomingUnoccupiedData.fTurnSpeed[0]);
			bsUnoccupiedSync.Read(IncomingUnoccupiedData.fTurnSpeed[1]);
			bsUnoccupiedSync.Read(IncomingUnoccupiedData.fTurnSpeed[2]);


			bool deSyncUno = false;


			if (IncomingUnoccupiedData.fPosition[2] > 10000.0f)
			{
				cheat_state_text("%s is sending bad secondary data. BIG POS", g_Players->pRemotePlayer[PlayerID]->iNameAllocated <= 0xF ? g_Players->pRemotePlayer[PlayerID]->szPlayerName : g_Players->pRemotePlayer[PlayerID]->pszPlayerName);
				ReceivedPacket->data[0] = 255;
			}

			if (vect3_length(IncomingUnoccupiedData.fMoveSpeed) > 1000.0f)
				deSyncUno = true;

			if (vect3_length(IncomingUnoccupiedData.fTurnSpeed) > 10000.0f)
				deSyncUno = true;


			if (deSyncUno)
			{
				cheat_state_text("%s is sending bad secondary data.", g_Players->pRemotePlayer[PlayerID]->iNameAllocated <= 0xF ? g_Players->pRemotePlayer[PlayerID]->szPlayerName : g_Players->pRemotePlayer[PlayerID]->pszPlayerName);
				ReceivedPacket->data[0] = 255;
			}
		}
#endif // DEBUG
	}

	if (ReceivedPacketID == ID_BULLET_SYNC)
	{
		traceLastFunc("ID_BULLET_SYNC");
		if (OLCheats->bShotRepeater)
		{
			BitStream bsBulletSync((unsigned char *)ReceivedPacket->data, ReceivedPacket->length, false);

			stBulletData BulletData;
			unsigned short PlayerID;

			bsBulletSync.IgnoreBits(8);
			bsBulletSync.Read(PlayerID);
			bsBulletSync.Read((PCHAR)&BulletData, sizeof(stBulletData));
			if (BulletData.sTargetID == g_Players->sLocalPlayerID)
			{
				BulletData.sTargetID = PlayerID;
				BulletData.byteWeaponID = g_Players->pLocalPlayer->byteCurrentWeapon;
				vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12],
					BulletData.fOrigin);
				BitStream bsBullet;
				bsBullet.Write((BYTE)ID_BULLET_SYNC);
				bsBullet.Write((PCHAR)&BulletData, sizeof(stBulletData));
				g_RakFuncs->Send(&bsBullet, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
				
			}
			

		}
		if (bAntiBadBullets)
		{
			BitStream bsBulletSync((unsigned char *)ReceivedPacket->data, ReceivedPacket->length, false);

			stBulletData BulletData;
			unsigned short PlayerID;

			bsBulletSync.IgnoreBits(8);
			bsBulletSync.Read(PlayerID);
			bsBulletSync.Read((PCHAR)&BulletData, sizeof(stBulletData));

			bool deSyncBullet = false;

			if (BulletData.byteWeaponID > 46 || BulletData.byteWeaponID < 0)
				deSyncBullet = true;

			if (!_finite(BulletData.fOrigin[0]) || !_finite(BulletData.fOrigin[1]) || !_finite(BulletData.fOrigin[2]))
				deSyncBullet = true;

			if (!_finite(BulletData.fTarget[0]) || !_finite(BulletData.fTarget[1]) || !_finite(BulletData.fTarget[2]))
				deSyncBullet = true;

			if (!_finite(BulletData.fCenter[0]) || !_finite(BulletData.fCenter[1]) || !_finite(BulletData.fCenter[2]))
				deSyncBullet = true;



			if (BulletData.fOrigin[0] > 35000.0f || BulletData.fOrigin[0] < -35000.0f || BulletData.fOrigin[1] > 35000.0f || BulletData.fOrigin[1] < -35000.0f || BulletData.fOrigin[2] > 35000.0f || BulletData.fOrigin[2] < -35000.0f)
				deSyncBullet = true;

			if (BulletData.fTarget[0] > 35000.0f || BulletData.fTarget[0] < -35000.0f || BulletData.fTarget[1] > 35000.0f || BulletData.fTarget[1] < -35000.0f || BulletData.fTarget[2] > 35000.0f || BulletData.fTarget[2] < -35000.0f)
				deSyncBullet = true;

			if (deSyncBullet)
			{
				ReceivedPacket->data[0] = 255;
				cheat_state_text("%s is sending bad bullet data.", g_Players->pRemotePlayer[PlayerID]->iNameAllocated <= 0xF ? g_Players->pRemotePlayer[PlayerID]->szPlayerName : g_Players->pRemotePlayer[PlayerID]->pszPlayerName);
			}
			float fPos[3] = { 0, 0, 0 };
			int bGotPos = getPlayerPos(PlayerID, fPos);

			if (bGotPos)
			{
				if (!_finite(fPos[2]))
				{
					ReceivedPacket->data[0] = 255;
					cheat_state_text("%s tried to send bugged bullets.", g_Players->pRemotePlayer[PlayerID]->iNameAllocated <= 0xF ? g_Players->pRemotePlayer[PlayerID]->szPlayerName : g_Players->pRemotePlayer[PlayerID]->pszPlayerName);
				}
			}
			else
				ReceivedPacket->data[0] = 255;
		}
	}

	if (ReceivedPacketID == ID_MARKERS_SYNC)
	{
		BitStream	bsMarkerSync(ReceivedPacket->data, ReceivedPacket->length, false);
		int			iNumberOfPlayers = 0;
		USHORT	playerID = USHORT(-1);
		short		sPosX, sPosY, sPosZ;
		bool		bIsPlayerActive = false;
	
		bsMarkerSync.IgnoreBits(8);
		bsMarkerSync.Read(iNumberOfPlayers);
		if (iNumberOfPlayers < 0 || iNumberOfPlayers > SAMP_MAX_PLAYERS)
			return ReceivedPacket;
		//if (g_Players->pLocalPlayer->iIsActorAlive)
		//{
			for (int i = 0; i < iNumberOfPlayers; i++)
			{
				bsMarkerSync.Read(playerID);
				bsMarkerSync.ReadCompressed(bIsPlayerActive);
				if (!bIsPlayerActive)
					continue;
				bsMarkerSync.Read(sPosX);
				bsMarkerSync.Read(sPosY);
				bsMarkerSync.Read(sPosZ);
				if (isBadSAMPPlayerID(playerID))
					continue;
				g_stStreamedOutInfo.iPlayerID[playerID] = playerID;
				g_stStreamedOutInfo.fPlayerPos[playerID][0] = (float)sPosX;
				g_stStreamedOutInfo.fPlayerPos[playerID][1] = (float)sPosY;
				g_stStreamedOutInfo.fPlayerPos[playerID][2] = (float)sPosZ;
			}
		//}
	}
	return ReceivedPacket;
}


bool HookedRakClientInterface::Connect(const char* host, unsigned short serverPort, unsigned short clientPort, unsigned int depreciated, int threadSleepTimer)
{
	traceLastFunc("HookedRakClientInterface::Connect");

	return g_RakFuncs->GetInterface()->Connect(host, serverPort, clientPort, depreciated, threadSleepTimer);
}

void HookedRakClientInterface::Disconnect(unsigned int blockDuration, unsigned char orderingChannel)
{
	traceLastFunc("HookedRakClientInterface::Disconnect");

	g_RakFuncs->GetInterface()->Disconnect(blockDuration, orderingChannel);
}

void HookedRakClientInterface::InitializeSecurity(const char *privKeyP, const char *privKeyQ)
{
	traceLastFunc("HookedRakClientInterface::InitializeSecurity");

	g_RakFuncs->GetInterface()->InitializeSecurity(privKeyP, privKeyQ);
}

void HookedRakClientInterface::SetPassword(const char *_password)
{
	traceLastFunc("HookedRakClientInterface::SetPassword");

	g_RakFuncs->GetInterface()->SetPassword(_password);
}

bool HookedRakClientInterface::HasPassword(void) const
{
	traceLastFunc("HookedRakClientInterface::HasPassword");

	return g_RakFuncs->GetInterface()->HasPassword();
}

bool HookedRakClientInterface::Send(const char *data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	traceLastFunc("HookedRakClientInterface::Send");


	return g_RakFuncs->GetInterface()->Send(data, length, priority, reliability, orderingChannel);
}

void HookedRakClientInterface::DeallocatePacket(Packet2 *packet)
{
	traceLastFunc("HookedRakClientInterface::DeallocatePacket");
	
	g_RakFuncs->GetInterface()->DeallocatePacket(packet);
}

void HookedRakClientInterface::PingServer(void)
{
	traceLastFunc("HookedRakClientInterface::PingServer");

	g_RakFuncs->GetInterface()->PingServer();
}

void HookedRakClientInterface::PingServer(const char* host, unsigned short serverPort, unsigned short clientPort, bool onlyReplyOnAcceptingConnections)
{
	traceLastFunc("HookedRakClientInterface::PingServer");

	g_RakFuncs->GetInterface()->PingServer(host, serverPort, clientPort, onlyReplyOnAcceptingConnections);
}

int HookedRakClientInterface::GetAveragePing(void)
{
	traceLastFunc("HookedRakClientInterface::GetAveragePing");

	return g_RakFuncs->GetInterface()->GetAveragePing();
}

int HookedRakClientInterface::GetLastPing(void) const
{
	traceLastFunc("HookedRakClientInterface::GetLastPing");

	return g_RakFuncs->GetInterface()->GetLastPing();
}

int HookedRakClientInterface::GetLowestPing(void) const
{
	traceLastFunc("HookedRakClientInterface::GetLowestPing");

	return g_RakFuncs->GetInterface()->GetLowestPing();
}

int HookedRakClientInterface::GetPlayerPing(const PlayerID playerId)
{
	traceLastFunc("HookedRakClientInterface::GetPlayerPing");

	return g_RakFuncs->GetInterface()->GetPlayerPing(playerId);
}

void HookedRakClientInterface::StartOccasionalPing(void)
{
	traceLastFunc("HookedRakClientInterface::StartOccasionalPing");

	g_RakFuncs->GetInterface()->StartOccasionalPing();
}

void HookedRakClientInterface::StopOccasionalPing(void)
{
	traceLastFunc("HookedRakClientInterface::StopOccasionalPing");

	g_RakFuncs->GetInterface()->StopOccasionalPing();
}

bool HookedRakClientInterface::IsConnected(void) const
{
	traceLastFunc("HookedRakClientInterface::IsConnected");

	return g_RakFuncs->GetInterface()->IsConnected();
}

unsigned int HookedRakClientInterface::GetSynchronizedRandomInteger(void) const
{
	traceLastFunc("HookedRakClientInterface::GetSynchronizedRandomInteger");

	return g_RakFuncs->GetInterface()->GetSynchronizedRandomInteger();
}

bool HookedRakClientInterface::GenerateCompressionLayer(unsigned int inputFrequencyTable[256], bool inputLayer)
{
	traceLastFunc("HookedRakClientInterface::GenerateCompressionLayer");

	return g_RakFuncs->GetInterface()->GenerateCompressionLayer(inputFrequencyTable, inputLayer);
}

bool HookedRakClientInterface::DeleteCompressionLayer(bool inputLayer)
{
	traceLastFunc("HookedRakClientInterface::DeleteCompressionLayer");

	return g_RakFuncs->GetInterface()->DeleteCompressionLayer(inputLayer);
}

void HookedRakClientInterface::RegisterAsRemoteProcedureCall(int* uniqueID, void(*functionPointer) (RPCParameters *rpcParms))
{
	traceLastFunc("HookedRakClientInterface::RegisterAsRemoteProcedureCall");

	g_RakFuncs->GetInterface()->RegisterAsRemoteProcedureCall(uniqueID, functionPointer);
}

void HookedRakClientInterface::RegisterClassMemberRPC(int* uniqueID, void *functionPointer)
{
	traceLastFunc("HookedRakClientInterface::RegisterClassMemberRPC");

	g_RakFuncs->GetInterface()->RegisterClassMemberRPC(uniqueID, functionPointer);
}

void HookedRakClientInterface::UnregisterAsRemoteProcedureCall(int* uniqueID)
{
	traceLastFunc("HookedRakClientInterface::UnregisterAsRemoteProcedureCall");

	g_RakFuncs->GetInterface()->UnregisterAsRemoteProcedureCall(uniqueID);
}

bool HookedRakClientInterface::RPC(int* uniqueID, const char *data, unsigned int bitLength, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp)
{
	traceLastFunc("HookedRakClientInterface::RPC");

	return g_RakFuncs->GetInterface()->RPC(uniqueID, data, bitLength, priority, reliability, orderingChannel, shiftTimestamp);
}

void HookedRakClientInterface::SetTrackFrequencyTable(bool b)
{
	traceLastFunc("HookedRakClientInterface::SetTrackFrequencyTable");

	g_RakFuncs->GetInterface()->SetTrackFrequencyTable(b);
}

bool HookedRakClientInterface::GetSendFrequencyTable(unsigned int outputFrequencyTable[256])
{
	traceLastFunc("HookedRakClientInterface::GetSendFrequencyTable");

	return g_RakFuncs->GetInterface()->GetSendFrequencyTable(outputFrequencyTable);
}

float HookedRakClientInterface::GetCompressionRatio(void) const
{
	traceLastFunc("HookedRakClientInterface::GetCompressionRatio");

	return g_RakFuncs->GetInterface()->GetCompressionRatio();
}

float HookedRakClientInterface::GetDecompressionRatio(void) const
{
	traceLastFunc("HookedRakClientInterface::GetDecompressionRatio");

	return g_RakFuncs->GetInterface()->GetDecompressionRatio();
}

void HookedRakClientInterface::AttachPlugin(void *messageHandler)
{
	traceLastFunc("HookedRakClientInterface::AttachPlugin");

	g_RakFuncs->GetInterface()->AttachPlugin(messageHandler);
}

void HookedRakClientInterface::DetachPlugin(void *messageHandler)
{
	traceLastFunc("HookedRakClientInterface::DetachPlugin");

	g_RakFuncs->GetInterface()->DetachPlugin(messageHandler);
}

BitStream * HookedRakClientInterface::GetStaticServerData(void)
{
	traceLastFunc("HookedRakClientInterface::GetStaticServerData");

	return g_RakFuncs->GetInterface()->GetStaticServerData();
}

void HookedRakClientInterface::SetStaticServerData(const char *data, const int length)
{
	traceLastFunc("HookedRakClientInterface::SetStaticServerData");

	g_RakFuncs->GetInterface()->SetStaticServerData(data, length);
}

BitStream * HookedRakClientInterface::GetStaticClientData(const PlayerID playerId)
{
	traceLastFunc("HookedRakClientInterface::GetStaticClientData");

	return g_RakFuncs->GetInterface()->GetStaticClientData(playerId);
}

void HookedRakClientInterface::SetStaticClientData(const PlayerID playerId, const char *data, const int length)
{
	traceLastFunc("HookedRakClientInterface::SetStaticClientData");

	g_RakFuncs->GetInterface()->SetStaticClientData(playerId, data, length);
}

void HookedRakClientInterface::SendStaticClientDataToServer(void)
{
	traceLastFunc("HookedRakClientInterface::SendStaticClientDataToServer");

	g_RakFuncs->GetInterface()->SendStaticClientDataToServer();
}

PlayerID HookedRakClientInterface::GetServerID(void) const
{
	traceLastFunc("HookedRakClientInterface::GetServerID");

	return g_RakFuncs->GetInterface()->GetServerID();
}

PlayerID HookedRakClientInterface::GetPlayerID(void) const
{
	traceLastFunc("HookedRakClientInterface::GetPlayerID");

	return g_RakFuncs->GetInterface()->GetPlayerID();
}

PlayerID HookedRakClientInterface::GetInternalID(void) const
{
	traceLastFunc("HookedRakClientInterface::GetInternalID");

	return g_RakFuncs->GetInterface()->GetInternalID();
}

const char* HookedRakClientInterface::PlayerIDToDottedIP(const PlayerID playerId) const
{
	traceLastFunc("HookedRakClientInterface::PlayerIDToDottedIP");

	return g_RakFuncs->GetInterface()->PlayerIDToDottedIP(playerId);
}

void HookedRakClientInterface::PushBackPacket(Packet2 *packet, bool pushAtHead)
{
	traceLastFunc("HookedRakClientInterface::PushBackPacket");

	g_RakFuncs->GetInterface()->PushBackPacket(packet, pushAtHead);
}

void HookedRakClientInterface::SetRouterInterface(void *routerInterface)
{
	traceLastFunc("HookedRakClientInterface::SetRouterInterface");

	g_RakFuncs->GetInterface()->SetRouterInterface(routerInterface);
}
void HookedRakClientInterface::RemoveRouterInterface(void *routerInterface)
{
	traceLastFunc("HookedRakClientInterface::RemoveRouterInterface");

	g_RakFuncs->GetInterface()->RemoveRouterInterface(routerInterface);
}

void HookedRakClientInterface::SetTimeoutTime(RakNetTime timeMS)
{
	traceLastFunc("HookedRakClientInterface::SetTimeoutTime");

	g_RakFuncs->GetInterface()->SetTimeoutTime(timeMS);
}

bool HookedRakClientInterface::SetMTUSize(int size)
{
	traceLastFunc("HookedRakClientInterface::SetMTUSize");

	return g_RakFuncs->GetInterface()->SetMTUSize(size);
}

int HookedRakClientInterface::GetMTUSize(void) const
{
	traceLastFunc("HookedRakClientInterface::GetMTUSize");

	return g_RakFuncs->GetInterface()->GetMTUSize();
}

void HookedRakClientInterface::AllowConnectionResponseIPMigration(bool allow)
{
	traceLastFunc("HookedRakClientInterface::AllowConnectionResponseIPMigration");

	g_RakFuncs->GetInterface()->AllowConnectionResponseIPMigration(allow);
}

void HookedRakClientInterface::AdvertiseSystem(const char *host, unsigned short remotePort, const char *data, int dataLength)
{
	traceLastFunc("HookedRakClientInterface::AdvertiseSystem");

	g_RakFuncs->GetInterface()->AdvertiseSystem(host, remotePort, data, dataLength);
}

RakNetStatisticsStruct* const HookedRakClientInterface::GetStatistics(void)
{
	traceLastFunc("HookedRakClientInterface::GetStatistics");

	return g_RakFuncs->GetInterface()->GetStatistics();
}

void HookedRakClientInterface::ApplyNetworkSimulator(double maxSendBPS, unsigned short minExtraPing, unsigned short extraPingVariance)
{
	traceLastFunc("HookedRakClientInterface::ApplyNetworkSimulator");

	g_RakFuncs->GetInterface()->ApplyNetworkSimulator(maxSendBPS, minExtraPing, extraPingVariance);
}

bool HookedRakClientInterface::IsNetworkSimulatorActive(void)
{
	traceLastFunc("HookedRakClientInterface::IsNetworkSimulatorActive");

	return g_RakFuncs->GetInterface()->IsNetworkSimulatorActive();
}

PlayerIndex HookedRakClientInterface::GetPlayerIndex(void)
{
	traceLastFunc("HookedRakClientInterface::GetPlayerIndex");

	return g_RakFuncs->GetInterface()->GetPlayerIndex();
}

bool HookedRakClientInterface::RPC_(int* uniqueID, BitStream *bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID)
{
	traceLastFunc("HookedRakClientInterface::RPC_");

	return g_RakFuncs->GetInterface()->RPC_(uniqueID, bitStream, priority, reliability, orderingChannel, shiftTimestamp, networkID);
}