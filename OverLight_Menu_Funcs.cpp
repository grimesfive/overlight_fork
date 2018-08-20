#include "main.h"

//OVERLIGHT MENU FUNCTIONS
int OverLight_CarFunctions_Callback(int op, struct menu_item *item)
{
	if (g_SAMP == NULL)
		return 0;
	int increase = 0;
	char name[128];
	switch (op)
	{
	case MENU_OP_SELECT:
		switch (item->id)
		{
		case ID_CAR_SHOOTER:
			OLCheats->bCarShooter ^= true;
			break;
		case ID_FANTOZZI_CLOUD:
			OLCheats->bFantozziCloud ^= true;
			break;
		case ID_FAKE_CAR_SHOOTER:
			OLCheats->bFakeShooter ^= true;
			break;
		case ID_INVISIBLE_BIKE:
			OLCheats->bInvisibleBike ^= true;
			break;
		case ID_INVISIBLE_BIKE_2:
			OLCheats->bInvisibleBike2 ^= true;
			break;
		case ID_FIX_INVISIBLE_FIRE:
			OLCheats->bFixInvisibleFire ^= true;
			break;
		case ID_SHOOTER_ONCE:
			OLCheats->bShooterOnce ^= true;
			break;
		case ID_FUS_RO_DAH:
			OLCheats->bFusRoDah ^= true;
			break;
		case ID_PICK_CAR:
			OLCheats->bPickCar ^= true;
			break;
		case ID_USE_NEAREST_TARGET:
			OLCheats->bUseNearestTarget ^= true;
			break;
		case ID_VEHICLE_ELEVATOR:
			OLCheats->bVehicleElevator ^= true;
			break;
		case ID_VEHICLE_ELEVATOR_DOWN:
			OLCheats->bVehicleElevatorDown ^= true;
			break;
		case ID_KICK_PASSENGER:
			OLCheats->bKickPassenger ^= true;
			break;
		case ID_STEAL_PASSENGER:
			OLCheats->bStealPassenger ^= true;
			break;
		case ID_CAR_STREAMED_GRABBER:
			OLCheats->bVehicleGrabber ^= true;
			break;
		case ID_TRAILER_GRABBER:
			OLCheats->bTrailerGrabber ^= true;
			break;
		case ID_CAR_MAP_GRABBER:
			OLCheats->bVehicleGrabberMap ^= true;
			break;
		case ID_PLANE_CRASHER:
			OLCheats->bPlaneCrasher ^= true;
			break;
		case ID_VORTEX_CRASHER:
			OLCheats->bVortexCrasher ^= true;
			break;
		case ID_USE_FRIEND_TARGET:
			OLCheats->bUseFriendTarget ^= true;
			break;
		case ID_DOOR_STORM:
			OLCheats->bDoorStorm ^= true;
			break;
		case ID_VEHICLE_BUGGER:
			OLCheats->bVehicleBugger ^= true;
			break;
			//sync type
		case ID_SYNC_INCAR_MENU:
			OLCheats->iSyncType = VEHICLE_SYNC_IN_CAR;
			break;
		case ID_SYNC_UNOCCUPIED_MENU:
			OLCheats->iSyncType = VEHICLE_SYNC_UNOCCUPIED;
			break;
		case ID_SYNC_INCAR_RPC_MENU:
			OLCheats->iSyncType = VEHICLE_SYNC_IN_CAR_RPC;
			break;

		}
		break;
	case MENU_OP_ENABLED:
		switch (item->id)
		{
			case ID_CAR_SHOOTER:
				return OLCheats->bCarShooter;
				break;
			case ID_FANTOZZI_CLOUD:
				return OLCheats->bFantozziCloud;
				break;
			case ID_FAKE_CAR_SHOOTER:
				return OLCheats->bFakeShooter;
				break;
			case ID_INVISIBLE_BIKE:
				return OLCheats->bInvisibleBike;
				break;
			case ID_FIX_INVISIBLE_FIRE:
				return OLCheats->bFixInvisibleFire;
				break;
			case ID_INVISIBLE_BIKE_2:
				return OLCheats->bInvisibleBike2;
				break;
			case ID_SHOOTER_ONCE:
				return OLCheats->bShooterOnce;
				break;
			case ID_FUS_RO_DAH:
				return OLCheats->bFusRoDah;
				break;
			case ID_PICK_CAR:
				return OLCheats->bPickCar;
				break;
			case ID_USE_NEAREST_TARGET:
				return OLCheats->bUseNearestTarget;
				break;
			case ID_VEHICLE_ELEVATOR:
				return OLCheats->bVehicleElevator;
				break;
			case ID_VEHICLE_ELEVATOR_DOWN:
				return OLCheats->bVehicleElevatorDown;
				break;
			case ID_KICK_PASSENGER:
				return OLCheats->bKickPassenger;
				break;
			case ID_STEAL_PASSENGER:
				return OLCheats->bStealPassenger;

			case ID_PLANE_CRASHER:
				return OLCheats->bPlaneCrasher;

			case ID_VORTEX_CRASHER:
				return OLCheats->bVortexCrasher;

			case ID_CAR_STREAMED_GRABBER:
				return OLCheats->bVehicleGrabber;

			case ID_TRAILER_GRABBER:
				return OLCheats->bTrailerGrabber;

			case ID_CAR_MAP_GRABBER:
				return OLCheats->bVehicleGrabberMap;

			case ID_USE_FRIEND_TARGET:
				return OLCheats->bUseFriendTarget;
			case ID_DOOR_STORM:
				return OLCheats->bDoorStorm;
			case ID_VEHICLE_BUGGER:
				return OLCheats->bVehicleBugger;

			case ID_SELECT_PLAYER_TARGET:
				if (OLCheats->Target_PlayerID != -1)
				{
					
					if (IsPlayerStreamed(OLCheats->Target_PlayerID))
					{
						snprintf(name, sizeof(name), "\aTarget: %s <%d>", getPlayerName(OLCheats->Target_PlayerID), OLCheats->Target_PlayerID);
						menu_item_name_set(item, name);
					}
					else
					{
						snprintf(name, sizeof(name), "\aTarget: <%d> [Unstreamed]", OLCheats->Target_PlayerID);
						menu_item_name_set(item, name);
					}
				}
				else
				{
					OLCheats->Target_PlayerID = NearestPlayer();
				}
				return 0;
			case ID_SELECT_PASSENGER_TARGET:
				if (OLCheats->Target_Passenger_PlayerID != -1)
				{

					if (IsPlayerStreamed(OLCheats->Target_Passenger_PlayerID))
					{
						snprintf(name, sizeof(name), "\aPassenger: %s <%d>", getPlayerName(OLCheats->Target_Passenger_PlayerID), OLCheats->Target_Passenger_PlayerID);
						menu_item_name_set(item, name);
					}
					else
					{
						snprintf(name, sizeof(name), "\aPassenger: <%d> [Unstreamed]", OLCheats->Target_Passenger_PlayerID);
						menu_item_name_set(item, name);
					}
				}
				else
				{
					OLCheats->Target_Passenger_PlayerID = NearestPlayer();
				}
				return 0;

			case ID_CAR_SEND_RATE:
			{
				snprintf(name, sizeof(name), "\aCar Send Rate: %d", OLCheats->SendRateCarCheatTick);
				menu_item_name_set(item, name);
				return 0;
			}
				//sync type
			case ID_SYNC_INCAR_MENU:
				if (OLCheats->iSyncType == VEHICLE_SYNC_IN_CAR)
					return 1;
				return 0;
			case ID_SYNC_UNOCCUPIED_MENU:
				if (OLCheats->iSyncType == VEHICLE_SYNC_UNOCCUPIED)
					return 1;
				return 0;
			case ID_SYNC_INCAR_RPC_MENU:
				if (OLCheats->iSyncType == VEHICLE_SYNC_IN_CAR_RPC)
					return 1;
				return 0;

		}
		
	case MENU_OP_DEC:
	case MENU_OP_INC:
		increase = (op == MENU_OP_DEC) ? -1 : 1;
		switch (item->id)
		{
			case ID_SELECT_PLAYER_TARGET:
				OLCheats->Target_PlayerID = NextPlayer(OLCheats->Target_PlayerID, increase);
				break;
			case ID_SELECT_PASSENGER_TARGET:
				OLCheats->Target_Passenger_PlayerID = NextPlayerPassenger(OLCheats->Target_Passenger_PlayerID, increase);
				break;
			case ID_CAR_SEND_RATE:
				OLCheats->SendRateCarCheatTick += 5 * increase;
				break;
		}
	}

	return 0;
}


