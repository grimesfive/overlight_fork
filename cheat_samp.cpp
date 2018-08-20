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

int			g_iSpectateEnabled = 0, g_iSpectateLock = 0, g_iSpectatePlayerID = -1;
int			g_iJoiningServer = 0;
int			iClickWarpEnabled = 0;
int			g_iNumPlayersMuted = 0;
bool		g_bPlayerMuted[SAMP_MAX_PLAYERS];

void sampMainCheat(void)
{
	traceLastFunc("sampMainCheat()");

	// g_Vehicles & g_Players pointers need to be refreshed or nulled
	if (isBadPtr_writeAny(g_SAMP->pPools, sizeof(stSAMPPools)))
	{
		g_Vehicles = NULL;
		g_Players = NULL;
	}
	else if (g_Vehicles != g_SAMP->pPools->pVehicle || g_Players != g_SAMP->pPools->pPlayer)
	{
		if (isBadPtr_writeAny(g_SAMP->pPools->pVehicle, sizeof(stVehiclePool)))
			g_Vehicles = NULL;
		else
			g_Vehicles = g_SAMP->pPools->pVehicle;
		if (isBadPtr_writeAny(g_SAMP->pPools->pPlayer, sizeof(stPlayerPool)))
			g_Players = NULL;
		else
			g_Players = g_SAMP->pPools->pPlayer;
	}

	// update GTA to SAMP translation structures
	update_translateGTASAMP_vehiclePool();
	update_translateGTASAMP_pedPool();

	spectateHandle();
	sampAntiHijack();

	// start chatbox logging
	if (set.chatbox_logging)
	{
		static int	chatbox_init;
		if (!chatbox_init)
		{
			SYSTEMTIME	time;
			GetLocalTime(&time);
			LogChatbox(false, "Session started at %02d/%02d/%02d", time.wDay, time.wMonth, time.wYear);
			chatbox_init = 1;
		}
	}

	if (KEYCOMBO_PRESSED(set.key_player_info_list))
		cheat_state->player_info_list ^= 1;

	if (KEYCOMBO_PRESSED(set.key_rejoin))
	{
		restartGame();
		disconnect(500);
		cheat_state_text("Rejoining in %d seconds...", set.rejoin_delay / 1000);
		cheat_state->_generic.rejoinTick = GetTickCount();
	}

	if (KEYCOMBO_PRESSED(set.key_respawn))
		playerSpawn();
	
	if (KEYCOMBO_DOWN(set.chat_secondary_key))
	{
		int			i, key, spam;
		const char	*msg;
		for (i = 0; i < INI_CHATMSGS_MAX; i++)
		{
			struct chat_msg *msg_item = &set.chat[i];
			if (msg_item->key == NULL)
				continue;
			if (msg_item->msg == NULL)
				continue;
			if (msg_item->key != key_being_pressed)
				continue;
			key = msg_item->key;
			msg = msg_item->msg;
			spam = msg_item->spam;
			if (spam)
			{
				if (msg)
					if (KEY_DOWN(key))
						say("%s", msg);
			}
			else
			{
				if (msg)
					if (KEY_PRESSED(key))
						say("%s", msg);
			}
		}
	}
	if (set.clickwarp_enabled && iIsSAMPSupported)
	{
		if (KEYCOMBO_PRESSED(set.key_clickwarp_enable))
		{
			g_iCursorEnabled ^= 1;
			toggleSAMPCursor(g_iCursorEnabled);
		}
		if (g_iCursorEnabled && KEYCOMBO_PRESSED(set.key_clickwarp_click))
		{
			iClickWarpEnabled = 1;
		}
	}

	static int	iSAMPHooksInstalled;
	if (!iSAMPHooksInstalled)
	{
		installSAMPHooks();
		iSAMPHooksInstalled = 1;
	}

	if (cheat_state->_generic.rejoinTick && cheat_state->_generic.rejoinTick < (GetTickCount() - set.rejoin_delay))
	{
		g_SAMP->iGameState = GAMESTATE_WAIT_CONNECT;
		cheat_state->_generic.rejoinTick = 0;
	}

	if (g_iJoiningServer == 1)
	{
		restartGame();
		disconnect(500);
		cheat_state_text("Joining server in %d seconds...", set.rejoin_delay / 1000);
		cheat_state->_generic.join_serverTick = GetTickCount();
		g_iJoiningServer = 2;
	}

	if (g_iJoiningServer == 2
		&& cheat_state->_generic.join_serverTick
		&&	 cheat_state->_generic.join_serverTick < (GetTickCount() - set.rejoin_delay))
	{
		g_SAMP->iGameState = GAMESTATE_WAIT_CONNECT;
		g_iJoiningServer = 0;
		cheat_state->_generic.join_serverTick = 0;
	}
}

void spectateHandle(void)
{
	if (g_iSpectateEnabled)
	{
		if (g_iSpectateLock) return;

		if (g_iSpectatePlayerID != -1)
		{
			if (g_Players->iIsListed[g_iSpectatePlayerID] != 0)
			{
				if (g_Players->pRemotePlayer[g_iSpectatePlayerID] != NULL)
				{
					int iState = getPlayerState(g_iSpectatePlayerID);

					if (iState == PLAYER_STATE_ONFOOT)
					{
						struct actor_info *pPlayer = getGTAPedFromSAMPPlayerID(g_iSpectatePlayerID);
						if (pPlayer == NULL) return;
						GTAfunc_CameraOnActor(pPlayer);
						g_iSpectateLock = 1;
					}
					else if (iState == PLAYER_STATE_DRIVER)
					{
						struct vehicle_info *pPlayerVehicleID = g_Players->pRemotePlayer[g_iSpectatePlayerID]->pPlayerData->pSAMP_Vehicle->pGTA_Vehicle;
						if (pPlayerVehicleID == NULL) return;
						GTAfunc_CameraOnVehicle(pPlayerVehicleID);
						g_iSpectateLock = 1;
					}
					else if (iState == PLAYER_STATE_PASSENGER)
					{
						struct vehicle_info *pPlayerVehicleID = g_Players->pRemotePlayer[g_iSpectatePlayerID]->pPlayerData->pSAMP_Vehicle->pGTA_Vehicle;
						if (pPlayerVehicleID == NULL) return;
						GTAfunc_CameraOnVehicle(pPlayerVehicleID);
						g_iSpectateLock = 1;
					}
				}
				else
				{
					cheat_state_text("Player is not streamed in");
					g_iSpectateEnabled = 0;
				}
			}
		}
	}
}

void spectatePlayer(int iPlayerID)
{
	if (iPlayerID == -1)
	{
		GTAfunc_TogglePlayerControllable(0);
		GTAfunc_LockActor(0);
		pGameInterface->GetCamera()->RestoreWithJumpCut();

		g_iSpectateEnabled = 0;
		g_iSpectateLock = 0;
		g_iSpectatePlayerID = -1;
		return;
	}

	g_iSpectatePlayerID = iPlayerID;
	g_iSpectateLock = 0;
	g_iSpectateEnabled = 1;
}

void sampAntiHijack(void)
{
	if (g_SAMP == NULL) return;
	traceLastFunc("sampAntiHijack()");

	vehicle_info *veh = vehicle_info_get(VEHICLE_SELF, VEHICLE_ALIVE);
	if (set.anti_carjacking && veh == NULL)
	{
		if (cheat_state->_generic.got_vehicle_id)
			cheat_state->_generic.got_vehicle_id = false;
		if (cheat_state->_generic.anti_carjackTick
			&&	 cheat_state->_generic.anti_carjackTick < (GetTickCount() - 500)
			&& cheat_state->_generic.car_jacked)
		{
			if (cheat_state->_generic.car_jacked_last_vehicle_id == 0)
			{
				GTAfunc_showStyledText("~r~Unable To Unjack~w~!", 1000, 5);
				cheat_state->_generic.anti_carjackTick = 0;
				cheat_state->_generic.car_jacked = false;
				return;
			}

			cheat_state->_generic.anti_carjackTick = 0;
			cheat_state->_generic.car_jacked = false;
			GTAfunc_PutActorInCar(GetVehicleByGtaId(cheat_state->_generic.car_jacked_last_vehicle_id));

//			struct vehicle_info *veh = GetVehicleByGtaId(cheat_state->_generic.car_jacked_last_vehicle_id);
			//if ( veh != NULL )
			//	vect3_copy( cheat_state->_generic.car_jacked_lastPos, &veh->base.matrix[4 * 3] );
			GTAfunc_showStyledText("~r~Car Unjacked~w~!", 1000, 5);
			return;
		}
	}
	else if (set.anti_carjacking)
	{
		if (veh->passengers[0] == actor_info_get(ACTOR_SELF, 0))
		{
			if (!cheat_state->_generic.got_vehicle_id)
			{
				cheat_state->_generic.car_jacked_last_vehicle_id = getPlayerVehicleGTAScriptingID(ACTOR_SELF);
				if (cheat_state->_generic.car_jacked_last_vehicle_id > 0)
					cheat_state->_generic.got_vehicle_id = true;
			}
		}
	}
}


