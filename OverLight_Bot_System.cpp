#include "main.h"

CBotFuncs *g_BotFuncs = new CBotFuncs();

CBotFuncs::CBotFuncs()
{
	memset(&BotClient, 0, sizeof(stBot));
	memset(&BotSettings, 0, sizeof(stBotSettings));
	memset(uiIsBot, 0, sizeof(uiIsBot));
	BotSettings.UpdateSendRate = 5;
	N_Client_to_Add_In_Queue = 5;
	Client_in_Queue = 0;
	BotSettings.fDistanceFollow = -0.8f;
	BotSettings.fDistanceFollowBetweenBots = 0.8f;
	BotSettings.fMultiplierSilent = 9.0f;
	//BotSettings.bCopyHealth = true;
	//BotSettings.bCopyArmor = true;
	BotSettings.bCopyWeapon = true;
	BotSettings.sTargetID = -1;
	BotSettings.uiAttackWeaponID = 27;
	BotSettings.NicknameStyle = 2;
	BotSettings.CurrentClientID = 0;
	BotSettings.bUseNearestTarget = true;
	BotSettings.sBotFollowAnimID = 881;

}

void CBotFuncs::OL_Add_Bot_Queue()
{
	traceLastFunc("OL_Add_Bot_Queue()");
	if (BotClient == nullptr || g_SAMP == nullptr || g_Players == nullptr)
		return;

	if (BotSettings.ClientCount != NULL)
	{
		if (!BotClient[BotSettings.ClientCount - 1].bJoined)
		{
			Client_in_Queue++;
		}
		else
		{
			OL_Add_Bot();
		}
	}
	else
	{
		OL_Add_Bot();
	}
}

void CBotFuncs::OL_Delete_Bot()
{
	if (BotClient == nullptr || g_SAMP == nullptr || g_Players == nullptr)
		return;
	if (BotSettings.ClientCount <= 0 || BotSettings.ClientJoined == 0)
	{
		BotSettings.ClientCount = 0;
		BotSettings.ClientJoined = 0;
		BotSettings.CurrentClientID = 0;
		cheat_state_text("All bots are deleted.");
		return;
	}
	BotClient[BotSettings.ClientCount - 1].pRakClient->Disconnect(1);
	RakNetworkFactory::
		DestroyRakClientInterface(BotClient[BotSettings.ClientCount - 1].
			pRakClient);
	uiIsBot[BotClient[BotSettings.ClientCount - 1].playerID][0] = 0;
	memset(&BotClient[BotSettings.ClientCount - 1], 0, sizeof(stBot));
	//BotClient[BotSettings.ClientCount - 1].bConnected = 0;
	//BotClient[BotSettings.ClientCount - 1].bJoined = 0;
	//BotClient[BotSettings.ClientCount - 1].pRakClient = 0;
	//BotClient[BotSettings.ClientCount - 1].playerID = 0;
	//BotClient[BotSettings.ClientCount - 1].uiChallenge = 0;
	BotSettings.ClientCount--;
	BotSettings.ClientJoined--;
	BotSettings.CurrentClientID = 0;
}

bool CBotFuncs::OL_Add_Bot_Nick(char *nick)
{
	traceLastFunc("OL_Add_Bot()");
	if (BotClient == nullptr || g_SAMP == nullptr || g_Players == nullptr)
		return false;
	sprintf(BotClient[BotSettings.ClientCount].NickName, nick);

	BotClient[BotSettings.ClientCount].pRakClient =
		RakNetworkFactory::GetRakClientInterface();

	if (BotClient[BotSettings.ClientCount].pRakClient == nullptr)
	{
		cheat_state_text("Problem With RakNet ptr");
		Log("Problem With RakNet ptr");
		return false;
	}
	if (!BotClient[BotSettings.ClientCount].pRakClient->Connect(g_SAMP->szIP,
		g_SAMP->ulPort,
		0, 0, 0))
	{
		cheat_state_text("Bot Client can't connect");
		Log("Bot Client can't connect");
		return false;
	}
	cheat_state_text("Sent connection request for client N. %d",
		BotSettings.ClientCount);
	BotClient[BotSettings.ClientCount].bConnected = true;
	BotSettings.ClientCount++;
	BotSettings.CurrentClientID = 0;
	return true;
}

bool CBotFuncs::OL_Add_Bot()
{
	traceLastFunc("OL_Add_Bot()");
	if (BotClient == nullptr || g_SAMP == nullptr || g_Players == nullptr)
		return false;
	if (BotSettings.UseFakeBot)
	{
		sprintf(BotClient[BotSettings.ClientCount].NickName, "%s_%d",
			getPlayerName(OLCheats->Target_PlayerID),
			BotSettings.ClientCount);
	}
	else
	{
		if (BotSettings.NicknameStyle == ID_NICK_LOCAL)
		{
			sprintf(BotClient[BotSettings.ClientCount].NickName, "%s_%d",
				getPlayerName(g_Players->sLocalPlayerID),
				BotSettings.ClientCount);
		}
		if (BotSettings.NicknameStyle == ID_NICK_REALISTIC)
		{
			char Tag[3 + 1];
			char InitialNick[2];
			char Nick[4 + 1];
			char finalNick[2 + 1];
			gen_randomuppercase(Tag, 3);
			gen_randomuppercase(InitialNick, 1);
			gen_randomletter(Nick, 4);
			gen_randomnumber(finalNick, 2);

			sprintf(BotClient[BotSettings.ClientCount].NickName, "[%s]%s%s%s",
				Tag, InitialNick, Nick, finalNick);
		}
		if (BotSettings.NicknameStyle == ID_NICK_BINARY)
		{
			char Nick[25];
			gen_randombinary(Nick, 20);
			sprintf(BotClient[BotSettings.ClientCount].NickName, "%s", Nick);
		}

	}
	BotClient[BotSettings.ClientCount].pRakClient =
		RakNetworkFactory::GetRakClientInterface();

	if (BotClient[BotSettings.ClientCount].pRakClient == nullptr)
	{
		cheat_state_text("Problem With RakNet ptr");
		Log("Problem With RakNet ptr");
		return false;
	}
	if (!BotClient[BotSettings.ClientCount].pRakClient->Connect(g_SAMP->szIP,
		g_SAMP->ulPort,
		0, 0, 0))
	{
		cheat_state_text("Bot Client can't connect");
		Log("Bot Client can't connect");
		return false;
	}
	cheat_state_text("Sent connection request for client N. %d",
		BotSettings.ClientCount);
	BotClient[BotSettings.ClientCount].bConnected = true;
	g_BotFuncs->BotClient[BotSettings.CurrentClientID].fHealth = 100.0f;
	BotSettings.ClientCount++;
	BotSettings.CurrentClientID = 0;
	return true;
}