int OverLight_PlayerFunctions_Callback(int op, struct menu_item *item)
{
	if (g_SAMP == NULL)
		return 0;
	int increase = 0;
	switch (op)
	{
	case MENU_OP_SELECT:
		switch (item->id)
		{
			case ID_INVERT_WALK:
				OLCheats->bInvertWalk ^= true;
				break;
			case ID_WHEEL_WALK:
				OLCheats->bWheelWalk ^= true;
				break;
			case ID_SLAPPER:
				OLCheats->bSlapper ^= true;
				break;
			case ID_ELEVATOR:
				OLCheats->bElevator ^= true;
				break;
			case ID_JETPACK_ELEVATOR:
				OLCheats->bJetpackElevator ^= true;
				break;
			case ID_INVERT_WALK_2:
				OLCheats->bInvertWalk2 ^= true;
				break;
			case ID_PLAYER_HOLOGRAM_MODE:
				OLCheats->bPlayerHologram ^= true;
				break;
			case ID_CAR_HOLOGRAM_MODE:
				OLCheats->bVehicleHologram ^= true;
				break;
			case ID_NOFALL:
				OLCheats->bNoFall ^= true;
				break;
			case ID_INVALID_AIMZ:
				OLCheats->bInvalidAimZ ^= true;
				break;
			case ID_USE_NEAREST_PLAYER:
				OLCheats->bUseNearestTarget ^= true;
				break;
			case ID_USE_FRIEND_TARGET2:
				OLCheats->bUseFriendTarget ^= true;
				break;
		}
		break;
	case MENU_OP_ENABLED:
		switch (item->id)
		{
			case ID_INVERT_WALK:
				return OLCheats->bInvertWalk;
			case ID_SLAPPER:
				return OLCheats->bSlapper;
			case ID_WHEEL_WALK:
				return OLCheats->bWheelWalk;
			case ID_ELEVATOR:
				return OLCheats->bElevator;
			case ID_JETPACK_ELEVATOR:
				return OLCheats->bJetpackElevator;
			case ID_INVERT_WALK_2:
				return OLCheats->bInvertWalk2;
			case ID_PLAYER_HOLOGRAM_MODE:
				return OLCheats->bPlayerHologram;
			case ID_CAR_HOLOGRAM_MODE:
				return OLCheats->bVehicleHologram;
			case ID_NOFALL:
				return OLCheats->bNoFall;
			case ID_INVALID_AIMZ:
				return OLCheats->bInvalidAimZ;
			case ID_USE_NEAREST_PLAYER:
				return OLCheats->bUseNearestTarget;
			case ID_USE_FRIEND_TARGET2:
				return OLCheats->bUseFriendTarget;

			case ID_SELECT_PLAYER_TARGET_2:
				if (OLCheats->Target_PlayerID != -1)
				{
					char name[128];
					if (IsPlayerStreamed(OLCheats->Target_PlayerID))
					{
						snprintf(name, sizeof(name), "\aTarget: %s <%d>", getPlayerName(OLCheats->Target_PlayerID), OLCheats->Target_PlayerID);
						menu_item_name_set(item, name);
					}
					else
					{
						snprintf(name, sizeof(name), "\aTarget: <%d> [Unstreamed]", OLCheats->Target_PlayerID);
						menu_item_name_set(item, name);
					}
				}
				else
				{
					OLCheats->Target_PlayerID = NearestPlayer();
				}
				return 0;

		}
		break;
	case MENU_OP_DEC:
	case MENU_OP_INC:
		increase = (op == MENU_OP_DEC) ? -1 : 1;
		switch (item->id)
		{
		case ID_SELECT_FAKE_FPS:
			OLCheats->iFPS += increase;
			break;
		case ID_SELECT_PLAYER_TARGET_2:
			OLCheats->Target_PlayerID = NextPlayer(OLCheats->Target_PlayerID, increase);
			break;
		}
	break;
	}
	return 0;
}


