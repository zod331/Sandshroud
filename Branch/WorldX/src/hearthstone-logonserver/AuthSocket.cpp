/*
 * Aspire Hearthstone
 * Copyright (C) 2008 - 2009 AspireDev <http://www.aspiredev.org/>
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

#include "LogonStdAfx.h"
#include <openssl/md5.h>

enum _errors
{
	CE_SUCCESS = 0x00,
	CE_IPBAN=0x01,									  //2bd -- unable to connect (some internal problem)
	CE_ACCOUNT_CLOSED=0x03,							 // "This account has been closed and is no longer in service -- Please check the registered email address of this account for further information.";
	CE_NO_ACCOUNT=0x04,								 //(5)The information you have entered is not valid.  Please check the spelling of the account name and password.  If you need help in retrieving a lost or stolen password and account
	CE_ACCOUNT_IN_USE=0x06,							 //This account is already logged in.  Please check the spelling and try again.
	CE_PREORDER_TIME_LIMIT=0x07,
	CE_SERVER_FULL=0x08,								//Could not log in at this time.  Please try again later.
	CE_WRONG_BUILD_NUMBER=0x09,						 //Unable to validate game version.  This may be caused by file corruption or the interference of another program.
	CE_UPDATE_CLIENT=0x0a,
	CE_ACCOUNT_FREEZED=0x0c
};

AuthSocket::AuthSocket(SOCKET fd) : Socket(fd, 32768, 4096)
{
	N.SetHexStr("894B645E89E1535BBDAD5B8B290650530801B18EBFBF5E8FAB3C82872A3E9BB7");
	g.SetDword(7);
	s.SetRand(256);
	m_authenticated = false;
	m_account = 0;
	last_recv = time(NULL);
	removedFromSet = false;
	m_patch=NULL;
	m_patchJob=NULL;
	_authSocketLock.Acquire();
	_authSockets.insert(this);
	_authSocketLock.Release();
}

AuthSocket::~AuthSocket()
{
	ASSERT(!m_patchJob);
}

void AuthSocket::OnDisconnect()
{
	if(!removedFromSet)
	{
		_authSocketLock.Acquire();
		_authSockets.erase(this);
		_authSocketLock.Release();
	}

	if(m_patchJob)
	{
		PatchMgr::getSingleton().AbortPatchJob(m_patchJob);
		m_patchJob=NULL;
	}
}

void AuthSocket::HandleChallenge()
{
	// No header
	if(GetReadBuffer().GetContiguiousBytes() < 4)
		return;	

	// Check the rest of the packet is complete.
	uint8 * ReceiveBuffer = (uint8*)GetReadBuffer().GetBufferStart();
	uint16 full_size = *(uint16*)&ReceiveBuffer[2];

	DEBUG_LOG("AuthChallenge","got header, body is 0x%02X bytes", full_size);

	if(GetReadBuffer().GetSize() < uint32(full_size+4))
		return;

	// Copy the data into our cached challenge structure
	if(full_size > sizeof(sAuthLogonChallenge_C))
	{
		Disconnect();
		return;
	}

	DEBUG_LOG("AuthChallenge","got full packet.");

	GetReadBuffer().Read(&m_challenge, full_size + 4);
 
	// Check client build.
	uint16 build = m_challenge.build;

	if(build > LogonServer::getSingleton().max_build)
	{
		// wtf?
		SendChallengeError(CE_WRONG_BUILD_NUMBER);
		return;
	}

	if(build < LogonServer::getSingleton().min_build)
	{
		// can we patch?
		char flippedloc[5] = {0,0,0,0,0};
		flippedloc[0] = m_challenge.country[3];
		flippedloc[1] = m_challenge.country[2];
		flippedloc[2] = m_challenge.country[1];
		flippedloc[3] = m_challenge.country[0];

		m_patch = PatchMgr::getSingleton().FindPatchForClient(build, flippedloc);
		if(m_patch == NULL)
		{
			// could not find a valid patch
			SendChallengeError(CE_WRONG_BUILD_NUMBER);
			return;
		}

		DEBUG_LOG("Patch", "Selected patch %u%s for client.", m_patch->Version,m_patch->Locality);

		BigNumber unk;
		unk.SetRand(128);

		uint8 response[119] = {
			0x00, 0x00, 0x00, 0x72, 0x50, 0xa7, 0xc9, 0x27, 0x4a, 0xfa, 0xb8, 0x77, 0x80, 0x70, 0x22,
			0xda, 0xb8, 0x3b, 0x06, 0x50, 0x53, 0x4a, 0x16, 0xe2, 0x65, 0xba, 0xe4, 0x43, 0x6f, 0xe3,
			0x29, 0x36, 0x18, 0xe3, 0x45, 0x01, 0x07, 0x20, 0x89, 0x4b, 0x64, 0x5e, 0x89, 0xe1, 0x53,
			0x5b, 0xbd, 0xad, 0x5b, 0x8b, 0x29, 0x06, 0x50, 0x53, 0x08, 0x01, 0xb1, 0x8e, 0xbf, 0xbf,
			0x5e, 0x8f, 0xab, 0x3c, 0x82, 0x87, 0x2a, 0x3e, 0x9b, 0xb7, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe1, 0x32, 0xa3,
			0x49, 0x76, 0x5c, 0x5b, 0x35, 0x9a, 0x93, 0x3c, 0x6f, 0x3c, 0x63, 0x6d, 0xc0, 0x00
		};
		Send(response, 119);
		return;
	}

	// Check for a possible IP ban on this client.
	BAN_STATUS ipb = IPBanner::getSingleton().CalculateBanStatus(GetRemoteAddress());

	switch(ipb)
	{
		case BAN_STATUS_PERMANENT_BAN:
			SendChallengeError(CE_ACCOUNT_CLOSED);
			return;

		case BAN_STATUS_TIME_LEFT_ON_BAN:
			SendChallengeError(CE_ACCOUNT_FREEZED);
			return;

		default:
			break;
	}

	// Null-terminate the account string
	if(m_challenge.I_len >= 0x50) { Disconnect(); return; }
	m_challenge.I[m_challenge.I_len] = 0;
	AccountName = (char*)&m_challenge.I;
	string::size_type i = AccountName.rfind("#");
	if( i != string::npos )
	{
		printf("# ACCOUNTNAME!\n");
		return;
	}

	// Look up the account information
	Log.Notice("AuthChallenge","Account Name: \"%s\"", AccountName.c_str());

	m_account = AccountMgr::getSingleton().GetAccount(AccountName);
	if(m_account == 0)
	{
		DEBUG_LOG("AuthChallenge","Invalid account.");

		// Non-existant account
		SendChallengeError(CE_NO_ACCOUNT);
		return;
	}


	// Check that the account isn't banned.
	if(m_account->Banned == 1)
	{
		SendChallengeError(CE_ACCOUNT_CLOSED);
		Log.Notice("AuthChallenge","Account closed state = %u", m_account->Banned);
		return;
	}
	else if(m_account->Banned > 0)
	{
		SendChallengeError(CE_ACCOUNT_FREEZED);
		Log.Notice("AuthChallenge","Account frozen state = %u", m_account->Banned);
		return;
	}
	else
		Log.Notice("AuthChallenge","Account OK, banned state = %u", m_account->Banned);

	// update cached locale
	if(!m_account->forcedLocale)
	{
		char temp[4];
		temp[0] = m_challenge.country[3];
		temp[1] = m_challenge.country[2];
		temp[2] = m_challenge.country[1];
		temp[3] = m_challenge.country[0];

		*(uint32*)&m_account->Locale[0] = *(uint32*)temp;
	}

	Sha1Hash sha;
	//uint32 tc = s.GetNumBytes();
	sha.UpdateData( s.AsByteArray(), 32 );
	sha.UpdateData( m_account->SrpHash, 20 );
	sha.Finalize();

	BigNumber x;
	x.SetBinary( sha.GetDigest(), sha.GetLength() );
	v = g.ModExp(x, N);
	b.SetRand(152);

	BigNumber gmod = g.ModExp(b, N);
	B = ((v * 3) + gmod) % N;
	ASSERT(gmod.GetNumBytes() <= 32);

	BigNumber unk;
	unk.SetRand(128);

	uint8 response[200];
	uint32 c = 0;
	response[c] = 0;										c += 1;
	response[c] = 0;										c += 1;
	response[c] = CE_SUCCESS;								c += 1;
	memcpy(&response[c], B.AsByteArray(), 32);				c += 32;
	response[c] = 1;										c += 1;
	response[c] = g.AsByteArray()[0];						c += 1;
	response[c] = 32;										c += 1;
	memcpy(&response[c], N.AsByteArray(), 32);				c += 32;
	memcpy(&response[c], s.AsByteArray(), s.GetNumBytes()); c += s.GetNumBytes();
	memcpy(&response[c], unk.AsByteArray(), 16);			c += 16;
	response[c] = 0;										c += 1;

	Send(response, c);
}

void AuthSocket::HandleProof()
{
	if(GetReadBuffer().GetSize() < sizeof(sAuthLogonProof_C))
		return ;

	// patch
	if(m_patch&&!m_account)
	{
		//RemoveReadBufferBytes(75,false);
		GetReadBuffer().Remove(75);
		DEBUG_LOG("AuthLogonProof","Intitiating PatchJob");
		uint8 bytes[2] = {0x01,0x0a};
		Send(bytes,2);
		PatchMgr::getSingleton().InitiatePatch(m_patch, this);
		return;
	}

	if(!m_account)
		return;

	DEBUG_LOG("AuthLogonProof","Interleaving and checking proof...");

	sAuthLogonProof_C lp;
	GetReadBuffer().Read(&lp, sizeof(sAuthLogonProof_C));

	BigNumber A;
	A.SetBinary(lp.A, 32);

	Sha1Hash sha;
	sha.UpdateBigNumbers(&A, &B, 0);
	sha.Finalize();

	BigNumber u;
	u.SetBinary(sha.GetDigest(), 20);
	
	BigNumber S = (A * (v.ModExp(u, N))).ModExp(b, N);
	uint8 t[32];
	uint8 t1[16];
	uint8 vK[40];
	memcpy(t, S.AsByteArray(), 32);
	for (int i = 0; i < 16; i++)
	{
		t1[i] = t[i*2];
	}
	sha.Initialize();
	sha.UpdateData(t1, 16);
	sha.Finalize();
	for (int i = 0; i < 20; i++)
	{
		vK[i*2] = sha.GetDigest()[i];
	}
	for (int i = 0; i < 16; i++)
	{
		t1[i] = t[i*2+1];
	}
	sha.Initialize();
	sha.UpdateData(t1, 16);
	sha.Finalize();
	for (int i = 0; i < 20; i++)
	{
		vK[i*2+1] = sha.GetDigest()[i];
	}
	m_sessionkey.SetBinary(vK, 40);

	uint8 hash[20];

	sha.Initialize();
	sha.UpdateBigNumbers(&N, NULL);
	sha.Finalize();
	memcpy(hash, sha.GetDigest(), 20);
	sha.Initialize();
	sha.UpdateBigNumbers(&g, NULL);
	sha.Finalize();
	for (int i = 0; i < 20; i++)
	{
		hash[i] ^= sha.GetDigest()[i];
	}
	BigNumber t3;
	t3.SetBinary(hash, 20);

	sha.Initialize();
	sha.UpdateData((const uint8*)m_account->UsernamePtr->c_str(), (int)m_account->UsernamePtr->size());
	sha.Finalize();

	BigNumber t4;
	t4.SetBinary(sha.GetDigest(), 20);

	sha.Initialize();
	sha.UpdateBigNumbers(&t3, &t4, &s, &A, &B, &m_sessionkey, NULL);
	sha.Finalize();

	BigNumber M;
	M.SetBinary(sha.GetDigest(), 20);

	// Compare M1 values.
	if(memcmp(lp.M1, M.AsByteArray(), 20) != 0)
	{
		// Authentication failed.
		//SendProofError(4, 0);
		SendChallengeError(CE_NO_ACCOUNT);
		DEBUG_LOG("AuthLogonProof","M1 values don't match.");
		return;
	}

	// Store sessionkey
	m_account->SetSessionKey(m_sessionkey.AsByteArray());

//	OUT_DEBUG("========================\nSession key: ");
//	for(uint32 z = 0; z < 40; ++z)
//		OUT_DEBUG("%.2X ", m_account->SessionKey[z]);
//	OUT_DEBUG("\n========================\n");


	// let the client know
	sha.Initialize();
	sha.UpdateBigNumbers(&A, &M, &m_sessionkey, 0);
	sha.Finalize();

	SendProofError(0, sha.GetDigest());
	DEBUG_LOG("AuthLogonProof","Authentication Success.");

	// we're authenticated now :)
	m_authenticated = true;

	// Don't update when IP banned, but update anyway if it's an account ban
	const char* m_sessionkey_hex = m_sessionkey.AsHexStr();
	sLogonSQL->Execute("UPDATE accounts SET lastlogin=NOW(), SessionKey = '%s', lastip='%s' WHERE acct=%u;", m_sessionkey_hex, GetRemoteIP().c_str(), m_account->AccountId);
}

void AuthSocket::SendChallengeError(uint8 Error)
{
	uint8 buffer[3];
	buffer[0] = buffer[1] = 0;
	buffer[2] = Error;

	Send(buffer, 3);
}

void AuthSocket::SendProofError(uint8 Error, uint8 * M2)
{
	uint8 buffer[32];
	memset(buffer, 0, 32);

	buffer[0] = 1;
	buffer[1] = Error;
	if(M2 == 0)
	{
#ifdef USING_BIG_ENDIAN
		*(uint32*)&buffer[2] = swap32(3);
#else
		*(uint32*)&buffer[2] = 3;
#endif
		Send(buffer, 6);
		return;
	}
	
	memcpy(&buffer[2], M2, 20);
	Send(buffer, 32);
}

#define AUTH_CHALLENGE 0
#define AUTH_PROOF 1
#define AUTH_RECHALLENGE 2
#define AUTH_REPROOF 3
#define REALM_LIST 16
#define INITIATE_TRANSFER 48		// 0x30
#define TRANSFER_DATA 49		// 0x31
#define ACCEPT_TRANSFER 50		// 0x32
#define RESUME_TRANSFER 51		// 0x33
#define CANCEL_TRANSFER 52		// 0x34
#define MAX_AUTH_CMD 53

typedef void (AuthSocket::*AuthHandler)();
static AuthHandler Handlers[MAX_AUTH_CMD] = {
		&AuthSocket::HandleChallenge,			// 0
		&AuthSocket::HandleProof,				// 1
		&AuthSocket::HandleReconnectChallenge,	// 2
		&AuthSocket::HandleReconnectProof,		// 3
		NULL,									// 4
		NULL,									// 5
		NULL,									// 6
		NULL,									// 7
		NULL,									// 8
		NULL,									// 9
		NULL,									// 10
		NULL,									// 11
		NULL,									// 12
		NULL,									// 13
		NULL,									// 14
		NULL,									// 15
		&AuthSocket::HandleRealmlist,			// 16
		NULL,									// 17
		NULL,									// 18
		NULL,									// 19
		NULL,									// 20
		NULL,									// 21
		NULL,									// 22
		NULL,									// 23
		NULL,									// 24
		NULL,									// 25
		NULL,									// 26
		NULL,									// 27
		NULL,									// 28
		NULL,									// 29
		NULL,									// 30
		NULL,									// 31
		NULL,									// 32
		NULL,									// 33
		NULL,									// 34
		NULL,									// 35
		NULL,									// 36
		NULL,									// 37
		NULL,									// 38
		NULL,									// 39
		NULL,									// 40
		NULL,									// 41
		NULL,									// 42
		NULL,									// 43
		NULL,									// 44
		NULL,									// 45
		NULL,									// 46
		NULL,									// 47
		NULL,									// 48
		NULL,									// 49
		&AuthSocket::HandleTransferAccept,		// 50
		&AuthSocket::HandleTransferResume,		// 51
		&AuthSocket::HandleTransferCancel,		// 52
};

void AuthSocket::OnRead()
{
	if(GetReadBuffer().GetContiguiousBytes() < 1)
		return;

	uint8 Command = *(uint8*)GetReadBuffer().GetBufferStart();
	last_recv = UNIXTIME;
	if(Command < MAX_AUTH_CMD && Handlers[Command] != NULL)
		(this->*Handlers[Command])();
	else
		Log.Notice("AuthSocket", "Unknown cmd %u", Command);
}

void AuthSocket::HandleRealmlist()
{
	sInfoCore.SendRealms(this);
}

void AuthSocket::HandleReconnectChallenge()
{
	// No header
	if(GetReadBuffer().GetContiguiousBytes() < 4)
		return;	

	// Check the rest of the packet is complete.
	uint8 * ReceiveBuffer = (uint8*)GetReadBuffer().GetBufferStart();
	uint16 full_size = *(uint16*)&ReceiveBuffer[2];
	DEBUG_LOG("ReconnectChallenge","got header, body is 0x%02X bytes", full_size);

	if(GetReadBuffer().GetSize() < (uint32)full_size+4)
		return;

	// Copy the data into our cached challenge structure
	if((size_t)(full_size+4) > sizeof(sAuthLogonChallenge_C))
	{
		Disconnect();
		return;
	}

	DEBUG_LOG("ReconnectChallenge","got full packet.");

	memcpy(&m_challenge, ReceiveBuffer, full_size + 4);
	GetReadBuffer().Read(&m_challenge, full_size + 4);

	// Check client build.
	if(m_challenge.build > LogonServer::getSingleton().max_build ||
		m_challenge.build < LogonServer::getSingleton().min_build)
	{
		SendChallengeError(CE_WRONG_BUILD_NUMBER);
		return;
	}

	// Check for a possible IP ban on this client.
	BAN_STATUS ipb = IPBanner::getSingleton().CalculateBanStatus(GetRemoteAddress());

	switch(ipb)
	{
	case BAN_STATUS_PERMANENT_BAN:
		SendChallengeError(CE_ACCOUNT_CLOSED);
		return;

	case BAN_STATUS_TIME_LEFT_ON_BAN:
		SendChallengeError(CE_ACCOUNT_FREEZED);
		return;

	default:
		break;
	}

	// Null-terminate the account string
	if( m_challenge.I_len >= 50 )
	{
		Disconnect();
		return;
	}

	m_challenge.I[m_challenge.I_len] = 0;

	// Look up the account information
	AccountName = (char*)&m_challenge.I;
	Log.Notice("ReconnectChallenge","Account Name: \"%s\"", AccountName.c_str());

	m_account = AccountMgr::getSingleton().GetAccount(AccountName);
	if(m_account == 0)
	{
		DEBUG_LOG("ReconnectChallenge","Invalid account.");

		// Non-existant account
		SendChallengeError(CE_NO_ACCOUNT);
		return;
	}


	// Check that the account isn't banned.
	if(m_account->Banned == 1)
	{
		SendChallengeError(CE_ACCOUNT_CLOSED);
		Log.Notice("ReconnectChallenge","Account banned state = %u", m_account->Banned);
		return;
	}
	else if(m_account->Banned > 0)
	{
		SendChallengeError(CE_ACCOUNT_FREEZED);
		Log.Notice("ReconnectChallenge","Account banned state = %u", m_account->Banned);
		return;
	}
	else
		DEBUG_LOG("ReconnectChallenge","Account banned state = %u", m_account->Banned);

	if(!m_account->SessionKey)
	{
		SendChallengeError(CE_SERVER_FULL);
		return;
	}

	/** Mangos figured this out, thanks for the structure.
	 */

    ///- Sending response
    ByteBuffer pkt;
    pkt << (uint8)  0x02;	//ReconnectChallenge
    pkt << (uint8)  0x00;
    rs.SetRand(16*8);
    pkt.append(rs.AsByteBuffer());	// 16 bytes random
    pkt << (uint64) 0x00 << (uint64) 0x00;	// 16 bytes zeros
    Send(pkt.contents(), uint32(pkt.size()));
}

