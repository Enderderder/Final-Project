//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2015 Media Design School
//
// File Name	: 
// Description	: 
// Author		: Your Name
// Mail			: your.name@mediadesign.school.nz
//

#ifndef __NETWORK_H__
#define __NETWORK_H__

//Types
enum EEntityType
{
	CLIENT = 1,
	SERVER
};

//constants
namespace
{
	unsigned const DEFAULT_SERVER_PORT = 12855;
	unsigned const DEFAULT_CLIENT_PORT = 13803;
	unsigned const MAX_MESSAGE_LENGTH = 256;
	unsigned const MAX_ADDRESS_LENGTH = 32;
}

namespace ErrorRoutines
{
	void PrintWSAErrorInfo(int iError);
}

//Forward Decalarations
class INetworkEntity;

class CNetwork
{
public:
	~CNetwork();
	
	bool Initialise(EEntityType _eType);
	void StartUp(); //A network has an ability to start up
	void ShutDown(); //& an ability to be shut down
	bool IsOnline() const;

	//Accessor methods
	INetworkEntity* GetNetworkEntity() const;

	// Singleton Methods
	static CNetwork* GetInstance();
	static void DestroyObject();

private:
	//Make the network class a singleton. There is only one instance of the network running
	CNetwork();
	CNetwork(const CNetwork& _kr) = delete;
	CNetwork& operator= (const CNetwork& _kr) = delete;

protected:
	//A network has a network entity
	INetworkEntity* m_pNetworkEntity;
	bool m_bOnline;

	// Singleton Instance
	static CNetwork* s_pNetwork;
};
#endif