int OverLight_DM_Stuff_Callback(int op, struct menu_item *item)
{
	if (g_SAMP == NULL)
		return 0;
	int increase = 0;
	switch (op)
	{
		case MENU_OP_SELECT:
			switch (item->id)
			{
				case ID_SILENT_AIM:
					OLCheats->bSilentAim ^= true;
					break;
				case ID_SHOT_WALL:
					OLCheats->bShotWall ^= true;
					OLCheats->bSilentAim = OLCheats->bShotWall;
					break;
				case ID_BYPASS_ANTICHEAT:
					OLCheats->bBypassAimAntiCheat ^= true;
					break;
				case ID_BYPASS_ANTICHEAT_2:
					OLCheats->bBypassAimAntiCheat2 ^= true;
					break;
				case ID_FAKE_LAG_SYNC:
					OLCheats->bFakeLagSync ^= true;
					break;
				case ID_SYNC_WHEN_AIM:
					OLCheats->bSyncWhenAim ^= true;
					break;
				case ID_DRAW_FINDER:
					OLCheats->bRenderFinder ^= true;
					break;
				case ID_FAKE_LAG_SPEED:
					OLCheats->bFakeLagSpeed ^= true;
					break;
				case ID_CHAMS:
					OLCheats->bChams ^= true;
					break;
				case ID_SHOT_REPEATER:
					OLCheats->bShotRepeater ^= true;
					break;
				case ID_AUTO_C_BUG:
					OLCheats->bAutoCBug ^= true;
					break;
				case ID_AUTO_SCROLL:
					OLCheats->bAutoScroll ^= true;
					break;
				case ID_ANTISTUN:
					OLCheats->bAntiStun ^= true;
					break;
				case ID_NOSPREAD:
					OLCheats->bNoSpread ^= true;
					break;
				case ID_AUTOSHOT:
					OLCheats->bAutoShot ^= true;
					break;
				case ID_SLOWSENSAIM:
					OLCheats->bSlowAim ^= true;
					break;
			}
			break;
		case MENU_OP_ENABLED:
			switch (item->id)
			{
				case ID_ANTISTUN:
					return OLCheats->bAntiStun;
				case ID_SILENT_AIM:
					return OLCheats->bSilentAim;
				case ID_SHOT_WALL:
					return OLCheats->bShotWall;
				case ID_BYPASS_ANTICHEAT:
					return OLCheats->bBypassAimAntiCheat;
				case ID_BYPASS_ANTICHEAT_2:
					return OLCheats->bBypassAimAntiCheat2;
				case ID_FAKE_LAG_SYNC:
					return OLCheats->bFakeLagSync;
				case ID_SYNC_WHEN_AIM:
					return OLCheats->bSyncWhenAim;
				case ID_DRAW_FINDER:
					return OLCheats->bRenderFinder;
				case ID_FAKE_LAG_SPEED:
					return OLCheats->bFakeLagSpeed;
				case ID_CHAMS:
					return OLCheats->bChams;
				case ID_SHOT_REPEATER:
					return OLCheats->bShotRepeater;
					break;
				case ID_AUTO_C_BUG:
					return OLCheats->bAutoCBug;
				case ID_AUTOSHOT:
					return OLCheats->bAutoShot;
				case ID_AUTO_SCROLL:
					return OLCheats->bAutoScroll;
				case ID_NOSPREAD:
					return OLCheats->bNoSpread;
				case ID_SLOWSENSAIM:
					return OLCheats->bSlowAim;
				case ID_SLOWINC:
				{
					char name[128];
					snprintf(name, sizeof(name), "\aSlow Aim: %.2f", OLCheats->fSlowValue);
					menu_item_name_set(item, name);
					return OLCheats->bSlowAim;
				}
				case ID_SPEED_MULTIPLIER:
				{
					char name[128];
					snprintf(name, sizeof(name), "\aSpeed Multipler: %.2f", OLCheats->fMultiplierFakeSpeed);
					menu_item_name_set(item, name);
					break;
				}


				case ID_FAKE_LAG_TIME_SYNC:
				{
					char name[128];
					snprintf(name, sizeof(name), "\aTime Sync: %d ms", OLCheats->dwTimeFakeLagSync);
					menu_item_name_set(item, name);
					return OLCheats->bFakeLagSync;
					break;
				}
				case ID_FAKE_LAG_TIME_DESYNC:
				{
					char name[128];
					snprintf(name, sizeof(name), "\aTime Desync (AFK): %d ms", OLCheats->dwTimeFakeLagDesync);
					menu_item_name_set(item, name);
					return OLCheats->bFakeLagSync;
					break;
				}
			}
		break;
		case MENU_OP_DEC:
		case MENU_OP_INC:
			increase = (op == MENU_OP_DEC) ? -1 : 1;
			switch (item->id)
			{
			case ID_SPEED_MULTIPLIER:
				OLCheats->fMultiplierFakeSpeed += (float)increase / 10.0f;
				break;
			case ID_FAKE_LAG_TIME_SYNC:
				OLCheats->dwTimeFakeLagSync += increase * 10.0f;
				break;
			case ID_FAKE_LAG_TIME_DESYNC:
				OLCheats->dwTimeFakeLagDesync += increase * 10.0f;
				break;
			case ID_SLOWINC:
				OLCheats->fSlowValue += increase * 10.0f;
				break;
			}
			break;
	}
	return 0;
}

int OverLight_Silent_Aim_Stuff_Callback(int op, struct menu_item *item)
{
	if (g_SAMP == NULL)
		return 0;
	int increase = 0;
	switch (op)
	{
	case MENU_OP_SELECT:
		switch (item->id)
		{
		case ID_FILTER_FRIEND:
			OLCheats->bFriendFilter ^= true;
			break;
		case ID_FILTER_TEAM:
			OLCheats->bTeamFilter ^= true;
			break;
		case ID_FILTER_COLOR:
			OLCheats->bColorFilter ^= true;
			break;
		case ID_FILTER_SKIN:
			OLCheats->bSkinFilter ^= true;
			break;
		case ID_FILTER_VISIBLE:
			OLCheats->bVisibleFilter ^= true;
			break;
		case ID_FILTER_AFK:
			OLCheats->bAFKFilter ^= true;
			break;
		case ID_FILTER_ALIVE:
			OLCheats->bAlive ^= true;
			break;
		case ID_NOT_OUT_OF_RANGE:
			OLCheats->bOutOfRange ^= true;
			break;

		}
		break;
	case MENU_OP_ENABLED:
		switch (item->id)
		{
		case ID_FILTER_FRIEND:
			return OLCheats->bFriendFilter;
		case ID_FILTER_TEAM:
			return OLCheats->bTeamFilter;
		case ID_FILTER_COLOR:
			return OLCheats->bColorFilter;
		case ID_FILTER_SKIN:
			return OLCheats->bSkinFilter;
		case ID_FILTER_VISIBLE:
			return OLCheats->bVisibleFilter;
		case ID_FILTER_AFK:
			return OLCheats->bAFKFilter;
		case ID_FILTER_ALIVE:
			return OLCheats->bAlive;
		case ID_NOT_OUT_OF_RANGE:
			return OLCheats->bOutOfRange;

		case ID_INACCURATE_AIM:
		{
			char name[128];
			snprintf(name, sizeof(name), "\aRandom Max Imprecision: %.2f", OLCheats->fInaccurateAim);
			menu_item_name_set(item, name);
			break;
		}
		}
		break;
	case MENU_OP_DEC:
	case MENU_OP_INC:
		increase = (op == MENU_OP_DEC) ? -1 : 1;
		switch (item->id)
		{
		case ID_INACCURATE_AIM:
			OLCheats->fInaccurateAim += (float)increase / 10.0f;
			break;
		}
		break;
	}
	return 0;
}

