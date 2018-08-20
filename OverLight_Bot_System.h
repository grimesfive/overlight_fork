#pragma once
extern class CBotFuncs *g_BotFuncs;

struct stBot
{
	RakClientInterface *pRakClient;
	char NickName[SAMP_MAX_PLAYER_NAME + 1];
	bool bConnected;
	bool bJoined;
	bool bSpawned;

	bool bIsInVehicle;
	uint16_t last_vehicleID;
	
	uint16_t playerID;
	unsigned int uiChallenge;

	float fLastOnFootPos[3];
	float fLastOnFootQuat[4];
	short sCurrentAnimationID;
	uint8_t byteSpecialAction;

	DWORD dwSendBulletTick;

	float fHealth;
	float fArmour;

	stBot()
	{
		fHealth = 100.0f;
	}
};

struct stBotSettings
{
	//int ClientCount;
	int ClientCount;
	DWORD UpdateSendRate;
	//int CurrentClientID;
	uint8_t CurrentClientID;
	uint8_t ClientJoined;
	uint8_t NicknameStyle;

	bool bReceiveRPC;

	bool FakePing;
	int incrasePing;
	bool UseFakeBot;
	int ClassID;

	bool bFollow;
	float fDistanceFollow;
	float fDistanceFollowBetweenBots;

	bool bShooter;
	
	short sTargetID;
	bool bUseNearestTarget;
	bool bBotAttacker;
	uint8_t uiAttackWeaponID;

	bool bBotElevator;
	bool bBotJetpackElevator;
	bool bBotPick;
	bool bBotSlapper;
	bool bBotFuck;
	bool bBotFollowAnim;
	short sBotFollowAnimID;
	bool bUIF_DM_Farmer;

	bool bBotSun;

	//advanced
	bool bCopyHealth;
	bool bCopyArmor;
	bool bCopyWeapon;
	bool bCopyChat;
	bool bCopyRPC;

	//temp 
	bool bBotSilent;
	float fMultiplierSilent;
	float fSelfPos[3];
	float fSelfSpeed[3];
	bool bStorm;
};

class CBotFuncs
{
public:

	CBotFuncs();

	~CBotFuncs();
	uint16_t Client_in_Queue;
	uint16_t N_Client_to_Add_In_Queue;
	uint16_t uiIsBot[SAMP_MAX_PLAYERS][2];

	stBotSettings BotSettings;
	/*uhmm maybe is better
	to use the vector*/
	stBot BotClient[256];
	void OL_Add_Bot_Queue();
	bool OL_Add_Bot();
	bool OL_Add_Bot_Nick(char *nick);
	void OL_Delete_Bot();
	void OL_Spawn_Bot(uint8_t ClientID);
	void OL_Random_Spawn_Bot(uint8_t ClientID);
	void OL_Bot_Die();
	void OL_Bot_Process();
	//RPC
	bool OL_Bot_SendRPC(uint8_t ClientID, int iRPCID, BitStream Params, PacketPriority Priority, PacketReliability Reliability, char OrderingChannel, bool ShiftTimestamp);
	void OL_Bot_ReceiveRPC(RakClientInterface ** pRakClient);

private:
	//RPC



	void OL_Packet_ConnectionAccepted(Packet *packet, uint8_t ClientID);
	void OL_Packet_Auth_Key(Packet *packet, uint8_t ClientID);
	void OL_Packet_AimSync(Packet *packet, uint8_t ClientID);
	void OL_Packet_BulletSync(Packet *packet, uint8_t ClientID);
	void OL_Packet_PlayerSync(Packet *packet, uint8_t ClientID);

	void OL_Bot_Follow();
	void OL_Bot_Follow_OnFoot(bool bFake);
	void OL_Bot_Follow_Passenger(bool bFake);
	void OL_Bot_Silent();
	void OL_Bot_Shooter();
	void OL_Bot_Attacker();
	void OL_Bot_Elevator();
	void OL_Bot_Slapper();
	void OL_Bot_Fuck();
	void OL_Bot_Pick();
	void OL_Bot_Follow_Anim();
	void OL_Bot_Sun();

	void OL_Bot_Door_Storm();
	void OL_Bot_Storm();

	void OL_UIF_DM_Farmer();

	void OL_Bot_Send_OnFoot(const float *fPos, const float *fSpeed, bool bFake, uint16_t sSurfingVehicleID = 0, float *fSurfPos = nullptr);
	void OL_Bot_Send_Passenger(bool bFake);
};

void OL_Bot_SetHealth(RPCParameters *params);
void OL_Bot_SetArmour(RPCParameters *params);