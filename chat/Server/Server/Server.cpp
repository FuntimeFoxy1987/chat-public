#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <chrono>
#include <vector>
#include <ctime>

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>

using namespace std;

#include "BaseDate.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable: 4996)

#define Global 0
#define Server 1

const string VersionPacket = "1.0.0";

struct User
{
	User()
	{
		Construction();
	}
	void Construction()
	{
		Connect = NULL;
		Authorization = false;
		login = "Undefine";
		password = "";
		intcoder = 0;
	}
	SOCKET Connect;
	bool Authorization;
	string login;
	string password;
	int intcoder;
};

vector<User> Users;
int Counter = 0;

BaseDate DateUser(".\\BaseDate\\users.json", false);
BaseDate DatePassword(".\\BaseDate\\password.json", false);

enum Packet
{
	P_ChatMessage,
	P_AuthorizationUser,
	//P_Disconnect,
	P_File,
	P_PVersion
};
enum Authorization
{
	A_Login,
	A_Intcoder,
	A_Password,
	A_AuthorizationEnd
};

string coder(string date, int intcoder)
{
	int dateIntCodeElement;
	for (int i = 0; i < date.size(); i++)
	{
		dateIntCodeElement = i + intcoder;
		if (dateIntCodeElement == 256)
		{
			dateIntCodeElement = 0;
		}
		int dateCodeElement;
		if (((int)date[i]) + dateIntCodeElement > 255)
		{
			dateCodeElement = ((int)date[i]) + dateIntCodeElement - 256;
		}
		else
		{
			dateCodeElement = ((int)date[i]) + dateIntCodeElement;
		}
		//cout << dateCodeElement << "[" << dateIntCodeElement << "],";
		date[i] = (char)dateCodeElement;
	}
	return date;
}

string encoder(string date, int intcoder)
{
	int dateIntCodeElement;
	for (int i = 0; i < date.size(); i++)
	{
		dateIntCodeElement = i + intcoder;
		if (dateIntCodeElement == 256)
		{
			dateIntCodeElement = 0;
		}
		int dateEncodeElement;
		if (((int)date[i]) - dateIntCodeElement < 0)
		{
			dateEncodeElement = 256 - (dateIntCodeElement - ((int)date[i]));
		}
		else
		{
			dateEncodeElement = ((int)date[i]) - dateIntCodeElement;
		}
		//cout << dateEncodeElement << "[" << dateIntCodeElement << "],";
		date[i] = (char)dateEncodeElement;
	}
	return date;
}