int OverLight_Bot_Stuff_Callback(int op, struct menu_item *item)
{
	if (g_SAMP == NULL)
		return 0;
	int increase = 0;
	short TargetID = g_BotFuncs->BotSettings.sTargetID;
	switch (op)
	{
	case MENU_OP_SELECT:
		switch (item->id)
		{
			case ID_CONNECT_BOT:
				g_BotFuncs->Client_in_Queue++;
				break;
			case ID_N_CONNECT_BOT:
				g_BotFuncs->Client_in_Queue += g_BotFuncs->N_Client_to_Add_In_Queue;
				break;
			case ID_FAKE_PING:
				g_BotFuncs->BotSettings.FakePing ^= true;
				break;

			case ID_DELETE_BOT:
				g_BotFuncs->OL_Delete_Bot();
				break;
			case ID_DELETE_ALL_BOT:
				for (int i = g_BotFuncs->BotSettings.ClientCount; i > 0; i--)
					g_BotFuncs->OL_Delete_Bot();
				break;
			case ID_BOT_FOLLOW:
				g_BotFuncs->BotSettings.bFollow ^= true;
				break;
			case ID_BOT_SPAWN:
				for(int j = 0; j < 2; j++)
					for (int i = g_BotFuncs->BotSettings.ClientCount -1; i >= 0; i--)
						g_BotFuncs->OL_Random_Spawn_Bot(i);
				break;
			case ID_BOT_SILENT:
				g_BotFuncs->BotSettings.bBotSilent ^= true;
				break;
			case ID_BOT_SHOOTER:
				g_BotFuncs->BotSettings.bShooter ^= true;
				break;
			case ID_BOT_USE_NEAREST_TARGET:
				g_BotFuncs->BotSettings.bUseNearestTarget ^= true;
				break;
			case ID_BOT_ATTACKER:
				g_BotFuncs->BotSettings.bBotAttacker ^= true;
				break;
			case ID_BOT_ELEVATOR:
				g_BotFuncs->BotSettings.bBotElevator ^= true;
				break;
			case ID_BOT_JETPACK_ELEVATOR:
				g_BotFuncs->BotSettings.bBotJetpackElevator ^= true;
				break;
			case ID_BOT_PICK:
				g_BotFuncs->BotSettings.bBotPick ^= true;
				break;
			case ID_BOT_SLAPPER:
				g_BotFuncs->BotSettings.bBotSlapper ^= true;
				break;
			case ID_BOT_FUCK:
				g_BotFuncs->BotSettings.bBotFuck ^= true;
				break;
			case ID_BOT_FOLLOW_ANIM:
				g_BotFuncs->BotSettings.bBotFollowAnim ^= true;
				break;
			case ID_USE_FRIEND_TARGET3:
				OLCheats->bUseFriendTarget ^= true;
				break;
			case ID_UIF_DM_FARMER:
				g_BotFuncs->BotSettings.bUIF_DM_Farmer ^= true;
				break;
			case ID_BOT_SUN:
				g_BotFuncs->BotSettings.bBotSun ^= true;
				break;
			case ID_BOT_STORM:
				g_BotFuncs->BotSettings.bStorm ^= true;
				break;
		}
		break;
	case MENU_OP_ENABLED:
		switch (item->id)
		{
		char name[128];
		case ID_N_CONNECT_BOT:
			snprintf(name, sizeof(name), "\aConnect %d Bots", g_BotFuncs->N_Client_to_Add_In_Queue);
			menu_item_name_set(item, name);
			break;
	
		case ID_FAKE_PING:
			return g_BotFuncs->BotSettings.FakePing;
			break;
	
		case ID_BOT_SILENT:
			return g_BotFuncs->BotSettings.bBotSilent;
			break;

		case ID_BOT_SHOOTER:
			return g_BotFuncs->BotSettings.bShooter;
			break;

		case ID_INCREASE_PING:
			snprintf(name, sizeof(name), "\aIncrease Ping: %d", g_BotFuncs->BotSettings.incrasePing);
			menu_item_name_set(item, name);
			break;

		case ID_BOT_FOLLOW:
			return g_BotFuncs->BotSettings.bFollow;
			break;
		case ID_BOT_DISTANCE:
		{
			snprintf(name, sizeof(name), "\aDistance From Player: %.2f", g_BotFuncs->BotSettings.fDistanceFollow);
			menu_item_name_set(item, name);

			return g_BotFuncs->BotSettings.bFollow;
			break;
		}
		break;
		case ID_BOT_DISTANCE_2:
		{
			snprintf(name, sizeof(name), "\aDistance From Bots: %.2f", g_BotFuncs->BotSettings.fDistanceFollowBetweenBots);
			menu_item_name_set(item, name);

			return g_BotFuncs->BotSettings.bFollow;
			break;
		}

		case ID_BOT_SILENT_MULTIPLIER:
		{
			snprintf(name, sizeof(name), "\aBot Silent Multiplier: %.2f", g_BotFuncs->BotSettings.fMultiplierSilent);
			menu_item_name_set(item, name);
			break;
		}
		case ID_BOT_WEAPON:
		{
			snprintf(name, sizeof(name), "\aBot Att.Weapon ID: %d" , g_BotFuncs->BotSettings.uiAttackWeaponID);
			menu_item_name_set(item, name);
			break;
		}

		case ID_BOT_DELAY:
		{
			snprintf(name, sizeof(name), "\aBot Delay: %d ms", g_BotFuncs->BotSettings.UpdateSendRate);
			menu_item_name_set(item, name);
			return true;
		}


		case ID_BOT_SELECT_TARGET:
			if (TargetID >= 0)
			{
				char name[128];
				if (IsPlayerStreamed(TargetID))
				{
					snprintf(name, sizeof(name), "\aTarget ID: %s <%d>", getPlayerName(TargetID), TargetID);
					menu_item_name_set(item, name);
				}
				else
				{
					snprintf(name, sizeof(name), "\aTarget ID: <%d> [Unstreamed]", TargetID);
					menu_item_name_set(item, name);
				}
			}
			else
			{
				//snprintf(name, sizeof(name), "\aTroll: <%d> [Unstreamed]", TargetID);
				//menu_item_name_set(item, "\aTarget ID: Nearest Player");
				g_BotFuncs->BotSettings.sTargetID = NearestPlayer();
			}
			break;
		case ID_BOT_USE_NEAREST_TARGET:
			return g_BotFuncs->BotSettings.bUseNearestTarget;
			break;
		case ID_BOT_ATTACKER:
			return g_BotFuncs->BotSettings.bBotAttacker;
			break;
		case ID_BOT_ELEVATOR:
			return g_BotFuncs->BotSettings.bBotElevator;
			break;
		case ID_BOT_SUN:
			return g_BotFuncs->BotSettings.bBotSun;
			break;
		case ID_BOT_JETPACK_ELEVATOR:
			return g_BotFuncs->BotSettings.bBotJetpackElevator;
			break;
		case ID_BOT_PICK:
			return g_BotFuncs->BotSettings.bBotPick;
			break;
		case ID_BOT_SLAPPER:
			return g_BotFuncs->BotSettings.bBotSlapper;
			break;
		case ID_BOT_FUCK:
			return g_BotFuncs->BotSettings.bBotFuck;
			break;
		case ID_BOT_FOLLOW_ANIM:
			return g_BotFuncs->BotSettings.bBotFollowAnim;
			break;
		case ID_BOT_FOLLOW_SEL_ANIM:
		{
			snprintf(name, sizeof(name), "\aBot Foll. Anim: %d", g_BotFuncs->BotSettings.sBotFollowAnimID);
			menu_item_name_set(item, name);
			break;
		}
		case ID_USE_FRIEND_TARGET3:
			return OLCheats->bUseFriendTarget;
		case ID_UIF_DM_FARMER:
			return g_BotFuncs->BotSettings.bUIF_DM_Farmer;
		case ID_BOT_STORM:
			return g_BotFuncs->BotSettings.bStorm;
	}
	break;
	case MENU_OP_DEC:
	case MENU_OP_INC:
		increase = (op == MENU_OP_DEC) ? -1 : 1;
		switch (item->id)
		{
		case ID_N_CONNECT_BOT:
			g_BotFuncs->N_Client_to_Add_In_Queue += increase;
			break;
		case ID_INCREASE_PING:
			g_BotFuncs->BotSettings.incrasePing += increase;
			break;

		case ID_BOT_DISTANCE:
			g_BotFuncs->BotSettings.fDistanceFollow += (float)increase / 10.0f;
			break;
		case ID_BOT_DISTANCE_2:
			g_BotFuncs->BotSettings.fDistanceFollowBetweenBots += (float)increase / 10.0f;
			break;

		case ID_BOT_SILENT_MULTIPLIER:
			g_BotFuncs->BotSettings.fMultiplierSilent += 0.01f * increase;
			break;
		case ID_BOT_SELECT_TARGET:
			g_BotFuncs->BotSettings.sTargetID = NextPlayer(g_BotFuncs->BotSettings.sTargetID, increase);
			break;
		case ID_BOT_WEAPON:
			g_BotFuncs->BotSettings.uiAttackWeaponID += increase;
			break;
		case ID_BOT_FOLLOW_SEL_ANIM:
			g_BotFuncs->BotSettings.sBotFollowAnimID += increase;
			break;

		case ID_BOT_DELAY:
			g_BotFuncs->BotSettings.UpdateSendRate += increase * 5;
			break;
		}
		break;
		
	}
	return 0;
}