void CBotFuncs::OL_Bot_Process()
{
	traceLastFunc("OL_Bot_Process() 1");
	if (!BotClient)		// || g_SAMP == nullptr || g_Players == nullptr)
		return;
	traceLastFunc("OL_Bot_Process() 2");
	//return;
	if (Client_in_Queue >= 1)
	{
		if (BotSettings.ClientCount == NULL)
		{
			Client_in_Queue--;
			OL_Add_Bot();
		}
		else if (BotClient[BotSettings.ClientCount - 1].bJoined)
		{
			Client_in_Queue--;
			OL_Add_Bot();
		}
	}
	uint8_t ClientID = BotSettings.CurrentClientID;
	traceLastFunc("OL_Bot_Process() 3");
	if (BotClient[ClientID].bConnected == false
		|| BotClient[ClientID].pRakClient == nullptr)
		return;
	OL_Bot_ReceiveRPC(&BotClient[ClientID].pRakClient);

	static DWORD dwUpdateTick = GetTickCount();

	if (GetTickCount() - dwUpdateTick >= BotSettings.UpdateSendRate)
	{

		traceLastFunc("OL_Bot_Process() 4");
		Packet *packet;
		unsigned char packetID;
		traceLastFunc("OL_Bot_Process() 5");
		for (packet = BotClient[ClientID].pRakClient->Receive();
			packet;
			BotClient[ClientID].pRakClient->DeallocatePacket(packet),
			packet = BotClient[ClientID].pRakClient->Receive())
		{
			if ((unsigned char)packet->data[0] == ID_TIMESTAMP)
			{
				if (packet->length > sizeof(unsigned char) + sizeof(unsigned int))
					packetID =
					(unsigned char)packet->data[sizeof(unsigned char) +
					sizeof(unsigned int)];
				else
				{
					continue;
				}
				//return;
			}
			else
				packetID = (unsigned char)packet->data[0];
			//cheat_state_text("Packet ID: %d", packetID);
			switch (packetID)
			{
			case ID_DISCONNECTION_NOTIFICATION:
				cheat_state_text("Server closed connection. Client ID: %d.",
					ClientID);
				Log("Server closed connection. Client ID: %d.", ClientID);
				for (int i = g_BotFuncs->BotSettings.ClientCount; i > 0; i--)
					g_BotFuncs->OL_Delete_Bot();
				goto exit_loop;
				break;
			case ID_CONNECTION_BANNED:
				cheat_state_text("You are banned from this server. Client ID: %d.",
					ClientID);
				Log("You are banned from this server. Client ID: %d.", ClientID);
				for (int i = g_BotFuncs->BotSettings.ClientCount; i > 0; i--)
					g_BotFuncs->OL_Delete_Bot();
				goto exit_loop;
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				cheat_state_text("Connection attempt failed. Client ID: %d.",
					ClientID);
				Log("Connection attempt failed. Client ID: %d.", ClientID);
				for (int i = g_BotFuncs->BotSettings.ClientCount; i > 0; i--)
					g_BotFuncs->OL_Delete_Bot();
				goto exit_loop;
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				cheat_state_text("Server Full. Client ID: %d.", ClientID);
				Log("Server Full. Client ID: %d.", ClientID);
				for (int i = g_BotFuncs->BotSettings.ClientCount; i > 0; i--)
					g_BotFuncs->OL_Delete_Bot();
				goto exit_loop;
				break;
			case ID_INVALID_PASSWORD:
				cheat_state_text("Password is Incorrect. Client ID: %d.",
					ClientID);
				Log("Password is Incorrect. Client ID: %d.", ClientID);
				for (int i = g_BotFuncs->BotSettings.ClientCount; i > 0; i--)
					g_BotFuncs->OL_Delete_Bot();
				goto exit_loop;
				break;
			case ID_CONNECTION_LOST:
				cheat_state_text("Server lost connection. Client ID: %d.",
					ClientID);
				Log("Server lost connection. Client ID: %d.", ClientID);
				for (int i = g_BotFuncs->BotSettings.ClientCount; i > 0; i--)
					g_BotFuncs->OL_Delete_Bot();
				goto exit_loop;
				break;
			case ID_AUTH_KEY:
				OL_Packet_Auth_Key(packet, ClientID);
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				OL_Packet_ConnectionAccepted(packet, ClientID);
				break;
			case ID_RECEIVED_STATIC_DATA:
				BotClient[ClientID].pRakClient->SendStaticClientDataToServer();
				break;
			case ID_MODIFIED_PACKET: // i receive this before ban, maybe is useless
				Client_in_Queue = 0;
				for (int i = g_BotFuncs->BotSettings.ClientCount; i > 0; i--)
					g_BotFuncs->OL_Delete_Bot();
				cheat_state_text
				("Bot Clients disconnected to prevent timeout/ban");
				Log("Bot Clients disconnected to prevent timeout/ban");
				goto exit_loop;
				break;

			case ID_AIM_SYNC:
				if (BotSettings.bFollow)
					OL_Packet_AimSync(packet, ClientID);
				break;

			case ID_BULLET_SYNC:
				if (BotSettings.bFollow)
					OL_Packet_BulletSync(packet, ClientID);
				break;
				//case ID_MARKERS_SYNC:
				//      OL_Packet_MarkersSync(packet, ClientID);
				//      break;
				//
			case ID_PLAYER_SYNC:
				OL_Packet_PlayerSync(packet, ClientID);
				break;
			}
		}

		if (g_BotFuncs->BotClient[ClientID].bJoined)
		{
			if (g_BotFuncs->BotSettings.bUseNearestTarget)
				g_BotFuncs->BotSettings.sTargetID = NearestPlayer();

			if (g_BotFuncs->BotSettings.bFollow)
				g_BotFuncs->OL_Bot_Follow();

			if (g_BotFuncs->BotSettings.bShooter)
				g_BotFuncs->OL_Bot_Shooter();

			if (g_BotFuncs->BotSettings.bBotSilent)
				g_BotFuncs->OL_Bot_Silent();
			if (g_BotFuncs->BotSettings.bBotAttacker)
				g_BotFuncs->OL_Bot_Attacker();
			if (g_BotFuncs->BotSettings.bBotElevator
				|| g_BotFuncs->BotSettings.bBotJetpackElevator)
				g_BotFuncs->OL_Bot_Elevator();
			if (g_BotFuncs->BotSettings.bBotSlapper)
				g_BotFuncs->OL_Bot_Slapper();
			if (g_BotFuncs->BotSettings.bBotFuck)
				g_BotFuncs->OL_Bot_Fuck();
			if (g_BotFuncs->BotSettings.bBotPick)
				g_BotFuncs->OL_Bot_Pick();
			if (g_BotFuncs->BotSettings.bBotFollowAnim)
				g_BotFuncs->OL_Bot_Follow_Anim();


			if (g_BotFuncs->BotSettings.bUIF_DM_Farmer)
				g_BotFuncs->OL_UIF_DM_Farmer();
			if (g_BotFuncs->BotSettings.bBotSun)
				g_BotFuncs->OL_Bot_Sun();

			g_BotFuncs->OL_Bot_Die();
			if (g_BotFuncs->BotSettings.bStorm)
				g_BotFuncs->OL_Bot_Storm();
		}
		BotSettings.CurrentClientID++;
		dwUpdateTick = GetTickCount();
	}
exit_loop:;
	if (BotSettings.CurrentClientID >= BotSettings.ClientCount)
	{
		BotSettings.CurrentClientID = 0;
	}

}

void CBotFuncs::OL_Bot_Door_Storm()	// for test this funk
{
	static short vehicleID = g_Players->pLocalPlayer->sCurrentVehicleID;

	stOnFootData OnFoot;
	memset(&OnFoot, 0, sizeof(stOnFootData));
	OnFoot.byteHealth =
		g_BotFuncs->BotClient[BotSettings.CurrentClientID].fHealth;
	OnFoot.byteArmor =
		g_BotFuncs->BotClient[BotSettings.CurrentClientID].fArmour;
	vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.
		matrix[12], OnFoot.fPosition);
	OnFoot.sSurfingVehicleID = vehicleID;
	OnFoot.fSurfingOffsets[2] = -100.0f;
	BitStream bsOnFoot;
	bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
	bsOnFoot.Write((PCHAR)& OnFoot, sizeof(stOnFootData));
	g_BotFuncs->BotClient[BotSettings.CurrentClientID].pRakClient->
		Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

	stUnoccupiedData Unoccupied;
	memset(&Unoccupied, 0, sizeof(stUnoccupiedData));
	vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.
		matrix[12], Unoccupied.fPosition);
	Unoccupied.fPosition[2] += 10.0f;
	Unoccupied.sVehicleID = vehicleID;
	Unoccupied.fHealth = 1000.0f;
	Unoccupied.fRoll[0] = 1.0f;
	Unoccupied.fDirection[1] = 1.0f;
	Unoccupied.fTurnSpeed[2] = 4.0f;
	Unoccupied.byteSeatID = 1;
	BitStream bsUnoccupied;
	bsUnoccupied.Write((BYTE)ID_UNOCCUPIED_SYNC);
	bsUnoccupied.Write((PCHAR)& Unoccupied, sizeof(stUnoccupiedData));
	g_BotFuncs->BotClient[BotSettings.CurrentClientID].pRakClient->
		Send(&bsUnoccupied, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);


	static int iDoorDamage = 0, iBumperDamage = 0;
	if (iDoorDamage == 0)
	{
		iDoorDamage = encode_doors(4, 4, 4, 4);
		iBumperDamage = encode_panels(3, 3, 3, 3, 3, 3, 3);
	}
	else
	{
		iDoorDamage = 0;
		iBumperDamage = 0;
	}
	BitStream bsDamageVehicle;
	bsDamageVehicle.Write(vehicleID);
	bsDamageVehicle.Write(iBumperDamage);	//(panel);
	bsDamageVehicle.Write(iDoorDamage);	//(door);
	bsDamageVehicle.Write(0);	//(lights);
	bsDamageVehicle.Write(0);	//tires
	g_BotFuncs->OL_Bot_SendRPC(BotSettings.CurrentClientID, RPC_DamageVehicle,
		bsDamageVehicle, HIGH_PRIORITY,
		UNRELIABLE_SEQUENCED, 0, FALSE);
}

void CBotFuncs::OL_Bot_Die()	//something is wrong
{
	if (g_BotFuncs->BotClient[BotSettings.CurrentClientID].fHealth <= 0)
	{
		cheat_state_text("bot died");
		Log("bot died");
		BitStream bsDeath;
		bsDeath.Write((BYTE)-1);
		bsDeath.Write((USHORT)-1);
		OL_Bot_SendRPC(BotSettings.CurrentClientID, RPC_Death, bsDeath,
			HIGH_PRIORITY, RELIABLE_SEQUENCED, NULL, false);
		g_BotFuncs->BotClient[BotSettings.CurrentClientID].fHealth = 100.0f;
		OL_Spawn_Bot(BotSettings.CurrentClientID);
	}
}