bool ProcessPacket(int index, Packet packettype)
{
	switch (packettype)
	{
	case P_PVersion:
	{
		int PV_size;
		recv(Users[index].Connect, (char*)& PV_size, sizeof(int), NULL);
		char* PV = new char[PV_size + 1];
		PV[PV_size] = '\0';
		recv(Users[index].Connect, PV, PV_size, NULL);
		if (PV == VersionPacket)
		{
			Packet Ptype = P_PVersion;
			send(Users[index].Connect, (char*)& Ptype, sizeof(Packet), NULL);
			bool bo = true;
			send(Users[index].Connect, (char*)& bo, sizeof(bool), NULL);
		}
		else 
		{
			Packet Ptype = P_PVersion;
			send(Users[index].Connect, (char*)& Ptype, sizeof(Packet), NULL);
			bool bo = false;
			send(Users[index].Connect, (char*)& bo, sizeof(bool), NULL);
		}
		break;
	}
	case P_ChatMessage:
	{
		int msg_size;
		recv(Users[index].Connect, (char*)& msg_size, sizeof(int), NULL);
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		recv(Users[index].Connect, msg, msg_size, NULL);

		int id_size;
		recv(Users[index].Connect, (char*)& id_size, sizeof(int), NULL);
		char* id = new char[id_size + 1];
		id[id_size] = '\0';
		recv(Users[index].Connect, id, id_size, NULL);

		int name_size;
		recv(Users[index].Connect, (char*)& name_size, sizeof(int), NULL);
		char* name = new char[name_size + 1];
		name[name_size] = '\0';
		recv(Users[index].Connect, name, name_size, NULL);

		for (int i = 0; i < Users.size(); i++)
		{
			if (i == index)
			{
				continue;
			}

			Packet msgtype = P_ChatMessage;
			send(Users[i].Connect, (char*)& msgtype, sizeof(Packet), NULL);
			send(Users[i].Connect, (char*)& msg_size, sizeof(int), NULL);
			send(Users[i].Connect, msg, msg_size, NULL);

			send(Users[i].Connect, (char*)& id_size, sizeof(int), NULL);
			send(Users[i].Connect, id, id_size, NULL);

			send(Users[i].Connect, (char*)& name_size, sizeof(int), NULL);
			send(Users[i].Connect, name, name_size, NULL);
		}
		delete[] msg;
		delete[] id;
		break;
	}
	case P_AuthorizationUser:
	{
		Authorization ASType;
		recv(Users[index].Connect, (char*)& ASType, sizeof(Authorization), NULL);

		if (ASType == A_Login)
		{
			//recv login
			int login_size;
			recv(Users[index].Connect, (char*)& login_size, sizeof(int), NULL);
			char *login = new char[login_size + 1];
			login[login_size] = '\0';
			recv(Users[index].Connect, login, login_size, NULL);

			Users[index].login = login;
			delete[] login;

			//send intcoder
			vector<string> puth = { Users[index].login, "intcoder" };
			Users[index].intcoder = stoi(DatePassword.get(puth));

			Packet msgtype = P_AuthorizationUser;
			send(Users[index].Connect, (char*)& msgtype, sizeof(Packet), NULL);
			Authorization ASTypeSend = A_Intcoder;
			send(Users[index].Connect, (char*)& ASTypeSend, sizeof(Authorization), NULL);
			send(Users[index].Connect, (char*)& Users[index].intcoder, sizeof(int), NULL);
			return true;
		}
		if (ASType == A_Password)
		{
			//recv password
			int password_size;
			recv(Users[index].Connect, (char*)& password_size, sizeof(int), NULL);
			char* password = new char[password_size + 1];
			password[password_size] = '\0';
			recv(Users[index].Connect, password, password_size, NULL);
			vector<string> puth = { Users[index].login, "password" };
			if (DatePassword.get(puth) == password)
			{
				cout << "User " << Users[index].login << " authorization" << endl;
				string password_str = password;

				delete[] password;

				password_str = encoder(password_str, Users[index].intcoder);

				srand(time(NULL));
				Users[index].intcoder = (rand() % 127) + 1;

				password_str = coder(password_str, Users[index].intcoder);

				puth = { Users[index].login, "intcoder" };
				DatePassword.set(puth, Users[index].intcoder);

				puth = { Users[index].login, "password" };
				DatePassword.set(puth, password_str);

				Users[index].Authorization = true;

				DatePassword.uploadDate();

				Packet msgtype = P_AuthorizationUser;
				send(Users[index].Connect, (char*)& msgtype, sizeof(Packet), NULL);
				Authorization ASTypeSend = A_AuthorizationEnd;
				send(Users[index].Connect, (char*)& ASTypeSend, sizeof(Authorization), NULL);

				puth = { Users[index].login, "id" };
				send(Users[index].Connect, (char*)& Users[index].Authorization, sizeof(bool), NULL);

				int size = DatePassword.get(puth).size();
				send(Users[index].Connect, (char*)& size, sizeof(int), NULL);
				send(Users[index].Connect, DatePassword.get(puth).c_str(), size, NULL);

				puth = { DatePassword.get(puth).c_str(), "name" };

				size = DateUser.get(puth).size();
				send(Users[index].Connect, (char*)& size, sizeof(int), NULL);
				send(Users[index].Connect, DateUser.get(puth).c_str(), size, NULL);
				return true;
			}
			else
			{
				cout << "User " << Users[index].login << " don't authorization" << endl;

				Users[index].Authorization = false;

				Packet msgtype = P_AuthorizationUser;
				send(Users[index].Connect, (char*)& msgtype, sizeof(Packet), NULL);
				Authorization ASTypeSend = A_AuthorizationEnd;
				send(Users[index].Connect, (char*)& ASTypeSend, sizeof(Authorization), NULL);

				puth = { Users[index].login, "id" };
				send(Users[index].Connect, (char*)& Users[index].Authorization, sizeof(bool), NULL);
				return true;
			}
		}
		
		break;
	}
	case P_File:
	{
		int puth_file_size;
		recv(Users[index].Connect, (char*)& puth_file_size, sizeof(int), NULL);
		char* puth_file = new char[puth_file_size + 1];
		puth_file[puth_file_size] = '\0';
		recv(Users[index].Connect, puth_file, puth_file_size, NULL);

		cout << "Puth file : " << puth_file << endl;

		string puth = "BaseDate/";
		puth += puth_file;

		Packet type = P_File;
		ifstream file;
		file.open(puth, ifstream::binary);
		send(Users[index].Connect, (char*)& type, sizeof(Packet), NULL);
		bool is_open = file.is_open();
		send(Users[index].Connect, (char*)& is_open, sizeof(bool), NULL);
		send(Users[index].Connect, (char*)& puth_file_size, sizeof(int), NULL);
		send(Users[index].Connect, puth_file, puth_file_size, NULL);
		if (file.is_open())
		{
			string str = "";
			char ch;
			while (file.get(ch))
			{
				str += ch;
			}
			int file_size = str.size();
			send(Users[index].Connect, (char*)& file_size, sizeof(int), NULL);
			send(Users[index].Connect, str.c_str(), file_size, NULL);
		}

		break;
	}
	default:
		cout << "Unrecognized packet: " << packettype << endl;
		break;
	}
	return true;
}