int OverLight_Client_Stuff_Callback(int op, struct menu_item *item)
{
	if (g_SAMP == NULL)
		return 0;
	int increase = 0;
	char name[128];
	switch (op)
	{
	case MENU_OP_SELECT:
		switch (item->id)
		{
		case ID_ANTI_CAR_TROLL:
			bAntiBadVehicle ^= true;
			break;
		case ID_ANTI_BULLET_CRASHER:
			bAntiBadAim ^= true;
			bAntiBadBullets ^= true;
			break;
		case ID_ANTI_BAD_VEHICLES:
			bAntiBadUnoccupied ^= true;
			break;
		case ID_RCON_ATTACK:
			OLCheats->bRCON_Attack ^= true;
			break;
		case ID_SERVER_LAG:
			OLCheats->bServerLag ^= true;
			break;
		case ID_JOIN_FLOOD:
			OLCheats->bJoinFlood ^= true;
			break;
		case ID_FAKE_FPS:
			OLCheats->bFakeFPS ^= true;
			break;
		case ID_ANTI_LOADING:
			OLCheats->bAntiLoading ^= true;
			break;
		case ID_CHANGE_GPCI:
			OLCheats->bChangeGPCIa ^= true;
			OL_ChangeSettings("OL_ChangeGPCI", OLCheats->bChangeGPCIa ? "true" : "false");
			cheat_state_text("Setting saved, restart the game to apply the changes.");
			break;
		}
		break;
	case MENU_OP_ENABLED:
		switch (item->id)
		{
		case ID_ANTI_CAR_TROLL:
			return bAntiBadVehicle;
			break;
		case ID_ANTI_BULLET_CRASHER:
			return bAntiBadBullets;
			break;
		case ID_ANTI_BAD_VEHICLES:
			return bAntiBadUnoccupied;
			break;
		case ID_RCON_ATTACK:
			return OLCheats->bRCON_Attack;
			break;
		case ID_SERVER_LAG:
			return OLCheats->bServerLag;
			break;
		case ID_JOIN_FLOOD:
			return OLCheats->bJoinFlood;
			break;
		case ID_FAKE_FPS:
			return OLCheats->bFakeFPS;
		case ID_ANTI_LOADING:
			return OLCheats->bAntiLoading;
		case ID_CHANGE_GPCI:
			return OLCheats->bChangeGPCIa;

		case ID_MESSAGES_LIMIT:
		{
			snprintf(name, sizeof(name), "\aMessages Limit: %d/sec", OLCheats->iLimitMessages);
			menu_item_name_set(item, name);
			return OLCheats->bServerLag;
			break;
		}
		case ID_SELECT_FAKE_FPS:
		{
			char name[128];
			snprintf(name, sizeof(name), "\aFPS: %d", OLCheats->iFPS);
			menu_item_name_set(item, name);
			return OLCheats->bFakeFPS;
		}
		}
		break;
	case MENU_OP_DEC:
	case MENU_OP_INC:
		increase = (op == MENU_OP_DEC) ? -1 : 1;
		switch (item->id)
		{
		case ID_MESSAGES_LIMIT:
			OLCheats->iLimitMessages += 25 * increase;
			break;
		case ID_SELECT_FAKE_FPS:
			OLCheats->iFPS += increase;
			break;
		}
		break;
	}
	return 0;
}

int OverLight_Bot_Follow_Callback(int op, struct menu_item *item)
{
	if (g_SAMP == NULL)
		return 0;


	int increase = 0;
	switch (op)
	{
	case MENU_OP_SELECT:
		switch (item->id)
		{
		case ID_BOT_COPY_HEALTH:
			g_BotFuncs->BotSettings.bCopyHealth ^= true;
			break;
		case ID_BOT_COPY_ARMOR:
			g_BotFuncs->BotSettings.bCopyArmor ^= true;
			break;
		case ID_BOT_COPY_WEAPON:
			g_BotFuncs->BotSettings.bCopyWeapon ^= true;
			break;
		case ID_BOT_COPY_CHAT:
			g_BotFuncs->BotSettings.bCopyChat ^= true;
			g_BotFuncs->BotSettings.bCopyRPC = false;
			break;
		case ID_BOT_COPY_RPC:
			g_BotFuncs->BotSettings.bCopyRPC ^= true;
			g_BotFuncs->BotSettings.bCopyChat = false;
			break;
		}
		break;
	case MENU_OP_ENABLED:
		switch (item->id)
		{
		case ID_BOT_COPY_HEALTH:
			return g_BotFuncs->BotSettings.bCopyHealth;
			break;
		case ID_BOT_COPY_ARMOR:
			return g_BotFuncs->BotSettings.bCopyArmor;
			break;
		case ID_BOT_COPY_WEAPON:
			return g_BotFuncs->BotSettings.bCopyWeapon;
			break;
		case ID_BOT_COPY_CHAT:
			return g_BotFuncs->BotSettings.bCopyChat;
			break;
		case ID_BOT_COPY_RPC:
			return g_BotFuncs->BotSettings.bCopyRPC;
			break;
		}
	break;
	/*
	case MENU_OP_DEC:
	case MENU_OP_INC:
		increase = (op == MENU_OP_DEC) ? -1 : 1;
		switch (item->id)
		{
		}
		break;*/
	}
	return 0;
}

