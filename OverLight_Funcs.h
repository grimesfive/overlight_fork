#pragma once

#define VEHICLE_SYNC_IN_CAR 0
#define VEHICLE_SYNC_UNOCCUPIED 1
#define VEHICLE_SYNC_IN_CAR_RPC 2


struct stOL_Cheats
{
	//stOL_Cheats(); <- because i used malloc
	DWORD SendRateCarCheatTick;
	DWORD SendRatePlayerCheatTick;
	bool bIsVehCheatActive;
	bool bWithoutDriver;
	bool bCarShooter;
	bool bFantozziCloud;
	bool bFakeShooter;
	bool bShooterOnce;
	bool bFusRoDah;
	bool bPickCar;
	bool bInvisibleBike;
	bool bInvisibleBike2;
	bool bFixInvisibleFire;
	bool bVehicleElevator;
	bool bVehicleElevatorDown;


	bool bVehicleGrabber;
	bool bVehicleGrabberMap;
	bool bGotPositionGrabber;
	bool bGotPositionMapGrabber;

	bool bTrailerGrabber;
	int iTrailerMain;

	bool bDoorStorm;
	
#ifdef OWNER
	bool bSurfInvisible;
	bool bSpecInvisible;
#endif	
	
	//crashers
	bool bPlaneCrasher;
	bool bVortexCrasher;
	bool bPlaneCrasher2;
	bool bVortexCrasher2;
	bool bBulletCrasher;
	bool bCarJackCrasher;
	bool bDetonatorCrasher;
	bool bStuntCrasher;
	bool bHunterCrasher;

	bool bVehicleBugger;

	bool bAutoCBug;
	bool bAutoShot;

	bool bTeamFilter;
	bool bFriendFilter;
	bool bColorFilter;
	bool bSkinFilter;
	bool bVisibleFilter;
	bool bAFKFilter;
	bool bAlive;
	bool bOutOfRange;

	bool bKickPassenger;
	bool bStealPassenger;


	bool bNoFall;
	bool bInvalidAimZ;
	

	int iVehicleID;
	int iSyncType;

	bool bIsPlayerCheatActive;
	bool bInvertWalk;
	bool bInvertWalk2;
	bool bFakeFPS;
	bool bFakePing;
	int iFPS;
	bool bSlapper;
	bool bSilentAim;
	bool bShotWall;
	bool bBypassAimAntiCheat;
	bool bBypassAimAntiCheat2;
	float fInaccurateAim;
	bool bFakeLagSync;
	DWORD dwTimeFakeLagSync;
	DWORD dwTimeFakeLagDesync;
	bool bSyncWhenAim;
	bool bFakeLagSpeed;
	float fMultiplierFakeSpeed;
	bool bChams;
	bool bPlayerHologram;
	bool bVehicleHologram;
	bool bAutoScroll;

	bool bNoSpread;
	bool bSlowAim;
	float fSlowValue;

	bool bOL_Crash;
	bool bOL_VersionUpdate;

	bool bRenderFinder;
	bool bElevator;
	bool bJetpackElevator;
	bool bShotRepeater;

	bool bRCON_Attack;
	bool bRCON_Attack_State;
	HANDLE hRCON;
	bool bServerLag;
	int iLimitMessages;
	bool bJoinFlood;
	bool bJoinFlood2;

	int Target_PlayerID;
	bool bUseNearestTarget;
	int Target_Passenger_PlayerID;

	float fColorChamsBehindWall[4];
	float fColorChamsInFrontWall[4];
	uint8_t ChamsTypeBehindWall;
	uint8_t ChamsTypeInFrontWall;
	//bool bUseWireFrameChamsBehindWall;
	//bool bUseWireFrameChamsInFrontWall;

	bool bFind;
	char *WordFindChat;
	int iCurrentFPS;

	int iStickID;

	bool bFontLoaded;

	bool bAntiLoading;

	bool bUseFriendTarget;

	bool bUsersPrinted;

	bool bPrintUsers;

	bool bPrintFriends;

	bool bPrintClock;

	bool bPlayerTags;

	bool bVehicleTags;

	int ChatFontSize;
	char ChatFontName[64];

	int SmallFontSize;
	char SmallFontName[64];

	int FooterFontSize;
	char FooterFontName[64];

	int MenuFontSize;
	char MenuFontName[64];

	int NicknameFontSize;
	char NicknameFontName[64];


	bool bUseDarkTheme;

	int iThemeColor[8];

	bool bWheelWalk;

	bool bAntiStun;

	bool bSpectate;
	int iSpectateID;

	bool bTranspBackGroundChat;


	bool bSendNickname;
	bool bSendServer;

	bool bChangeGPCIa;
};

struct stMenuSettings
{
	bool				bPosInitalized;
	float				fMenuPos[2];
	int					iRowHeight;
	int					iMenuHeight;
	int					iLogoHeight;
};

struct stEnemyStats
{
	//this for calculate the effective speed and something like a ping
	int iPlayerOnFootTick[2];
	float fPlayerPos[2][3];
};

enum LastTick
{
	LAST,
	SECOND_LAST
};

extern struct stMenuSettings MenuSettings;
extern struct stEnemyStats EnemyStats[SAMP_MAX_PLAYERS];
//extern stOL_Cheats *OLCheats;
extern std::list<std::string> overlight_list;
void OL_VehCheats();
int NearestPlayer();
int NearestPassenger();
int GetAimingPlayer(bool bFromWhenPress = true);
bool IsPlayerStreamed(uint16_t playerID);
int NextPlayer(int iplayerID, int increase);
int NextPlayerPassenger(int iplayerID, int increase);
void KickPassenger();
void StealPassenger();
bool GetPlayerQuaternion(uint16_t playerID, float* fQuaternion);
void OL_ChangeSettings(char *name, char *value);
void OL_CalculateLagPosition(int playerID, float fPos[3]);
void OL_Chat_Send(char *message);
void OL_ShowSuggestion(float x, float y);
void initialize_OL_Cheats();