void AuthSocket::HandleReconnectProof()
{
	if( m_account == NULL )
		return;

	// Load sessionkey from account database.
	QueryResult * result = sLogonSQL->Query ("SELECT SessionKey FROM accounts WHERE acct = %u", m_account->AccountId);
	if(result)
	{
		Field * field = result->Fetch();
		K.SetHexStr(field[0].GetString ());
		delete result;
	}
	else
	{
	    // Disconnect if the sessionkey invalid or not found
		DEBUG_LOG("AuthReConnectProof","No matching SessionKey found while user %s tried to login.", AccountName.c_str());
		Disconnect();
		return;
	}

	if(GetReadBuffer().GetSize() < sizeof(sAuthLogonProofKey_C))
		return;

	sAuthLogonProofKey_C lp;
	GetReadBuffer().Read(&lp, sizeof(sAuthLogonProofKey_C));

	BigNumber A;
	A.SetBinary(lp.R1, 16);

	Sha1Hash sha;
	sha.Initialize();
	sha.UpdateData(AccountName);
	sha.UpdateBigNumbers(&A, &rs, &K, 0);
	sha.Finalize();

	if (!memcmp(sha.GetDigest(), lp.R2, SHA_DIGEST_LENGTH))
	{
		///- Sending response
		ByteBuffer pkt;
		pkt << (uint8)  0x03;	//ReconnectProof
		pkt << (uint8)  0x00;
		pkt << (uint16) 0x00;	// 2 bytes zeros
		Send(pkt.contents(), uint32(pkt.size()));

		// we're authenticated now :)
		m_authenticated = true;

		DEBUG_LOG("AuthReConnectProof","Authentication Success.");
	}
	else
		DEBUG_LOG("AuthReConnectProof","Authentication Failed.");

}

void AuthSocket::HandleTransferAccept()
{
	DEBUG_LOG("AuthSocket","Accepted transfer");
	if(!m_patch)
		return;

	GetReadBuffer().Remove(1);
	PatchMgr::getSingleton().BeginPatchJob(m_patch,this,0);
}

void AuthSocket::HandleTransferResume()
{
	DEBUG_LOG("AuthSocket","Resuming transfer");
	if(!m_patch)
		return;

	GetReadBuffer().Remove(1);
	uint64 size;
	GetReadBuffer().Read(&size, 8);
	if(size>=m_patch->FileSize)
		return;

	PatchMgr::getSingleton().BeginPatchJob(m_patch,this,(uint32)size);
}

void AuthSocket::HandleTransferCancel()
{
	GetReadBuffer().Remove(1);
	Disconnect();
}