int OverLight_Chams_Callback(int op, struct menu_item *item)
{
	if (g_SAMP == NULL)
		return 0;
	int increase = 0;
	char name[128];
	switch (op)
	{
	case MENU_OP_SELECT:
		switch (item->id)
		{
		case ID_BEHIND_SOLID:
			OLCheats->ChamsTypeBehindWall = D3DFILL_SOLID;
			break;
		case ID_BEHIND_WIREFRAME:
			OLCheats->ChamsTypeBehindWall = D3DFILL_WIREFRAME;
			break;
		case ID_BEHIND_POINT:
			OLCheats->ChamsTypeBehindWall = D3DFILL_POINT;
			break;
		case ID_INFRONT_SOLID:
			OLCheats->ChamsTypeInFrontWall = D3DFILL_SOLID;
			break;
		case ID_INFRONT_WIREFRAME:
			OLCheats->ChamsTypeInFrontWall = D3DFILL_WIREFRAME;
			break;
		case ID_INFRONT_POINT:
			OLCheats->ChamsTypeInFrontWall = D3DFILL_POINT;
			break;
		}
		break;
	case MENU_OP_ENABLED:
		switch (item->id)
		{
		case ID_BEHIND_RED:
		{
			snprintf(name, sizeof(name), "\aRed: %.f", OLCheats->fColorChamsBehindWall[0]);
			menu_item_name_set(item, name);
			break;
		}
		case ID_BEHIND_GREEN:
		{
			snprintf(name, sizeof(name), "\aGreen: %.f", OLCheats->fColorChamsBehindWall[1]);
			menu_item_name_set(item, name);
			break;
		}
		case ID_BEHIND_BLUE:
		{
			snprintf(name, sizeof(name), "\aBlue: %.f", OLCheats->fColorChamsBehindWall[2]);
			menu_item_name_set(item, name);
			break;
		}

		case ID_BEHIND_SOLID:
		{
			if (OLCheats->ChamsTypeBehindWall == D3DFILL_SOLID)
			{
				return true;
			}
			else
			{
				return false;
			}
			break;
		}
		case ID_BEHIND_WIREFRAME:
		{
			if (OLCheats->ChamsTypeBehindWall == D3DFILL_WIREFRAME)
			{
				return true;
			}
			else 
			{
				return false;
			}
			break;
		}
		case ID_BEHIND_POINT:
		{
			if (OLCheats->ChamsTypeBehindWall == D3DFILL_POINT)
			{
				return true;
			}
			else
			{
				return false;
			}
			break;
		}

		case ID_INFRONT_SOLID:
		{
			if (OLCheats->ChamsTypeInFrontWall == D3DFILL_SOLID)
			{
				return true;
			}
			else
			{
				return false;
			}
			break;
		}
		case ID_INFRONT_WIREFRAME:
		{
			if (OLCheats->ChamsTypeInFrontWall == D3DFILL_WIREFRAME)
			{
				return true;
			}
			else
			{
				return false;
			}
			break;
		}
		case ID_INFRONT_POINT:
		{
			if (OLCheats->ChamsTypeInFrontWall == D3DFILL_POINT)
			{
				return true;
			}
			else
			{
				return false;
			}
			break;
		}
		case ID_INFRONT_RED:
		{
			snprintf(name, sizeof(name), "\aRed: %.f", OLCheats->fColorChamsInFrontWall[0]);
			menu_item_name_set(item, name);
			break;
		}
		case ID_INFRONT_GREEN:
		{
			snprintf(name, sizeof(name), "\aGreen: %.f", OLCheats->fColorChamsInFrontWall[1]);
			menu_item_name_set(item, name);
			break;
		}
		case ID_INFRONT_BLUE:
		{
			snprintf(name, sizeof(name), "\aBlue: %.f", OLCheats->fColorChamsInFrontWall[2]);
			menu_item_name_set(item, name);
			break;
		}

		case ID_INFRONT_ALPHA:
		{
			snprintf(name, sizeof(name), "\aAplha: %.f", OLCheats->fColorChamsInFrontWall[3]);
			menu_item_name_set(item, name);
			break;
		}

		case ID_BEHIND_ALPHA:
		{
			snprintf(name, sizeof(name), "\aAplha: %.f", OLCheats->fColorChamsBehindWall[3]);
			menu_item_name_set(item, name);
			break;
		}

		}
	break;
	case MENU_OP_DEC:
	case MENU_OP_INC:
		increase = (op == MENU_OP_DEC) ? -1 : 1;
		switch (item->id)
		{
		case ID_BEHIND_RED:
			if ((OLCheats->fColorChamsBehindWall[0] + increase * 15.0f) > 255
				|| (OLCheats->fColorChamsBehindWall[0] + increase * 15.0f) < 0)
				break;
			OLCheats->fColorChamsBehindWall[0] += increase * 15.0f;
			Set_Chams_Color_Behind_Wall();
			break;
		case ID_BEHIND_GREEN:
			if ((OLCheats->fColorChamsBehindWall[1] + increase * 15.0f) > 255
				|| (OLCheats->fColorChamsBehindWall[1] + increase * 15.0f) < 0)
				break;
			OLCheats->fColorChamsBehindWall[1] += increase * 15.0f;
			Set_Chams_Color_Behind_Wall();
			break;
		case ID_BEHIND_BLUE:
			if ((OLCheats->fColorChamsBehindWall[2] + increase * 15.0f) > 255
				|| (OLCheats->fColorChamsBehindWall[2] + increase * 15.0f) < 0)
				break;
			OLCheats->fColorChamsBehindWall[2] += increase * 15.0f;
			Set_Chams_Color_Behind_Wall();
			break;
		case ID_INFRONT_RED:
			if ((OLCheats->fColorChamsInFrontWall[0] + increase * 15.0f) > 255
				|| (OLCheats->fColorChamsInFrontWall[0] + increase * 15.0f) < 0)
				break;
			OLCheats->fColorChamsInFrontWall[0] += increase * 15.0f;
			Set_Chams_Color_InFront_Wall();
			break;
		case ID_INFRONT_GREEN:
			if ((OLCheats->fColorChamsInFrontWall[1] + increase * 15.0f) > 255
				|| (OLCheats->fColorChamsInFrontWall[1] + increase * 15.0f) < 0)
				break;
			OLCheats->fColorChamsInFrontWall[1] += increase * 15.0f;
			Set_Chams_Color_InFront_Wall();
			break;
		case ID_INFRONT_BLUE:
			if ((OLCheats->fColorChamsInFrontWall[2] + increase * 15.0f) > 255
				|| (OLCheats->fColorChamsInFrontWall[2] + increase * 15.0f) < 0)
				break;
			OLCheats->fColorChamsInFrontWall[2] += increase * 15.0f;
			Set_Chams_Color_InFront_Wall();
			break;
		case ID_BEHIND_ALPHA:
			if ((OLCheats->fColorChamsBehindWall[3] + increase * 15.0f) > 255
				|| (OLCheats->fColorChamsBehindWall[3] + increase * 15.0f) < 0)
				break;
			OLCheats->fColorChamsBehindWall[3] += increase * 15.0f;
			Set_Chams_Color_Behind_Wall();
			break;
		case ID_INFRONT_ALPHA:
			if ((OLCheats->fColorChamsInFrontWall[3] + increase * 15.0f) > 255
				|| (OLCheats->fColorChamsInFrontWall[3] + increase * 15.0f) < 0)
				break;
			OLCheats->fColorChamsInFrontWall[3] += increase * 15.0f;
			Set_Chams_Color_InFront_Wall();
			break;
		}
		break;
	}
	return 0;
}

