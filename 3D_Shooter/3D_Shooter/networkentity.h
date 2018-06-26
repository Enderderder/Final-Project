/*
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2018 Media Design School
//
// File Name    : networkentity.h
// Description	:
// Author       : Richard Wulansari & Jacob Dewse
// Mail         : richard.wul7481@mediadesign.school.nz, jacob.dew7364@mediadesign.school.nz
*/

#ifndef __NETWORKENTITY_H__
#define __NETWORKENTITY_H__

#include <string>
#include <sstream>

enum EMessageType : unsigned char
{
	HANDSHAKE,
	DATA,
	USERNAME,
	KEEPALIVE,
	COMMAND,
	LOGGIN,
	BROADCAST
};

struct GameInfo
{
	float position;
	int health;
};

struct TPacket 
{
	unsigned short MessageType;
	char MessageContent[50];
	char PacketData[60];
	unsigned short PacketSize;

	//void set_packet(short _x, short _y, WORD _object_type, short _object_index, WORD _param)
	void Serialize(EMessageType _MessageType, const char* _message)
	{
		
		MessageType = _MessageType;
		strcpy_s(MessageContent, strlen(_message) + 1, _message);

		std::ostringstream oss;
		oss << MessageType;
		oss << " ";
		oss << MessageContent;
		
		std::string _strToSend = oss.str();
		const char* _pcToSend = _strToSend.c_str();

		strcpy_s(PacketData, strlen(_pcToSend) + 1, _pcToSend);

		PacketSize = static_cast<unsigned short>(_strToSend.size());
	}
	
	TPacket Deserialize(char* _PacketData)
	{
		std::string _strTemp(_PacketData);
		std::istringstream iss(_strTemp);
		
		iss >> this->MessageType;
		
		//iss >> this->MessageContent;
		std::string _tempMessageBuffer;
		getline(iss, _tempMessageBuffer);
		strcpy_s(MessageContent, _tempMessageBuffer.length() + 1, _tempMessageBuffer.c_str());
		
		return *this;
	}

};

class INetworkEntity
{
public:
	virtual bool Initialise() = 0; //Implicit in the intialization is the creation and binding of the socket
	virtual bool SendData(char* _pcDataToSend) = 0;
	virtual void ReceiveData(char* _pcBufferToReceiveData) = 0;
	virtual void GetRemoteIPAddress(char *_pcSendersIP) = 0;
	virtual unsigned short GetRemotePort() = 0;
};
#endif 