void CBotFuncs::OL_Bot_ReceiveRPC(RakClientInterface ** pRakClient)
{
	int RPC_SetHealth = RPC_SetPlayerHealth;
	int RPC_SetArmour = RPC_SetPlayerArmour;
	pRakClient[0]->RegisterAsRemoteProcedureCall(&RPC_SetHealth,
		OL_Bot_SetHealth);
	pRakClient[0]->RegisterAsRemoteProcedureCall(&RPC_SetArmour,
		OL_Bot_SetArmour);
}

void OL_Bot_SetHealth(RPCParameters * params)
{
	BitStream bsData(params->input, params->numberOfBitsOfData / 8, false);
	bsData.Read(g_BotFuncs->BotClient[g_BotFuncs->BotSettings.CurrentClientID].
		fHealth);
}

void OL_Bot_SetArmour(RPCParameters * params)
{
	BitStream bsData(params->input, params->numberOfBitsOfData / 8, false);
	bsData.Read(g_BotFuncs->BotClient[g_BotFuncs->BotSettings.CurrentClientID].
		fArmour);
}


void CBotFuncs::OL_Bot_Sun()
{
	float fPos[3], fSpeed[3];
	static float Angle = 0;
	vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.
		matrix[12], fPos);
	fPos[2] += 5.0f;
	float AbsSpeed = 50.0f;

	fSpeed[0] = AbsSpeed * cos(Angle);
	fSpeed[1] = AbsSpeed * sin(Angle);

	Angle += DEGTORAD(115);

	OL_Bot_Send_OnFoot(fPos, fSpeed, false);
}





// You can get weapon while u have godmode (by server), lmaoo
// Released on public for error(my mistake), sorry for that :c
// Fixed after UIF Build 113
void CBotFuncs::OL_UIF_DM_Farmer()
{
	BitStream bsSendCommand;

	if (GetTickCount() -
		BotClient[BotSettings.CurrentClientID].dwSendBulletTick > 2000)
	{
		BitStream bsGiveDamage;
		bsGiveDamage.Write(true);
		bsGiveDamage.Write(g_Players->sLocalPlayerID);
		bsGiveDamage.Write(10);
		bsGiveDamage.Write(g_Players->pLocalPlayer->byteCurrentWeapon);
		bsGiveDamage.Write(3);
		OL_Bot_SendRPC(BotSettings.CurrentClientID, RPC_GiveTakeDamage,
			bsGiveDamage, HIGH_PRIORITY, RELIABLE_SEQUENCED, NULL,
			false);

		BitStream bsSendCommand;
		char commands[10][10] = {
			"/ac",
			"/a51",
			"/gcdm",
			"/karate",
			"/rocket",
			"/mini",
			"/mini2",
			"/gren",
			"/mf",
			"/de"
		};

		char *command = commands[rand() % 10];

		int iStrlen = strlen(command);
		bsSendCommand.Write(iStrlen);
		bsSendCommand.Write(command, iStrlen);
		OL_Bot_SendRPC(BotSettings.CurrentClientID, RPC_ServerCommand,
			bsSendCommand, HIGH_PRIORITY, RELIABLE, 0, FALSE);
		BotClient[BotSettings.CurrentClientID].dwSendBulletTick =
			GetTickCount();
	}
}