void Disconnect(int index)
{
	//cout << "Client disconnect : " << Users[index].Connect << "[" << index << "]" << endl;
	//Users[index].Construction();
	auto iter = Users.cbegin();
	Users.erase(iter + index);
	Counter = index;
}

void ClientHandler(int index)
{
	Packet packettype;
	//cout << "[" << index << "]" << endl;
	while (true)
	{
		if (recv(Users[index].Connect, (char*)& packettype, sizeof(Packet), NULL) == SOCKET_ERROR)
		{
			Disconnect(index);
			return;
		}

		if (!ProcessPacket(index, packettype))
		{
			Disconnect(index);
			/*packettype = P_Disconnect;
			cout << "-->" <<endl;
			if (recv(Users[index].Connect, (char*)& packettype, sizeof(Packet), NULL) == SOCKET_ERROR)
			{
				cout << "-->2" <<endl;
				Disconnect(index);
				return;
			}*/
			break;
		}
	}
	closesocket(Users[index].Connect);
}

__int64 timeNow()
{
	SYSTEMTIME time_mill;
	GetLocalTime(&time_mill);
	auto time_now = chrono::system_clock::now();
	
	time_t time = std::chrono::system_clock::to_time_t(time_now);

	return time * 1000 + time_mill.wMilliseconds;
}

void starusUsers()
{
	while (true)
	{
		system("cls");
		cout << (char)218 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)194 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)194 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)191 << endl;
		cout << (char)179 << "login   " << (char)179 << "connect " << (char)179 << "autho" << (char)179 << "" << endl;
		for (int i = 0; i < Users.size(); i++)
		{
			cout << (char)195 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)197 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)197 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)180 << endl;
			string str = { (char)179,' ',' ',' ',' ',' ',' ',' ',' ',(char)179 };
			if (Users[i].login != "")
			{
				if (Users[i].login.size() > 8)
				{
					for (int l = 0; l < 8; l++)
					{
						str[l + 1] = Users[i].login[l];
					}
				}
				else
				{
					for (int l = 0; l < Users[i].login.size(); l++)
					{
						str[l + 1] = Users[i].login[l];
					}
				}
			}
			int connectSize = 1;
			if ((__int64)Users[i].Connect < 100000000)
			{
				__int64 f = (__int64)Users[i].Connect;
				while (f / pow(10, connectSize))
				{
					f /= pow(10, connectSize);
					connectSize++;
				}
				str += to_string((__int64)Users[i].Connect);
				for (f = 0; f < (8 - connectSize); f++)
				{
					str += " ";
				}
			}
			str += (char)179;
			if (Users[i].Authorization)
			{
				str += "true ";
				str += (char)179;
			}
			else
			{
				str += "false";
				str += (char)179;
			}
			cout << str << endl;
		}
		cout << (char)192 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)193 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)193 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)217 << endl;
		Sleep(100);
	}
}