//bool bReportChecked = false;
//short PlayerID;
//void CheckReport()
//{
//	if (bReportChecked && KEY_DOWN(VK_LCONTROL) && KEY_DOWN(VK_LSHIFT))
//	{
//		say("/Spec %d", PlayerID);
//		bReportChecked = false;
//	}
//}

void HandleRPCPacketFunc(unsigned char id, RPCParameters *rpcParams, void(*callback) (RPCParameters *))
{
	traceLastFunc("HandleRPCPacketFunc");
	if (set.netPatchAssoc[id][INCOMING_RPC] != nullptr && set.netPatchAssoc[id][INCOMING_RPC]->enabled)
		return;
	
	if (rpcParams != nullptr && rpcParams->numberOfBitsOfData >= 8)
	{
		switch (id)
		{
			case RPC_PlayerSpectatePlayer:
			{
				BitStream bsData(rpcParams->input, rpcParams->numberOfBitsOfData / 8, false);
				uint16_t playerId;
				bool mode;
				
				bsData.Read(playerId);
				bsData.Read(mode);
				OLCheats->iSpectateID = playerId;
				//bsData.Read(unk);
				//Log("RPC_PlayerSpectatePlayer");
				cheat_state_text("RPC_PlayerSpectatePlayer, %d %d %d", playerId, mode, rpcParams->numberOfBitsOfData);
			}
			case RPC_PlayerSpectateVehicle:
			{
				BitStream bsData(rpcParams->input, rpcParams->numberOfBitsOfData / 8, false);
				uint16_t playerId;
				bool mode;

				bsData.Read(playerId);
				bsData.Read(mode);
				//bsData.Read(unk);
				//Log("RPC_PlayerSpectatePlayer");
				cheat_state_text("RPC_PlayerSpectateVehicle, %d %d %d", playerId, mode, rpcParams->numberOfBitsOfData);
			}
			//
			//case 171: // wrong idk wat is
			//{
			//	Log("RPC 171 SPEC");
			//	cheat_state_text("RPC 171 SPEC");
			//}
			//case 172: // wrong idk wat is
			//{
			//	Log("RPC 172 SPEC");
			//	cheat_state_text("RPC 172 SPEC");
			//}
			//case 173: // wrong idk wat is
			//{
			//	Log("RPC 173 SPEC");
			//	cheat_state_text("RPC 173 SPEC");
			//}
			//
			/*
			case RPC_UpdateScoresPingsIPs:
			{
				Log("RPC_UpdateScoresPingsIPs RECEIVED");
				cheat_state_text("RPC_UpdateScoresPingsIPs RECEIVED");
			}*/
			
			case RPC_SetPlayerHealth:
			{
				traceLastFunc("RPC_SetPlayerHealth");
				if (!set.enable_extra_godmode || !cheat_state->_generic.hp_cheat) break;

				actor_info *self = actor_info_get(ACTOR_SELF, NULL);
				if (self)
				{
					BitStream bsData(rpcParams->input, rpcParams->numberOfBitsOfData / 8, false);
					float fHealth;
					bsData.Read(fHealth);
					if (fHealth < self->hitpoints)
					{
						cheat_state_text("Warning: Server tried change your health to %0.1f", fHealth);
						return; // exit from the function without processing RPC
					}
				}
				break;
			}

			case RPC_SetVehicleHealth:
			{
				traceLastFunc("RPC_SetVehicleHealth");
				if (!set.enable_extra_godmode || !cheat_state->_generic.hp_cheat) break;

				vehicle_info *vself = vehicle_info_get(VEHICLE_SELF, NULL);
				if (vself)
				{
					BitStream bsData(rpcParams->input, rpcParams->numberOfBitsOfData / 8, false);
					short sId;
					float fHealth;
					bsData.Read(sId);
					bsData.Read(fHealth);
					if (sId == g_Players->pLocalPlayer->sCurrentVehicleID && fHealth < vself->hitpoints)
					{
						cheat_state_text("Warning: Server tried change your vehicle health to %0.1f", fHealth);
						return; // exit from the function without processing RPC
					}
				}
				break;
			}
			case RPC_ClientMessage:
			{
				traceLastFunc("RPC_ClientMessage");
				if (!set.anti_spam && !set.chatbox_logging) break;

				BitStream		bsData(rpcParams->input, rpcParams->numberOfBitsOfData / 8, false);
				uint32_t		dwStrLen, dwColor;
				char			szMsg[256];
				static char		last_servermsg[256];
				static DWORD	allow_show_again = 0;

				if (cheat_state->_generic.cheat_panic_enabled)
					break;

				bsData.Read(dwColor);
				bsData.Read(dwStrLen);
				if (dwStrLen >= sizeof(szMsg)) dwStrLen = sizeof(szMsg) - 1;
				bsData.Read(szMsg, dwStrLen);
				szMsg[dwStrLen] = '\0';

				if (set.anti_spam && (strcmp(last_servermsg, szMsg) == 0 && GetTickCount() < allow_show_again))
					return; // exit without processing rpc

				// might be a personal message by muted player - look for name in server message
				// ignore message, if name was found
				if (set.anti_spam && g_iNumPlayersMuted > 0)
				{
					int i, j;
					char *playerName = NULL;
					for (i = 0, j = 0; i < SAMP_MAX_PLAYERS && j < g_iNumPlayersMuted; i++)
					{
						if (g_bPlayerMuted[i])
						{
							playerName = (char *)getPlayerName(i);

							if (playerName == NULL)
							{
								// Player not connected anymore - remove player from muted list
								g_bPlayerMuted[i] = false;
								g_iNumPlayersMuted--;
								continue;
							}
							else if (strstr(szMsg, playerName) != NULL)
							{
								return;
							}
							j++;
						}
					}
				}
				if (set.chatbox_logging)
					LogChatbox(false, "%s", szMsg);
				strncpy_s(last_servermsg, szMsg, sizeof(last_servermsg) - 1);
				allow_show_again = GetTickCount() + 5000;
				break;
			}
			case RPC_Chat:
			{
				traceLastFunc("RPC_Chat");
				//if (!set.anti_spam && !set.chatbox_logging) break;

				BitStream		bsData(rpcParams->input, rpcParams->numberOfBitsOfData / 8, false);
				static char		last_clientmsg[SAMP_MAX_PLAYERS][256];
				static DWORD	allow_show_again = 0;
				uint16_t		playerId = uint16_t(-1);
				uint8_t			byteTextLen;
				char			szText[256];

				if (cheat_state->_generic.cheat_panic_enabled)
					break;

				bsData.Read(playerId);
				if (isBadSAMPPlayerID(playerId))
					break;

				bsData.Read(byteTextLen);
				bsData.Read(szText, byteTextLen);
				szText[byteTextLen] = '\0';
				
				//if (set.anti_spam && ((strcmp(last_clientmsg[playerId], szText) == 0 && GetTickCount() < allow_show_again) || (g_iNumPlayersMuted > 0 && g_bPlayerMuted[playerId])))
				//	return; // exit without processing rpc

				// nothing to copy anymore, after chatbox_logging, so copy this before
				strncpy_s(last_clientmsg[playerId], szText, sizeof(last_clientmsg[playerId]) - 1);

				if (set.chatbox_logging)
					LogChatbox(false, "%s: %s", getPlayerName(playerId), szText);
				
				if (g_BotFuncs->BotSettings.bFollow
					&& g_BotFuncs->BotSettings.bCopyChat
					&& g_BotFuncs->BotSettings.UseFakeBot
					&& OLCheats->Target_PlayerID == playerId)
				{	
					uint8_t startBracket = 0, endBracket = 0;
					bool bCheckID = false;
					std::string message(szText);
					
					bool bOpenBracket = false;
					for (int i = 0; i < 256; i++)
					{
						if (bOpenBracket)
						{
							if (szText[i] == ')')
							{
								bCheckID = true;
								endBracket = i;
								break;
							}
						}
						if (szText[i] == '(')
						{
							startBracket = i;
							bOpenBracket = true;
						}
					}
					if(bCheckID)
						message.replace(startBracket, endBracket - startBracket + 1, "");
					strcpy(szText, message.c_str());
					BitStream bsSend;
					BYTE byteTextLen = (BYTE)strlen(szText);
					bsSend.Write(byteTextLen);
					bsSend.Write(szText, byteTextLen);
					for (int i = g_BotFuncs->BotSettings.ClientCount - 1; i >= 0; i--)
						g_BotFuncs->OL_Bot_SendRPC(i, RPC_Chat, bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false);

					//say(szText);
					//cheat_state_text(szText);
				}
					//g_BotFuncs->BotClient[g_BotFuncs->BotSettings.ClientCount - 1]->pRakClient->RPC(&RPC, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
				//}

				// ---- CUSTOM FOR ADMINS -----

				//std::string message(szText);
				//std::string report = "reported";
				//if (message.find(report) != std::string::npos)
				//{
				//	bool bOpenBracket = false;
				//	std::string temp;
				//	for (int i = message.find(report); i < 256; i++)
				//	{
				//		if (bOpenBracket)
				//		{
				//			if (szText[i] == ')')
				//				break;
				//			std::string temp2(1, szText[i]);
				//			temp += temp2;					
				//		}
				//		if (szText[i] == '(')
				//		{
				//			bOpenBracket = true;
				//			bReportChecked = true;
				//		}
				//
				//	}
				//	PlayerID = atoi(temp.c_str());
				//	
				//}
				//allow_show_again = GetTickCount() + 5000;
				break;
			}
			case RPC_InitGame:
			{
				traceLastFunc("RPC_InitGame");
				RakNet::BitStream bsData(rpcParams->input, rpcParams->numberOfBitsOfData / 8, false);

				bsData.SetReadOffset(363);
				bsData.Read(g_NewModSa->byteLagComp);
				break;
			}
			//case 44:
			//	break;
			//case 47:
			//	break;
			//case 58:
			//	break;
			//case 36:
			//	break;
			//case 108:
			//	break;
			//case 86:
			//	break;
			//case 137:
			//	break;
			//case 138:
			//	break;
			//case 69:
			//	break;
			//case 55:
			//	break;
			//case 166:
			//	break;
			//case 43:
			//	break;
			//case 165:
			//	break;
			//case 113:
			//	break;
			//case 164:
			//	break;
			//case 95:
			//	break;
			//case 63:
			//	break;
			//case 134:
			//	break;
			//case 135:
			//	break;
			//case 24:
			//	break;
			//case 99:
			//	break;
			//case 106:
			//	break;
			//case 84:
			//	break;
			//case 96:
			//	break;
			//case 32:
			//	break;
			//case 163:
			//	break;
			//case 156:
			//	break;
			//case 72:
			//	break;
			//case 11:
			//	break;
			//default:
			//	cheat_state_text("RPC Receive Log: %d", id);
			//	Log("RPC Receive Log: %d", id);
			//	break;
		} // switch

	}
	callback(rpcParams);
}