void CBotFuncs::OL_Bot_Fuck()
{
	short playerID = BotSettings.sTargetID;
	//      float fQuaternion[4];
	if (IsPlayerStreamed(playerID))
	{
		stOnFootData OnFoot;
		memset(&OnFoot, 0, sizeof(stOnFootData));
		OnFoot.byteHealth =
			g_BotFuncs->BotClient[BotSettings.CurrentClientID].fHealth;
		OnFoot.byteArmor =
			g_BotFuncs->BotClient[BotSettings.CurrentClientID].fArmour;
		vect3_copy(&g_Players->pRemotePlayer[playerID]->pPlayerData->
			pSAMP_Actor->pGTA_Ped->base.matrix[12], OnFoot.fPosition);
		vect3_copy(g_Players->pRemotePlayer[playerID]->pPlayerData->
			pSAMP_Actor->pGTA_Ped->speed, OnFoot.fMoveSpeed);
		//vect4_copy(g_Players->pRemotePlayer[playerID]->pPlayerData->onFootData.fQuaternion, OnFoot.fQuaternion);
		GetPlayerQuaternion(playerID, OnFoot.fQuaternion);
		OnFoot.fPosition[0] +=
			-g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->
			pGTAEntity->base.matrix[4] * 0.3f;
		OnFoot.fPosition[1] +=
			-g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->
			pGTAEntity->base.matrix[5] * 0.3f;
		OnFoot.fPosition[2] +=
			-g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->
			pGTAEntity->base.matrix[6] * 0.3f;
		OnFoot.fPosition[2] +=
			-g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->
			pGTAEntity->base.matrix[6] * 0.3f;
		OnFoot.fPosition[0] -=
			(float)BotSettings.CurrentClientID *
			g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->
			pGTAEntity->base.matrix[4] * 0.3f;
		OnFoot.fPosition[1] -=
			(float)BotSettings.CurrentClientID *
			g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->
			pGTAEntity->base.matrix[5] * 0.3f;
		OnFoot.fPosition[2] -=
			(float)BotSettings.CurrentClientID *
			g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->
			pGTAEntity->base.matrix[6] * 0.3f;
		OnFoot.sCurrentAnimationID = 1666;
		OnFoot.sAnimFlags = 4356;
		OnFoot.byteSpecialAction = 3;
		OnFoot.fMoveSpeed[2] = -0.01f;
		BitStream bsOnFoot;
		bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
		bsOnFoot.Write((PCHAR)& OnFoot, sizeof(stOnFootData));
		g_BotFuncs->BotClient[BotSettings.CurrentClientID].pRakClient->
			Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
}

void CBotFuncs::OL_Bot_Follow_Anim()
{
	short playerID = BotSettings.sTargetID;
	if (IsPlayerStreamed(playerID))
	{
		stOnFootData OnFoot;
		memset(&OnFoot, 0, sizeof(stOnFootData));
		OnFoot.byteHealth =
			g_BotFuncs->BotClient[BotSettings.CurrentClientID].fHealth;
		OnFoot.byteArmor =
			g_BotFuncs->BotClient[BotSettings.CurrentClientID].fArmour;
		vect3_copy(&g_Players->pRemotePlayer[playerID]->pPlayerData->
			pSAMP_Actor->pGTA_Ped->base.matrix[12], OnFoot.fPosition);
		vect3_copy(g_Players->pRemotePlayer[playerID]->pPlayerData->
			pSAMP_Actor->pGTA_Ped->speed, OnFoot.fMoveSpeed);
		//vect4_copy(g_Players->pRemotePlayer[playerID]->pPlayerData->onFootData.fQuaternion, OnFoot.fQuaternion);
		GetPlayerQuaternion(playerID, OnFoot.fQuaternion);
		OnFoot.fPosition[0] +=
			-g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->
			pGTAEntity->base.matrix[4] * 0.3f;
		OnFoot.fPosition[1] +=
			-g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->
			pGTAEntity->base.matrix[5] * 0.3f;
		OnFoot.fPosition[2] +=
			-g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->
			pGTAEntity->base.matrix[6] * 0.3f;
		OnFoot.fPosition[0] -=
			(float)BotSettings.CurrentClientID *
			g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->
			pGTAEntity->base.matrix[4] * 0.3f;
		OnFoot.fPosition[1] -=
			(float)BotSettings.CurrentClientID *
			g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->
			pGTAEntity->base.matrix[5] * 0.3f;
		OnFoot.fPosition[2] -=
			(float)BotSettings.CurrentClientID *
			g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->
			pGTAEntity->base.matrix[6] * 0.3f;
		OnFoot.sCurrentAnimationID = BotSettings.sBotFollowAnimID;
		OnFoot.sAnimFlags = 4356;
		OnFoot.byteSpecialAction = 3;
		OnFoot.fMoveSpeed[2] = -0.01f;
		BitStream bsOnFoot;
		bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
		bsOnFoot.Write((PCHAR)& OnFoot, sizeof(stOnFootData));
		g_BotFuncs->BotClient[BotSettings.CurrentClientID].pRakClient->
			Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
}

void CBotFuncs::OL_Bot_Storm()
{
	float fRange = 400.0f;
	float fSpeed = 99.0f;
	stOnFootData OnFoot;
	memset(&OnFoot, 0, sizeof(stOnFootData));
	OnFoot.byteHealth = BotClient[BotSettings.CurrentClientID].fHealth;
	vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.
		matrix[12], OnFoot.fPosition);
	OnFoot.fPosition[0] +=
		(fRange / 2.f) - (float)rand() / (float)(RAND_MAX / fRange);
	OnFoot.fPosition[1] +=
		(fRange / 2.f) - (float)rand() / (float)(RAND_MAX / fRange);
	OnFoot.fMoveSpeed[0] +=
		(fSpeed / 2.f) - (float)rand() / (float)(RAND_MAX / fSpeed);
	OnFoot.fMoveSpeed[1] +=
		(fSpeed / 2.f) - (float)rand() / (float)(RAND_MAX / fSpeed);
	OnFoot.fMoveSpeed[2] +=
		(fSpeed / 2.f) - (float)rand() / (float)(RAND_MAX / fSpeed);
	OnFoot.fQuaternion[0] = 1.0f - (float)rand() / (float)(RAND_MAX / 2.0f);
	OnFoot.fQuaternion[1] = 1.0f - (float)rand() / (float)(RAND_MAX / 2.0f);
	OnFoot.fQuaternion[2] = 1.0f - (float)rand() / (float)(RAND_MAX / 2.0f);
	OnFoot.fQuaternion[3] = 1.0f - (float)rand() / (float)(RAND_MAX / 2.0f);
	OnFoot.sCurrentAnimationID = rand() % 2000;
	OnFoot.sAnimFlags = 4356;
	BitStream bsOnFoot;
	bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
	bsOnFoot.Write((PCHAR)& OnFoot, sizeof(stOnFootData));
	g_BotFuncs->BotClient[BotSettings.CurrentClientID].pRakClient->
		Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void CBotFuncs::OL_Bot_Slapper()
{
	short playerID = BotSettings.sTargetID;
	static int state = 0;
	if (IsPlayerStreamed(playerID))
	{
		stOnFootData OnFoot;
		memset(&OnFoot, 0, sizeof(stOnFootData));
		OnFoot.byteHealth = BotClient[BotSettings.CurrentClientID].fHealth;
		vect3_copy(&g_Players->pRemotePlayer[playerID]->pPlayerData->
			pSAMP_Actor->pGTA_Ped->base.matrix[12], OnFoot.fPosition);
		OnFoot.sCurrentAnimationID = 1666;
		OnFoot.sAnimFlags = 4356;
		switch (state)
		{
		case 0:
			//OnFoot.fPosition[0] += -10.0f;
			//OnFoot.fPosition[2] += 0.25f;
			OnFoot.fMoveSpeed[0] = -(10.0f + (rand() % 999) / 100.0f);
			OnFoot.fMoveSpeed[1] = 0;
			OnFoot.fMoveSpeed[2] = 0.25f;
			state++;
			break;
		case 1:
			//OnFoot.fPosition[1] += 10.0f;
			//OnFoot.fPosition[2] += 0.25f;
			OnFoot.fMoveSpeed[0] = 0;
			OnFoot.fMoveSpeed[1] = (10.0f + (rand() % 999) / 100.0f);
			OnFoot.fMoveSpeed[2] = 0.25f;
			state++;
			break;
		case 2:
			//OnFoot.fPosition[0] += -10.0f;
			//OnFoot.fPosition[2] += 0.25f;
			OnFoot.fMoveSpeed[0] = (10.0f + (rand() % 999) / 100.0f);
			OnFoot.fMoveSpeed[1] = 0;
			OnFoot.fMoveSpeed[2] = 0.25f;
			state++;
			break;
		case 3:
			//OnFoot.fPosition[1] += 10.0f;
			//OnFoot.fPosition[2] += 0.25f;
			OnFoot.fMoveSpeed[0] = 0;
			OnFoot.fMoveSpeed[1] = -(10.0f + (rand() % 999) / 100.0f);
			OnFoot.fMoveSpeed[2] = 0.25f;
			state = 0;
			break;
		}
		BitStream bsOnFoot;
		bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
		bsOnFoot.Write((PCHAR)& OnFoot, sizeof(stOnFootData));
		g_BotFuncs->BotClient[BotSettings.CurrentClientID].pRakClient->
			Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
}

void CBotFuncs::OL_Bot_Pick()
{
	render->D3DBox((pPresentParam.BackBufferWidth / 2) - 20.0f,
		(pPresentParam.BackBufferHeight / 2) - 1.0f, 40.0f, 2.0f,
		0xFF00FFFF);
	render->D3DBox((pPresentParam.BackBufferWidth / 2) - 1.0f,
		(pPresentParam.BackBufferHeight / 2) - 20.0f, 2.0f, 40.0f,
		0xFF00FFFF);
	static bool bVehiclePicked = false;
	//static int BotID = 0;
	stOnFootData OnFootData;
	memset(&OnFootData, 0, sizeof(stOnFootData));
	OnFootData.byteHealth =
		g_BotFuncs->BotClient[BotSettings.CurrentClientID].fHealth;
	OnFootData.byteArmor =
		g_BotFuncs->BotClient[BotSettings.CurrentClientID].fArmour;
	OnFootData.sCurrentAnimationID =
		g_BotFuncs->BotClient[BotSettings.CurrentClientID].sCurrentAnimationID;
	OnFootData.byteSpecialAction =
		g_BotFuncs->BotClient[BotSettings.CurrentClientID].byteSpecialAction;
	OnFootData.sAnimFlags = 4356;
	//for fluid work
	OnFootData.fMoveSpeed[2] = -0.01f;

	if (KEY_DOWN(VK_RBUTTON))
	{
		if (g_NewModSa->iAimClientBot == BotSettings.CurrentClientID
			|| bVehiclePicked)
		{
			bVehiclePicked = true;
			vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.
				matrix[12], OnFootData.fPosition);
			OnFootData.fPosition[0] += cam_matrix[4] * 3.0f;
			OnFootData.fPosition[1] += cam_matrix[5] * 3.0f;
			OnFootData.fPosition[2] += cam_matrix[6] * 3.0f;
			vect3_copy(OnFootData.fPosition,
				g_BotFuncs->BotClient[BotSettings.CurrentClientID].
				fLastOnFootPos);

			D3DXQUATERNION Quat;
			D3DXQUATERNION Quat2;
			Quat.w = g_Players->pLocalPlayer->onFootData.fQuaternion[0];
			Quat.x = g_Players->pLocalPlayer->onFootData.fQuaternion[1];
			Quat.y = g_Players->pLocalPlayer->onFootData.fQuaternion[2];
			Quat.z = g_Players->pLocalPlayer->onFootData.fQuaternion[3];
			//
			D3DXVECTOR3 Axis;
			Axis.x = 0;
			Axis.y = 0;
			Axis.z = 1;
			D3DXQuaternionRotationAxis(&Quat2, &Axis, M_PI);
			D3DXQuaternionMultiply(&Quat, &Quat, &Quat2);
			OnFootData.fQuaternion[0] = Quat.w;
			OnFootData.fQuaternion[1] = Quat.x;
			OnFootData.fQuaternion[2] = Quat.y;
			OnFootData.fQuaternion[3] = Quat.z;

			vect4_copy(OnFootData.fQuaternion,
				g_BotFuncs->BotClient[BotSettings.CurrentClientID].
				fLastOnFootQuat);

			BitStream bsOnFoot;
			bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
			bsOnFoot.Write((PCHAR)& OnFootData, sizeof(stOnFootData));
			g_BotFuncs->BotClient[BotSettings.CurrentClientID].pRakClient->
				Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

			//for fluid work
			BotSettings.CurrentClientID--;
		}
	}
	else
	{
		bVehiclePicked = false;
		vect3_copy(g_BotFuncs->BotClient[BotSettings.CurrentClientID].
			fLastOnFootPos, OnFootData.fPosition);
		vect4_copy(g_BotFuncs->BotClient[BotSettings.CurrentClientID].
			fLastOnFootQuat, OnFootData.fQuaternion);

		//cheat_state_text("%d", OnFootData.sCurrentAnimationID);
		BitStream bsOnFoot;
		bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
		bsOnFoot.Write((PCHAR)& OnFootData, sizeof(stOnFootData));
		g_BotFuncs->BotClient[BotSettings.CurrentClientID].pRakClient->
			Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}


}

void CBotFuncs::OL_Bot_Elevator()
{
	short playerID = BotSettings.sTargetID;
	if (IsPlayerStreamed(playerID))
	{
		stOnFootData OnFootData;
		memset(&OnFootData, 0, sizeof(stOnFootData));
		vect3_copy(&g_Players->pRemotePlayer[playerID]->pPlayerData->
			pSAMP_Actor->pGTA_Ped->base.matrix[12],
			OnFootData.fPosition);
		//vect3_copy(g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->speed, OnFootData.fMoveSpeed);
		OnFootData.fPosition[0] +=
			g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->
			pGTA_Ped->speed[0] * 10.97f;
		OnFootData.fPosition[1] +=
			g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->
			pGTA_Ped->speed[1] * 10.97f;
		OnFootData.fPosition[2] +=
			g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->
			pGTA_Ped->speed[2] * 10.97f;
		OnFootData.fQuaternion[0] = 0.50f;
		OnFootData.fQuaternion[1] = -0.50f;
		OnFootData.fQuaternion[2] = 0.50f;
		OnFootData.fQuaternion[3] = 0.50f;
		OnFootData.fMoveSpeed[0] =
			g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->
			pGTAEntity->speed[0] * 1.5;
		OnFootData.fMoveSpeed[1] =
			g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->
			pGTAEntity->speed[1] * 1.5;
		OnFootData.fMoveSpeed[2] = 10.0f;	//random_float(5.0f, 30.0f);
		OnFootData.fPosition[2] -= (float)(rand() % 70) / 100.0f;
		if (BotSettings.bBotJetpackElevator)
			OnFootData.byteSpecialAction = 2;
		OnFootData.byteHealth =
			g_BotFuncs->BotClient[BotSettings.CurrentClientID].fHealth;
		OnFootData.byteArmor =
			g_BotFuncs->BotClient[BotSettings.CurrentClientID].fArmour;

		BitStream bsOnFoot;
		bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
		bsOnFoot.Write((PCHAR)& OnFootData, sizeof(stOnFootData));
		g_BotFuncs->BotClient[BotSettings.CurrentClientID].pRakClient->
			Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
}

void CBotFuncs::OL_Bot_Shooter()
{
	if (KEY_DOWN(VK_LBUTTON))
	{
		float fPos[3], fSpeed[3];
		bool bFake = BotSettings.UseFakeBot;
		float fRandomDistance = 5.5f + (rand() % 1000) / 100;
		if (bFake)
		{
			vect3_copy(g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->
				pPlayerData->onFootData.fPosition, fPos);
			fPos[0] +=
				g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->
				pSAMP_Actor->pGTAEntity->base.matrix[4] * fRandomDistance;
			fPos[1] +=
				g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->
				pSAMP_Actor->pGTAEntity->base.matrix[5] * fRandomDistance;
			fPos[2] +=
				g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->
				pSAMP_Actor->pGTAEntity->base.matrix[6] * fRandomDistance;

			fSpeed[0] +=
				g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->
				pSAMP_Actor->pGTAEntity->base.matrix[4] * 7.5f;
			fSpeed[1] +=
				g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->
				pSAMP_Actor->pGTAEntity->base.matrix[5] * 7.5f;
			fSpeed[2] +=
				g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->
				pSAMP_Actor->pGTAEntity->base.matrix[6] * 7.5f;

			OL_Bot_Send_OnFoot(fPos, fSpeed, bFake);
		}
		else
		{
			vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.
				matrix[12], fPos);
			fPos[0] += cam_matrix[4] * fRandomDistance;
			fPos[1] += cam_matrix[5] * fRandomDistance;
			fPos[2] += cam_matrix[6] * fRandomDistance;
			fSpeed[0] = cam_matrix[4] * 7.5f;
			fSpeed[1] = cam_matrix[5] * 7.5f;
			fSpeed[2] = cam_matrix[6] * 7.5f;
			OL_Bot_Send_OnFoot(fPos, fSpeed, bFake);

		}
	}

}

void CBotFuncs::OL_Packet_PlayerSync(Packet * packet, uint8_t ClientID)
{
	if (g_BotFuncs->BotSettings.bBotSilent)
	{
		BitStream bsOnFootSync((unsigned char *)packet->data, packet->length,
			false);
		unsigned short PlayerID;
		bsOnFootSync.IgnoreBits(8);
		bsOnFootSync.Read(PlayerID);
		if (BotSettings.UseFakeBot)
		{
			if (PlayerID != OLCheats->Target_PlayerID)
				return;
		}
		else
		{
			if (PlayerID != g_Players->sLocalPlayerID)
				return;
		}
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
		bsOnFootSync.ReadNormQuat(OnFootData.fQuaternion[0],
			OnFootData.fQuaternion[1],
			OnFootData.fQuaternion[2],
			OnFootData.fQuaternion[3]);


		BYTE byteHealthArmour;
		BYTE byteHealth, byteArmour;
		BYTE byteArmTemp = 0, byteHlTemp = 0;

		bsOnFootSync.Read(byteHealthArmour);
		byteArmTemp = (byteHealthArmour & 0x0F);
		byteHlTemp = (byteHealthArmour >> 4);

		if (byteArmTemp == 0xF)
			byteArmour = 100;
		else if (byteArmTemp == 0)
			byteArmour = 0;
		else
			byteArmour = byteArmTemp * 7;

		if (byteHlTemp == 0xF)
			byteHealth = 100;
		else if (byteHlTemp == 0)
			byteHealth = 0;
		else
			byteHealth = byteHlTemp * 7;

		OnFootData.byteHealth = byteHealth;
		OnFootData.byteArmor = byteArmour;

		BYTE byteWeaponKey;

		//bsOnFootSync.Read(OnFootData.byteCurrentWeapon);
		bsOnFootSync.Read(byteWeaponKey);
		OnFootData.byteCurrentWeapon = byteWeaponKey & 6;
		OnFootData.byteSpecialKeys = byteWeaponKey & 2;

		bsOnFootSync.Read(OnFootData.byteSpecialAction);

		bsOnFootSync.ReadVector(OnFootData.fMoveSpeed[0],
			OnFootData.fMoveSpeed[1],
			OnFootData.fMoveSpeed[2]);

		bool bHasSurfInfo;
		bsOnFootSync.Read(bHasSurfInfo);
		if (bHasSurfInfo)
		{
			bsOnFootSync.Read(OnFootData.sSurfingVehicleID);
			bsOnFootSync.Read(OnFootData.fSurfingOffsets[0]);
			bsOnFootSync.Read(OnFootData.fSurfingOffsets[1]);
			bsOnFootSync.Read(OnFootData.fSurfingOffsets[2]);
		}
		else
			OnFootData.sSurfingVehicleID = -1;

		bool bAnimation;
		bsOnFootSync.Read(bAnimation);
		if (bAnimation)
			bsOnFootSync.Read(OnFootData.sCurrentAnimationID);

		vect3_copy(OnFootData.fPosition, BotSettings.fSelfPos);
		vect3_copy(OnFootData.fMoveSpeed, BotSettings.fSelfSpeed);
	}

	//}
	//memcpy(&PlayerInfo[PlayerID].OnFootSync, &OnFootData, sizeof(stOnFootData));
	//PlayerInfo[PlayerID].fPos[0] = PlayerInfo[PlayerID].OnFootSync.fPosition[0];
	//PlayerInfo[PlayerID].fPos[1] = PlayerInfo[PlayerID].OnFootSync.fPosition[1];
	//PlayerInfo[PlayerID].fPos[2] = PlayerInfo[PlayerID].OnFootSync.fPosition[2];
}


void CBotFuncs::OL_Packet_BulletSync(Packet * packet, uint8_t ClientID)
{
	if (BotSettings.bCopyWeapon)
	{
		BitStream bsBullet((unsigned char *)packet->data, packet->length,
			false);
		uint16_t playerID;

		bsBullet.IgnoreBits(8);
		bsBullet.Read(playerID);
		if (BotSettings.UseFakeBot)
		{
			if (playerID != OLCheats->Target_PlayerID)
				return;
			stBulletData BulletData;
			memset(&BulletData, 0, sizeof(stBulletData));
			bsBullet.Read((char *)&BulletData, sizeof(stBulletData));
			BitStream bsSend;
			bsSend.Write((BYTE)ID_BULLET_SYNC);
			bsSend.Write((char *)&BulletData, sizeof(stBulletData));
			g_BotFuncs->BotClient[ClientID].pRakClient->Send(&bsSend,
				HIGH_PRIORITY,
				UNRELIABLE_SEQUENCED,
				0);
		}
		else
		{
			if (playerID == g_Players->sLocalPlayerID)
			{
				stBulletData BulletData;
				memset(&BulletData, 0, sizeof(stBulletData));
				bsBullet.Read((char *)&BulletData, sizeof(stBulletData));
				BitStream bsSend;
				bsSend.Write((BYTE)ID_BULLET_SYNC);
				bsSend.Write((PCHAR)& BulletData, sizeof(stBulletData));
				g_BotFuncs->BotClient[ClientID].pRakClient->Send(&bsSend,
					HIGH_PRIORITY,
					UNRELIABLE_SEQUENCED,
					0);
			}
		}
	}
}

void CBotFuncs::OL_Packet_AimSync(Packet * packet, uint8_t ClientID)
{
	BitStream bsAim((BYTE *)packet->data, packet->length, false);
	uint16_t playerID;

	bsAim.IgnoreBits(8);
	bsAim.Read(playerID);
	if (BotSettings.UseFakeBot)
	{
		if (playerID != OLCheats->Target_PlayerID)
			return;
		stAimData AimData;
		memset(&AimData, 0, sizeof(stAimData));
		bsAim.Read((char *)&AimData, sizeof(stAimData));
		BitStream bsSend;
		bsSend.Write((BYTE)ID_AIM_SYNC);
		bsSend.Write((PCHAR)& AimData, sizeof(stAimData));
		g_BotFuncs->BotClient[ClientID].pRakClient->Send(&bsSend,
			HIGH_PRIORITY,
			UNRELIABLE_SEQUENCED,
			0);
	}
	else
	{
		if (playerID != g_Players->sLocalPlayerID)
			return;
		stAimData AimData;
		memset(&AimData, 0, sizeof(stAimData));
		bsAim.Read((char *)&AimData, sizeof(stAimData));
		BitStream bsSend;
		bsSend.Write((BYTE)ID_AIM_SYNC);
		//comment for work total remote
		//bsSend.Write((PCHAR)&g_Players->pLocalPlayer->aimData, sizeof(stAimData));
		bsSend.Write((PCHAR)& AimData, sizeof(stAimData));
		g_BotFuncs->BotClient[ClientID].pRakClient->Send(&bsSend,
			HIGH_PRIORITY,
			UNRELIABLE_SEQUENCED,
			0);
	}

}

void CBotFuncs::OL_Bot_Silent()
{
	static float fPos[3];
	if (vect3_near_zero(fPos))
	{
		vect3_copy(g_Players->pLocalPlayer->onFootData.fPosition, fPos);
	}
	stOnFootData OnFootData;
	memset(&OnFootData, 0, sizeof(stOnFootData));
	vect3_copy(fPos, OnFootData.fPosition);
	OnFootData.byteCurrentWeapon =
		g_Players->pLocalPlayer->onFootData.byteCurrentWeapon;
	OnFootData.sKeys = 132;
	OnFootData.byteHealth = 100;
	BitStream bsOnFoot;
	bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
	bsOnFoot.Write((PCHAR)& OnFootData, sizeof(stOnFootData));
	g_BotFuncs->BotClient[BotSettings.CurrentClientID].pRakClient->
		Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

	stAimData AimData;
	memset(&AimData, 0, sizeof(stAimData));
	AimData.byteCamMode = 65;	//work with all weapon and sniper
	AimData.vecAimf1[0] = 0;
	AimData.vecAimf1[1] = 0;
	AimData.vecAimf1[2] = -0.01f;
	vect3_copy(BotSettings.fSelfPos, AimData.vecAimPos);
	AimData.vecAimPos[0] += BotSettings.fSelfSpeed[0]
		* BotSettings.fMultiplierSilent;
	AimData.vecAimPos[1] += BotSettings.fSelfSpeed[1]
		* BotSettings.fMultiplierSilent;
	AimData.vecAimPos[2] += 1.0f + BotSettings.fSelfSpeed[2]
		* BotSettings.fMultiplierSilent;
	BitStream bsAim;
	bsAim.Write((BYTE)ID_AIM_SYNC);
	bsAim.Write((PCHAR)& AimData, sizeof(stAimData));
	g_BotFuncs->BotClient[BotSettings.CurrentClientID].pRakClient->Send(&bsAim,
		HIGH_PRIORITY,
		UNRELIABLE_SEQUENCED,
		0);

}

void CBotFuncs::OL_Bot_Follow_Passenger(bool bFake)
{
	if (!g_BotFuncs->BotClient[BotSettings.CurrentClientID].bIsInVehicle)
	{
		//for UIF protection

		short sVehicleID;
		BitStream bsEnterVehicle;
		if (bFake)
		{
			sVehicleID =
				g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->
				inCarData.sVehicleID;
		}
		else
		{
			sVehicleID = g_Players->pLocalPlayer->inCarData.sVehicleID;
		}

		CVehicle *pVehicle =
			pGameInterface->GetPools()->
			GetVehicle((DWORD *)getGTAVehicleFromSAMPVehicleID(sVehicleID));
		if (isBadPtr_GTA_pVehicle(pVehicle))
			return;
		float fPos[3], fNull[3] = { 0, 0, 0 };
		fPos[0] = pVehicle->GetInterface()->Placeable.matrix->vPos.fX;
		fPos[1] = pVehicle->GetInterface()->Placeable.matrix->vPos.fY;
		fPos[2] = pVehicle->GetInterface()->Placeable.matrix->vPos.fZ;
		OL_Bot_Send_OnFoot(fPos, fNull, bFake, sVehicleID, fNull);

		bsEnterVehicle.Write(sVehicleID);
		bsEnterVehicle.Write((BYTE)BotSettings.CurrentClientID + 1);
		g_BotFuncs->OL_Bot_SendRPC(BotSettings.CurrentClientID,
			RPC_EnterVehicle, bsEnterVehicle,
			HIGH_PRIORITY, RELIABLE_SEQUENCED, NULL,
			false);
		g_BotFuncs->BotClient[BotSettings.CurrentClientID].bIsInVehicle = true;

	}
	OL_Bot_Send_Passenger(bFake);
}

void CBotFuncs::OL_Bot_Follow_OnFoot(bool bFake)
{

	if (g_BotFuncs->BotClient[BotSettings.CurrentClientID].bIsInVehicle)
	{
		BitStream bsExitVehicle;
		bsExitVehicle.Write(g_BotFuncs->BotClient[BotSettings.CurrentClientID].
			last_vehicleID);
		g_BotFuncs->OL_Bot_SendRPC(BotSettings.CurrentClientID,
			RPC_ExitVehicle, bsExitVehicle,
			HIGH_PRIORITY, RELIABLE_SEQUENCED, NULL,
			false);
		g_BotFuncs->BotClient[BotSettings.CurrentClientID].bIsInVehicle = false;
	}
	float rest = (BotSettings.CurrentClientID + 1) % 2;
	int versor = (rest == 0) ? 1 : -1;
	int versor2 = (rest == 0) ? -1 : 0;
	float fPos[3], fPos2[3], fSpeed[3], fRotation;
	bool bSurf = false;

	if (bFake)
	{
		vect3_copy(g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->
			pPlayerData->onFootData.fPosition, fPos);

		vect3_copy(fPos, fPos2);

		vect3_copy(g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->
			pPlayerData->onFootData.fMoveSpeed, fSpeed);
		fRotation =
			g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->
			pSAMP_Actor->pGTAEntity->fCurrentRotation;

		fPos2[0] =
			(BotSettings.fDistanceFollow +
			(BotSettings.CurrentClientID +
				versor2) * BotSettings.fDistanceFollowBetweenBots) *
				(sinf(-fRotation)) +
			((BotSettings.CurrentClientID + 1 +
				versor2) * BotSettings.fDistanceFollowBetweenBots * versor) *
			cosf(fRotation);
		fPos2[1] =
			(BotSettings.fDistanceFollow +
			(BotSettings.CurrentClientID +
				versor2) * BotSettings.fDistanceFollowBetweenBots) *
				(cosf(-fRotation)) +
			((BotSettings.CurrentClientID + 1 +
				versor2) * BotSettings.fDistanceFollowBetweenBots * versor) *
			sinf(fRotation);
		fPos2[2] = 0;

		fPos[0] += fPos2[0];
		fPos[1] += fPos2[1];
		fPos[2] += fPos2[2];

		fSpeed[2] -= 0.01;

		if (g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->
			onFootData.sSurfingVehicleID != 0)
		{
			OL_Bot_Send_OnFoot(fPos, fSpeed, bFake,
				g_Players->pRemotePlayer[OLCheats->
				Target_PlayerID]->
				pPlayerData->onFootData.sSurfingVehicleID,
				fPos2);
		}
		else
			OL_Bot_Send_OnFoot(fPos, fSpeed, bFake);
	}
	else
	{
		vect3_copy(g_Players->pLocalPlayer->onFootData.fPosition, fPos);

		vect3_copy(fPos, fPos2);

		vect3_copy(g_Players->pLocalPlayer->onFootData.fMoveSpeed, fSpeed);
		fRotation =
			g_Players->pLocalPlayer->pSAMP_Actor->pGTAEntity->fCurrentRotation;



		fPos2[0] =
			(BotSettings.fDistanceFollow +
			(BotSettings.CurrentClientID +
				versor2) * BotSettings.fDistanceFollowBetweenBots) *
				(sinf(-fRotation)) +
			((BotSettings.CurrentClientID + 1 +
				versor2) * BotSettings.fDistanceFollowBetweenBots * versor) *
			cosf(fRotation);
		fPos2[1] =
			(BotSettings.fDistanceFollow +
			(BotSettings.CurrentClientID +
				versor2) * BotSettings.fDistanceFollowBetweenBots) *
				(cosf(-fRotation)) +
			((BotSettings.CurrentClientID + 1 +
				versor2) * BotSettings.fDistanceFollowBetweenBots * versor) *
			sinf(fRotation);
		fPos2[2] = 0;

		fPos[0] += fPos2[0];
		fPos[1] += fPos2[1];
		fPos[2] += fPos2[2];

		fSpeed[2] -= 0.01;

		if (g_Players->pLocalPlayer->onFootData.sSurfingVehicleID != 0)
		{

			OL_Bot_Send_OnFoot(fPos, fSpeed, bFake,
				g_Players->pLocalPlayer->onFootData.
				sSurfingVehicleID, fPos2);
		}
		else
			OL_Bot_Send_OnFoot(fPos, fSpeed, bFake);

	}


}

void CBotFuncs::OL_Bot_Attacker()
{
	short playerID = BotSettings.sTargetID;
	if (IsPlayerStreamed(playerID))
	{
		float angle =
			(float)(BotSettings.CurrentClientID +
				1) / (float)(BotSettings.ClientCount);
		float alpha =
			(float)BotSettings.CurrentClientID * (2.0f * M_PI) /
			(float)BotSettings.ClientCount;
		//Log("%d / %d =  %f", BotSettings.CurrentClientID + 1, BotSettings.ClientCount, angle);
		D3DXQUATERNION Quat;
		D3DXVECTOR3 Axis;
		Axis.x = 1;
		Axis.y = 0;
		Axis.z = 0;
		D3DXQuaternionRotationAxis(&Quat, &Axis, (2 * M_PI) - alpha);

		stOnFootData OnFootData;
		memset(&OnFootData, 0, sizeof(stOnFootData));
		vect3_copy(&g_Players->pRemotePlayer[playerID]->pPlayerData->
			pSAMP_Actor->pGTA_Ped->base.matrix[12],
			OnFootData.fPosition);
		vect3_copy(g_Players->pRemotePlayer[playerID]->pPlayerData->
			pSAMP_Actor->pGTA_Ped->speed, OnFootData.fMoveSpeed);
		OnFootData.fQuaternion[0] = Quat.x;
		OnFootData.fQuaternion[1] = Quat.y;
		OnFootData.fQuaternion[2] = Quat.z;
		OnFootData.fQuaternion[3] = Quat.w;

		OnFootData.fPosition[1] += cosf(alpha) * 5.0f;
		OnFootData.fPosition[0] += sinf(alpha) * 5.0f;
		

		OnFootData.byteCurrentWeapon = BotSettings.uiAttackWeaponID;
		OnFootData.sKeys = 4;
		OnFootData.byteSpecialAction = 1;
		OnFootData.byteHealth = 100;
		OnFootData.fMoveSpeed[2] = -1;
		BitStream bsOnFoot;
		bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
		bsOnFoot.Write((PCHAR)& OnFootData, sizeof(stOnFootData));
		g_BotFuncs->BotClient[BotSettings.CurrentClientID].pRakClient->
			Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

		if (g_NewModSa->byteLagComp)
		{
			if (GetTickCount() -
				g_BotFuncs->BotClient[BotSettings.CurrentClientID].
				dwSendBulletTick > 500)
			{
				stBulletData BulletData;
				memset(&BulletData, 0, sizeof(stBulletData));
				BulletData.sTargetID = playerID;
				BulletData.byteType = 1;
				BulletData.byteWeaponID = BotSettings.uiAttackWeaponID;
				vect3_copy(&g_Players->pRemotePlayer[playerID]->pPlayerData->
					pSAMP_Actor->pGTA_Ped->base.matrix[12],
					BulletData.fOrigin);
				vect3_copy(&g_Players->pRemotePlayer[playerID]->pPlayerData->
					pSAMP_Actor->pGTA_Ped->base.matrix[12],
					BulletData.fTarget);
				BulletData.fCenter[2] = 0.5f;
				BitStream bsSend;
				bsSend.Write((BYTE)ID_BULLET_SYNC);
				bsSend.Write((PCHAR)& BulletData, sizeof(stBulletData));
				g_BotFuncs->BotClient[BotSettings.CurrentClientID].pRakClient->
					Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
				g_BotFuncs->BotClient[BotSettings.CurrentClientID].
					dwSendBulletTick = GetTickCount();
			}
		}
		else
		{
			stAimData AimData;
			memset(&AimData, 0, sizeof(stAimData));
			AimData.byteCamMode = 65;	//work with all weapon and sniper
			AimData.vecAimf1[0] = 0;
			AimData.vecAimf1[1] = 0;
			AimData.vecAimf1[2] = -0.01f;
			vect3_copy(&g_Players->pRemotePlayer[playerID]->pPlayerData->
				pSAMP_Actor->pGTA_Ped->base.matrix[12],
				AimData.vecAimPos);
			AimData.vecAimPos[0] +=
				g_Players->pRemotePlayer[playerID]->pPlayerData->onFootData.
				fMoveSpeed[0] * 10.97f;
			AimData.vecAimPos[1] +=
				g_Players->pRemotePlayer[playerID]->pPlayerData->onFootData.
				fMoveSpeed[1] * 10.97f;
			AimData.vecAimPos[2] +=
				1.0f +
				g_Players->pRemotePlayer[playerID]->pPlayerData->onFootData.
				fMoveSpeed[2] * 10.97f;
			BitStream bsSend;
			bsSend.Write((BYTE)ID_AIM_SYNC);
			bsSend.Write((PCHAR)& AimData, sizeof(stAimData));
			g_BotFuncs->BotClient[BotSettings.CurrentClientID].pRakClient->
				Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
		}
	}
}

void
CBotFuncs::OL_Bot_Follow()
{

	if (BotSettings.UseFakeBot)
	{
		if (IsPlayerStreamed(OLCheats->Target_PlayerID))
		{
			switch (g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->
				pPlayerData->bytePlayerState)
			{
			case PLAYER_STATE_ONFOOT:
				OL_Bot_Follow_OnFoot(true);
				break;

			case PLAYER_STATE_DRIVER:
				OL_Bot_Follow_Passenger(true);
				break;

			default:
				OL_Bot_Follow_OnFoot(true);
				break;
			}

		}
		else
		{
			cheat_state_text("Player/Enemy ins't streamed");
			return;
		}
	}
	else
	{
		if (g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->pedFlags.bInVehicle)
		{
			OL_Bot_Follow_Passenger(false);
		}
		else
		{
			OL_Bot_Follow_OnFoot(false);
		}

	}

	//vect3_copy(OnFootData.fPosition, pos);
}


void CBotFuncs::OL_Packet_Auth_Key(Packet * packet, uint8_t ClientID)
{
	Log("Auth key for Client ID: %d", ClientID);
	char *auth_key = "";
	bool found_key = false;

	for (int x = 0; x < 1535; x++)
	{
		if (!strcmp(((char *)packet->data + 2), AuthKeyTable[x][0]))
		{
			auth_key = AuthKeyTable[x][1];
			found_key = true;
			break;
		}
	}

	if (found_key)
	{
		RakNet::BitStream bsKey;
		BYTE byteAuthKeyLen;

		byteAuthKeyLen = (BYTE)strlen(auth_key);

		bsKey.Write((BYTE)ID_AUTH_KEY);
		bsKey.Write((BYTE)byteAuthKeyLen);
		bsKey.Write(auth_key, byteAuthKeyLen);

		BotClient[ClientID].pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE,
			NULL);

	}
}

void CBotFuncs::OL_Packet_ConnectionAccepted(Packet * packet, uint8_t ClientID)
{
	cheat_state_text("ConnectionAccepted for Client ID: %d", ClientID);
	Log("ConnectionAccepted for Client ID: %d", ClientID);
	BitStream bsSuccAuth((unsigned char *)packet->data, packet->length,
		false);

	bsSuccAuth.IgnoreBits(8);	// ID_CONNECTION_REQUEST_ACCEPTED
	bsSuccAuth.IgnoreBits(32);	// binaryAddress
	bsSuccAuth.IgnoreBits(16);	// port
	bsSuccAuth.Read(BotClient[ClientID].playerID);
	bsSuccAuth.Read(BotClient[ClientID].uiChallenge);

	uiIsBot[BotClient[ClientID].playerID][0] = 1;
	uiIsBot[BotClient[ClientID].playerID][1] = ClientID;

	int iVersion = 4057;		// NETGAME_VERSION;

	char auth_bs[4 * 16] = { 0 };
	char client_version[] = "0.3.7";	// CLIENT_VERSION;

	gen_rand_gpci(auth_bs, 0x3E9);

	BYTE byteNameLen = (BYTE)strlen(BotClient[ClientID].NickName);
	BYTE byteAuthBSLen = (BYTE)strlen(auth_bs);
	BYTE byteVersionLen = (BYTE)strlen(client_version);

	unsigned int uiClientChallengeResponse =
		BotClient[ClientID].uiChallenge ^ iVersion;

	BitStream bsSend;

	bsSend.Write(iVersion);
	bsSend.Write((BYTE)1);	//byteMod

	bsSend.Write(byteNameLen);
	bsSend.Write(BotClient[ClientID].NickName, byteNameLen);

	bsSend.Write(uiClientChallengeResponse);

	bsSend.Write(byteAuthBSLen);
	bsSend.Write(auth_bs, byteAuthBSLen);

	bsSend.Write(byteVersionLen);
	bsSend.Write(client_version, byteVersionLen);

	OL_Bot_SendRPC(ClientID, RPC_ClientJoin, bsSend, HIGH_PRIORITY,
		RELIABLE_ORDERED, 0, false);
	BotClient[ClientID].bJoined = true;
	BotSettings.ClientJoined++;
	//BitStream bsSpawnRequest;
	//bsSpawnRequest.Write(1);
	//OL_Bot_SendRPC(ClientID, RPC_RequestClass, bsSpawnRequest, HIGH_PRIORITY, RELIABLE, 0, false);
	//
	//RakNet::BitStream bsSendRequestSpawn;
	//OL_Bot_SendRPC(ClientID, RPC_RequestSpawn, bsSendRequestSpawn, HIGH_PRIORITY, RELIABLE, 0, false);
	//BitStream bsSendSpawn;
	//OL_Bot_SendRPC(ClientID, RPC_Spawn, bsSendSpawn, HIGH_PRIORITY, RELIABLE, 0, false);
}

bool CBotFuncs::OL_Bot_SendRPC(uint8_t ClientID, int iRPCID, BitStream Params,
	PacketPriority Priority,
	PacketReliability Reliability,
	char OrderingChannel, bool ShiftTimestamp)
{
	return BotClient[ClientID].pRakClient->RPC(&iRPCID, &Params, Priority,
		Reliability, OrderingChannel,
		ShiftTimestamp,
		UNASSIGNED_NETWORK_ID, NULL);
}

void CBotFuncs::OL_Random_Spawn_Bot(uint8_t ClientID)
{

	int ClassID = rand() % (g_SAMP->pSettings->iClassesAvailable - 1);

	BitStream bsSpawnRequest;
	bsSpawnRequest.Write(ClassID);
	OL_Bot_SendRPC(ClientID, RPC_RequestClass, bsSpawnRequest, HIGH_PRIORITY,
		RELIABLE, 0, FALSE);

	BitStream bsSendRequestSpawn;
	OL_Bot_SendRPC(ClientID, RPC_RequestSpawn, bsSendRequestSpawn,
		HIGH_PRIORITY, RELIABLE, 0, FALSE);
	BitStream bsSendSpawn;
	OL_Bot_SendRPC(ClientID, RPC_Spawn, bsSendSpawn, HIGH_PRIORITY, RELIABLE,
		0, FALSE);
	g_BotFuncs->BotClient[BotSettings.CurrentClientID].fHealth = 100.0f;
}

void CBotFuncs::OL_Spawn_Bot(uint8_t ClientID)
{
	BitStream bsSpawnRequest;
	bsSpawnRequest.Write(g_BotFuncs->BotSettings.ClassID);
	OL_Bot_SendRPC(ClientID, RPC_RequestClass, bsSpawnRequest, HIGH_PRIORITY,
		RELIABLE, 0, FALSE);

	BitStream bsSendRequestSpawn;
	OL_Bot_SendRPC(ClientID, RPC_RequestSpawn, bsSendRequestSpawn,
		HIGH_PRIORITY, RELIABLE, 0, FALSE);
	BitStream bsSendSpawn;
	OL_Bot_SendRPC(ClientID, RPC_Spawn, bsSendSpawn, HIGH_PRIORITY, RELIABLE,
		0, FALSE);
	g_BotFuncs->BotClient[BotSettings.CurrentClientID].fHealth = 100.0f;
}

void CBotFuncs::OL_Bot_Send_OnFoot(const float *fPos, const float *fSpeed,
	bool bFake, uint16_t sSurfingVehicleID,
	float *fSurfPos)
{
	stOnFootData OnFootData;
	memset(&OnFootData, 0, sizeof(stOnFootData));
	if (bFake)
	{
		memcpy(&OnFootData,
			&g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->
			pPlayerData->onFootData, sizeof(stOnFootData));
	}
	else
	{
		memcpy(&OnFootData, &g_Players->pLocalPlayer->onFootData,
			sizeof(stOnFootData));
	}
	vect3_copy(fPos, OnFootData.fPosition);
	vect3_copy(fSpeed, OnFootData.fMoveSpeed);
	if (BotSettings.bCopyHealth)
	{
		if (OnFootData.byteHealth > 100)
			OnFootData.byteHealth = 100;
	}
	else
	{
		OnFootData.byteHealth =
			g_BotFuncs->BotClient[BotSettings.CurrentClientID].fHealth;
	}

	if (!BotSettings.bCopyArmor)
		OnFootData.byteArmor =
		g_BotFuncs->BotClient[BotSettings.CurrentClientID].fArmour;


	if (!BotSettings.bCopyWeapon)
		OnFootData.byteCurrentWeapon = 0;

	if (sSurfingVehicleID != 0)
	{
		vect3_copy(fSurfPos, OnFootData.fSurfingOffsets);
	}

	BitStream bsOnFoot;
	bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
	bsOnFoot.Write((PCHAR)& OnFootData, sizeof(stOnFootData));
	g_BotFuncs->BotClient[BotSettings.CurrentClientID].pRakClient->
		Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	vect3_copy(OnFootData.fPosition,
		g_BotFuncs->BotClient[BotSettings.CurrentClientID].
		fLastOnFootPos);
	vect4_copy(OnFootData.fQuaternion,
		g_BotFuncs->BotClient[BotSettings.CurrentClientID].
		fLastOnFootQuat);
}

void CBotFuncs::OL_Bot_Send_Passenger(bool bFake)
{
	stPassengerData PassengerData;
	memset(&PassengerData, 0, sizeof(stPassengerData));
	if (bFake)
	{
		PassengerData.sVehicleID =
			g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->
			inCarData.sVehicleID;
		PassengerData.byteSeatID = BotSettings.CurrentClientID + 1;
		PassengerData.byteHealth =
			g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->
			inCarData.bytePlayerHealth;
		PassengerData.byteArmor =
			g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->
			inCarData.byteArmor;
		PassengerData.byteCurrentWeapon =
			g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->
			inCarData.byteCurrentWeapon;
		vect3_copy(g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->
			pPlayerData->inCarData.fPosition, PassengerData.fPosition);
	}
	else
	{
		PassengerData.sVehicleID =
			g_Players->pLocalPlayer->inCarData.sVehicleID;
		PassengerData.byteSeatID = BotSettings.CurrentClientID + 1;
		PassengerData.byteHealth =
			g_Players->pLocalPlayer->inCarData.bytePlayerHealth;
		PassengerData.byteArmor = g_Players->pLocalPlayer->inCarData.byteArmor;
		PassengerData.byteCurrentWeapon =
			g_Players->pLocalPlayer->inCarData.byteCurrentWeapon;
		vect3_copy(g_Players->pLocalPlayer->inCarData.fPosition,
			PassengerData.fPosition);
	}

	if (BotSettings.bCopyHealth)
	{
		if (PassengerData.byteHealth > 100)
			PassengerData.byteHealth = 100;
	}
	else
	{
		PassengerData.byteHealth =
			g_BotFuncs->BotClient[BotSettings.CurrentClientID].fHealth;
	}

	if (!BotSettings.bCopyArmor)
		PassengerData.byteArmor =
		g_BotFuncs->BotClient[BotSettings.CurrentClientID].fArmour;

	if (!BotSettings.bCopyWeapon)
		PassengerData.byteCurrentWeapon = 0;


	BitStream bsPassenger;
	bsPassenger.Write((BYTE)ID_PASSENGER_SYNC);
	bsPassenger.Write((PCHAR)& PassengerData, sizeof(stPassengerData));
	g_BotFuncs->BotClient[BotSettings.CurrentClientID].pRakClient->
		Send(&bsPassenger, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

}

CBotFuncs::~CBotFuncs()
{
	for (int i = g_BotFuncs->BotSettings.ClientCount; i > 0; i--)
		g_BotFuncs->OL_Delete_Bot();

	delete g_BotFuncs;
}
