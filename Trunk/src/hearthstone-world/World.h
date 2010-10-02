/*
 * Sandshroud Hearthstone
 * Copyright (C) 2010 - 2011 Sandshroud <http://www.sandshroud.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __WORLD_H
#define __WORLD_H

#define IS_INSTANCE(a) (a>1 && a!=530 && a != 571)

class Object;
class WorldPacket;
class WorldSession;
class Unit;
class Creature;
class GameObject;
class DynamicObject;
class Player;
class EventableObjectHolder;
class MapMgr;
class Battleground;

enum LogonServerTypes
{
    LOGON_ASCENT        = 0x00,
    LOGON_HEARTHSTONE    = 0x01,
    LOGON_ARCEMU        = 0x02,
    LOGON_MANGOS        = 0x04,
    LOGON_ASPIRE        = 0x08
};

enum Rates
{
    RATE_HEALTH=0,
    RATE_POWER1,
    RATE_POWER2,
    RATE_POWER3,
    RATE_DROP0, // separate rates for each quality level
    RATE_DROP1,
    RATE_DROP2,
    RATE_DROP3,
    RATE_DROP4,
    RATE_DROP5,
    RATE_DROP6,
    RATE_MONEY,
    RATE_QUEST_MONEY,
    RATE_XP,
    RATE_RESTXP,
    RATE_QUESTXP,
    RATE_HONOR,
    RATE_QUESTREPUTATION,
    RATE_KILLREPUTATION,
    RATE_SKILLCHANCE,
    RATE_SKILLRATE,
    RATE_ARENAPOINTMULTIPLIER2X,
    RATE_ARENAPOINTMULTIPLIER3X,
    RATE_ARENAPOINTMULTIPLIER5X,
    RATE_EOTS_CAPTURERATE,
    MAX_RATES
};

enum IntRates
{
    INTRATE_SAVE=0,
    INTRATE_COMPRESSION,
    INTRATE_PVPTIMER,
    MAX_INTRATES
};

enum EventIdFlags
{
    EVENTID_FLAG_NONE = 0,
    EVENTID_FLAG_PHASE = 1,
    EVENTID_FLAG_MODELID = 2,
    EVENTID_FLAG_EQUIP = 4, //this obviously cannot be used for gameobjects
    EVENTID_FLAG_SPAWN = 8
};

enum EnviromentalDamage
{
    DAMAGE_EXHAUSTED = 0,
    DAMAGE_DROWNING = 1,
    DAMAGE_FALL = 2,
    DAMAGE_LAVA = 3,
    DAMAGE_SLIME = 4,
    DAMAGE_FIRE = 5
};

enum CharCreateErrors
{
    SUCCESS,
    FAILURE,
    CANCELLED,
    DISCONNECT_FROM_SERVER,
    FAILED_TO_CONNECT,
    CONNECTED,
    WRONG_CLIENT_VERSION,
    CONNECTING_TO_SERVER,
    NEGOTIATING_SECURITY,
    NEGOTIATING_SECURITY_COMPLETE,
    NEGOTIATING_SECURITY_FAILED,
    AUTHENTICATING,
    AUTHENTICATION_SUCCESSFUL,
    AUTHENTICATION_FAILED,
    LOGIN_UNAVAIBLE,
    SERVER_IS_NOT_VALID,
    SYSTEM_UNAVAIBLE,
    SYSTEM_ERROR,
    BILLING_SYSTEM_ERROR,
    ACCOUNT_BILLING_EXPIRED,
    WRONG_CLIENT_VERSION_2,
    UNKNOWN_ACCOUNT,
    INCORRECT_PASSWORD,
    SESSION_EXPIRED,
    SERVER_SHUTTING_DOWN,
    ALREADY_LOGGED_IN,
    INVALID_LOGIN_SERVER,
    POSITION_IN_QUEUE_0,
    THIS_ACCOUNT_HAS_BEEN_BANNED,
    THIS_CHARACTER_STILL_LOGGED_ON,
    YOUR_WOW_SUBSCRIPTION_IS_EXPIRED,
    THIS_SESSION_HAS_TIMED_OUT,
    THIS_ACCOUNT_TEMP_SUSPENDED,
    ACCOUNT_BLOCKED_BY_PARENTAL_CONTROL,
    RETRIEVING_REALMLIST,
    REALMLIST_RETRIEVED,
    UNABLE_TO_CONNECT_REALMLIST_SERVER,
    INVALID_REALMLIST,
    GAME_SERVER_DOWN,
    CREATING_ACCOUNT,
    ACCOUNT_CREATED,
    ACCOUNT_CREATION_FAIL,
    RETRIEVE_CHAR_LIST,
    CHARLIST_RETRIEVED,
    CHARLIST_ERROR,
    CREATING_CHARACTER,
    CHARACTER_CREATED,
    ERROR_CREATING_CHARACTER,
    CHARACTER_CREATION_FAIL,
    NAME_IS_IN_USE,
    CREATION_OF_RACE_DISABLED,
    ALL_CHARS_ON_PVP_REALM_MUST_AT_SAME_SIDE,
    ALREADY_HAVE_MAXIMUM_CHARACTERS,
    ALREADY_HAVE_MAXIMUM_CHARACTERS_2,
    SERVER_IS_CURRENTLY_QUEUED,
    ONLY_PLAYERS_WHO_HAVE_CHARACTERS_ON_THIS_REALM,
    NEED_EXPANSION_ACCOUNT,
    DELETING_CHARACTER,
    CHARACTER_DELETED,
    CHARACTER_DELETION_FAILED,
    ENTERING_WOW,
    LOGIN_SUCCESFUL,
    WORLD_SERVER_DOWN,
    A_CHARACTER_WITH_THAT_NAME_EXISTS,
    NO_INSTANCE_SERVER_AVAIBLE,
    LOGIN_FAILED,
    LOGIN_FOR_THAT_RACE_DISABLED,
    LOGIN_FOR_THAT_RACE_CLASS_DISABLED,//check
    ENTER_NAME_FOR_CHARACTER,
    NAME_AT_LEAST_TWO_CHARACTER,
    NAME_AT_MOST_12_CHARACTER,
    NAME_CAN_CONTAIN_ONLY_CHAR,
    NAME_CONTAIN_ONLY_ONE_LANG,
    NAME_CONTAIN_PROFANTY,
    NAME_IS_RESERVED,
    YOU_CANNOT_USE_APHOS,
    YOU_CAN_ONLY_HAVE_ONE_APHOS,
    YOU_CANNOT_USE_SAME_LETTER_3_TIMES,
    NO_SPACE_BEFORE_NAME,
    BLANK,
    INVALID_CHARACTER_NAME,
    BLANK_1
    //All further codes give the number in dec.
};

// ServerMessages.dbc
enum ServerMessageType
{
    SERVER_MSG_SHUTDOWN_TIME      = 1,
    SERVER_MSG_RESTART_TIME       = 2,
    SERVER_MSG_STRING             = 3,
    SERVER_MSG_SHUTDOWN_CANCELLED = 4,
    SERVER_MSG_RESTART_CANCELLED  = 5
};

enum ServerShutdownType
{
    SERVER_SHUTDOWN_TYPE_SHUTDOWN    = 1,
    SERVER_SHUTDOWN_TYPE_RESTART    = 2,
};

enum WorldMapInfoFlag
{
    WMI_INSTANCE_ENABLED    = 0x1,
    WMI_INSTANCE_WELCOME    = 0x2,
    WMI_INSTANCE_MULTIMODE    = 0x4,
    WMI_INSTANCE_XPACK_01    = 0x8, //The Burning Crusade expansion
    WMI_INSTANCE_XPACK_02    = 0x10, //Wrath of the Lich King expansion
    WMI_INSTANCE_XPACK_03    = 0x20, //Cataclysm expansion
};

enum AccountFlags
{
    ACCOUNT_FLAG_VIP            = 0x1,
    ACCOUNT_FLAG_NO_AUTOJOIN    = 0x2,
    //ACCOUNT_FLAG_XTEND_INFO    = 0x4,
    ACCOUNT_FLAG_XPACK_01        = 0x8,
    ACCOUNT_FLAG_XPACK_02        = 0x10,
    ACCOUNT_FLAG_XPACK_03        = 0x20,
};

#pragma pack(push,1)
struct MapInfo
{
    uint32 mapid;
    uint32 screenid;
    uint32 type;
    uint32 playerlimit;
    uint32 minlevel;
    float repopx;
    float repopy;
    float repopz;
    uint32 repopmapid;
    char * name;
    uint32 flags;
    uint32 cooldown;
     uint32 required_quest;
    uint32 required_item;
    uint32 heroic_key[2];
    float update_distance;
    uint32 checkpoint_id;
    uint32 phasehorde;
    uint32 phasealliance;
    bool collision;

    bool HasFlag(uint32 flag)
    {
        return (flags & flag) != 0;
    }
};

#pragma pack(pop)

enum REALM_TYPE
{
    REALM_PVE = 0,
    REALM_PVP = 1,
};
struct AreaTable;

struct insert_playeritem
{
    uint32 ownerguid;
    uint32 entry;
    uint32 wrapped_item_id;
    uint32 wrapped_creator;
    uint32 creator;
    uint32 count;
    uint32 charges;
    uint32 flags;
    uint32 randomprop;
    uint32 randomsuffix;
    uint32 itemtext;
    uint32 durability;
    int32 containerslot;
    int32 slot;
    string enchantments;
};

struct insert_playerskill
{
    uint32 player_guid;
    uint32 skill_id;
    uint32 type;
    uint32 currentlvl;
    uint32 maxlvl;
};

struct insert_playerquest
{
    uint32 player_guid;
    uint32 quest_id;
    uint32 slot;
    uint32 time_left;
    uint32 explored_area1;
    uint32 explored_area2;
    uint32 explored_area3;
    uint32 explored_area4;
    uint32 mob_kill1;
    uint32 mob_kill2;
    uint32 mob_kill3;
    uint32 mob_kill4;
    uint32 slain;
};

struct insert_playerglyph
{
    uint32 player_guid;
    uint32 spec;
    uint32 glyph1;
    uint32 glyph2;
    uint32 glyph3;
    uint32 glyph4;
    uint32 glyph5;
    uint32 glyph6;
};

struct insert_playertalent
{
    uint32 player_guid;
    uint32 spec;
    uint32 tid;
    uint32 rank;
};

struct insert_playerspell
{
    uint32 player_guid;
    uint32 spellid;
};

class BasicTaskExecutor : public ThreadContext
{
    CallbackBase * cb;
    uint32 priority;
public:
    BasicTaskExecutor(CallbackBase * Callback, uint32 Priority) : cb(Callback), priority(Priority) {}
    ~BasicTaskExecutor() { delete cb; }
    bool run();
};

class Task
{
    CallbackBase * _cb;
public:
    Task(CallbackBase * cb) : _cb(cb), completed(false), in_progress(false) {}
    ~Task() { delete _cb; }
    bool completed;
    bool in_progress;
    void execute();
};

struct CharacterLoaderThread : public Singleton<CharacterLoaderThread>, public ThreadContext
{
#ifdef WIN32
    HANDLE hEvent;
#else
    pthread_cond_t cond;
    pthread_mutex_t mutex;
#endif
public:
    CharacterLoaderThread();
    ~CharacterLoaderThread();
    void OnShutdown();
    bool run();
};

class TaskList
{
    set<Task*> tasks;
    Mutex queueLock;
public:
    Task * GetTask();
    void AddTask(Task* task);
    void RemoveTask(Task * task)
    {
        queueLock.Acquire();
        tasks.erase(task);
        queueLock.Release();
    }

    void spawn();
    void kill();

    void wait();
    void waitForThreadsToExit();
    uint32 thread_count;
    bool running;

    Mutex tcMutex;
    void incrementThreadCount()
    {
        tcMutex.Acquire();
        ++thread_count;
        tcMutex.Release();
    }

    void decrementThreadCount()
    {
        tcMutex.Acquire();
        --thread_count;
        tcMutex.Release();
    }
};

enum BasicTaskExecutorPriorities
{
    BTE_PRIORITY_LOW        = 0,
    BTE_PRIORITY_MED        = 1,
    BTW_PRIORITY_HIGH       = 2,
};

class TaskExecutor : public ThreadContext
{
    TaskList * starter;
public:
    TaskExecutor(TaskList * l) : starter(l) { l->incrementThreadCount(); }
    ~TaskExecutor() { starter->decrementThreadCount(); }

    bool run();
};

class NewsAnnouncer : public ThreadContext
{
    struct NewsAnnouncement
    {
        uint32 m_id;
        uint32 m_lastTime;
        uint32 m_timePeriod;
        int32 m_factionMask;
        string m_message;
    };

    map<uint32, NewsAnnouncement> m_announcements;

    void _UpdateMessages();
    void _ReloadMessages();
    void _SendMessage(NewsAnnouncement *ann);
    void _Init();
public:
    bool run();
};

class WorldSocket;

// Slow for remove in middle, oh well, wont get done much.
typedef std::list<WorldSocket*> QueueSet;
typedef set<WorldSession*> SessionSet;

// The maximum level attainable, period, regardless of flags on your account.
#define MAXIMUM_ATTAINABLE_LEVEL 100 // Crow: Lets use 100, since its the highest the DBCs will support

class SERVER_DECL World : public Singleton<World>, public EventableObject
{
public:
    World();
    ~World();

    uint32 GetMaxLevel(Player* plr);

    /** Reloads the config and sets all of the setting variables
     */
    void Rehash(bool load);

    void CleanupCheaters();
    WorldSession* FindSession(uint32 id);
    WorldSession* FindSessionByName(const char *);
    void AddSession(WorldSession *s);
    void RemoveSession(uint32 id);

    void AddGlobalSession(WorldSession *GlobalSession);
    void RemoveGlobalSession(WorldSession *GlobalSession);
    void DeleteGlobalSession(WorldSession *GlobalSession);

    HEARTHSTONE_INLINE size_t GetSessionCount() const { return m_sessions.size(); }

    HEARTHSTONE_INLINE size_t GetQueueCount() { return mQueuedSessions.size(); }
    void GetStats(uint32 * GMCount, float * AverageLatency);

    HEARTHSTONE_INLINE uint32 GetPlayerLimit() const { return m_playerLimit; }
    void SetPlayerLimit(uint32 limit) { m_playerLimit = limit; }

    HEARTHSTONE_INLINE bool getAllowMovement() const { return m_allowMovement; }
    void SetAllowMovement(bool allow) { m_allowMovement = allow; }
    HEARTHSTONE_INLINE bool getGMTicketStatus() { return m_gmTicketSystem; };
    bool toggleGMTicketStatus()
    {
        m_gmTicketSystem = !m_gmTicketSystem;
        return m_gmTicketSystem;
    };

    HEARTHSTONE_INLINE std::string getGmClientChannel() { return GmClientChannel; }

    // MOTD line 1
    void SetMotd(const char *motd) { m_motd = motd; }
    HEARTHSTONE_INLINE const char* GetMotd() const { return m_motd.c_str(); }

    // MOTD line 2
    void SetMotd2(const char *motd2) { m_motd2 = motd2; }
    HEARTHSTONE_INLINE const char* GetMotd2() const { return m_motd2.c_str(); }

    HEARTHSTONE_INLINE time_t GetGameTime() const { return m_gameTime; }

    bool SetInitialWorldSettings();

    void SendWorldText(const char *text, WorldSession *self = 0);
    void SendWorldWideScreenText(const char *text, WorldSession *self = 0);
    void SendGlobalMessage(WorldPacket *packet, WorldSession *self = 0);
    void SendZoneMessage(WorldPacket *packet, uint32 zoneid, WorldSession *self = 0);
    void SendInstanceMessage(WorldPacket *packet, uint32 instanceid, WorldSession *self = 0);
    void SendFactionMessage(WorldPacket *packet, uint8 teamId);
    void SendGMWorldText(const char* text, bool admin = false);
    void SendAdministratorMessage(WorldPacket *packet);
    void SendGamemasterMessage(WorldPacket *packet);
    void SendMessageToGMs(WorldSession *self, const char * text, ...);

    HEARTHSTONE_INLINE void SetStartTime(uint32 val) { m_StartTime = val; }
    HEARTHSTONE_INLINE uint32 GetUptime(void) { return (uint32)UNIXTIME - m_StartTime; }
    HEARTHSTONE_INLINE uint32 GetStartTime(void) { return m_StartTime; }
    std::string GetUptimeString();

    // update the world server every frame
    void Update(time_t diff);
    void CheckForExpiredInstances();

    void UpdateSessions(uint32 diff);

    HEARTHSTONE_INLINE void setRate(int index,float value)
    {
        regen_values[index]=value;
    }

    HEARTHSTONE_INLINE float getRate(int index)
    {
        return regen_values[index];
    }

    HEARTHSTONE_INLINE uint32 getIntRate(int index)
    {
        return int_rates[index];
    }

    HEARTHSTONE_INLINE void setIntRate(int index, uint32 value)
    {
        int_rates[index] = value;
    }

    // talent inspection lookup tables
    std::map< uint32, uint32 > InspectTalentTabPos;
    std::map< uint32, uint32 > InspectTalentTabSize;
    std::map< uint32, uint32 > InspectTalentTabBit;
    uint32 InspectTalentTabPages[12][3];

    // map text emote to spell prices
    typedef std::map< uint32, uint32> SpellPricesMap;
    SpellPricesMap mPrices;

    HEARTHSTONE_INLINE uint32 GetTimeOut(){return TimeOut;}

    struct NameGenData
    {
        string name;
        uint32 type;
    };
    vector<NameGenData> _namegendata[3];
    void LoadNameGenData();
    std::string GenerateName(uint32 type = 0);

    std::map<uint32, AreaTable*> mAreaIDToTable;
    std::map<uint32, AreaTable*> mZoneIDToTable;

    std::map<uint32,uint32> TeachingSpellMap;
    uint32 GetTeachingSpell(uint32 NormalSpellId)
    {
        map<uint32,uint32>::iterator i = TeachingSpellMap.find(NormalSpellId);
        if(i!=TeachingSpellMap.end())
            return i->second;
        return 0;
    }

    uint32 AddQueuedSocket(WorldSocket* Socket);
    void RemoveQueuedSocket(WorldSocket* Socket);
    uint32 GetQueuePos(WorldSocket* Socket);
    void UpdateQueuedSessions(uint32 diff);

    Mutex queueMutex;

    uint32 mQueueUpdateInterval;

    bool cross_faction_world;
    bool free_guild_charters;
    bool m_useIrc;
    uint32 m_deathKnightReqLevel;
    bool m_deathKnightOnePerAccount;

    void SaveAllPlayers();

    bool LogCheaters;
    bool LogCommands;
    bool LogPlayers;
    bool LogChats;

    string MapPath;
    string vMapPath;
    string MMapPath;
    bool UnloadMapFiles;
    bool BreathingEnabled;
    bool SpeedhackProtection;
    bool Collision;
    bool PathFinding;
    bool CalculatedHeightChecks;
    bool AHEnabled;
    int LogonServerType;

    uint32 mInWorldPlayerCount;
    uint32 mAcceptedConnections;
    uint32 SocketSendBufSize;
    uint32 SocketRecvBufSize;

    uint32 HordePlayers;
    uint32 AlliancePlayers;
    uint32 PeakSessionCount;
    bool SendStatsOnJoin;
    bool SendMovieOnJoin;
    SessionSet gmList;
    RWLock gmList_lock;

    uint32 expansionUpdateTime;

    void DeleteObject(Object* obj);

    uint32 compression_threshold;

    uint8 StartLevel;
    uint32 StartGold;

    void    SetKickAFKPlayerTime(uint32 idletimer){m_KickAFKPlayers=idletimer;}
    uint32    GetKickAFKPlayerTime(){return m_KickAFKPlayers;}

    uint32 GetRealmType() { return realmtype; }

    uint32 flood_lines;
    uint32 flood_seconds;
    uint32 flood_message_time;
    uint32 flood_mute_after_flood;
    uint32 flood_caps_min_len;
    float flood_caps_pct;
    bool flood_message;
    bool gm_skip_attunement;
    bool gm_force_robes;
    bool CheckProfessions;

    bool show_gm_in_who_list;
    uint32 map_unload_time;

    bool antihack_teleport;
    bool antihack_speed;
    bool antihack_flight;
    bool antihack_wallclimb;
    bool antihack_cheatengine;
    bool no_antihack_on_gm;

    bool free_arena_teams;

    bool display_free_items;

    //Enable/Disable specific battlegrounds/arenas
    bool wg_enabled;
    bool av_enabled;
    bool ab_enabled;
    bool wsg_enabled;
    bool eots_enabled;

    // Level Caps
    uint32 LevelCap_Custom_All;

    // Duel Talent cost
    uint32 dualTalentTrainCost;

    // could add configs for every expansion..

    void CharacterEnumProc(QueryResultVector& results, uint32 AccountId);
    void LoadAccountDataProc(QueryResultVector& results, uint32 AccountId);

    void PollCharacterInsertQueue(DatabaseConnection * con);
    void PollMailboxInsertQueue(DatabaseConnection * con);
    void DisconnectUsersWithAccount(const char * account, WorldSession * session);
    void DisconnectUsersWithIP(const char * ip, WorldSession * session);
    void DisconnectUsersWithPlayerName(const char * plr, WorldSession * session);

    void LogoutPlayers();

    void BackupDB();

    void LogGM(WorldSession* session, string message, ...);
    void LogCheater(WorldSession* session, string message, ...);
    void LogPlayer(WorldSession* session, string message, ...);
    void LogChat(WorldSession* session, string message, ...);