bool OnSendRPC(int uniqueID, BitStream *parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp)
{

	//if (OLCheats->bSilentAim && uniqueID == RPC_GiveTakeDamage)
	//{
	//	bool bGive;
	//	parameters->Read(bGive);
	//	if (!bGive)
	//		return false;
	//}
	//if (uniqueID == RPC_SetInteriorId)
	//{
	//	BYTE byteInteriorID;
	//	parameters->Read(byteInteriorID);
	//	cheat_state_text("SetInteriorId %d", byteInteriorID);
	//	Log("SetInteriorId %d", byteInteriorID);
	//}
	//
	//if (uniqueID == RPC_SetPlayerInterior)
	//{
	//	BYTE byteInteriorID;
	//	parameters->Read(byteInteriorID);
	//	cheat_state_text("RPC_SetPlayerInterior %d", byteInteriorID);
	//	Log("RPC_SetPlayerInterior %d", byteInteriorID);
	//}
	//Log("RPC Send Log: %d", uniqueID);
	//cheat_state_text("RPC Send Log: %d", uniqueID);

	//if (uniqueID == 168)
	//{
	//	cheat_state_text("dasdsdasasd");
	//	return false;
	//}
	//if (uniqueID == RPC_Death)
	//{
	//	BYTE Reason;
	//	USHORT Killer;
	//	parameters->Read(Reason);
	//	parameters->Read(Killer);
	//	Log("reason = %d", Reason);
	//	Log("killer = %d", Killer);
	//}
	/*
	if (uniqueID == RPC_UpdateScoresPingsIPs)
	{
		Log("RPC_UpdateScoresPingsIPs SENT");
		cheat_state_text("RPC_UpdateScoresPingsIPs SENT");
	}*/

	if (uniqueID == RPC_ClientJoin)
	{

		BYTE byteAuthBSLen;
		char auth_bs[64] = { 0 };
		char szClientVer[] = "0.3.7";
		BYTE iClientVerLen = sizeof(szClientVer) - 1;
		parameters->Read(g_NewModSa->iVersion);
		parameters->Read(g_NewModSa->byteMod);
		parameters->Read(g_NewModSa->byteNameLen);
		parameters->Read(g_NewModSa->szNickName, g_NewModSa->byteNameLen);
		parameters->Read(g_NewModSa->Challenge);
		parameters->Read(byteAuthBSLen);
		parameters->Read(auth_bs, byteAuthBSLen);
		parameters->Read(iClientVerLen);
		parameters->Read(szClientVer, iClientVerLen);
		parameters->Reset();
		Log("byteAuthBSLen %d", byteAuthBSLen);
		Log("auth_bs %s", auth_bs);
		
		//Log("%d", &OLCheats->bChangeGPCI);
		if(OLCheats->bChangeGPCIa)
			gen_rand_gpci(auth_bs, 0x3E9);
		
		byteAuthBSLen = (BYTE)strlen(auth_bs);
		
		parameters->Write(g_NewModSa->iVersion);
		parameters->Write(g_NewModSa->byteMod);
		parameters->Write(g_NewModSa->byteNameLen);
		parameters->Write(g_NewModSa->szNickName, g_NewModSa->byteNameLen);
	
		parameters->Write(g_NewModSa->Challenge);
		parameters->Write(byteAuthBSLen);
		parameters->Write(auth_bs, byteAuthBSLen);

		parameters->Write(iClientVerLen);
		parameters->Write(szClientVer, iClientVerLen);
		Log("NEW byteAuthBSLen %d", byteAuthBSLen);
		Log("NEW auth_bs %s", auth_bs);
		cheat_state_text("serial spoofed");
	}

	//f (uniqueID == RPC_InitGame)
	//
	//	BitStream bsData(parameters->input, parameters->numberOfBitsOfData / 8, false);
	//	parameters->SetReadOffset(363);
	//	parameters->Read(g_NewModSa->byteLagComp);
	//
	//
	if (uniqueID == RPC_Chat && g_Players != nullptr)
	{
		uint8_t byteTextLen;
		char szText[256];

		parameters->Read(byteTextLen);
		parameters->Read(szText, byteTextLen);
		szText[byteTextLen] = '\0';

		if (set.chatbox_logging)
			LogChatbox(false, "%s: %s", getPlayerName(g_Players->sLocalPlayerID), szText);
		
		if (g_BotFuncs->BotSettings.bCopyChat
			&& !g_BotFuncs->BotSettings.UseFakeBot
			&&	g_BotFuncs->BotSettings.bFollow)
		{
			BitStream bsSend;
			BYTE byteTextLen = (BYTE)strlen(szText);
			bsSend.Write(byteTextLen);
			bsSend.Write(szText, byteTextLen);
			for (int i = g_BotFuncs->BotSettings.ClientCount - 1; i >= 0; i--)
				g_BotFuncs->OL_Bot_SendRPC(i, RPC_Chat, bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false);
		}
	}

	//if (OLCheats->bSilentAim && uniqueID == RPC_GiveTakeDamage)
	//	return false;
	if (uniqueID == RPC_GiveTakeDamage)
	{
		bool bGive;
		parameters->Read(bGive);
		if (!bGive && OLCheats->bSilentAim)
			return false;
	//}
		//bool bGive;
		USHORT PlayerID;
		float fAmmount;
		int WeaponID;
		int BodyPart;
		//parameters->Read(bGive);
		parameters->Read(PlayerID);
		parameters->Read(fAmmount);
		parameters->Read(WeaponID);
		parameters->Read(BodyPart);

	}

	if (uniqueID == RPC_ClientCheck && cheat_state && cheat_state->_generic.hp_cheat)
	{
		uint8_t type = 0;
		parameters->Read(type);
		if (type == 2)
		{
			uint32_t arg = 0;
			uint8_t response = 0;
			parameters->Read(arg);
			parameters->Read(response);

			// remove invulnerability flags from our real flags
			uint32_t fakeFlags = arg & (0xFF00FFFF | ((~ACTOR_FLAGS_INVULNERABLE) << 16));

			// reform packet data
			parameters->SetWriteOffset(0);
			parameters->Write(type);
			parameters->Write(fakeFlags);
			parameters->Write(response);
		}
	}

	if (set.netPatchAssoc[uniqueID][OUTCOMING_RPC] != nullptr && set.netPatchAssoc[uniqueID][OUTCOMING_RPC]->enabled)
		return false;

	if (g_BotFuncs->BotSettings.bCopyRPC
		&& g_BotFuncs->BotSettings.bFollow)
	{
		for (int i = g_BotFuncs->BotSettings.ClientCount - 1; i >= 0; i--)
			g_BotFuncs->OL_Bot_SendRPC(i, uniqueID, *parameters, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false);
	}




	return true;
}