int OverLight_Bot_Control_Callback(int op, struct menu_item *item)
{
	if (g_SAMP == NULL)
		return 0;
	int increase = 0;
	static int botID = -1;
	char name[128];
	switch (op)
	{
	/*
	case MENU_OP_SELECT:
		switch (item->id)
		{
		}
		break;*/
	case MENU_OP_ENABLED:
		switch (item->id)
		{
		case ID_BOT_ID:
			if (botID == -1)
			{
				snprintf(name, sizeof(name), "\aBot ID: ALL");
				menu_item_name_set(item, name);
			}
			else 
			{
				snprintf(name, sizeof(name), "\aBot ID: %d", botID);
				menu_item_name_set(item, name);
			}
			break;
		case ID_BOT_ANIMATION:
			if (botID == -1)
			{
				snprintf(name, sizeof(name), "\aAnimation ID: %d", g_BotFuncs->BotClient[0].sCurrentAnimationID);
				menu_item_name_set(item, name);
			}
			else
			{
				snprintf(name, sizeof(name), "\aAnimation ID: %d", g_BotFuncs->BotClient[botID].sCurrentAnimationID);
				menu_item_name_set(item, name);
			}
			break;
		}
	break;
	case MENU_OP_DEC:
	case MENU_OP_INC:
		increase = (op == MENU_OP_DEC) ? -1 : 1;
		switch (item->id)
		{
		case ID_BOT_ID:
			if (((botID + increase) < g_BotFuncs->BotSettings.ClientCount)
				&& (botID + increase) >= -1)
			{
				botID += increase;
			}
			break;
		case ID_BOT_ANIMATION:
			if (botID == -1)
			{
				if ((g_BotFuncs->BotClient[0].sCurrentAnimationID + increase) >= -1)
				{
					g_BotFuncs->BotClient[0].sCurrentAnimationID += increase;
					for (int i = 0; i < g_BotFuncs->BotSettings.ClientCount; i++)
					{
						g_BotFuncs->BotClient[i].sCurrentAnimationID = g_BotFuncs->BotClient[0].sCurrentAnimationID;
					}
				}
			}
			else 
			{
				if ((g_BotFuncs->BotClient[botID].sCurrentAnimationID + increase) > -1)
				{
					g_BotFuncs->BotClient[botID].sCurrentAnimationID += increase;
				}
			}
			break;
		case ID_BOT_ACTION:
			if (botID == -1)
			{
				if ((g_BotFuncs->BotClient[0].byteSpecialAction + increase) >= -1)
				{
					g_BotFuncs->BotClient[0].byteSpecialAction += increase;
					for (int i = 0; i < g_BotFuncs->BotSettings.ClientCount; i++)
					{
						g_BotFuncs->BotClient[i].byteSpecialAction = g_BotFuncs->BotClient[0].byteSpecialAction;
					}
				}
			}
			else
			{
				if ((g_BotFuncs->BotClient[botID].byteSpecialAction + increase) > -1)
				{
					g_BotFuncs->BotClient[botID].byteSpecialAction += increase;
				}
			}
			break;
		}
		break;
	}
	return 0;
}

int OverLight_Bot_Connect_Callback(int op, struct menu_item *item)
{
	if (g_SAMP == NULL)
		return 0;
	char name[128];
	int increase = 0;
	switch (op)
	{
	case MENU_OP_SELECT:
		switch (item->id)
		{
		case ID_BOT_STEAL_NICK:
			g_BotFuncs->BotSettings.UseFakeBot ^= true;
			break;
		case ID_MENU_NICK_LOCAL:
			g_BotFuncs->BotSettings.NicknameStyle = ID_NICK_LOCAL;
			break;
		case ID_MENU_NICK_REALISTIC:
			g_BotFuncs->BotSettings.NicknameStyle = ID_NICK_REALISTIC;
			break;
		case ID_MENU_NICK_BINARY:
			g_BotFuncs->BotSettings.NicknameStyle = ID_NICK_BINARY;
			break;
		case ID_BOT_SPAWN2:
			for (int j = 0; j < 2; j++)
				for (int i = g_BotFuncs->BotSettings.ClientCount - 1; i >= 0; i--)
					g_BotFuncs->OL_Spawn_Bot(i);
			break;
		}
		break;
	case MENU_OP_ENABLED:
		switch (item->id)
		{
		case ID_BOT_STEAL_NICK:
			return g_BotFuncs->BotSettings.UseFakeBot;
			break;
		case ID_MENU_NICK_LOCAL:
			if (g_BotFuncs->BotSettings.NicknameStyle == ID_NICK_LOCAL)
			{
				return true;
			}
			else 
			{
				return false;
			}
			break;
		case ID_MENU_NICK_REALISTIC:
			if (g_BotFuncs->BotSettings.NicknameStyle == ID_NICK_REALISTIC)
			{
				return true;
			}
			else
			{
				return false;
			}
			break;
		case ID_MENU_NICK_BINARY:
			if (g_BotFuncs->BotSettings.NicknameStyle == ID_NICK_BINARY)
			{
				return true;
			}
			else
			{
				return false;
			}
			break;
		case ID_BOT_CLASS_ID:
		{
			snprintf(name, sizeof(name), "\aClass ID: %d", g_BotFuncs->BotSettings.ClassID);
			menu_item_name_set(item, name);
			break;
		}

		case ID_BOT_PLAYER:
			if (OLCheats->Target_PlayerID != -1)
			{
				char name[128];
				if (IsPlayerStreamed(OLCheats->Target_PlayerID))
				{
					snprintf(name, sizeof(name), "\aPlayer: %s <%d>", getPlayerName(OLCheats->Target_PlayerID), OLCheats->Target_PlayerID);
					menu_item_name_set(item, name);
				}
				else
				{
					snprintf(name, sizeof(name), "\aPlayer: <%d> [Unstreamed]", OLCheats->Target_PlayerID);
					menu_item_name_set(item, name);
				}
			}
			else
			{
				OLCheats->Target_PlayerID = NearestPlayer();
			}
			return g_BotFuncs->BotSettings.UseFakeBot;
			break;
		}
	break;
	case MENU_OP_DEC:
	case MENU_OP_INC:
		increase = (op == MENU_OP_DEC) ? -1 : 1;
		switch (item->id)
		{
		case ID_BOT_PLAYER:
			OLCheats->Target_PlayerID = NextPlayer(OLCheats->Target_PlayerID, increase);
			break;
		case ID_BOT_CLASS_ID:
			g_BotFuncs->BotSettings.ClassID += increase;
			break;
		}
		break;
	}
	return 0;
}

int OverLight_Credits_Callback(int op, struct menu_item *item)
{
	return 0;
}

