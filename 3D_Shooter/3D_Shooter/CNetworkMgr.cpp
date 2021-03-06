/*
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2018 Media Design School
//
// File Name    : CNetworkMgr.cpp
// Description	:
// Author       : Richard Wulansari & Jacob Dewse
// Mail         : richard.wul7481@mediadesign.school.nz, jacob.dew7364@mediadesign.school.nz
*/

// This Include --------------------------------------------------------------------------------
#include "CNetworkMgr.h"

// Network Include -----------------------------------------------------------------------------
#include "consoletools.h"
#include "network.h"
#include "client.h"
#include "server.h"
#include "InputLineBuffer.h"
#include <functional>

// Static Variable -----------------------------------------------------------------------------
CNetworkMgr* CNetworkMgr::s_pNetworkMgr = nullptr;

// Global Variable -----------------------------------------------------------------------------
CInputLineBuffer _InputBuffer(MAX_MESSAGE_LENGTH);
static CNetwork* _rNetwork = CNetwork::GetInstance();

//A pointer to hold a client instance
CClient* _pClient = nullptr;
//A pointer to hold a server instance
CServer* _pServer = nullptr;

char _cIPAddress[MAX_ADDRESS_LENGTH]; // An array to hold the IP Address as a string
//ZeroMemory(&_cIPAddress, strlen(_cIPAddress));

char* _pcPacketData = 0; //A local buffer to receive packet data info

CNetworkMgr::CNetworkMgr()
{}

CNetworkMgr::~CNetworkMgr()
{
	if (_eNetworkEntityType == CLIENT)
	{
		m_ClientReceiveThread.detach();
		//m_ClientReceiveThread.join();
	}
	else 
	{
		m_ServerReceiveThread.detach();
		//m_ServerReceiveThread.join();
	}
	//Shut Down the Network
	//_rNetwork->ShutDown();
	//_rNetwork->DestroyObject();

	delete[] _pcPacketData;
}

CNetworkMgr * CNetworkMgr::GetInstance()
{
	if (s_pNetworkMgr == nullptr)
	{
		s_pNetworkMgr = new CNetworkMgr();
	}

	return s_pNetworkMgr;
}

void CNetworkMgr::DestroyInstance()
{
	delete s_pNetworkMgr;
	s_pNetworkMgr = nullptr;
}

void CNetworkMgr::StartNetwork(EEntityType _eNetworkEntityType)
{
	_pcPacketData = new char[MAX_MESSAGE_LENGTH];
	strcpy_s(_pcPacketData, strlen("") + 1, "");

	//Get the instance of the network
	_rNetwork->StartUp();

	if (!_rNetwork->Initialise(_eNetworkEntityType))
	{
		std::cout << "Unable to initialise the Network........Press any key to continue......";
		_getch();
		return;
	}

	//Run receive on a separate thread so that it does not block the main client thread.
	if (_eNetworkEntityType == CLIENT) //if network entity is a client
	{
		_pClient = static_cast<CClient*>(_rNetwork->GetNetworkEntity());
		m_ClientReceiveThread = std::thread(&CClient::ReceiveData, _pClient, std::ref(_pcPacketData));
		//m_ClientReceiveThread.detach();
	}

	//Run receive of server also on a separate thread 
	else if (_eNetworkEntityType == SERVER) //if network entity is a server
	{
		_pServer = static_cast<CServer*>(_rNetwork->GetNetworkEntity());
		m_ServerReceiveThread = std::thread(&CServer::ReceiveData, _pServer, std::ref(_pcPacketData));
		//m_ServerReceiveThread.detach();
	}
}

EEntityType CNetworkMgr::GetEntityType()
{
	return _eNetworkEntityType;

}

void CNetworkMgr::ClientMainLoop()
{
	if (_rNetwork->IsOnline())
	{
		if (_pClient != nullptr)
		{
			//If the message queue is empty 
			if (_pClient->GetWorkQueue()->empty())
			{
				//Don't do anything
			}
			else
			{
				//Retrieve off a message from the queue and process it
				std::string temp;
				_pClient->GetWorkQueue()->pop(temp);
				_pClient->ProcessData(const_cast<char*>(temp.c_str()));
			}
		}
	}
}

void CNetworkMgr::ServerMainLoop()
{
	if (_rNetwork->IsOnline())
	{
		if (_pServer != nullptr)
		{
			if (!_pServer->GetWorkQueue()->empty())
			{
				_rNetwork->GetNetworkEntity()->GetRemoteIPAddress(_cIPAddress);
				std::cout << _cIPAddress
				<< ":" << _rNetwork->GetNetworkEntity()->GetRemotePort() << "> " << _pcPacketData << std::endl;

				//Retrieve off a message from the queue and process it
				_pServer->GetWorkQueue()->pop(_pcPacketData);
				_pServer->ProcessData(_pcPacketData);
			}
		}
	}
}

bool CNetworkMgr::IsNetOnline() const
{
	return _rNetwork->IsOnline();
}

void CNetworkMgr::SetAsServer()
{
	_eNetworkEntityType = SERVER;
}

void CNetworkMgr::SetAsClient()
{
	_eNetworkEntityType = CLIENT;
}