bool OnSendPacket(BitStream *parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	if (OLCheats->bFakeLagSync)
	{
		static DWORD SyncTick = GetTickCount();
		if (GetTickCount() - SyncTick > OLCheats->dwTimeFakeLagSync)
		{
			if ((GetTickCount() - SyncTick) > (OLCheats->dwTimeFakeLagSync + OLCheats->dwTimeFakeLagDesync))
			{
				SyncTick = GetTickCount();
			}
			else 
			{
				if (!(OLCheats->bSyncWhenAim && g_Players->pLocalPlayer->onFootData.stSampKeys.keys_aim))
				{
					return false;
				}
				else
				{
					cheat_state_text("Sync beacuse you are aiming");
				}
			}
		}
	}
	if (OLCheats->bPlaneCrasher
		|| OLCheats->bVortexCrasher
		|| OLCheats->bPlaneCrasher2
		|| OLCheats->bVortexCrasher2
		|| OLCheats->bStuntCrasher
		|| OLCheats->bVehicleBugger
		|| OLCheats->bDetonatorCrasher
		|| OLCheats->bTrailerGrabber)
		return false;
	//if (OLCheats->bKickPassenger)
	//	return false;

	uint8_t packetId;
	parameters->Read(packetId);
	bool bEditFootSync = false;
	bool bEditAimSync = false;
	bool bEditVehicleSync = false;
	bool bEditBulletSync = false;
	switch (packetId)
	{
	case ID_SPECTATOR_SYNC:
		stSpectatorData SpectatorData;
		parameters->ResetReadPointer();
		parameters->Read(packetId);
		parameters->Read((char *)&SpectatorData, sizeof(stSpectatorData));
		/*
		cheat_state_text("%f\n%f\n%f\n%d",
			SpectatorData.fPosition[0],
			SpectatorData.fPosition[1],
			SpectatorData.fPosition[2],
			SpectatorData.sLeftRightKeys);
			*/
		parameters->Reset();
		parameters->Write((BYTE)ID_SPECTATOR_SYNC);
		parameters->Write((PCHAR)&SpectatorData, sizeof(stSpectatorData));
		g_RakFuncs->Send(parameters, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
		return false;
		break;

	case ID_PLAYER_SYNC:
		if (OLCheats->bBypassAimAntiCheat2
			|| OLCheats->bInvertWalk
			|| OLCheats->bInvertWalk2
			|| OLCheats->bFakeLagSpeed
			|| OLCheats->bPlayerHologram
			|| OLCheats->bWheelWalk)
		{
			stOnFootData OnFootData;
			parameters->ResetReadPointer();
			parameters->Read(packetId);
			parameters->Read((char *)&OnFootData, sizeof(stOnFootData));

			if (OLCheats->bBypassAimAntiCheat2)
			{
				bEditFootSync = true;
				if (g_Players->pLocalPlayer->onFootData.stSampKeys.keys_primaryFire
					|| g_Players->pLocalPlayer->onFootData.stSampKeys.keys_secondaryFire__shoot)
				{
					OnFootData.sKeys = 4;
				}
			}

			if (OLCheats->bWheelWalk)
			{
				static float fAngle = 0;
				float fZero[3] = { 0.0f, 0.0f, 0.0f };
				fAngle += vect3_dist(fZero, OnFootData.fMoveSpeed) * 2.f;
				bEditFootSync = true;
				D3DXQUATERNION Quat, Quat2;
				Quat.w = OnFootData.fQuaternion[0];
				Quat.x = OnFootData.fQuaternion[1];
				Quat.y = OnFootData.fQuaternion[2];
				Quat.z = OnFootData.fQuaternion[3];
				//
				D3DXVECTOR3 Axis;
				Axis.x = 1;
				Axis.y = 0;
				Axis.z = 0;
				D3DXQuaternionRotationAxis(&Quat2, &Axis, fAngle);
				D3DXQuaternionMultiply(&Quat, &Quat, &Quat2);
				OnFootData.fQuaternion[0] = Quat.w;
				OnFootData.fQuaternion[1] = Quat.x;
				OnFootData.fQuaternion[2] = Quat.y;
				OnFootData.fQuaternion[3] = Quat.z;
				OnFootData.sCurrentAnimationID = 1068;
				OnFootData.sAnimFlags = 4356;
			}

			if (OLCheats->bInvertWalk)
			{
				bEditFootSync = true;

				D3DXQUATERNION Quat, Quat2;
				Quat.w = OnFootData.fQuaternion[0];
				Quat.x = OnFootData.fQuaternion[1];
				Quat.y = OnFootData.fQuaternion[2];
				Quat.z = OnFootData.fQuaternion[3];
				//
				D3DXVECTOR3 Axis;
				Axis.x = 0;
				Axis.y = 1;
				Axis.z = 0;
				D3DXQuaternionRotationAxis(&Quat2, &Axis, M_PI);
				D3DXQuaternionMultiply(&Quat, &Quat, &Quat2);
				OnFootData.fQuaternion[0] = Quat.w;
				OnFootData.fQuaternion[1] = Quat.x;
				OnFootData.fQuaternion[2] = Quat.y;
				OnFootData.fQuaternion[3] = Quat.z;

			}

			if (OLCheats->bInvertWalk2)
			{
				bEditFootSync = true;

				D3DXQUATERNION Quat;
				D3DXQUATERNION Quat2;
				Quat.w = OnFootData.fQuaternion[0];
				Quat.x = OnFootData.fQuaternion[1];
				Quat.y = OnFootData.fQuaternion[2];
				Quat.z = OnFootData.fQuaternion[3];
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

			}
			if (OLCheats->bFakeLagSpeed)
			{
				bEditFootSync = true;
				//stOnFootData OnFootData;
				//memcpy(&OnFootData, &g_Players->pLocalPlayer->onFootData, sizeof(stOnFootData));
				OnFootData.fMoveSpeed[0] *= OLCheats->fMultiplierFakeSpeed;
				OnFootData.fMoveSpeed[1] *= OLCheats->fMultiplierFakeSpeed;

				//parameters->Reset();
				//parameters->Write((BYTE)ID_PLAYER_SYNC);
				//parameters->Write((PCHAR)&OnFootData, sizeof(stOnFootData));
				//g_RakFuncs->Send(parameters, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
				//return false;
			}
			if (OLCheats->bPlayerHologram)
			{
				bEditFootSync = true;
				//stOnFootData OnFootData;
				//memcpy(&OnFootData, &g_Players->pLocalPlayer->onFootData, sizeof(stOnFootData));
				OnFootData.byteSpecialAction = 3;
				//parameters->Reset();
				//parameters->Write((BYTE)ID_PLAYER_SYNC);
				//parameters->Write((PCHAR)&OnFootData, sizeof(stOnFootData));
				//g_RakFuncs->Send(parameters, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
			}
			if (bEditFootSync)
			{
				parameters->Reset();
				parameters->Write((BYTE)ID_PLAYER_SYNC);
				parameters->Write((PCHAR)&OnFootData, sizeof(stOnFootData));
				g_RakFuncs->Send(parameters, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
				return false;
			}
		}
		break;
	case ID_VEHICLE_SYNC:
		if (OLCheats->bDoorStorm)
			return false;
		stInCarData InCar;
		memcpy(&InCar, &g_Players->pLocalPlayer->inCarData, sizeof(stInCarData));
		if (OLCheats->bInvisibleBike || OLCheats->bInvisibleBike2)
		{
			//parameters->ResetReadPointer();
			//parameters->Read(packetId);
			//parameters->Read((PCHAR)&InCar, sizeof(stInCarData)); don't work
			bEditVehicleSync = true;
			InCar.byteCurrentWeapon = 0;
			InCar.fTrainSpeed = NAN;
		}
		if (OLCheats->bInvertWalk)
		{
			bEditVehicleSync = true;
			D3DXQUATERNION Quat;
			D3DXQUATERNION Quat2;
			Quat.w = InCar.fQuaternion[0];
			Quat.x = InCar.fQuaternion[1];
			Quat.y = InCar.fQuaternion[2];
			Quat.z = InCar.fQuaternion[3];
			//
			D3DXVECTOR3 Axis;
			Axis.x = 0;
			Axis.y = 1;
			Axis.z = 0;
			D3DXQuaternionRotationAxis(&Quat2, &Axis, M_PI);
			D3DXQuaternionMultiply(&Quat, &Quat, &Quat2);
			InCar.fQuaternion[0] = Quat.w;
			InCar.fQuaternion[1] = Quat.x;
			InCar.fQuaternion[2] = Quat.y;
			InCar.fQuaternion[3] = Quat.z;
		}
		if (OLCheats->bInvertWalk2)
		{
			bEditVehicleSync = true;
			D3DXQUATERNION Quat;
			D3DXQUATERNION Quat2;
			Quat.w = InCar.fQuaternion[0];
			Quat.x = InCar.fQuaternion[1];
			Quat.y = InCar.fQuaternion[2];
			Quat.z = InCar.fQuaternion[3];
			//
			D3DXVECTOR3 Axis;
			Axis.x = 0;
			Axis.y = 0;
			Axis.z = 1;
			D3DXQuaternionRotationAxis(&Quat2, &Axis, M_PI);
			D3DXQuaternionMultiply(&Quat, &Quat, &Quat2);
			InCar.fQuaternion[0] = Quat.w;
			InCar.fQuaternion[1] = Quat.x;
			InCar.fQuaternion[2] = Quat.y;
			InCar.fQuaternion[3] = Quat.z;
		}

		if (bEditVehicleSync)
		{
			parameters->Reset();
			parameters->Write((BYTE)ID_VEHICLE_SYNC);
			parameters->Write((PCHAR)&InCar, sizeof(stInCarData));
			g_RakFuncs->Send(parameters, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
			return false;
		}
		break;
	case ID_BULLET_SYNC:
		/*
		if (1)
		{
			stBulletData BulletData;
			parameters->ResetReadPointer();
			parameters->Read(packetId);
			parameters->Read((PCHAR)&BulletData, sizeof(stBulletData));
			Log("BulletData.byteType = %d", BulletData.byteType);
			Log("BulletData.byteWeaponID = %d", BulletData.byteWeaponID);
			Log("BulletData.fCenter[0] = %f", BulletData.fCenter[0]);
			Log("BulletData.fCenter[1] = %f", BulletData.fCenter[1]);
			Log("BulletData.fCenter[2] = %f", BulletData.fCenter[2]);
			Log("BulletData.fOrigin[0] = %f", BulletData.fOrigin[0]);
			Log("BulletData.fOrigin[1] = %f", BulletData.fOrigin[1]);
			Log("BulletData.fOrigin[2] = %f", BulletData.fOrigin[2]);
			Log("BulletData.fTarget[0] = %f", BulletData.fTarget[0]);
			Log("BulletData.fTarget[1] = %f", BulletData.fTarget[1]);
			Log("BulletData.fTarget[2] = %f", BulletData.fTarget[2]);
			Log("BulletData.sTargetID = %d", BulletData.sTargetID);
		}*/
		if (OLCheats->bSilentAim || OLCheats->bFixInvisibleFire)
		{
			stBulletData BulletData;
			parameters->ResetReadPointer();
			parameters->Read(packetId);
			parameters->Read((PCHAR)&BulletData, sizeof(stBulletData));
			if (g_NewModSa->byteLagComp && OLCheats->bSilentAim)
			{
				if (g_NewModSa->iAimPlayerID == -1)
					return true;
				if (!IsPlayerStreamed(g_NewModSa->iAimPlayerID))
					return true;
				BulletData.byteType = 1;
				vect3_copy(&g_Players->pRemotePlayer[g_NewModSa->iAimPlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12],
					BulletData.fOrigin);
				vect3_copy(&g_Players->pRemotePlayer[g_NewModSa->iAimPlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12],
					BulletData.fTarget);
				if (OLCheats->fInaccurateAim > 0)
				{
					if (g_Players->pRemotePlayer[g_NewModSa->iAimPlayerID]->pPlayerData->bytePlayerState == PLAYER_STATE_ONFOOT)
					{
						BulletData.fCenter[0] = (OLCheats->fInaccurateAim / 2) - ((rand() % (int)(OLCheats->fInaccurateAim * 10.0f) / 100.0f));
						BulletData.fCenter[1] = (OLCheats->fInaccurateAim / 2) - ((rand() % (int)(OLCheats->fInaccurateAim * 10.0f) / 100.0f));
						BulletData.fCenter[2] = 0.50f - ((rand() % 100) / 100.0f);
					}
					else 
					{
						BulletData.fCenter[0] = 0.0f;
						BulletData.fCenter[1] = 0.0f;
						BulletData.fCenter[2] = 0.56 - ((rand() % 5) / 100.0f);
					}
				}
				else 
				{
					BulletData.fCenter[0] =	0;
					BulletData.fCenter[1] =	0;
					if (g_Players->pRemotePlayer[g_NewModSa->iAimPlayerID]->pPlayerData->bytePlayerState == PLAYER_STATE_ONFOOT)
					{
						BulletData.fCenter[2] = 0;
					}
					else 
					{
						BulletData.fCenter[2] = 0.56 - ((rand() % 5) / 100.0f);
					}
				}
				BulletData.sTargetID = g_NewModSa->iAimPlayerID;
				if (OLCheats->bBypassAimAntiCheat)
				{
					uint8_t random_ac_bypass = rand() % 3;
					if (random_ac_bypass == 1)
					{
						BitStream bsGiveDamage;
						bsGiveDamage.Write(false);
						bsGiveDamage.Write((USHORT)g_NewModSa->iAimPlayerID);
						bsGiveDamage.Write(fWeaponDamage[g_Players->pLocalPlayer->byteCurrentWeapon]);
						bsGiveDamage.Write((int)g_Players->pLocalPlayer->byteCurrentWeapon);
						//Log("Custom Send: damage %f, player %d, weapon %d", fWeaponDamage[g_Players->pLocalPlayer->byteCurrentWeapon], g_NewModSa->iAimPlayerID, g_Players->pLocalPlayer->byteCurrentWeapon);
						bsGiveDamage.Write((rand() % 7) + 3);
						g_RakFuncs->RPC(RPC_GiveTakeDamage, &bsGiveDamage, HIGH_PRIORITY, RELIABLE_SEQUENCED, NULL, false);
					}
					else 
					{
						BulletData.byteType = 0;
					}
					
				}
				else 
				{
					if (g_Players->pRemotePlayer[g_NewModSa->iAimPlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->hitpoints != 0)
					{
						BitStream bsGiveDamage;
						bsGiveDamage.Write(false);
						bsGiveDamage.Write((USHORT)g_NewModSa->iAimPlayerID);
						bsGiveDamage.Write(fWeaponDamage[g_Players->pLocalPlayer->byteCurrentWeapon]);
						bsGiveDamage.Write((int)g_Players->pLocalPlayer->byteCurrentWeapon);
						//Log("Custom Send: damage %f, player %d, weapon %d", fWeaponDamage[g_Players->pLocalPlayer->byteCurrentWeapon], g_NewModSa->iAimPlayerID, g_Players->pLocalPlayer->byteCurrentWeapon);
						bsGiveDamage.Write((rand() % 7) + 3);
						g_RakFuncs->RPC(RPC_GiveTakeDamage, &bsGiveDamage, HIGH_PRIORITY, RELIABLE_SEQUENCED, NULL, false);
					}
				}
				bEditBulletSync = true;
			}

			if (OLCheats->bFixInvisibleFire)
			{
				BulletData.byteWeaponID = 34;
				bEditBulletSync = true;
			}
			if (bEditBulletSync)
			{
				parameters->Reset();
				parameters->Write((BYTE)ID_BULLET_SYNC);
				parameters->Write((PCHAR)&BulletData, sizeof(stBulletData));
				g_RakFuncs->Send(parameters, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
				return false;
			}
		}
		break;
	case ID_AIM_SYNC:
		//if (1)
		//{
		//	stAimData AimData;
		//	parameters->ResetReadPointer();
		//	parameters->Read(packetId);
		//	parameters->Read((PCHAR)&AimData, sizeof(stAimData));
		//	cheat_state_text("%d", AimData.byteCamMode);
		//}

		if (OLCheats->bSilentAim || OLCheats->bInvalidAimZ)
		{

			stAimData AimData;
			parameters->ResetReadPointer();
			parameters->Read(packetId);
			parameters->Read((PCHAR)&AimData, sizeof(stAimData));
			if (OLCheats->bSilentAim)
			{
				bEditAimSync = true;
				if (g_NewModSa->iAimPlayerID == -1)
					return true;
				if (!IsPlayerStreamed(g_NewModSa->iAimPlayerID)
					|| /* of course*/ (g_Players->pLocalPlayer->onFootData.byteCurrentWeapon == 40))// he.. he
					return true;

				if (g_NewModSa->byteLagComp)
				{

					if (OLCheats->bShotWall)
					{
						AimData.byteCamMode = 55;
						if (OLCheats->bBypassAimAntiCheat)
						{
							AimData.vecAimf1[0] = g_Players->pLocalPlayer->aimData.vecAimf1[0];
							AimData.vecAimf1[1] = g_Players->pLocalPlayer->aimData.vecAimf1[1];
							AimData.vecAimf1[2] = g_Players->pLocalPlayer->aimData.vecAimf1[2] + 0.25f;
						}
						else
						{

							AimData.vecAimf1[0] = 0;
							AimData.vecAimf1[1] = 0;
							AimData.vecAimf1[2] = -0.01f;

						}
						vect3_copy(&g_Players->pRemotePlayer[g_NewModSa->iAimPlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12],
							AimData.vecAimPos);
					}
				}
				else
				{
					//lagcomp off
					//AimData.byteCamMode = 65; //work with all weapon and sniper


					if (!IsPlayerStreamed(g_NewModSa->iAimPlayerID))
						return true;

					float fEnemyPos[3], fSelfPos[3];

					OL_CalculateLagPosition(g_NewModSa->iAimPlayerID, fEnemyPos);

					//OL_CalculateAimf1(fEnemyPos, AimData.vecAimPos, AimData.vecAimf1);

					//AimData.fAimZ = 0.0f;
					
					vect3_copy(&g_Players->pRemotePlayer[g_NewModSa->iAimPlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12],
						fEnemyPos);
					vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12],
						fSelfPos);
					AimData.vecAimf1[0] = 0;
					//AimData.vecAimf1[1] = 0; uhmmm problems with height
					AimData.vecAimf1[1] = 0;//((fEnemyPos[2] - 3.5f) > fSelfPos[2]) ? 1.0f : 0.f;
					AimData.vecAimf1[2] = ((fEnemyPos[2] - 3.5f) > fSelfPos[2]) ? 1.0f : -0.01f;//-0.01f;

					vect3_copy(&g_Players->pRemotePlayer[g_NewModSa->iAimPlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12],
						AimData.vecAimPos);

					/* nub wey
					AimData.vecAimPos[0] += g_Players->pRemotePlayer[g_NewModSa->iAimPlayerID]->pPlayerData->onFootData.fMoveSpeed[0]
						* 10.97f;
					AimData.vecAimPos[1] += g_Players->pRemotePlayer[g_NewModSa->iAimPlayerID]->pPlayerData->onFootData.fMoveSpeed[1]
						* 10.97f;
					AimData.vecAimPos[2] += 1.0f + g_Players->pRemotePlayer[g_NewModSa->iAimPlayerID]->pPlayerData->onFootData.fMoveSpeed[2]
						* 10.97f;*/

					OL_CalculateLagPosition(g_NewModSa->iAimPlayerID, AimData.vecAimPos);
					
				}
				
			}
			if (OLCheats->bInvalidAimZ)
			{
				bEditAimSync = true;
				AimData.fAimZ = NAN;
			}

			if (bEditAimSync)
			{
				parameters->Reset();
				parameters->Write((BYTE)ID_AIM_SYNC);
				parameters->Write((PCHAR)&AimData, sizeof(stAimData));
				g_RakFuncs->Send(parameters, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
				return false;
			}
		}
		break;
	case ID_STATS_UPDATE:
		

		if (OLCheats->bFakeFPS)
		{	
			static int i = INT_MAX;
			stStatsData StatsData;
			parameters->ResetReadPointer();
			parameters->Read(packetId);
			parameters->Read((PCHAR)&StatsData, sizeof(stStatsData));
			StatsData.iAmmo = i;
			parameters->Reset();
			parameters->Write((BYTE)ID_STATS_UPDATE);
			parameters->Write((PCHAR)&StatsData, sizeof(stStatsData));
			if (i == 0)
			{
				i = INT_MAX;
			}
			i -= OLCheats->iFPS;
			if (i < 0)
				i = 0;
		}

		break;
	}


	if (set.netPatchAssoc[packetId][OUTCOMING_PACKET] != nullptr && set.netPatchAssoc[packetId][OUTCOMING_PACKET]->enabled)
		return false;
	return true;
}



bool OnReceivePacket(Packet *p)
{
	if (p->data == nullptr || p->length == 0)
		return true;
	if (set.netPatchAssoc[p->data[0]][INCOMING_PACKET] != nullptr && set.netPatchAssoc[p->data[0]][INCOMING_PACKET]->enabled)
		return false;
	if (p->data[0] == ID_MARKERS_SYNC) // packetId
	{
		BitStream	bs(p->data, p->length, false);
		int			iNumberOfPlayers = 0;
		uint16_t	playerID = uint16_t(-1);
		short		sPos[3] = { 0, 0, 0 };
		bool		bIsPlayerActive = false;

		bs.IgnoreBits(8);
		bs.Read(iNumberOfPlayers);
		if (iNumberOfPlayers < 0 || iNumberOfPlayers > SAMP_MAX_PLAYERS)
			return true;

		for (int i = 0; i < iNumberOfPlayers; i++)
		{
			bs.Read(playerID);
			bs.ReadCompressed(bIsPlayerActive);
			if (bIsPlayerActive == 0 || isBadSAMPPlayerID(playerID))
				continue;
			bs.Read(sPos);
			g_stStreamedOutInfo.iPlayerID[playerID] = playerID;
			g_stStreamedOutInfo.fPlayerPos[playerID][0] = sPos[0];
			g_stStreamedOutInfo.fPlayerPos[playerID][1] = sPos[1];
			g_stStreamedOutInfo.fPlayerPos[playerID][2] = sPos[2];
		}
	}
	return true;
}

// commands below

void cmd_change_server(char *param)	// 127.0.0.1 7777 Username Password
{
	traceLastFunc("cmd_change_server()");

//	bool	success = false;

	char	IP[128], Nick[SAMP_MAX_PLAYER_NAME], Password[128] = "", Port[128];
	int		iPort;

	int ipc = sscanf(param, "%s%s%s%s", IP, Port, Nick, Password);
	if (ipc < 2)
	{
		addMessageToChatWindow("USAGE: /mod_change_server <ip> <port> <Username> <Server Password>");
		addMessageToChatWindow("Variables that are set to \"NULL\" (capitalized) will be ignored.");
		addMessageToChatWindow("If you set the Password to \"NULL\" it is set to <no server password>.");
		addMessageToChatWindow("Username and password can also be left out completely.");
		return;
	}
	if (stricmp(IP, "NULL") == NULL)
		strcpy(IP, g_SAMP->szIP);

	if (stricmp(Port, "NULL") == NULL)
		iPort = g_SAMP->ulPort;
	else
		iPort = atoi(Port);

	if (ipc > 2)
	{
		if (stricmp(Nick, "NULL") != NULL)
		{
			if (strlen(Nick) > SAMP_ALLOWED_PLAYER_NAME_LENGTH)
				Nick[SAMP_ALLOWED_PLAYER_NAME_LENGTH] = '\0';
			setLocalPlayerName(Nick);
		}
	}
	if (ipc > 3)
	{
		if (stricmp(Password, "NULL") == NULL)
			strcpy(Password, "");
	}

	changeServer(IP, iPort, Password);
}

void cmd_change_server_fav(char *param)
{
	traceLastFunc("cmd_change_server_fav()");

	if (strlen(param) == 0)
	{
		addMessageToChatWindow("/mod_fav_server <server name/part of server name>");
		addMessageToChatWindow("In order to see the favorite server list type: /mod_fav_server list");
		return;
	}

	if (strncmp(param, "list", 4) == 0)
	{
		int count = 0;
		for (int i = 0; i < INI_SERVERS_MAX; i++)
		{
			if (set.server[i].server_name == NULL)
				continue;

			count++;
			addMessageToChatWindow("%s", set.server[i].server_name);
		}
		if (count == 0)
			addMessageToChatWindow("No servers in favorite server list. Edit the ini file to add some.");
		return;
	}

	for (int i = 0; i < INI_SERVERS_MAX; i++)
	{
		if (set.server[i].server_name == NULL || set.server[i].ip == NULL
			|| strlen(set.server[i].ip) < 7 || set.server[i].port == 0)
			continue;

		if (!findstrinstr((char *)set.server[i].server_name, param))
			continue;

		if (!set.use_current_name)
			setLocalPlayerName(set.server[i].nickname);

		changeServer(set.server[i].ip, set.server[i].port, set.server[i].password);

		return;
	}

	addMessageToChatWindow("/mod_fav_server <server name/part of server name>");
	return;
}

void cmd_current_server(char *param)
{
	addMessageToChatWindow("Server Name: %s", g_SAMP->szHostname);
	addMessageToChatWindow("Server Address: %s:%i", g_SAMP->szIP, g_SAMP->ulPort);
	addMessageToChatWindow("Username: %s", getPlayerName(g_Players->sLocalPlayerID));
}

void cmd_tele_loc(char *param)
{
	if (strlen(param) == 0)
	{
		addMessageToChatWindow("USAGE: /mod_tele_loc <location name>");
		addMessageToChatWindow("Use /mod_tele_locations to show the location names.");
		addMessageToChatWindow("The more specific you are on location name the better the result.");
		return;
	}

	for (int i = 0; i < STATIC_TELEPORT_MAX; i++)
	{
		if (strlen(set.static_teleport_name[i]) == 0 || vect3_near_zero(set.static_teleport[i].pos))
			continue;

		if (!findstrinstr(set.static_teleport_name[i], param))
			continue;

		cheat_state_text("Teleported to: %s.", set.static_teleport_name[i]);
		cheat_teleport(set.static_teleport[i].pos, set.static_teleport[i].interior_id);
		return;
	}

	addMessageToChatWindow("USAGE: /mod_tele_loc <location name>");
	addMessageToChatWindow("Use /mod_tele_locations to show the location names.");
	addMessageToChatWindow("The more specific you are on location name the better the result.");
}

void cmd_tele_locations(char *)
{
	for (int i = 0; i < STATIC_TELEPORT_MAX; i++)
	{
		if (strlen(set.static_teleport_name[i]) == 0 || vect3_near_zero(set.static_teleport[i].pos))
			continue;
		addMessageToChatWindow("%s", set.static_teleport_name[i]);
	}

	addMessageToChatWindow("To teleport use the menu or: /mod_tele_loc <location name>");
}

void cmd_pickup(char *params)
{
	if (!strlen(params))
	{
		addMessageToChatWindow("USAGE: /mod_pickup <pickup id>");
		return;
	}

	g_RakFuncs->SendPickUp(atoi(params));
}

void cmd_setclass(char *params)
{
	if (!strlen(params))
	{
		addMessageToChatWindow("USAGE: /mod_setclass <class id>");
		return;
	}

	g_RakFuncs->RequestClass(atoi(params));
	g_RakFuncs->SendSpawn();
}

void cmd_fakekill(char *params)
{
	int killer, reason, amount;
	if (!strlen(params) || sscanf(params, "%d%d%d", &killer, &reason, &amount) < 3)
		return addMessageToChatWindow("USAGE: /mod_fakekill <killer id> <reason> <amount>");

	if (amount < 1 || killer < 0 || killer > SAMP_MAX_PLAYERS)
		return;

	for (int i = 0; i < amount; i++)
		g_RakFuncs->SendDeath(killer, reason);
}

void cmd_bot_animation(char *params)
{
	int botID, animationID;
	if (!strlen(params) || sscanf(params, "%d %d", &botID, &animationID) < 2)
		return addMessageToChatWindow("USAGE: /.bot_anim <BOT ID> <Animation ID>");
	g_BotFuncs->BotSettings.bBotPick = true;
	g_BotFuncs->BotClient[botID].sCurrentAnimationID = animationID;
}

void cmd_nothing(char *params)
{
	say("--> Download OverLight Mod <--");
}

void cmd_find_chat(char *params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /.find_chat <word>");
	OLCheats->bFind = true;
	OLCheats->WordFindChat = params;
}

void cmd_stop_find_chat(char *params)
{
	OLCheats->bFind = false;
}

void cmd_fake_fps(char *params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /.fake_fps <value>");
	int iFPS = atoi(params);
	OLCheats->bFakeFPS = true;
	OLCheats->iFPS = iFPS;
	addMessageToChatWindow("Fake FPS value changed to: %d", iFPS);
}

void cmd_add_bot_nick(char *params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /.bot_nick nickname");
	g_BotFuncs->OL_Add_Bot_Nick(params);
}

void cmd_bot_steal_nick(char *params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /.bot_steal_nick <player ID>");
	int playerID = atoi(params);
	g_BotFuncs->BotSettings.UseFakeBot = true;
	OLCheats->Target_PlayerID = playerID;
	g_BotFuncs->OL_Add_Bot();
}

void cmd_bot_ping_increase(char *params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /.bot_ping <value>, you can set big negative value for get ping 0");
	int increasePing = atoi(params);
	g_BotFuncs->BotSettings.FakePing = true;
	g_BotFuncs->BotSettings.incrasePing = increasePing;
}

void cmd_bot_follow_anim(char *params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /.bot_fanim <Animation ID>");
	int iAnimID = atoi(params);
	g_BotFuncs->BotSettings.bBotFollowAnim = true;
	g_BotFuncs->BotSettings.sBotFollowAnimID = iAnimID;
}

void cmd_add_friend(char *params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /.add_friend <player id>");
	int playerid = atoi(params);
	
	if (isBadSAMPPlayerID(playerid) || g_Players->iIsListed[playerid] != 1)
		return addMessageToChatWindow("Player does not exist.");

	std::ofstream outputFile;
	char filename[512];
	char *nick;
	snprintf(filename, sizeof(filename), "%s\\" M0D_FOLDER "%s", g_szWorkingDirectory, "friends.ini");
	outputFile.open(filename, std::fstream::out | std::fstream::app);

	if (g_Players->pRemotePlayer[playerid]->iNameAllocated <= 0xF)
	{
		nick = g_Players->pRemotePlayer[playerid]->szPlayerName;
	}
	else 
	{
		nick = g_Players->pRemotePlayer[playerid]->pszPlayerName;
	}
	outputFile << nick << "\n";
	outputFile.close();
	addMessageToChatWindow("Added %s [ID: %d]", nick, playerid);

}


void cmd_warp(char *params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /mod_warp <player id>");

	int playerid = atoi(params);
	if (isBadSAMPPlayerID(playerid) || g_Players->iIsListed[playerid] != 1)
		return addMessageToChatWindow("Player does not exist.");

	float pos[3];
	actor_info *actor = nullptr;
	if (g_Players->pRemotePlayer[playerid]->pPlayerData == nullptr
		|| g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Actor == nullptr)
	{
		if (vect3_near_zero(g_stStreamedOutInfo.fPlayerPos[playerid]))
			return addMessageToChatWindow("Player is not streamed in.");

		vect3_copy(g_stStreamedOutInfo.fPlayerPos[playerid], pos);
		pos[1] += 1.0f;
		cheat_teleport(pos, 0);
		return;
	}

	if (!getPlayerPos(playerid, pos) ||
		g_Players->pRemotePlayer[playerid]->pPlayerData == nullptr ||
		g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Actor == nullptr ||
		(actor = g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Actor->pGTA_Ped) == nullptr)
	{
		return addMessageToChatWindow("Bad player info.");
	}

	pos[1] += 1.0f;
	cheat_teleport(pos, actor->base.interior_id);
}

void cmd_showCMDS(char *)
{
	for (int i = 0; i < g_m0dCmdNum; i++)
	{
		addMessageToChatWindow("%s", g_m0dCmdlist[i]);
	}
}

void cmd_ol_show_cmds(char *)
{
	addMessageToChatWindow("/.add_friend <player ID> /.fake_fps <value fps> /.stick <id> /.ol_cmds");
	addMessageToChatWindow("/.bot_nick <nickname> /.bot_steal_nick <player ID>");
	addMessageToChatWindow("/.bot_ping <increase value> /.bot_anim <BOT ID> <Anim ID> /.bot_fanim <Anim ID>");
	addMessageToChatWindow("/.find_chat <words> /.stop_find /.spec");
}


void cmd_stick(char *params)
{
	cheat_state->actor.stick ^= true;
	cheat_state->vehicle.stick ^= true;
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /.stick <player id>");
	OLCheats->iStickID = atoi(params);
	addMessageToChatWindow("Sticked");
}

void cmd_ol_spec(char *params)
{
	int playerID = atoi(params);
	g_Players->pLocalPlayer->iIsSpectating = 1;
	g_Players->pLocalPlayer->iSpectateID = playerID;
	g_Players->pLocalPlayer->iInitiatedSpectating = 1;
	OLCheats->iSpectateID = playerID;
	OLCheats->bSpectate = true;
}

void cmd_ol_specoff(char *params)
{
	g_Players->pLocalPlayer->iIsSpectating = 0;
	OLCheats->bSpectate = false;
}

void cmd_ol_bot_say(char *params)
{
	char Say[50];

	int botID;

	if (!strlen(params) || sscanf(params, "%d %[^\n]", &botID, &Say) < 2)
		return addMessageToChatWindow("USAGE: /.bot_say <BOT ID> <Text bla bla>");

	if (botID >= g_BotFuncs->BotSettings.ClientCount)
		return addMessageToChatWindow("This bot id isn't connected");
	BitStream bsChat;
	if (Say[0] == '/')
	{
		int Len;
		Len = strlen(Say);
		

		bsChat.Write(Len);
		bsChat.Write(Say, Len);
		g_BotFuncs->OL_Bot_SendRPC(botID, RPC_ServerCommand, bsChat, HIGH_PRIORITY, RELIABLE_SEQUENCED, NULL, false);
	}	
	else
	{
		BYTE Len;
		Len = (BYTE)strlen(Say);


		bsChat.Write(Len);
		bsChat.Write(Say, Len);
		g_BotFuncs->OL_Bot_SendRPC(botID, RPC_Chat, bsChat, HIGH_PRIORITY, RELIABLE_SEQUENCED, NULL, false);
	}
}

void initChatCmds(void)
{
	if (g_m0dCommands == true)
		return;

	cheat_state_text("initiated modcommands");
	g_m0dCommands = true;

	addClientCommand("mod_show_cmds", cmd_showCMDS);
	addClientCommand("mod_change_server", cmd_change_server);
	addClientCommand("mod_fav_server", cmd_change_server_fav);
	addClientCommand("mod_current_server", cmd_current_server);
	addClientCommand("mod_tele_loc", cmd_tele_loc);
	addClientCommand("mod_teleport_location", cmd_tele_loc);
	addClientCommand("mod_tele_locations", cmd_tele_locations);
	addClientCommand("mod_teleport_locations", cmd_tele_locations);
	addClientCommand("mod_pickup", cmd_pickup);
	addClientCommand("mod_setclass", cmd_setclass);
	addClientCommand("mod_fakekill", cmd_fakekill);
	addClientCommand("mod_warp", cmd_warp);
	// troll admins xD
	//addClientCommand("ban", cmd_nothing);
	//addClientCommand("rcon ban", cmd_nothing);
	//addClientCommand("rcon kick", cmd_nothing);
	//addClientCommand("banip", cmd_nothing);
	//addClientCommand("kick", cmd_nothing);
	addClientCommand(".add_friend", cmd_add_friend);
	addClientCommand(".fake_fps", cmd_fake_fps);
	addClientCommand(".bot_nick", cmd_add_bot_nick);
	addClientCommand(".bot_steal_nick", cmd_bot_steal_nick);
	addClientCommand(".bot_ping", cmd_bot_ping_increase);
	addClientCommand(".bot_anim", cmd_bot_animation);
	addClientCommand(".bot_fanim", cmd_bot_follow_anim);
	addClientCommand(".bot_say", cmd_ol_bot_say);
	addClientCommand(".find_chat", cmd_find_chat);
	addClientCommand(".stop_find", cmd_stop_find_chat);
	addClientCommand(".stick", cmd_stick);
	addClientCommand(".ol_cmds", cmd_ol_show_cmds);
	addClientCommand(".spec", cmd_ol_spec);
	addClientCommand(".specoff", cmd_ol_specoff);

}

void BIG_NUM_MUL(unsigned long in[5], unsigned long out[6], unsigned long factor)
{

	//based on TTMath library by Tomasz Sowa.

	unsigned long src[5] = { 0 };
	for (int i = 0; i < 5; i++)
		src[i] = ((in[4 - i] >> 24) | ((in[4 - i] << 8) & 0x00FF0000) | ((in[4 - i] >> 8) & 0x0000FF00) | (in[4 - i] << 24));

	unsigned long long tmp = 0;

	tmp = unsigned long long(src[0])*unsigned long long(factor);
	out[0] = tmp & 0xFFFFFFFF;
	out[1] = tmp >> 32;
	tmp = unsigned long long(src[1])*unsigned long long(factor) + unsigned long long(out[1]);
	out[1] = tmp & 0xFFFFFFFF;
	out[2] = tmp >> 32;
	tmp = unsigned long long(src[2])*unsigned long long(factor) + unsigned long long(out[2]);
	out[2] = tmp & 0xFFFFFFFF;
	out[3] = tmp >> 32;
	tmp = unsigned long long(src[3])*unsigned long long(factor) + unsigned long long(out[3]);
	out[3] = tmp & 0xFFFFFFFF;
	out[4] = tmp >> 32;
	tmp = unsigned long long(src[4])*unsigned long long(factor) + unsigned long long(out[4]);
	out[4] = tmp & 0xFFFFFFFF;
	out[5] = tmp >> 32;

	for (int i = 0; i < 12; i++)
	{
		unsigned char temp = ((unsigned char*)out)[i];
		((unsigned char*)out)[i] = ((unsigned char*)out)[23 - i];
		((unsigned char*)out)[23 - i] = temp;
	}
}


// thanks to raksamp devs!
int gen_rand_gpci(char buf[64], unsigned long factor)
{
	unsigned char out[6 * 4] = { 0 };

	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	for (int i = 0; i < 6 * 4; ++i)
		out[i] = alphanum[rand() % (sizeof(alphanum) - 1)];


	out[6 * 4] = 0;

	BIG_NUM_MUL((unsigned long*)out, (unsigned long*)out, factor);

	unsigned int notzero = 0;
	buf[0] = '0'; buf[1] = '\0';

	if (factor == 0) return 1;

	int pos = 0;
	for (int i = 0; i < 24; i++)
	{
		unsigned char tmp = out[i] >> 4;
		unsigned char tmp2 = out[i] & 0x0F;

		if (notzero || tmp)
		{
			buf[pos++] = (char)((tmp > 9) ? (tmp + 55) : (tmp + 48));
			if (!notzero) notzero = 1;
		}

		if (notzero || tmp2)
		{
			buf[pos++] = (char)((tmp2 > 9) ? (tmp2 + 55) : (tmp2 + 48));
			if (!notzero) notzero = 1;
		}
	}
	buf[pos] = 0;

	return pos;
}

void gen_random(char *s, const int len)
{
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i)
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];

	s[len] = 0;
}

void gen_randomletter(char *s, const int len)
{
	static const char alphanum[] =
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i)
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];

	s[len] = 0;
}

void gen_randomnumber(char *s, const int len)
{
	static const char alphanum[] =
		"0123456789";

	for (int i = 0; i < len; ++i)
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];

	s[len] = 0;
}

void gen_randombinary(char *s, const int len)
{
	static const char alphanum[] =
		"01";

	for (int i = 0; i < len; ++i)
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];

	s[len] = 0;
}

void gen_randomuppercase(char *s, const int len)
{
	static const char alphanum[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	for (int i = 0; i < len; ++i)
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];

	s[len] = 0;
}