int OverLight_HUD_Settings_Callback(int op, struct menu_item *item)
{
	if (g_SAMP == NULL)
		return 0;
	int increase = 0;
	char value[10];
	char name[128];
	switch (op)
	{
	case MENU_OP_SELECT:
		switch (item->id)
		{
		case ID_PLAYER_TAGS:
			OLCheats->bPlayerTags ^= true;
			break;
		case ID_VEHICLE_TAGS:
			OLCheats->bVehicleTags ^= true;
			break;
		case ID_FONT_CHAT_SIZE:
			itoa(OLCheats->ChatFontSize, value, 10);
			OL_ChangeSettings("chat_font_size", value);
			cheat_state_text("Font saved, restart the game to apply the changes.");
			break;
		case ID_FONT_SMALL_SIZE:
			itoa(OLCheats->SmallFontSize, value, 10);
			OL_ChangeSettings("small_font_size", value);
			cheat_state_text("Font saved, restart the game to apply the changes.");
			break;
		case ID_FONT_FOOTER_SIZE:
			itoa(OLCheats->FooterFontSize, value, 10);
			OL_ChangeSettings("footer_font_size", value);
			cheat_state_text("Font saved, restart the game to apply the changes.");
			break;
		case ID_FONT_MENU_SIZE:
			itoa(OLCheats->MenuFontSize, value, 10);
			OL_ChangeSettings("menu_font_size", value);
			cheat_state_text("Font saved, restart the game to apply the changes.");
			break;
		case ID_FONT_NICK_SIZE:
			itoa(OLCheats->NicknameFontSize, value, 10);
			OL_ChangeSettings("nickname_font_name", value);
			cheat_state_text("Font saved, restart the game to apply the changes.");
			break;
		}
		break;
	case MENU_OP_ENABLED:
		switch (item->id)
		{
		case ID_PLAYER_TAGS:
			return OLCheats->bPlayerTags;
		case ID_VEHICLE_TAGS:
			return OLCheats->bVehicleTags;
		case ID_FONT_CHAT_SIZE:
		{
			snprintf(name, sizeof(name), "\aChat Font Size: %d", OLCheats->ChatFontSize);
			menu_item_name_set(item, name);
			break;
		}
		case ID_FONT_SMALL_SIZE:
		{
			snprintf(name, sizeof(name), "\aSmall Font Size: %d", OLCheats->SmallFontSize);
			menu_item_name_set(item, name);
			break;
		}
		case ID_FONT_FOOTER_SIZE:
		{
			snprintf(name, sizeof(name), "\aFooter Font Size: %d", OLCheats->FooterFontSize);
			menu_item_name_set(item, name);
			break;
		}
		case ID_FONT_MENU_SIZE:
		{
			snprintf(name, sizeof(name), "\aMenu Font Size: %d", OLCheats->MenuFontSize);
			menu_item_name_set(item, name);
			break;
		}
		case ID_FONT_NICK_SIZE:
		{
			snprintf(name, sizeof(name), "\aNickname Font Size: %d", OLCheats->NicknameFontSize);
			menu_item_name_set(item, name);
			break;
		}
			
		}
	break;
	case MENU_OP_DEC:
	case MENU_OP_INC:
		increase = (op == MENU_OP_DEC) ? -1 : 1;
		switch (item->id)
		{
		case ID_FONT_NICK_SIZE:
		{
			OLCheats->NicknameFontSize += increase;
			break;
		}
		case ID_FONT_CHAT_SIZE:
		{
			OLCheats->ChatFontSize += increase;
			break;
		}
		case ID_FONT_SMALL_SIZE:
		{
			OLCheats->SmallFontSize += increase;
			break;
		}
		case ID_FONT_FOOTER_SIZE:
		{
			OLCheats->FooterFontSize += increase;
			break;
		}
		case ID_FONT_MENU_SIZE:
		{
			OLCheats->MenuFontSize += increase;
			break;
		}
		}
		break;
	}
	return 0;

}

int OverLight_Crashers_Callback(int op, struct menu_item *item)
{
	if (g_SAMP == NULL)
		return 0;
	int increase = 0;
	switch (op)
	{
	case MENU_OP_SELECT:
		switch (item->id)
		{
			case ID_PLANE_CRASHER2:
				OLCheats->bPlaneCrasher2 ^= true;
				break;
			case ID_VORTEX_CRASHER2:
				OLCheats->bVortexCrasher2 ^= true;
				break;
			case ID_PLANE_CRASHER_:
				OLCheats->bPlaneCrasher ^= true;
				break;
			case ID_VORTEX_CRASHER_:
				OLCheats->bVortexCrasher ^= true;
				break;
			case ID_BULLET_CRASHER:
				OLCheats->bBulletCrasher ^= true;
				break;
			case ID_CARJACK_CRASHER:
				OLCheats->bCarJackCrasher ^= true;
				break;
			case ID_STUNT_CRASHER:
				OLCheats->bStuntCrasher ^= true;
				break;
			case ID_HUNTER_CRASHER:
				OLCheats->bHunterCrasher ^= true;
				break;
			case ID_DETONATOR_CRASHER:
				OLCheats->bDetonatorCrasher ^= true;
				break;
			case ID_JETPACK_ROLL_CRASHER:
				ShellExecute(NULL, "open", "https://www.youtube.com/watch?v=dQw4w9WgXcQ", NULL, NULL, SW_SHOW);
				break;

		}
		break;
	case MENU_OP_ENABLED:
		switch (item->id)
		{
			case ID_PLANE_CRASHER2:
				return OLCheats->bPlaneCrasher2;
			case ID_VORTEX_CRASHER2:
				return OLCheats->bVortexCrasher2;
			case ID_PLANE_CRASHER_:
				return OLCheats->bPlaneCrasher;
			case ID_VORTEX_CRASHER_:
				return OLCheats->bVortexCrasher;
			case ID_BULLET_CRASHER:
				return OLCheats->bBulletCrasher;
			case ID_CARJACK_CRASHER:
				return OLCheats->bCarJackCrasher;
			case ID_STUNT_CRASHER:
				return OLCheats->bStuntCrasher;
			case ID_HUNTER_CRASHER:
				return OLCheats->bHunterCrasher;
			case ID_DETONATOR_CRASHER:
				return OLCheats->bDetonatorCrasher;
		}
	break;
	/*
	case MENU_OP_DEC:
	case MENU_OP_INC:
		increase = (op == MENU_OP_DEC) ? -1 : 1;
		switch (item->id)
		{

		}
		break;*/
	}
	return 0;
}

int OverLight_Chat_Callback(int op, struct menu_item *item)
{
		if (g_SAMP == NULL)
			return 0;
		int increase = 0;
		switch (op)
		{
		case MENU_OP_SELECT:
			switch (item->id)
			{
			case ID_TRANSP:
				OLCheats->bTranspBackGroundChat ^= true;
				break;
			case ID_SEND_NICKNAME:
				OLCheats->bSendNickname ^= true;
				OL_ChangeSettings("OL_Chat_send_nickname", OLCheats->bSendNickname ? "true" : "false");
				break;
			case ID_SEND_SERVER:
				OLCheats->bSendServer ^= true;
				OL_ChangeSettings("OL_Chat_send_server", OLCheats->bSendServer ? "true" : "false");
				break;
			}
			break;
		case MENU_OP_ENABLED:
			switch (item->id)
			{
			case ID_TRANSP:
				return OLCheats->bTranspBackGroundChat;
			case ID_SEND_NICKNAME:
				return OLCheats->bSendNickname;
			case ID_SEND_SERVER:
				return OLCheats->bSendServer;
			}
		break;
		/*case MENU_OP_DEC:
		case MENU_OP_INC:
			increase = (op == MENU_OP_DEC) ? -1 : 1;
			switch (item->id)
			{
			default:
				break;
			}
			break;*/
		}
		return 0;
}


//	if (g_SAMP == NULL)
//		return 0;
//	int increase = 0;
//	switch (op)
//	{
//	case MENU_OP_SELECT:
//		switch (item->id)
//		{
//		}
//		break;
//	case MENU_OP_ENABLED:
//		switch (item->id)
//		{
//		}
//	break;
//	case MENU_OP_DEC:
//	case MENU_OP_INC:
//		increase = (op == MENU_OP_DEC) ? -1 : 1;
//		switch (item->id)
//		{
//		}
//		break;
//	}
//	return 0;