protected:
    // update Stuff, FIXME: use diff
    time_t _UpdateGameTime()
    {
        // Update Server time
        time_t thisTime = UNIXTIME;
        m_gameTime += thisTime - m_lastTick; //in seconds

        if(m_gameTime >= 86400)            // One day has passed
            m_gameTime -= 86400;

        m_lastTick = thisTime;

        return m_gameTime;
    }
    void FillSpellReplacementsTable();

private:
    EventableObjectHolder * eventholder;
    //! Timers
    typedef HM_NAMESPACE::hash_map<uint32, WorldSession*> SessionMap;
    SessionMap m_sessions;
    RWLock m_sessionlock;

    typedef HM_NAMESPACE::hash_map<uint32, AreaTrigger*> AreaTriggerMap;
    AreaTriggerMap m_AreaTrigger;

protected:
    Mutex SessionsMutex;//FOR GLOBAL !
    SessionSet GlobalSessions;

    float regen_values[MAX_RATES];
    uint32 int_rates[MAX_INTRATES];

    uint32 m_playerLimit;
    bool m_allowMovement;
    bool m_gmTicketSystem;
    std::string m_motd;
    std::string m_motd2;

    uint32 realmtype;

    time_t m_gameTime;
    time_t m_lastTick;
    uint32 TimeOut;

    uint32 m_StartTime;
    uint32 m_queueUpdateTimer;

    QueueSet mQueuedSessions;

    uint32    m_KickAFKPlayers;//don't lag the server if you are useless anyway :P