/*struct icon
{
	string file;
	string url;
};

enum active
{
	ofline,
	online,
	idle
};

class User
{
public:
	User()
	{
		id = 0;
		ip = (char*)"";
		//login = "";
		//passwork = "";
		icon.file = "";
		active = ofline;
		regTime = 0;
		conTime = 0;

		permission = 0;
		DiscordOauth2 = false;
		DiscordID = 0;
	}
	/*void setIp(string str)
	{
		ip = new char[str.size() + 1];
		for (int i = 0; i < str.size(); i++)
		{
			ip[i] = str[i];
		}
		ip[str.size()] = '\0';
	}
	int id;
	string ip;
	string login;
	string passwork;
	icon icon;
	active active;
	int regTime;
	int conTime;

	int permission;
	bool DiscordOauth2;
	int DiscordID;
};

struct DateUser
{
	int numberUsers;
	vector<User> users;
	//User* users;
	~DateUser()
	{
		//delete[] users;
	}
};*/

/*class BaseDateUser
{
public:
	BaseDateUser(int numberUsers)
	{
		//this->numberUsers = numberUsers;
		//this->users = new DateUser[numberUsers];
	}
	void RegisterUserFull(DateUser user)
	{
		Date.users[0] = user;
	}
	void RegisterUser(string login, string password)
	{

	}
private:
	DateUser* Date;
};*/

/*void upDate(User object)
{
	ofstream file;
	file.open(".\\BaseDate\\users.date");
	if (!file.is_open())
	{
		cout << "Error update base date users: open base date users";
	}
	else
	{
		//file << object.ip;
		//DateUser u[(const*)object.users.size];
		//u = object.users[0];
		file.write((char*)&object, sizeof(User));
	}
	file.close();
}

User loadDate()
{
	ifstream file;
	User object;
	file.open(".\\BaseDate\\users.date");
	if (!file.is_open())
	{
		cout << "Error update base date users: open base date users";
	}
	else
	{
		file.read((char*)&object, sizeof(User));
		cout << (char*)&object;
		string str = object.ip.c_str();
		//delete[] object.ip.c_str();
		object.ip.~basic_string();
		object.ip = str;

		cout << object.ip << endl;
	}
	file.close();
	return object;
}*/

int main(int argc, char* argv[])
{
#if Server
	WSAData wsaDate;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaDate) != 0)
	{
		cout << "Error load library";
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	//addr.sin_addr.S_un.S_addr = INADDR_ANY;
#if Global
	addr.sin_addr.s_addr = INADDR_ANY;
#else
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
#endif
	addr.sin_port = htons(1987);
	addr.sin_family = AF_INET;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)& addr, sizeof(addr));
	listen(sListen, SOMAXCONN /*Number connect*/);

	cout << "Server the be starting!\nListening for incoming connections...." << endl;

	//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)starusUsers, NULL, NULL, NULL);

	SOCKET newConnection;
	for (int i = 0; i < 100; i++)
	{
		newConnection = accept(sListen, (SOCKADDR*)& addr /*после выполения будет содержать айпи клиента*/, &sizeofaddr);

		if (newConnection == 0)
		{
			cout << "Error connect client" << endl;
		}
		else {
			//cout << "Client : "<< addr.sin_addr.s_addr << "," << inet_ntoa(*(IN_ADDR*)& addr) << " Connected!\n";
			/*string msg = "Hewwo client!";
			int msg_size = msg.size();
			Packet msgtype = P_ChatMessage;
			send(newConnection, (char*)& msgtype, sizeof(Packet), NULL);
			send(newConnection, (char*)&msg_size, sizeof(int), NULL);
			send(newConnection, msg.c_str(), msg_size, NULL);*/

			//Counter = 0;
			if (Counter == Users.size())
			{
				User newUser;
				newUser.Connect = newConnection;
				//Users.push_back(newUser);
				Users.push_back(newUser);

				CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(Counter), NULL, NULL);

				Counter++;
			}
			else {
				User newUser;
				newUser.Connect = newConnection;
				Users[Counter] = newUser;

				CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(Counter), NULL, NULL);
			}
		}
	}
#else
	//cout << coder("8483++9()?", 1) << endl;
	cout << timeNow() << endl;
	ifstream file;
	file.open("ddhykv9-665580bb-104a-411f-b431-7ed577440cc6.png", ifstream::binary);
	cout << file.is_open() << endl;
	if (file.is_open())
	{
		string str = "";
		char ch;
		while (file.get(ch))
		{
			str += ch;
		}
		cout << str.size();
	}
#endif
	system("pause");
	return 0;
}