public:
    std::string GmClientChannel;
    bool m_reqGmForCommands;
    bool m_lfgForNonLfg;
    list<SpellEntry*> dummyspells;
    bool m_limitedNames;
    bool m_useAccountData;

    char * m_banTable;

    float m_movementCompressThreshold;
    float m_movementCompressThresholdCreatures;
    uint32 m_movementCompressRate;
    uint32 m_movementCompressInterval;
    float m_speedHackThreshold;
    float m_speedHackLatencyMultiplier;
    uint32 m_speedHackResetInterval;
    uint32 m_CEThreshold;
    float m_wallhackthreshold;

    // shutdown
    uint32 m_shutdownTime;
    uint32 m_shutdownType;
    uint32 m_shutdownLastTime;

    void QueueShutdown(uint32 delay, uint32 type);
    void CancelShutdown();
    void UpdateShutdownStatus();
    void UpdatePlayerItemInfos();

    bool VerifyName(const char * name, size_t nlen)
    {
        const char * p;
        size_t i;

        static const char * bannedCharacters = "\t\v\b\f\a\n\r\\\"\'\?<>[](){}_=+-|/!@#$%^&*~`.,0123456789\0";
        static const char * allowedCharacters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        if(m_limitedNames)
        {
            for(i = 0; i < nlen; i++)
            {
                p = allowedCharacters;
                for(; *p != 0; ++p)
                {
                    if(name[i] == *p)
                        goto cont;
                }

                return false;
cont:
                continue;
            }
        }
        else
        {
            for(i = 0; i < nlen; i++)
            {
                p = bannedCharacters;
                while(*p != 0 && name[i] != *p && name[i] != 0)
                    ++p;

                if(*p != 0)
                    return false;
            }
        }

        return true;
    }
};

#define sCLT CharacterLoaderThread::getSingleton()
#define sWorld World::getSingleton()

#endif