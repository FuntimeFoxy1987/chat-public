#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <string>

#include <fstream>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable: 4996)

#define Global 0
#define Client 1
#define spriPChat_ 0
#define textureArrow_ 0
#define Chat_ 0

using namespace std;

const string VersionProgram = "0.0.0";
const string VersionPacket = "1.0.0";

using namespace sf;

string programs_Puth;

string resourse = "C:\\ProgramData\\inifiscalam\\resourse\\";

SOCKET Connection;

string windows[2];
bool windowsOn = true;
bool AuthorizationUserWindows = false;

enum Packet
{
	P_ChatMessage,
	P_AuthorizationUser,
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

enum WindowsActive
{
	W_Loading,
	W_Authorization,
	W_Chat,
	W_ConnectAuthorization,
	W_ConnectChat,
	w_Close,
	W_Nothing
};

enum Shape_Active
{
	S_Nothing,
	S_AnNothing,
	S_Animation,
	S_AnAnimation,
	S_Rotation
};

enum TableActive
{
	T_Full,
	T_Shift,
	T_AnShift,
	T_Smoll
};

struct AuthorizationStruct
{
	AuthorizationStruct()
	{
		login = "";
		password = "";
		intcoder = 0;
	}
	string login;
	string password;
	int intcoder;
};

struct User
{
	User()
	{
		id = "";
		Authorization = false;
		ErrorPassword = false;
	}
	string id;
	bool Authorization;
	bool ErrorPassword;
	string name;
};

struct Message
{
	String content;
	string id;
	String name;
};

struct Avatar
{
	Texture texture;
	string id;
};

class Batton
{
public:
	Batton(Vector2i Size, Color Active)
	{
		this->Size = Size;
		ActiveColor = Active;

		Shape.setSize(Vector2f(this->Size.x, this->Size.y));
		Shape.setFillColor(Color(54, 49, 61));

		inviz = false;
	}

	Batton(Vector2i Size)
	{
		this->Size = Size;
		inviz = true;
	}

	void setPosition(Vector2i position)
	{
		Position = position;
		Shape.setPosition(Vector2f(this->Position.x, this->Position.y));
		Icon.setPosition(Vector2f(this->Position.x, this->Position.y));
	}

	Vector2i getPosition()
	{
		return Position;
	}

	bool active(Vector2i &mouse_windows)
	{
		if (!inviz)
		{
			Vector2i mousePositionBatton(mouse_windows.x - Position.x, mouse_windows.y - Position.y);
			if (mousePositionBatton.x > 0 && mousePositionBatton.x < Size.x && mousePositionBatton.y > 0 && mousePositionBatton.y < Size.y)
			{
				Shape.setFillColor(ActiveColor);
				return true;
			}
			else
			{
				Shape.setFillColor(Color(54, 49, 61));
				return false;
			}
		}
		else
		{
			Vector2i mousePositionBatton(mouse_windows.x - Position.x, mouse_windows.y - Position.y);
			if (mousePositionBatton.x > 0 && mousePositionBatton.x < Size.x && mousePositionBatton.y > 0 && mousePositionBatton.y < Size.y)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	void draw(RenderWindow &windows)
	{
		if (!inviz)
		{
			windows.draw(Shape);
		}
		windows.draw(Icon);
	}
private:
	Vector2i Size;
	Vector2i Position;
	RectangleShape Shape;
	Color ActiveColor;
	Sprite Icon;
	float K;
	bool inviz;
};

class TextTable
{
public:
	TextTable(Font font, Vector2f position, float size, float k, float MaxSize, bool up)
	{
		culon = 8;

		this->font = font;
		this->k = k;
		position.y = position.y + (culon * k);
		this->position = position;
		this->size = size;
		this->up = up;
		active_ = false;
		sizeTableFind = false;
		this->MaxSize = MaxSize;
	}

	TextTable(Font font, Vector2f position, Vector2i sizeTable, Vector2f positionTable, float size , float k, float MaxSize, bool up)
	{
		culon = 8;

		this->font = font;
		this->k = k;
		position.y = position.y + (culon * k);
		this->position = position;
		this->sizeTable = sizeTable;
		this->positionTable = positionTable;
		this->size = size;
		this->up = up;
		active_ = false;
		sizeTableFind = true;
		this->MaxSize = MaxSize;
	}

	void setString(String &str)
	{
		//528. 2 line
		Text text;
		text.setFont(font);
		text.setCharacterSize(this->size);
		this->str = "";
		String Buffer = "";
		for (int i = 0; i < str.getSize(); i++)
		{
			//QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
			this->str += str[i];
			Buffer += str[i];
			text.setString(Buffer);
#if textureArrow_
			if (text.getLocalBounds().width > 527)
			{
				if (i + 1 != str.getSize())
				{
					this->str += "\n";
					Buffer = "";
				}
			}
#else
			if (text.getGlobalBounds().width > MaxSize)
			{
				if (i + 1 != str.getSize())
				{
					this->str += '\n';
					Buffer = "";
				}
			}
#endif
		}
	}

	String getString()
	{
		return str;
	}

	int getLine()
	{
		int line = 0;

		for (int i = 0; i < str.getSize(); i++)
		{
			if (str[i] == '\n')
			{
				line++;
			}
		}
		return line;
	}

	void setPosition(Vector2f position)
	{
		this->position = position;
	}

	Vector2f getPositionTable()
	{
		return position;
	}

	Vector2f getPosition(Text& text)
	{
		text.setString(str);
		Vector2f PosText = position;
		if (up)
		{
			int line = getLine();

			if (line != 0)
			{
				PosText.y = position.y - text.getLocalBounds().height + 11;
			}
			else
			{
				PosText.y = position.y;
			}

		}
		return PosText;
	}

	void setMaxSize(float MaxSize)
	{
		this->MaxSize = MaxSize;
	}

	float getMaxSize()
	{
		return MaxSize;
	}

	bool active(Vector2i& mouse_windows)
	{
		if (sizeTableFind)
		{
			Vector2i mousePositionBatton(mouse_windows.x - positionTable.x, mouse_windows.y - positionTable.y);
			if (mousePositionBatton.x > 0 && mousePositionBatton.x < sizeTable.x && mousePositionBatton.y > 0 && mousePositionBatton.y < sizeTable.y)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		return active_;
	}

	void setActive(bool active)
	{
		this->active_ = active;
	}

	bool getActive()
	{
		return active_;
	}

	void draw(RenderWindow& windows, Text &text)
	{
		Vector2f PosText = position;
		text.setFont(font);
		text.setCharacterSize(size);
		text.setString(str);
		if (up)
		{
			int line = getLine();

			if (line != 0)
			{
				PosText.y = position.y - text.getLocalBounds().height + 11;
			}
			else
			{
				PosText.y = position.y;
			}
			
		}
		text.setPosition(PosText);
		windows.draw(text);
	}

	void drawUnco(RenderWindow& windows, Text& text)
	{
		Vector2f PosText = position;
		text.setFont(font);
		text.setCharacterSize(size);
		text.setString(str);
		if (up)
		{
			int line = getLine();

			if (line != 0)
			{
				PosText.y = position.y - text.getLocalBounds().height + 11;
			}
			else
			{
				PosText.y = position.y;
			}

		}
		text.setPosition(PosText);

		String str2 = "";
		for (int i = 0; i < str.getSize(); i++)
		{
			str2 += "*";
		}
		text.setString(str2);

		windows.draw(text);
	}

private:
	Vector2f position;
	Vector2i sizeTable;
	Font font;
	float size;
	String str;
	float k;
	float culon;
	bool up;
	bool active_;
	bool sizeTableFind;
	Vector2f positionTable;
	float MaxSize;
};

AuthorizationStruct *AS = new AuthorizationStruct;

User user;

WindowsActive windowsActive;

vector<Message> Messages;
vector<Avatar> avatars;

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

string convert_16bit_in_8bit(String str)
{
	string str2 = "";
	for (int l = 0; l < str.getSize(); l++)
	{
		int charStr = (int)str[l];
		int array[2] = { 0, 0 };
		int two = 7;
		int index = 0;
		for (int i = 15; i > -1; i--)
		{
			if (i == 7)
			{
				index++;
				two = 7;
			}
			if (charStr - pow(2, i) > 0 || charStr - pow(2, i) == 0)
			{
				array[index] += pow(2, two);
				charStr -= pow(2, i);
			}
			two--;
		}
		str2 += (char)array[0];
		str2 += (char)array[1];
	}
	return str2;
}

String convert_8bit_in_16bit(string str)
{
	String str2 = "";
	for (int l = 0; l < str.size();)
	{
		int intStr = 0;
		int two = 15;
		int array[2] = { (int)str[l], (int)str[l + 1] };
		for (int index = 0; index < 2; index++)
		{
			for (int i = 7; i > -1; i--)
			{
				if (array[index] - pow(2, i) > 0 || array[index] - pow(2, i) == 0)
				{
					intStr += pow(2, two);
					array[index] -= pow(2, i);
				}
				two--;
			}
		}
		l += 2;
		str2 += static_cast<char32_t>(intStr);
	}
	return str2;
}

bool LOADSTAK = false;

void AuthorizationUser(string login, string password)
{
	AS->login = login;
	AS->password = password;

	Packet msgtype = P_AuthorizationUser;
	send(Connection, (char*)& msgtype, sizeof(Packet), NULL);

	//send login
	Authorization ASType = A_Login;
	send(Connection, (char*)& ASType, sizeof(Authorization), NULL);

	int login_size = AS->login.size();
	send(Connection, (char*)& login_size, sizeof(int), NULL);
	send(Connection, AS->login.c_str(), login_size, NULL);
}

bool ProcessPacket(Packet packettype)
{
	switch (packettype)
	{
	case P_PVersion:
	{
		bool bo = true;
		recv(Connection, (char*)& bo, sizeof(bool), NULL);
		if (!bo)
		{
			exit(400);
		}
		else
		{
			windowsActive = W_Loading;
		}
		break;
	}
	case P_ChatMessage:
	{
		int msg_size;
		recv(Connection, (char*)& msg_size, sizeof(int), NULL);
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		recv(Connection, msg, msg_size, NULL);

		int id_size;
		recv(Connection, (char*)& id_size, sizeof(int), NULL);
		char* id = new char[id_size + 1];
		id[id_size] = '\0';
		recv(Connection, id, id_size, NULL);

		int name_size;
		recv(Connection, (char*)& name_size, sizeof(int), NULL);
		char* name = new char[name_size + 1];
		name[name_size] = '\0';
		recv(Connection, name, name_size, NULL);

		Message message;
		message.content = convert_8bit_in_16bit(msg);
		message.id = id;
		message.name = name;
		Messages.push_back(message);

		delete[] msg;
		delete[] id;
		break;
	}
	case P_AuthorizationUser:
	{
		Authorization ASType;
		recv(Connection, (char*)& ASType, sizeof(Authorization), NULL);

		if (ASType == A_Intcoder)
		{
			//recv intcoder
			recv(Connection, (char*)& AS->intcoder, sizeof(int), NULL);

			//send password
			AS->password = coder(AS->password, AS->intcoder);
			int password_size = AS->password.size();
			Packet msgtype = P_AuthorizationUser;
			send(Connection, (char*)& msgtype, sizeof(Packet), NULL);
			Authorization ASTypeSend = A_Password;
			send(Connection, (char*)& ASTypeSend, sizeof(Authorization), NULL);
			send(Connection, (char*)& password_size, sizeof(int), NULL);
			send(Connection, AS->password.c_str(), password_size, NULL);
		}
		if (ASType == A_AuthorizationEnd)
		{
			//recv Authorization
			recv(Connection, (char*)& user.Authorization, sizeof(bool), NULL);
			
			if (user.Authorization)
			{
				cout << "User " << AS->login << " authorization" << endl;

				int size;
				recv(Connection, (char*)& size, sizeof(int), NULL);
				char* date = new char[size + 1];
				date[size] = '\0';
				recv(Connection, date, size, NULL);
				user.id = date;

				delete[] date;

				recv(Connection, (char*)& size, sizeof(int), NULL);
				date = new char[size + 1];
				date[size] = '\0';
				recv(Connection, date, size, NULL);
				windowsActive = W_Chat;
				user.name = date;

				delete AS;
			}
			else 
			{
				cout << "User " << AS->login << " don't authorization" << endl;
				//AuthorizationUser();
				windowsActive = W_Authorization;
				user.ErrorPassword = true;
			}
		}
		break;
	}
	case P_File:
	{
		if (LOADSTAK)
		{
			LOADSTAK = false;
			bool is_open;
			//cout << "[--> 1" << endl;
			recv(Connection, (char*)& is_open, sizeof(bool), NULL);
			//cout << "[--> 2]" << endl;

			int puth_file_size;
			//cout << "[--> 3]" << endl;
			recv(Connection, (char*)& puth_file_size, sizeof(int), NULL);
			//cout << "[--> 4]" << endl;
			char* puth_file = new char[puth_file_size + 1];
			puth_file[puth_file_size] = '\0';
			//cout << "[--> 5]" << endl;
			recv(Connection, puth_file, puth_file_size, NULL);
			//cout << "[--> 6]" << endl;

			if (is_open)
			{
				int file_size;
				//cout << "[--> 7]" << endl;
				recv(Connection, (char*)& file_size, sizeof(int), NULL);
				//cout << "[--> 8]" << endl;
				char* file = new char[file_size + 1];
				file[file_size] = '\0';
				//cout << "[--> 9]" << endl;
				bool downloadFile = recv(Connection, file, file_size, NULL);
				//cout << "DLF : " << downloadFile << endl;

				string puth = resourse;
				puth += puth_file;
				ofstream Ofile;
				Ofile.open(puth, ofstream::binary);
				for (int i = 0; i < file_size; i++)
				{
					//cout << "[" << i << "] : " << puth << endl;
					Ofile << file[i];
				}
				cout << "load file : " << puth_file << endl;
			}
			else
			{
				cout << "don't load file : " << puth_file << endl;
			}
		}
		break;
	}

	default:
		windows[0] = "Server [error]";
		windows[1] = "Press \"esc\" close client";
		windowsOn = false;
		return false;
		//cout << "Unrecognized packet: " << packettype << endl;
		break;
	}
	return true;
}

void ClientHandler()
{
	Packet packettype;
	while (true)
	{
		if (windowsActive == w_Close)
		{
			break;
		}
		recv(Connection, (char*)&packettype, sizeof(Packet), NULL);

		if (!ProcessPacket(packettype))
		{
			break;
		}
	}
	if (windowsActive == W_Authorization)
	{
		windowsActive = W_ConnectAuthorization;
	}
	if (windowsActive == W_Chat)
	{
		cout << "-->";
		windowsActive = W_ConnectChat;
	}
	closesocket(Connection);
}

bool loadFile(string puth_file)
{
	if (!LOADSTAK)
	{
		string puth = resourse;
		puth += puth_file;
		ifstream file;
		file.open(puth);
		if (!file.is_open())
		{
			Packet msgtype = P_File;
			int puth_file_size = puth_file.size();
			LOADSTAK = true;
			send(Connection, (char*)& msgtype, sizeof(Packet), NULL);
			send(Connection, (char*)& puth_file_size, sizeof(int), NULL);
			send(Connection, puth_file.c_str(), puth_file_size, NULL);
			return false;
		}
		return true;
	}
	return false;
}

void ShapeRender(RenderWindow& window, Shape_Active active, float number, float size, float Width_shape, Vector2f position, Color backgraundColor, Color ShapeColor, float shift)
{
	if (active == S_AnNothing)
	{
		float Height_shape = sqrt(pow(size, 2) + pow(size, 2) - (2 * pow(size, 2) * (-0.5)));
		float Shift_Width_shapeX = Width_shape * cos(30 * (3.14 / 180));
		float Shift_Width_shapeY = Width_shape * sin(30 * (3.14 / 180));

		if (number == 1)
		{
			RectangleShape shape;
			shape.setFillColor(backgraundColor);
			shape.setSize(Vector2f(Height_shape, Width_shape));
			shape.setPosition(Vector2f(position.x - size + (Shift_Width_shapeX / 2), position.y - (size * 2) - (Shift_Width_shapeY / 2)));
			shape.setRotation(60);

			CircleShape cir;
			cir.setFillColor(backgraundColor);
			cir.setRadius(Width_shape / 2);
			cir.setPosition(Vector2f(position.x - size - cir.getRadius(), position.y - (size * 2) - cir.getRadius()));

			CircleShape cir2;
			cir2.setFillColor(backgraundColor);
			cir2.setRadius(Width_shape / 2);
			cir2.setPosition(Vector2f(position.x - size + (Height_shape / 2) - cir2.getRadius(), position.y - (size * 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180))) - cir2.getRadius())));

			window.draw(shape);
			window.draw(cir);
			window.draw(cir2);
		}
		if (number == 2)
		{
			RectangleShape shape2;
			shape2.setFillColor(backgraundColor);
			shape2.setSize(Vector2f(Height_shape, Width_shape));
			shape2.setPosition(Vector2f(position.x - size - (Height_shape / 2), position.y - (size * 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180)))) - (Width_shape / 2)));

			CircleShape cir2;
			cir2.setFillColor(backgraundColor);
			cir2.setRadius(Width_shape / 2);
			cir2.setPosition(Vector2f(position.x - size + (Height_shape / 2) - cir2.getRadius(), position.y - (size * 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180))) - cir2.getRadius())));

			CircleShape cir3;
			cir3.setFillColor(backgraundColor);
			cir3.setRadius(Width_shape / 2);
			cir3.setPosition(Vector2f(position.x - size - (Height_shape / 2) - cir3.getRadius(), position.y - (size * 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180)))) - cir3.getRadius()));

			window.draw(shape2);
			window.draw(cir2);
			window.draw(cir3);
		}
		if (number == 3)
		{
			RectangleShape shape3;
			shape3.setFillColor(backgraundColor);
			shape3.setSize(Vector2f(Height_shape, Width_shape));
			shape3.setPosition(Vector2f(position.x - size + (Shift_Width_shapeX / 2), position.y - (size * 2) + (Shift_Width_shapeY / 2)));
			shape3.setRotation(120);

			CircleShape cir;
			cir.setFillColor(backgraundColor);
			cir.setRadius(Width_shape / 2);
			cir.setPosition(Vector2f(position.x - size - cir.getRadius(), position.y - (size * 2) - cir.getRadius()));

			CircleShape cir3;
			cir3.setFillColor(backgraundColor);
			cir3.setRadius(Width_shape / 2);
			cir3.setPosition(Vector2f(position.x - size - (Height_shape / 2) - cir3.getRadius(), position.y - (size * 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180)))) - cir3.getRadius()));

			window.draw(shape3);
			window.draw(cir);
			window.draw(cir3);
		}
	}
	if (active == S_Nothing)
	{
		float Height_shape = sqrt(pow(size, 2) + pow(size, 2) - (2 * pow(size, 2) * (-0.5)));
		float Shift_Width_shapeX = Width_shape * cos(30 * (3.14 / 180));
		float Shift_Width_shapeY = Width_shape * sin(30 * (3.14 / 180));

		if (number == 1)
		{
			RectangleShape shape;
			shape.setFillColor(ShapeColor);
			shape.setSize(Vector2f(Height_shape, Width_shape));
			shape.setPosition(Vector2f(position.x - size + (Shift_Width_shapeX / 2), position.y - (size * 2) - (Shift_Width_shapeY / 2)));
			shape.setRotation(60);

			CircleShape cir;
			cir.setFillColor(ShapeColor);
			cir.setRadius(Width_shape / 2);
			cir.setPosition(Vector2f(position.x - size - cir.getRadius(), position.y - (size * 2) - cir.getRadius()));

			CircleShape cir2;
			cir2.setFillColor(ShapeColor);
			cir2.setRadius(Width_shape / 2);
			cir2.setPosition(Vector2f(position.x - size + (Height_shape / 2) - cir2.getRadius(), position.y - (size * 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180))) - cir2.getRadius())));

			window.draw(shape);
			window.draw(cir);
			window.draw(cir2);
		}
		if (number == 2)
		{
			RectangleShape shape2;
			shape2.setFillColor(ShapeColor);
			shape2.setSize(Vector2f(Height_shape, Width_shape));
			shape2.setPosition(Vector2f(position.x - size - (Height_shape / 2), position.y - (size * 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180)))) - (Width_shape / 2)));

			CircleShape cir2;
			cir2.setFillColor(ShapeColor);
			cir2.setRadius(Width_shape / 2);
			cir2.setPosition(Vector2f(position.x - size + (Height_shape / 2) - cir2.getRadius(), position.y - (size * 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180))) - cir2.getRadius())));

			CircleShape cir3;
			cir3.setFillColor(ShapeColor);
			cir3.setRadius(Width_shape / 2);
			cir3.setPosition(Vector2f(position.x - size - (Height_shape / 2) - cir3.getRadius(), position.y - (size * 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180)))) - cir3.getRadius()));

			window.draw(shape2);
			window.draw(cir2);
			window.draw(cir3);
		}
		if (number == 3)
		{
			RectangleShape shape3;
			shape3.setFillColor(ShapeColor);
			shape3.setSize(Vector2f(Height_shape, Width_shape));
			shape3.setPosition(Vector2f(position.x - size + (Shift_Width_shapeX / 2), position.y - (size * 2) + (Shift_Width_shapeY / 2)));
			shape3.setRotation(120);

			CircleShape cir;
			cir.setFillColor(ShapeColor);
			cir.setRadius(Width_shape / 2);
			cir.setPosition(Vector2f(position.x - size - cir.getRadius(), position.y - (size * 2) - cir.getRadius()));

			CircleShape cir3;
			cir3.setFillColor(ShapeColor);
			cir3.setRadius(Width_shape / 2);
			cir3.setPosition(Vector2f(position.x - size - (Height_shape / 2) - cir3.getRadius(), position.y - (size * 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180)))) - cir3.getRadius()));

			window.draw(shape3);
			window.draw(cir);
			window.draw(cir3);
		}
	}
	if (active == S_Animation)
	{
		float Height_shape = sqrt(pow(size, 2) + pow(size, 2) - (2 * pow(size, 2) * (-0.5)));
		float Shift_Width_shapeX = Width_shape * cos(30 * (3.14 / 180));
		float Shift_Width_shapeY = Width_shape * sin(30 * (3.14 / 180));

		if (number == 1)
		{
			RectangleShape shape;
			shape.setFillColor(ShapeColor);
			shape.setSize(Vector2f(shift, Width_shape));
			shape.setPosition(Vector2f(position.x - size + (Shift_Width_shapeX / 2), position.y - (size * 2) - (Shift_Width_shapeY / 2)));
			shape.setRotation(60);

			CircleShape cir;
			cir.setFillColor(ShapeColor);
			cir.setRadius(Width_shape / 2);
			cir.setPosition(Vector2f(position.x - size - cir.getRadius(), position.y - (size * 2) - cir.getRadius()));

			CircleShape cir2;
			cir2.setFillColor(ShapeColor);
			cir2.setRadius(Width_shape / 2);
			cir2.setPosition(Vector2f(position.x - size - cir.getRadius() + (shift / 2), position.y - (size * 2) - cir.getRadius() + (shift * cos(30 * (3.14 / 180)))));

			window.draw(shape);
			window.draw(cir);
			window.draw(cir2);
		}
		if (number == 2)
		{
			RectangleShape shape2;
			shape2.setFillColor(ShapeColor);
			shape2.setSize(Vector2f(shift, Width_shape));
			shape2.setPosition(Vector2f(position.x - size + (Height_shape / 2) - shift, position.y - (size * 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180)))) - (Width_shape / 2)));

			CircleShape cir2;
			cir2.setFillColor(ShapeColor);
			cir2.setRadius(Width_shape / 2);
			cir2.setPosition(Vector2f(position.x - size + (Height_shape / 2) - cir2.getRadius(), position.y - (size * 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180))) - cir2.getRadius())));

			CircleShape cir3;
			cir3.setFillColor(ShapeColor);
			cir3.setRadius(Width_shape / 2);
			cir3.setPosition(Vector2f(position.x - size + (Height_shape / 2) - cir3.getRadius() - shift, position.y - (size * 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180)))) - cir3.getRadius()));

			window.draw(shape2);
			window.draw(cir2);
			window.draw(cir3);
		}
		if (number == 3)
		{
			RectangleShape shape3;
			shape3.setFillColor(ShapeColor);
			shape3.setSize(Vector2f(shift, Width_shape));
			shape3.setPosition(Vector2f(position.x - size - (Height_shape / 2) + (Shift_Width_shapeX / 2) + (shift / 2), position.y - (size * 2) + (Shift_Width_shapeY / 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180)))) - (shift * cos(30 * (3.14 / 180)))));
			shape3.setRotation(120);

			CircleShape cir;
			cir.setFillColor(ShapeColor);
			cir.setRadius(Width_shape / 2);
			cir.setPosition(Vector2f(position.x - size - (Height_shape / 2) - cir.getRadius() + (shift / 2), position.y - (size * 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180)))) - cir.getRadius() - (shift * cos(30 * (3.14 / 180)))));

			CircleShape cir3;//
			cir3.setFillColor(ShapeColor);
			cir3.setRadius(Width_shape / 2);
			cir3.setPosition(Vector2f(position.x - size - (Height_shape / 2) - cir3.getRadius(), position.y - (size * 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180)))) - cir3.getRadius()));

			window.draw(shape3);
			window.draw(cir);
			window.draw(cir3);
		}
	}
	if (active == S_AnAnimation)
	{
		float Height_shape = sqrt(pow(size, 2) + pow(size, 2) - (2 * pow(size, 2) * (-0.5)));
		float Shift_Width_shapeX = Width_shape * cos(30 * (3.14 / 180));
		float Shift_Width_shapeY = Width_shape * sin(30 * (3.14 / 180));

		if (number == 1)
		{
			RectangleShape shape;
			shape.setFillColor(ShapeColor);
			shape.setSize(Vector2f(Height_shape- shift, Width_shape));
			shape.setPosition(Vector2f(position.x - size + (Shift_Width_shapeX / 2) + (shift / 2), position.y - (size * 2) - (Shift_Width_shapeY / 2) + (shift * cos(30 * (3.14 / 180)))));
			shape.setRotation(60);

			CircleShape cir;
			cir.setFillColor(ShapeColor);
			cir.setRadius(Width_shape / 2);
			cir.setPosition(Vector2f(position.x - size - cir.getRadius() + (shift / 2), position.y - (size * 2) - cir.getRadius() + (shift * cos(30 * (3.14 / 180)))));

			CircleShape cir2;
			cir2.setFillColor(ShapeColor);
			cir2.setRadius(Width_shape / 2);
			cir2.setPosition(Vector2f(position.x - size + (Height_shape / 2) - cir2.getRadius(), position.y - (size * 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180))) - cir2.getRadius())));

			window.draw(shape);
			window.draw(cir);
			window.draw(cir2);
		}
		if (number == 2)
		{
			RectangleShape shape2;
			shape2.setFillColor(ShapeColor);
			shape2.setSize(Vector2f(Height_shape - shift, Width_shape));
			shape2.setPosition(Vector2f(position.x - size - (Height_shape / 2), position.y - (size * 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180)))) - (Width_shape / 2)));

			CircleShape cir2;
			cir2.setFillColor(ShapeColor);
			cir2.setRadius(Width_shape / 2);
			cir2.setPosition(Vector2f(position.x - size + (Height_shape / 2) - cir2.getRadius() - shift, position.y - (size * 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180))) - cir2.getRadius())));

			CircleShape cir3;
			cir3.setFillColor(ShapeColor);
			cir3.setRadius(Width_shape / 2);
			cir3.setPosition(Vector2f(position.x - size - (Height_shape / 2) - cir3.getRadius(), position.y - (size * 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180)))) - cir3.getRadius()));

			window.draw(shape2);
			window.draw(cir2);
			window.draw(cir3);
		}
		if (number == 3)
		{
			RectangleShape shape3;
			shape3.setFillColor(ShapeColor);
			shape3.setSize(Vector2f(Height_shape - shift, Width_shape));
			shape3.setPosition(Vector2f(position.x - size + (Shift_Width_shapeX / 2), position.y - (size * 2) + (Shift_Width_shapeY / 2)));
			shape3.setRotation(120);

			CircleShape cir;
			cir.setFillColor(ShapeColor);
			cir.setRadius(Width_shape / 2);
			cir.setPosition(Vector2f(position.x - size - cir.getRadius(), position.y - (size * 2) - cir.getRadius()));

			CircleShape cir3;
			cir3.setFillColor(ShapeColor);
			cir3.setRadius(Width_shape / 2);
			cir3.setPosition(Vector2f(position.x - size - (Height_shape / 2) - cir3.getRadius() + (shift / 2), position.y - (size * 2) + ((Height_shape / 2) * (sin(60 * (3.14 / 180)) / cos(60 * (3.14 / 180)))) - cir3.getRadius() - (shift * cos(30 * (3.14 / 180)))));

			window.draw(shape3);
			window.draw(cir);
			window.draw(cir3);
		}
	}
}

float shift = 0;

String file;
bool typeAnimation = true;

void triangle(RenderWindow &window, Text& text, Text& text2, float k)
{
	float size = 100 * k;
	float Width_shape = 15 * k; 
	float Height_shape = sqrt(pow(size, 2) + pow(size, 2) - (2 * pow(size, 2) * (-0.5)));
	float Shift_Width_shapeX = Width_shape * cos(30 * (3.14 / 180));
	float Shift_Width_shapeY = Width_shape * sin(30 * (3.14 / 180));
	Vector2f position = Vector2f(456 * k, 300 * k);

	Color color = Color(54, 49, 61);

	/////////////////
	//CircleShape Cir_;
	//Cir_.setFillColor(Color(80, 80, 80));
	//Cir_.setRadius(size);
	//Cir_.setPosition(Vector2f(position.x - (size * 2), position.y - (size * 2)));
	/////////////////

	window.clear(Color(71, 61, 86));

	if (typeAnimation)
	{
		ShapeRender(window, S_Animation, 1, size, Width_shape, position, color, Color(152, 108, 214), shift);
		ShapeRender(window, S_Animation, 2, size, Width_shape, position, color, Color(152, 108, 214), shift);
		ShapeRender(window, S_Animation, 3, size, Width_shape, position, color, Color(152, 108, 214), shift);
	}
	else
	{
		ShapeRender(window, S_AnAnimation, 1, size, Width_shape, position, color, Color(152, 108, 214), shift);
		ShapeRender(window, S_AnAnimation, 2, size, Width_shape, position, color, Color(152, 108, 214), shift);
		ShapeRender(window, S_AnAnimation, 3, size, Width_shape, position, color, Color(152, 108, 214), shift);
	}

	window.draw(text);
	text2.setString(file);
	text2.setPosition(Vector2f(356 * k - (text.getLocalBounds().width / 2), 350 * k));
	window.draw(text2);
	window.display();

	shift += 5 * k;
	if (shift > Height_shape)
	{
		shift = 0;
		typeAnimation = !typeAnimation;
	}
}

void ProssesWindowsLoad()
{
	const float k = 1 / (2850.0 / VideoMode::getDesktopMode().width);

	Vector2i StartPressedShiftWindows;

	RenderWindow window(VideoMode(712 * k, 500 * k), "Global chat", Style::None, ContextSettings(0, 0, 8));

	bool MousePositionTable = false;
	bool WindowsShift = false;

	Mouse mouse;
	Vector2i mousePositionWindows;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				window.close();
				windowsActive = w_Close;
			}
			if (windowsActive != W_Loading)
			{
				window.close();
			}
		}

		mousePositionWindows = Vector2i(mouse.getPosition().x - window.getPosition().x, mouse.getPosition().y - window.getPosition().y);

		if (mousePositionWindows.x > 0 && mousePositionWindows.x < window.getSize().x && mousePositionWindows.y > 0 && mousePositionWindows.y < window.getSize().y)
		{
			MousePositionTable = true;
		}
		else
		{
			MousePositionTable = false;
		}

		if (MousePositionTable && Mouse::isButtonPressed(Mouse::Left) && !WindowsShift)
		{
			WindowsShift = true;
			StartPressedShiftWindows = mousePositionWindows;
		}

		if (WindowsShift && Mouse::isButtonPressed(Mouse::Left))
		{
			window.setPosition(Vector2i(mouse.getPosition().x - StartPressedShiftWindows.x, mouse.getPosition().y - StartPressedShiftWindows.y));
		}
		else
		{
			WindowsShift = false;
		}

		Font Font_normal;
		Font_normal.loadFromFile(resourse + "MYRIADPRO-REGULAR.otf");
		Font Font_bold;
		Font_bold.loadFromFile(resourse + "MYRIADPRO-BOLD.otf");
		Text text;
		text.setFont(Font_bold);
		text.setCharacterSize(25 * k);
		text.setString(L"Загрузка данных...");
		text.setPosition(Vector2f(356 * k - (text.getLocalBounds().width / 2), 300 * k));


		Text text2;
		text2.setFont(Font_bold);
		text2.setCharacterSize(25 * k);

		Sleep(5);

		triangle(window, text, text2, k);
	}
}

void ProssesWindows()
{
	while (windowsActive == W_Nothing)
	{

	}
	if (windowsActive == W_Loading)
	{
		bool load = true;
		Clock clock;
		clock.restart();
		while (load)
		{
			if (loadFile("MYRIADPRO-REGULAR.otf"))
			{
				if (loadFile("MYRIADPRO-BOLD.otf"))
				{
					load = false;
				}
			}
			if (clock.getElapsedTime().asSeconds() > 30)
			{
				system("Client.exe");
				system("pause");
				windowsActive = w_Close;
				exit(404);
			}
		}
		load = true;

		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ProssesWindowsLoad, NULL, NULL, NULL);

		while (load)
		{
			Sleep(100);
			file = "global-channel.png";
			if (loadFile("global-channel.png"))
			{
				Sleep(100);
				file = "user-date.png";
				if (loadFile("user-date.png"))
				{
					Sleep(100);
					file = "user-date-avatar.png";
					if (loadFile("user-date-avatar.png"))
					{
						Sleep(100);
						file = "PChat.png";
						if (loadFile("PChat.png"))
						{
							Sleep(100);
							file = "global-avatar.png";
							if (loadFile("global-avatar.png"))
							{
								Sleep(100);
								file = "backgraund.png";
								if (loadFile("backgraund.png"))
								{
									Sleep(100);
									file = "register-table.png";
									if (loadFile("register-table.png"))
									{
										Sleep(100);
										file = "register-table-active.png";
										if (loadFile("register-table-active.png"))
										{
											Sleep(100);
											file = "close.png";
											if (loadFile("close.png"))
											{
												Sleep(100);
												file = "logo.png";
												if (loadFile("logo.png"))
												{
													Sleep(100);
													file = "menu.png";
													if (loadFile("menu.png"))
													{
														Sleep(100);
														file = "search.png";
														if (loadFile("search.png"))
														{
															Sleep(100);
															file = "user.png";
															if (loadFile("user.png"))
															{
																Sleep(100);
																file = "user-active.png";
																if (loadFile("user-active.png"))
																{
																	Sleep(100);
																	file = "messages.png";
																	if (loadFile("messages.png"))
																	{
																		Sleep(100);
																		file = "messages-active.png";
																		if (loadFile("messages-active.png"))
																		{
																			Sleep(100);
																			file = "exit.png";
																			if (loadFile("exit.png"))
																			{
																				Sleep(100);
																				file = "searchBackgraund.png";
																				if (loadFile("searchBackgraund.png"))
																				{
																					load = false;
																					windowsActive = W_Authorization;
																				}
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if (windowsActive != W_Loading)
	{
		//вспомогательные переменные
		const float k = 1 / (2850.0 / VideoMode::getDesktopMode().width);

		bool MousePositionTable = false;
		bool WindowsShift = false;
		Vector2i StartPressedShiftWindows;
		Vector2f KResize(1, 1);
		Vector2f OldSizeWindows(1425 * k, 1000 * k);

		String BufferCin;

		Font Font_normal;
		Font_normal.loadFromFile(resourse + "MYRIADPRO-REGULAR.otf");
		Font Font_bold;
		Font_bold.loadFromFile(resourse + "MYRIADPRO-BOLD.otf");

		bool CinActive = true;
		Text text;

		TextTable textCin(Font_normal, Vector2f(350 * k, 922 * k), 25 * k, k, 1022 * k, true);

		Clock clock;
		Clock clockAnimation;

		TableActive TableActiveMenu;
		TableActiveMenu = T_Full;

		//////////////////////////////

		RenderWindow window(VideoMode(1425 * k, 1000 * k), "Global chat", Style::None, ContextSettings(0, 0, 8));
		window.setPosition(Vector2i(VideoMode::getDesktopMode().width / 4, VideoMode::getDesktopMode().height / 4.5));

		RectangleShape shape(Vector2f(300 * k, window.getSize().y));
		shape.setFillColor(Color(68, 62, 77));

		RectangleShape shape2(Vector2f(window.getSize().x, 50 * k));
		shape2.setFillColor(Color(54, 49, 61));

		RectangleShape shape3(Vector2f(window.getSize().x - (300 * k), 75 * k));
		shape3.setPosition(Vector2f(300 * k, 50 * k));
		shape3.setFillColor(Color(84, 82, 94));

		RectangleShape Table(Vector2f(0, 950 * k));
		Table.setPosition(Vector2f(300 * k, 50 * k));
		Table.setFillColor(Color(72, 70, 79));

		RectangleShape Table2(Vector2f(300 * k, 950 * k));
		Table2.setPosition(Vector2f(0, 50 * k));
		Table2.setFillColor(Color(68, 62, 77, 0));

		RectangleShape Table3(Vector2f(75 * k, 950 * k));
		Table3.setPosition(Vector2f(225 * k, 50 * k));
		Table3.setFillColor(Color(68, 62, 77));

		float TableShift = 0;

		/*RectangleShape shape4(Vector2f(475 * k, 75 * k));
		shape4.setPosition(Vector2f(0, 50 * k));
		shape4.setFillColor(Color(73, 71, 82));*/

		//~
		/*Texture textureGlobalChannel;
		textureGlobalChannel.loadFromFile(resourse + "global-channel.png");
		textureGlobalChannel.setSmooth(true);
		Sprite spriGlobalChannel;
		spriGlobalChannel.setTexture(textureGlobalChannel);
		spriGlobalChannel.setPosition(Vector2f(0, 125 * k));
		spriGlobalChannel.setScale(Vector2f(k, k));*/

		//~2
		RectangleShape shape6(Vector2f(70 * k, 70 * k));
		shape6.setPosition(Vector2f(15 * k, window.getSize().y - (85 * k)));
		shape6.setFillColor(Color(104, 95, 117));

		Texture textureUserDate;
		textureUserDate.loadFromFile(resourse + "user-date.png");
		//textureUserDate.setSmooth(true);
		Sprite spriUserDate;
		spriUserDate.setTexture(textureUserDate);
		spriUserDate.setPosition(Vector2f(0, window.getSize().y - (100 * k)));
		spriUserDate.setScale(Vector2f(k, k));

		Texture textureUserDateAvatar;
		textureUserDateAvatar.loadFromFile(resourse + "user-date-avatar.png");
		//textureUserDateAvatar.setSmooth(true);

		Texture texturePChat;
		texturePChat.loadFromFile(resourse + "PChat.png");
		texturePChat.setSmooth(true);
#if spriPChat_
		Sprite spriPChat;
		spriPChat.setTexture(texturePChat);
		spriPChat.setPosition(Vector2f(475 * k, window.getSize().y - (78 * k)));
		spriPChat.setScale(Vector2f(k, k));
#else
		RectangleShape spriPChat(Vector2f(1075 * k, 50 * k));
		spriPChat.setPosition(Vector2f(325 * k, window.getSize().y - (78 * k)));
		spriPChat.setFillColor(Color(92, 89, 99));
#endif
#if textureArrow_
		Texture textureArrow;
		textureArrow.loadFromFile("arrow.png");
		Sprite spriArrow;
		spriArrow.setTexture(textureArrow);
		spriArrow.setScale(Vector2f(k, k));
#endif

		Texture textureGlobalAvatar;
		textureGlobalAvatar.loadFromFile(resourse + "logo.png");
		textureGlobalAvatar.setSmooth(true);
		Sprite spriGlobalAvatar;
		spriGlobalAvatar.setTexture(textureGlobalAvatar);
		spriGlobalAvatar.setPosition(Vector2f(15 * k, 65 * k));
		spriGlobalAvatar.setScale(Vector2f(k, k));

		Texture textureBackgraund;
		textureBackgraund.loadFromFile(resourse + "backgraund.png");
		textureBackgraund.setSmooth(true);
		Sprite BackGraund;
		BackGraund.setTexture(textureBackgraund);
		BackGraund.setPosition(Vector2f(0, 0));
		BackGraund.setScale(Vector2f(k, k));

		Texture textureRegisterTable;
		textureRegisterTable.loadFromFile(resourse + "register-table.png");
		Sprite RegisterTable;
		RegisterTable.setTexture(textureRegisterTable);
		RegisterTable.setPosition(Vector2f(506 * k, 375 * k));
		RegisterTable.setScale(Vector2f(k, k));

		Texture textureRegisterTableActive;
		textureRegisterTableActive.loadFromFile(resourse + "register-table-active.png");
		Sprite RegisterTableActive;
		RegisterTableActive.setTexture(textureRegisterTableActive);
		RegisterTableActive.setPosition(Vector2f(506 * k, 375 * k));
		RegisterTableActive.setScale(Vector2f(k, k));

		Texture textureMenu;
		textureMenu.loadFromFile(resourse + "menu.png");
		textureMenu.setSmooth(true);
		Sprite Menu;
		Menu.setTexture(textureMenu);
		Menu.setPosition(Vector2f(237 * k, 65 * k));
		Menu.setScale(Vector2f(k, k));

		Texture textureSearch;
		textureSearch.loadFromFile(resourse + "search.png");
		textureSearch.setSmooth(true);
		Sprite Search;
		Search.setTexture(textureSearch);
		Search.setPosition(Vector2f(15 * k, 177 * k));
		Search.setScale(Vector2f(k, k));

		Texture textureUser;
		textureUser.loadFromFile(resourse + "user.png");
		textureUser.setSmooth(true);
		Sprite User;
		User.setTexture(textureUser);
		User.setPosition(Vector2f(15 * k, 252 * k));
		User.setScale(Vector2f(k, k));

		Texture textureUserActive;
		textureUserActive.loadFromFile(resourse + "user-active.png");
		textureUserActive.setSmooth(true);
		Sprite UserActive;
		UserActive.setTexture(textureUserActive);
		UserActive.setPosition(Vector2f(15 * k, 252 * k));
		UserActive.setScale(Vector2f(k, k));

		Texture textureMessages;
		textureMessages.loadFromFile(resourse + "messages.png");
		textureMessages.setSmooth(true);
		Sprite MessagesSpri;
		MessagesSpri.setTexture(textureMessages);
		MessagesSpri.setPosition(Vector2f(15 * k, 327 * k));
		MessagesSpri.setScale(Vector2f(k, k));

		Texture textureMessagesActive;
		textureMessagesActive.loadFromFile(resourse + "messages-active.png");
		textureMessagesActive.setSmooth(true);
		Sprite MessagesSpriActive;
		MessagesSpriActive.setTexture(textureMessagesActive);
		MessagesSpriActive.setPosition(Vector2f(15 * k, 327 * k));
		MessagesSpriActive.setScale(Vector2f(k, k));

		Texture textureExit;
		textureExit.loadFromFile(resourse + "exit.png");
		textureExit.setSmooth(true);
		Sprite Exit;
		Exit.setTexture(textureExit);
		Exit.setPosition(Vector2f(237 * k, 927 * k));
		Exit.setScale(Vector2f(k, k));

		Texture textureSearchBackgraund;
		textureSearchBackgraund.loadFromFile(resourse + "searchBackgraund.png");
		Sprite SearchBackgraund;
		SearchBackgraund.setTexture(textureSearchBackgraund);
		SearchBackgraund.setPosition(Vector2f(15 * k, 177 * k));
		SearchBackgraund.setScale(Vector2f(k, k));

		Mouse mouse;
		Vector2i mousePositionWindows;

		/////////////////////////////////////////////////////////////////////////
		Batton close(Vector2i(50 * k, 50 * k), Color(128, 55, 55));
		close.setPosition(Vector2i(1375 * k + 1, 0));

		Batton BattonUser(Vector2i(270 * k, 45 * k));
		BattonUser.setPosition(Vector2i(15 * k, 252 * k));

		Batton BattonMessages(Vector2i(270 * k, 45 * k));
		BattonMessages.setPosition(Vector2i(15 * k, 327 * k));

		Batton BattonMenu(Vector2i(45 * k, 45 * k));
		BattonMenu.setPosition(Vector2i(237 * k, 65 * k));

		bool bo = false;

		Texture textureClose;
		textureClose.loadFromFile(resourse + "close.png");
		textureClose.setSmooth(true);
		Sprite spriClose;
		spriClose.setTexture(textureClose);
		spriClose.setPosition(Vector2f(1375 * k + 1, 1));
		spriClose.setScale(Vector2f(k, k));

		TextTable textCinLogin(Font_normal, Vector2f(531 * k, 383 * k), Vector2i(400 * k, 50 * k), Vector2f(506 * k, 385 * k), 25 * k, k, 950 * k, false);
		textCinLogin.setActive(true);

		TextTable textCinPassword(Font_normal, Vector2f(531 * k, 458 * k), Vector2i(400 * k, 50 * k), Vector2f(506 * k, 450 * k), 25 * k, k, 950 * k, false);
		textCinPassword.setActive(false);

		while (window.isOpen())
		{
			bool backspase = false;

			Event event;
			while (window.pollEvent(event))
			{
				if (event.type == Event::Closed)
				{
					window.close();
					windowsActive = w_Close;
					//closesocket(Connection);
				}
				if (event.type == Event::TextEntered)
				{
					cout << event.key.code << endl;

					switch (event.key.code)
					{
					case 1:
						//block
						break;
					case 2:
						//block
						break;
					case 3:
						//block
						break;
					case 4:
						//block
						break;
					case 5:
						//block
						break;
					case 6:
						//block
						break;
					case 7:
						//block
						break;
					case 8:
						if (BufferCin.getSize() > 0)
							BufferCin = String::fromUtf32(BufferCin.begin(), BufferCin.end() - 1);
						break;
					case 9:
						//block
						break;
					case 10:
						//block
						break;
					case 11:
						//block
						break;
					case 12:
						//block
						break;
					case 13:
#if textureArrow_
						BufferCin += '\n';
#endif
						break;
					case 14:
						//block
						break;
					case 15:
						//block
						break;
					case 16:
						//block
						break;
					case 17:
						//block
						break;
					case 18:
						//block
						break;
					case 19:
						//block
						break;
					case 20:
						//block
						break;
					case 21:
						//block
						break;
					case 22:
						BufferCin += Clipboard::getString();
						break;
					case 23:
						//block
						break;
					case 24:
						//block
						break;
					case 25:
						//block
						break;
					case 26:
						//block
						break;
					case 27:
						//block
						break;
					case 28:
						//block
						break;
					case 29:
						//block
						break;
					default:
						if (event.key.code < 2000) {
							BufferCin += static_cast<char32_t>(event.key.code);
						}
						else {
							// Time to consider sf::String or some other unicode-capable string
						}
						break;
					}

					if (event.key.code == 8)
					{
						backspase = true;
					}
				}
			}

			if (event.key.code != 8)
			{
				backspase = false;
			}

			mousePositionWindows = Vector2i(mouse.getPosition().x - window.getPosition().x, mouse.getPosition().y - window.getPosition().y);

			if (mousePositionWindows.x > 0 && mousePositionWindows.x < 1425 * k && mousePositionWindows.y > 0 && mousePositionWindows.y < 50 * k)
			{
				MousePositionTable = true;
			}
			else
			{
				MousePositionTable = false;
			}

			if (Keyboard::isKeyPressed(Keyboard::Escape) || (close.active(mousePositionWindows) && Mouse::isButtonPressed(Mouse::Left)))
			{
				window.close();
				windowsActive = w_Close;
				closesocket(Connection);
				break;
			}

			if (MousePositionTable && Mouse::isButtonPressed(Mouse::Left) && !WindowsShift)
			{
				WindowsShift = true;
				StartPressedShiftWindows = mousePositionWindows;
			}

			if (WindowsShift && Mouse::isButtonPressed(Mouse::Left))
			{
				window.setPosition(Vector2i(mouse.getPosition().x - StartPressedShiftWindows.x, mouse.getPosition().y - StartPressedShiftWindows.y));
			}
			else
			{
				WindowsShift = false;
			}

			if (windowsActive == W_Authorization)
			{
				if (Mouse::isButtonPressed(Mouse::Left))
				{
					if (textCinLogin.active(mousePositionWindows))
					{
						BufferCin = "";
						textCinLogin.setActive(true);
					}
					else
					{
						textCinLogin.setActive(false);
					}

					if (textCinPassword.active(mousePositionWindows))
					{
						BufferCin = "";
						textCinPassword.setActive(true);
					}
					else
					{
						textCinPassword.setActive(false);
					}
				}


				window.clear(Color(72, 70, 79));
				window.draw(BackGraund);
				if ((mousePositionWindows.x - 506 * k) > 0 && (mousePositionWindows.x - 506 * k) < 400 * k && (mousePositionWindows.y - 525 * k) > 0 && (mousePositionWindows.y - 525 * k) < 50 * k)
				{
					window.draw(RegisterTableActive);
					if (Mouse::isButtonPressed(Mouse::Left))
					{
						if (textCinLogin.getString() != "" && textCinPassword.getString() != "")
						{
							string login = textCinLogin.getString();
							string password = textCinPassword.getString();
							AuthorizationUser(login, password);
							String str = "";
							textCinPassword.setString(str);
						}
					}
				}
				else
				{
					window.draw(RegisterTable);
				}
				window.draw(shape2);
				close.draw(window);
				window.draw(spriClose);

				if (textCinLogin.getActive())
				{
					String str = textCinLogin.getString();
					if (backspase && str.getSize() != 0)
					{
						str = String::fromUtf32(str.begin(), str.end() - 1);
					}
					str += BufferCin;
					BufferCin = "";
					textCinLogin.setString(str);
				}
				textCinLogin.draw(window, text);

				if (textCinPassword.getActive())
				{
					String str = textCinPassword.getString();
					if (backspase && str.getSize() != 0)
					{
						str = String::fromUtf32(str.begin(), str.end() - 1);
					}
					str += BufferCin;
					BufferCin = "";
					textCinPassword.setString(str);
				}
				textCinPassword.drawUnco(window, text);

				text.setString(L"Авторизация");
				text.setPosition(Vector2f(706 * k - (text.getGlobalBounds().width / 2), 335 * k));
				window.draw(text);

				Text text2;
				text2.setString(L"Логин");
				text2.setFont(Font_normal);
				text2.setPosition(Vector2f(531 * k, 378 * k));
				text2.setCharacterSize(15 * k);
				text2.setFillColor(Color(200, 200, 200));
				window.draw(text2);

				text2.setString(L"Пароль");
				text2.setFont(Font_normal);
				text2.setPosition(Vector2f(531 * k, 453 * k));
				text2.setCharacterSize(15 * k);
				text2.setFillColor(Color(200, 200, 200));
				window.draw(text2);

				text2.setString(L"Вход");
				text2.setFont(Font_normal);
				text2.setCharacterSize(25 * k);
				text2.setPosition(Vector2f(706 * k - (text2.getGlobalBounds().width / 2), 533 * k));
				text2.setFillColor(Color(255, 255, 255));
				window.draw(text2);

				if (user.ErrorPassword)
				{
					text2.setString(L"Не верный логин или пароль");
					text2.setFont(Font_normal);
					text2.setCharacterSize(15 * k);
					text2.setPosition(Vector2f(706 * k - (text2.getGlobalBounds().width / 2), 358 * k));
					text2.setFillColor(Color(255, 100, 100));
					window.draw(text2);
				}

				window.display();
			}

			if (windowsActive == W_Chat || windowsActive == W_ConnectChat)
			{
				Texture textureUserAvatar;
				loadFile(user.id + ".png");
				textureUserAvatar.loadFromFile(resourse + user.id + ".png");
				textureUserAvatar.setSmooth(true);
				Sprite spriUserAvatar;
				spriUserAvatar.setTexture(textureUserAvatar);
				spriUserAvatar.setPosition(Vector2f(15 * k, window.getSize().y - (85 * k)));
				spriUserAvatar.setScale(Vector2f((70.0 / textureUserAvatar.getSize().x) * k, (70.0 / textureUserAvatar.getSize().y) * k));

				if (CinActive)
				{
					if (Keyboard::isKeyPressed(Keyboard::Enter) && windowsActive == W_Chat)
					{
						if (BufferCin != "")
						{
							Message message;
							message.content = BufferCin;
							message.id = user.id;
							message.name = user.name;
							Messages.push_back(message);

							string msg = convert_16bit_in_8bit(BufferCin);

							int msg_size = msg.size();
							Packet msgtype = P_ChatMessage;
							send(Connection, (char*)& msgtype, sizeof(Packet), NULL);
							send(Connection, (char*)& msg_size, sizeof(int), NULL);
							send(Connection, msg.c_str(), msg_size, NULL);

							int id_size = user.id.size();
							send(Connection, (char*)& id_size, sizeof(int), NULL);
							send(Connection, user.id.c_str(), id_size, NULL);

							int name_size = user.name.size();
							send(Connection, (char*)& name_size, sizeof(int), NULL);
							send(Connection, user.name.c_str(), name_size, NULL);

							BufferCin = "";
						}
					}

					textCin.setString(BufferCin);
					if (TableActiveMenu == T_Full)
					{
						spriPChat.setPosition(Vector2f(325 * k, textCin.getPosition(text).y - 8));
						spriPChat.setSize(Vector2f(1075 * k, (972 * k) - spriPChat.getPosition().y));
					}
					if (TableActiveMenu == T_Shift || TableActiveMenu == T_AnShift)
					{
						spriPChat.setPosition(Vector2f(spriPChat.getPosition().x, textCin.getPosition(text).y - 8));
						spriPChat.setSize(Vector2f(spriPChat.getSize().x, (972 * k) - spriPChat.getPosition().y));
					}
					if (TableActiveMenu == T_Smoll)
					{
						spriPChat.setPosition(Vector2f(100 * k, textCin.getPosition(text).y - 8));
						spriPChat.setSize(Vector2f(1300 * k, (972 * k) - spriPChat.getPosition().y));
					}
#if textureArrow_
					if (BufferCin != "")
					{
						spriPChat.setSize(Vector2f(825 * k, (972 * k) - spriPChat.getPosition().y));
						spriArrow.setPosition(Vector2f(1350 * k, spriPChat.getPosition().y + (spriPChat.getSize().y / 2) - (25 * k)));
					}
					else
					{
						spriPChat.setSize(Vector2f(900 * k, (972 * k) - spriPChat.getPosition().y));
					}
#else
					//spriPChat.setSize(Vector2f(1075 * k, (972 * k) - spriPChat.getPosition().y));
#endif 

				}

				window.clear(Color(72, 70, 79));

				//интерфейс/////////////////////
				window.draw(shape);
				window.draw(shape2);
				//window.draw(shape4);

				//window.draw(spriGlobalChannel);
				//if (TableActiveMenu == T_Full)
				//{
				window.draw(shape6);
				window.draw(spriGlobalAvatar);
				window.draw(spriUserAvatar);
				window.draw(spriUserDate);
				//}
				if (TableActiveMenu != T_Smoll)
				{
					window.draw(SearchBackgraund);
				}

				close.draw(window);
				window.draw(spriClose);

				if (BattonMenu.active(mousePositionWindows) && TableActiveMenu == T_Full && Mouse::isButtonPressed(Mouse::Left))
				{
					TableActiveMenu = T_Shift;
				}

				if (BattonMenu.active(mousePositionWindows) && TableActiveMenu == T_Smoll && Mouse::isButtonPressed(Mouse::Left))
				{
					TableActiveMenu = T_AnShift;
				}
				////////////////////////////
				if (TableActiveMenu == T_Full)
				{
					window.draw(Menu);
					window.draw(Search);
					if (!BattonUser.active(mousePositionWindows) || TableActiveMenu != T_Full)
					{
						window.draw(User);
					}
					else
					{
						window.draw(UserActive);
					}
					if (!BattonMessages.active(mousePositionWindows) || TableActiveMenu != T_Full)
					{
						window.draw(MessagesSpri);
					}
					else
					{
						window.draw(MessagesSpriActive);
					}
					window.draw(Exit);
				}
				////////////////////////////
				//if (TableActiveMenu == T_Full)
				//{
				text.setString(user.name);
				text.setPosition(Vector2f(100 * k, 920 * k));
				window.draw(text);

				text.setString("Online");
				text.setPosition(Vector2f(100 * k, 955 * k));
				window.draw(text);

				text.setString("Indoris-Scai");
				text.setPosition(Vector2f(90 * k, 70 * k));
				window.draw(text);
				//}

				text.setString("Search...");
				text.setPosition(Vector2f(90 * k, 182 * k));
				window.draw(text);

				if (!BattonUser.active(mousePositionWindows))
				{
					text.setColor(Color(255, 255, 255));
					text.setString("User");
					text.setPosition(Vector2f(90 * k, 257 * k));
					window.draw(text);
				}
				else
				{
					text.setColor(Color(68, 62, 77));
					text.setString("User");
					text.setPosition(Vector2f(90 * k, 257 * k));
					window.draw(text);
					text.setColor(Color(255, 255, 255));
				}
				if (!BattonMessages.active(mousePositionWindows))
				{
					text.setColor(Color(255, 255, 255));
					text.setString("Messages");
					text.setPosition(Vector2f(90 * k, 332 * k));
					window.draw(text);
				}
				else
				{
					text.setColor(Color(68, 62, 77));
					text.setString("Messages");
					text.setPosition(Vector2f(90 * k, 332 * k));
					window.draw(text);
					text.setColor(Color(255, 255, 255));
				}

				window.draw(Table2);
				////////////////////////////////
				/*if (TableActiveMenu == T_Shift || TableActiveMenu == T_AnShift)
				{
					window.draw(Table3);
				}*/
				////////////////////////////////
				if (TableActiveMenu != T_Full)
				{
					window.draw(Menu);
					window.draw(Search);
					if (!BattonUser.active(mousePositionWindows) || TableActiveMenu != T_Full)
					{
						window.draw(User);
					}
					else
					{
						window.draw(UserActive);
					}
					if (!BattonMessages.active(mousePositionWindows) || TableActiveMenu != T_Full)
					{
						window.draw(MessagesSpri);
					}
					else
					{
						window.draw(MessagesSpriActive);
					}
					window.draw(Exit);
				}
				////////////////////////////////
				window.draw(Table);
				window.draw(shape3);
				window.draw(spriPChat);

				text.setString("Global channel");
				text.setPosition(Vector2f(shape3.getPosition().x + (25 * k), 70 * k));
				window.draw(text);
				////////////////////////////////

				if (Messages.size() != 0)
				{
					Vector2f StartPositionMessage(325 * k - TableShift, 847 * k);
					int z = 0;
					for (int i = Messages.size() - 1; i > -1; i--)
					{
						if (StartPositionMessage.y > 0)
						{
							Text text3;
							TextTable textMessage(Font_normal, Vector2f(StartPositionMessage.x + (75 * k), StartPositionMessage.y + (25 * k)), 25 * k, k, 947 * k + TableShift, false);
							textMessage.setString(Messages[i].content);

							int line = textMessage.getLine();

							if (line != 0)
							{
								StartPositionMessage.y -= ((25 * k) + text3.getGlobalBounds().height);
								textMessage.setPosition(Vector2f(StartPositionMessage.x + (75 * k), StartPositionMessage.y + (25 * k)));
							}

							textMessage.draw(window, text3);

							if (Messages[i].id == user.id)
							{
								Sprite MessageAuthorAvatar;
								MessageAuthorAvatar.setTexture(textureUserAvatar);
								MessageAuthorAvatar.setPosition(Vector2f(StartPositionMessage.x, StartPositionMessage.y));
								MessageAuthorAvatar.setScale(Vector2f((50.0 / textureUserAvatar.getSize().x) * k, (50.0 / textureUserAvatar.getSize().y) * k));
								window.draw(MessageAuthorAvatar);
							}
							else
							{
								bool find = false;
								for (int l = 0; l < avatars.size(); l++)
								{
									if (avatars[l].id == Messages[i].id)
									{
										Sprite MessageAuthorAvatar;
										MessageAuthorAvatar.setTexture(avatars[l].texture);
										MessageAuthorAvatar.setPosition(Vector2f(StartPositionMessage.x, StartPositionMessage.y));
										MessageAuthorAvatar.setScale(Vector2f((50.0 / avatars[l].texture.getSize().x) * k, (50.0 / avatars[l].texture.getSize().y) * k));
										window.draw(MessageAuthorAvatar);

										l = avatars.size();
										find = true;
									}
								}
								if (!find)
								{
									if (loadFile(Messages[i].id + ".png"))
									{
										Texture avatar;
										avatar.loadFromFile(resourse + Messages[i].id + ".png");
										avatar.setSmooth(true);
										Avatar avar;
										avar.texture = avatar;
										avar.id = Messages[i].id;
										avatars.push_back(avar);
									}
								}
							}

							Sprite MessageAuthorAvatarDate;
							MessageAuthorAvatarDate.setTexture(textureUserDateAvatar);
							MessageAuthorAvatarDate.setPosition(Vector2f(StartPositionMessage.x, StartPositionMessage.y));
							MessageAuthorAvatarDate.setScale(Vector2f(k, k));
							window.draw(MessageAuthorAvatarDate);

							Text text2;
							text2.setFont(Font_bold);
							text2.setString(Messages[i].name);
							text2.setPosition(Vector2f(StartPositionMessage.x + (75 * k), StartPositionMessage.y));
							text2.setCharacterSize(25 * k);
							window.draw(text2);

							if (line != 0)
							{
								StartPositionMessage.y -= ((50 * k) + text3.getGlobalBounds().height);
							}
							else
							{
								StartPositionMessage.y -= ((75 * k) + text3.getGlobalBounds().height);
							}
						}
					}
				}

				////////////////////////////////
				textCin.setPosition(Vector2f(350 * k - TableShift, textCin.getPositionTable().y));
				textCin.setMaxSize(1022 * k + TableShift);
				textCin.draw(window, text);
#if textureArrow_
				if (textCin.getString() != "")
				{
					window.draw(spriArrow);
				}
#endif

				float time = clock.getElapsedTime().asMicroseconds() / 1000;

				if (time < 500)
				{
					RectangleShape sha(Vector2f(1, 25 * k));
					if (text.getGlobalBounds().width == 0)
					{
						sha.setPosition(Vector2f(350 * k + text.getGlobalBounds().width - TableShift, 930 * k));
					}
					else
					{
						sha.setPosition(Vector2f(355 * k + text.getGlobalBounds().width - TableShift, 930 * k));
					}
					window.draw(sha);
				}
				else
				{
					if (time > 1000)
					{
						clock.restart();
					}
				}
				////////////////////////////

				if (clockAnimation.getElapsedTime().asMilliseconds() > 5 && TableActiveMenu == T_Shift)
				{
					if (Table.getSize().x < 225 * k)
					{
						TableShift += 5 * k;

						Table.setSize(Vector2f(Table.getSize().x + 5 * k, Table.getSize().y));
						Table.setPosition(Vector2f(Table.getPosition().x - 5 * k, Table.getPosition().y));

						Table2.setSize(Vector2f(Table2.getSize().x - 5 * k, Table2.getSize().y));
						if (Table2.getFillColor().a != 255)
						{
							Table2.setFillColor(Color(Table2.getFillColor().r, Table2.getFillColor().g, Table2.getFillColor().b, (int)Table2.getFillColor().a + (255 / 45)));
						}

						Menu.setPosition(Vector2f(Menu.getPosition().x - 5 * k, Menu.getPosition().y));
						Exit.setPosition(Vector2f(Exit.getPosition().x - 5 * k, Exit.getPosition().y));

						shape3.setSize(Vector2f(shape3.getSize().x + 5 * k, shape3.getSize().y));
						shape3.setPosition(Vector2f(shape3.getPosition().x - 5 * k, shape3.getPosition().y));

						spriPChat.setSize(Vector2f(spriPChat.getSize().x + 5 * k, spriPChat.getSize().y));
						spriPChat.setPosition(Vector2f(spriPChat.getPosition().x - 5 * k, spriPChat.getPosition().y));

						Table3.setPosition(Vector2f(Table3.getPosition().x - 5 * k, Table3.getPosition().y));
					}
					else
					{
						TableActiveMenu = T_Smoll;
						BattonMenu.setPosition(Vector2i(15 * k, 65 * k));
						TableShift = 225 * k;
						Table2.setFillColor(Color(Table2.getFillColor().r, Table2.getFillColor().g, Table2.getFillColor().b, 255));
					}
					clockAnimation.restart();
				}

				if (clockAnimation.getElapsedTime().asMilliseconds() > 5 && TableActiveMenu == T_AnShift)
				{
					if (Table.getSize().x > 0)
					{
						TableShift -= 5 * k;
						Table.setSize(Vector2f(Table.getSize().x - 5 * k, Table.getSize().y));
						Table.setPosition(Vector2f(Table.getPosition().x + 5 * k, Table.getPosition().y));

						Table2.setSize(Vector2f(Table2.getSize().x + 5 * k, Table2.getSize().y));
						if (Table2.getFillColor().a != 0)
						{
							Table2.setFillColor(Color(Table2.getFillColor().r, Table2.getFillColor().g, Table2.getFillColor().b, (int)Table2.getFillColor().a - (255 / 45)));
						}

						Menu.setPosition(Vector2f(Menu.getPosition().x + 5 * k, Menu.getPosition().y));
						BattonMenu.setPosition(Vector2i(BattonMenu.getPosition().x + 5 * k, BattonMenu.getPosition().y));
						Exit.setPosition(Vector2f(Exit.getPosition().x + 5 * k, Exit.getPosition().y));

						shape3.setSize(Vector2f(shape3.getSize().x - 5 * k, shape3.getSize().y));
						shape3.setPosition(Vector2f(shape3.getPosition().x + 5 * k, shape3.getPosition().y));

						spriPChat.setSize(Vector2f(spriPChat.getSize().x - 5 * k, spriPChat.getSize().y));
						spriPChat.setPosition(Vector2f(spriPChat.getPosition().x + 5 * k, spriPChat.getPosition().y));

						Table3.setPosition(Vector2f(Table3.getPosition().x + 5 * k, Table3.getPosition().y));
					}
					else
					{
						TableActiveMenu = T_Full;
						BattonMenu.setPosition(Vector2i(237 * k, 65 * k));
						TableShift = 0;
						Table2.setFillColor(Color(Table2.getFillColor().r, Table2.getFillColor().g, Table2.getFillColor().b, 0));
					}
					clockAnimation.restart();
				}

				window.display();
			}
		}
#if Chat
		while (window.isOpen())
		{
			Event event;
			while (window.pollEvent(event))
			{
				if (event.type == Event::Closed)
					window.close();
				if (event.type == Event::TextEntered)
				{
					cout << event.key.code << endl;

					switch (event.key.code)
					{
					case 1:
						//block
						break;
					case 2:
						//block
						break;
					case 3:
						//block
						break;
					case 4:
						//block
						break;
					case 5:
						//block
						break;
					case 6:
						//block
						break;
					case 7:
						//block
						break;
					case 8:
						if (BufferCin.getSize() > 0)
							BufferCin = String::fromUtf32(BufferCin.begin(), BufferCin.end() - 1);
						break;
					case 9:
						//block
						break;
					case 10:
						//block
						break;
					case 11:
						//block
						break;
					case 12:
						//block
						break;
					case 13:
#if textureArrow_
						BufferCin += '\n';
#endif
						break;
					case 14:
						//block
						break;
					case 15:
						//block
						break;
					case 16:
						//block
						break;
					case 17:
						//block
						break;
					case 18:
						//block
						break;
					case 19:
						//block
						break;
					case 20:
						//block
						break;
					case 21:
						//block
						break;
					case 22:
						BufferCin += Clipboard::getString();
						break;
					case 23:
						//block
						break;
					case 24:
						//block
						break;
					case 25:
						//block
						break;
					case 26:
						//block
						break;
					case 27:
						//block
						break;
					case 28:
						//block
						break;
					case 29:
						//block
						break;
					default:
						if (event.key.code < 2000) {
							BufferCin += static_cast<char32_t>(event.key.code);
						}
						else {
							// Time to consider sf::String or some other unicode-capable string
						}
						break;
					}
				}
			}

			mousePositionWindows = Vector2i(mouse.getPosition().x - window.getPosition().x, mouse.getPosition().y - window.getPosition().y);

			if (mousePositionWindows.x > 0 && mousePositionWindows.x < 1425 * k && mousePositionWindows.y > 0 && mousePositionWindows.y < 50 * k)
			{
				MousePositionTable = true;
			}
			else
			{
				MousePositionTable = false;
			}

			if (Keyboard::isKeyPressed(Keyboard::Escape) || (close.active(mousePositionWindows) && Mouse::isButtonPressed(Mouse::Left)))
			{
				window.close();
				break;
			}

			if (MousePositionTable && Mouse::isButtonPressed(Mouse::Left) && !WindowsShift)
			{
				WindowsShift = true;
				StartPressedShiftWindows = mousePositionWindows;
			}

			if (WindowsShift && Mouse::isButtonPressed(Mouse::Left))
			{
				window.setPosition(Vector2i(mouse.getPosition().x - StartPressedShiftWindows.x, mouse.getPosition().y - StartPressedShiftWindows.y));
			}
			else
			{
				WindowsShift = false;
			}

			/*if (bo)
			{
				window.setSize(Vector2u(window.getSize().x + 100, window.getSize().y));

				KResize.x = (float)OldSizeWindows.x / (OldSizeWindows.x + 100);

				shape.setScale(Vector2f(0.9056603773584905660377358490566, KResize.y));
			}
			else
			{
				window.setSize(Vector2u(window.getSize().x - 100, window.getSize().y));
				KResize.x = 1;
				shape.setScale(Vector2f(KResize.x, KResize.y));
				//window.setView = VideoMode(1425 * k, window.getSize().y);
				//shape.setSize(Vector2f(475, window.getSize().y));
			}

			bo = !bo;


			*/

			//string str = to_string(mousePositionWindows.x) + ";" + to_string(mousePositionWindows.y) + "\n" + to_string(close.active(mousePositionWindows));

			if (CinActive)
			{
				if (Keyboard::isKeyPressed(Keyboard::Enter))
				{
					if (BufferCin != "")
					{
						Message message;
						message.content = BufferCin;
						Messages.push_back(message);
						BufferCin = "";
					}
				}

				textCin.setString(BufferCin);
				spriPChat.setPosition(Vector2f(500 * k, textCin.getPosition(text).y - 8));
#if textureArrow_
				if (BufferCin != "")
				{
					spriPChat.setSize(Vector2f(825 * k, (972 * k) - spriPChat.getPosition().y));
					spriArrow.setPosition(Vector2f(1350 * k, spriPChat.getPosition().y + (spriPChat.getSize().y / 2) - (25 * k)));
				}
				else
				{
					spriPChat.setSize(Vector2f(900 * k, (972 * k) - spriPChat.getPosition().y));
				}
#else
				spriPChat.setSize(Vector2f(900 * k, (972 * k) - spriPChat.getPosition().y));
#endif 

			}

			window.clear(Color(72, 70, 79));

			//интерфейс/////////////////////
			window.draw(shape);
			window.draw(shape2);
			window.draw(shape3);
			window.draw(shape4);
			window.draw(shape6);

			window.draw(spriGlobalChannel);
			window.draw(spriGlobalAvatar);
			window.draw(spriUserAvatar);
			window.draw(spriUserDate);
			window.draw(spriPChat);

			close.draw(window);
			window.draw(spriClose);
			////////////////////////////////
			textCin.draw(window, text);
#if textureArrow_
			if (textCin.getString() != "")
			{
				window.draw(spriArrow);
			}
#endif

			float time = clock.getElapsedTime().asMicroseconds() / 1000;

			if (time < 500)
			{
				RectangleShape sha(Vector2f(1, 25 * k));
				if (text.getGlobalBounds().width == 0)
				{
					sha.setPosition(Vector2f(525 * k + text.getGlobalBounds().width, 930 * k));
				}
				else
				{
					sha.setPosition(Vector2f(530 * k + text.getGlobalBounds().width, 930 * k));
				}
				window.draw(sha);
			}
			else
			{
				if (time > 1000)
				{
					clock.restart();
				}
			}
			////////////////////////////////
			text.setString("FunTemis");
			text.setPosition(Vector2f(142 * k, 899 * k));
			window.draw(text);

			text.setString("Online");
			text.setPosition(Vector2f(142 * k, 939 * k));
			window.draw(text);

			text.setString("Global channel");
			text.setPosition(Vector2f(142 * k, 188 * k));
			window.draw(text);

			text.setString("Servel online");
			text.setPosition(Vector2f((237.5 * k) - (text.getLocalBounds().width / 2), 75 * k));
			window.draw(text);

			text.setString("Global channel");
			text.setPosition(Vector2f(500 * k, 75 * k));
			window.draw(text);

			if (Messages.size() != 0)
			{
				Vector2f StartPositionMessage(525 * k, 847 * k);
				for (int i = 0; i < Messages.size(); i++)
				{
					Text text3;
					TextTable textMessage(Font_normal, Vector2f(StartPositionMessage.x + (75 * k), StartPositionMessage.y + (25 * k)), 25 * k, k, false);
					textMessage.setString(Messages[i].content);

					int line = textMessage.getLine();

					if (line != 0)
					{
						StartPositionMessage.y -= ((25 * k) + text3.getGlobalBounds().height);
						textMessage.setPosition(Vector2f(StartPositionMessage.x + (75 * k), StartPositionMessage.y + (25 * k)));
					}

					textMessage.draw(window, text3);

					Sprite MessageAuthorAvatar;
					MessageAuthorAvatar.setTexture(textureUserAvatar);
					MessageAuthorAvatar.setPosition(Vector2f(StartPositionMessage.x, StartPositionMessage.y));
					MessageAuthorAvatar.setScale(Vector2f((50.0 / textureUserAvatar.getSize().x) * k, (50.0 / textureUserAvatar.getSize().y) * k));
					window.draw(MessageAuthorAvatar);

					Sprite MessageAuthorAvatarDate;
					MessageAuthorAvatarDate.setTexture(textureUserDateAvatar);
					MessageAuthorAvatarDate.setPosition(Vector2f(StartPositionMessage.x, StartPositionMessage.y));
					MessageAuthorAvatarDate.setScale(Vector2f(k, k));
					window.draw(MessageAuthorAvatarDate);

					Text text2;
					text2.setFont(Font_bold);
					text2.setString("FunTemis");
					text2.setPosition(Vector2f(StartPositionMessage.x + (75 * k), StartPositionMessage.y));
					text2.setCharacterSize(25 * k);
					window.draw(text2);

					if (line != 0)
					{
						StartPositionMessage.y -= ((50 * k) + text3.getGlobalBounds().height);
					}
					else
					{
						StartPositionMessage.y -= ((75 * k) + text3.getGlobalBounds().height);
					}
				}
			}
			////////////////////////////////

			window.display();
	}
#endif
	}
}

int main(int argc, char* argv[])
{
	////////////////////
	/*programs_Puth = argv[0];
	int slash = 0;
	for (int i = 0; i < programs_Puth.size(); i++)
	{
		if (programs_Puth[i] == '\\')
		{
			slash++;
		}
	}
	bool bo = false;
	int slash2 = 0;
	string str = "";
	for (int i = 0; i < programs_Puth.size(); i++)
	{
		if (programs_Puth[i] == '\\')
		{
			slash2++;
		}
		if(slash == slash2)
		{
			bo = true;
		}
		if (bo)
		{
			str += programs_Puth[i];
		}
	}
	programs_Puth = str;
	cout << programs_Puth;*/
	//system("pause");
	//exit(404);
	////////////////////
	system("mkdir C:\\ProgramData\\inifiscalam\\resourse");
	setlocale(LC_ALL, "Ru");
#if Client
	WSAData wsaDate;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaDate) != 0)
	{
		cout << "Error load library";
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
#if Global
	addr.sin_addr.s_addr = inet_addr("195.133.197.213");
#else
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
#endif
	addr.sin_port = htons(1987);
	addr.sin_family = AF_INET;

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)& addr, sizeof(addr)) != 0)
	{
		cout << "Error connect to server.\n";
		system("pause");
		return 1;
	}
	cout << "Connected!\n";

	//windows[0] = "Server [online]";

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

	windowsActive = W_Nothing;

	Packet Ptype = P_PVersion;
	send(Connection, (char*)& Ptype, sizeof(Packet), NULL);

	int PV_size = VersionPacket.size();

	send(Connection, (char*)& PV_size, sizeof(int), NULL);
	send(Connection, VersionPacket.c_str(), PV_size, NULL);

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ProssesWindows, NULL, NULL, NULL);

	string msg;

	cout << (windowsActive != w_Close) << endl;

	while (windowsActive != w_Close)
	{
		if (user.Authorization)
		{
			/*getline(cin, msg);
			int msg_size = msg.size();
			Packet msgtype = P_ChatMessage;
			send(Connection, (char*)& msgtype, sizeof(Packet), NULL);
			send(Connection, (char*)& msg_size, sizeof(int), NULL);
			send(Connection, msg.c_str(), msg_size, NULL);
			int name_size = user.name.size();
			send(Connection, (char*)& name_size, sizeof(int), NULL);
			send(Connection, user.name.c_str(), name_size, NULL);
			Sleep(10);*/
		}
		//cout << windows[0] << endl;
		//cout << windows[1] << endl;
		//system("cls");
	}

#else
	//вспомогательные переменные
	const float k = 1  / (2850.0 / VideoMode::getDesktopMode().width);

	bool MousePositionTable = false;
	bool WindowsShift = false;
	Vector2i StartPressedShiftWindows;
	Vector2f KResize(1,1);
	Vector2f OldSizeWindows(1425 * k, 1000 * k);

	String BufferCin;

	Font Font_normal;
	Font_normal.loadFromFile("resourse/MYRIADPRO-REGULAR.otf");
	Font Font_bold;
	Font_bold.loadFromFile("resourse/MYRIADPRO-BOLD.otf");

	bool CinActive = true;
	Text text;

	TextTable textCin(Font_normal, Vector2f(525 * k, 922 * k), 25 * k, k, true);

	Clock clock;

	vector<Message> Messages;

	//////////////////////////////

	RenderWindow window(VideoMode(1425 * k, 1000 * k), "Global chat", Style::None);
	window.setPosition(Vector2i(1920 / 2, 0));

	RectangleShape shape(Vector2f(475 * k, window.getSize().y));
	shape.setFillColor(Color(68, 62, 77));

	RectangleShape shape2(Vector2f(window.getSize().x, 50 * k));
	shape2.setFillColor(Color(54, 49, 61));

	RectangleShape shape3(Vector2f(window.getSize().x - (475 * k), 75 * k));
	shape3.setPosition(Vector2f(475 * k, 50 * k));
	shape3.setFillColor(Color(84, 82, 94));

	RectangleShape shape4(Vector2f(475 * k, 75 * k));
	shape4.setPosition(Vector2f(0, 50 * k));
	shape4.setFillColor(Color(73, 71, 82));

	//~
	Texture textureGlobalChannel;
	textureGlobalChannel.loadFromFile("resourse/global-channel.png");
	Sprite spriGlobalChannel;
	spriGlobalChannel.setTexture(textureGlobalChannel);
	spriGlobalChannel.setPosition(Vector2f(0, 125 * k));
	spriGlobalChannel.setScale(Vector2f(k, k));

	//~2
	RectangleShape shape6(Vector2f(80 * k, 80 * k));
	shape6.setPosition(Vector2f(37 * k, window.getSize().y - (101 * k)));
	shape6.setFillColor(Color(104, 95, 117));

	Texture textureUserDate;
	textureUserDate.loadFromFile("resourse/user-date.png");
	Sprite spriUserDate;
	spriUserDate.setTexture(textureUserDate);
	spriUserDate.setPosition(Vector2f(0, window.getSize().y - (125 * k)));
	spriUserDate.setScale(Vector2f(k, k));

	Texture textureUserDateAvatar;
	textureUserDateAvatar.loadFromFile("resourse/user-date-avatar.png");

	Texture texturePChat;
	texturePChat.loadFromFile("resourse/PChat.png");
#if spriPChat_
	Sprite spriPChat;
	spriPChat.setTexture(texturePChat);
	spriPChat.setPosition(Vector2f(475 * k, window.getSize().y - (78 * k)));
	spriPChat.setScale(Vector2f(k, k));
#else
	RectangleShape spriPChat(Vector2f(900 * k, 50 * k));
	spriPChat.setPosition(Vector2f(500 * k, window.getSize().y - (78 * k)));
	spriPChat.setFillColor(Color(92, 89, 99)); 
#endif
#if textureArrow_
	Texture textureArrow;
	textureArrow.loadFromFile("arrow.png");
	Sprite spriArrow;
	spriArrow.setTexture(textureArrow);
	spriArrow.setScale(Vector2f(k, k));
#endif

	Texture textureUserAvatar;
	textureUserAvatar.loadFromFile("resourse/702750219623989399.png");
	Sprite spriUserAvatar;
	spriUserAvatar.setTexture(textureUserAvatar);
	spriUserAvatar.setPosition(Vector2f(37 * k, 899 * k));
	spriUserAvatar.setScale(Vector2f((80.0 / textureUserAvatar.getSize().x) * k, (80.0 / textureUserAvatar.getSize().y) * k));

	Texture textureGlobalAvatar;
	textureGlobalAvatar.loadFromFile("resourse/global-avatar.png");
	Sprite spriGlobalAvatar;
	spriGlobalAvatar.setTexture(textureGlobalAvatar);
	spriGlobalAvatar.setPosition(Vector2f(37 * k, 163 * k));
	spriGlobalAvatar.setScale(Vector2f(k, k));

	Texture textureBackgraund;
	textureBackgraund.loadFromFile("resourse/backgraund.png");
	Sprite BackGraund;
	BackGraund.setTexture(textureBackgraund);
	BackGraund.setPosition(Vector2f(0,0));
	BackGraund.setScale(Vector2f(k, k));

	Texture textureRegisterTable;
	textureRegisterTable.loadFromFile("resourse/register-table.png");
	Sprite RegisterTable;
	RegisterTable.setTexture(textureRegisterTable);
	RegisterTable.setPosition(Vector2f(506 * k, 375 * k));
	RegisterTable.setScale(Vector2f(k, k));

	Texture textureRegisterTableActive;
	textureRegisterTableActive.loadFromFile("resourse/register-table-active.png");
	Sprite RegisterTableActive;
	RegisterTableActive.setTexture(textureRegisterTableActive);
	RegisterTableActive.setPosition(Vector2f(506 * k, 375 * k));
	RegisterTableActive.setScale(Vector2f(k, k));

	Mouse mouse;
	Vector2i mousePositionWindows;

	/////////////////////////////////////////////////////////////////////////
	Image imageClose;
	imageClose.loadFromFile("resourse/close.png");
	Batton close(Vector2i(50 * k, 50 * k), Color(128,55,55));
	close.setPosition(Vector2i(1375 * k + 1, 0));

	bool bo = false;

	Texture textureClose;
	textureClose.loadFromFile("resourse/close.png");
	Sprite spriClose;
	spriClose.setTexture(textureClose);
	spriClose.setPosition(Vector2f(1375 * k + 1, 1));
	spriClose.setScale(Vector2f(k, k));

#if Chat
	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
			if (event.type == Event::TextEntered)
			{
				cout << event.key.code << endl;

				switch (event.key.code)
				{
				case 1:
					//block
					break;
				case 2:
					//block
					break;
				case 3:
					//block
					break;
				case 4:
					//block
					break;
				case 5:
					//block
					break;
				case 6:
					//block
					break;
				case 7:
					//block
					break;
				case 8:
					if (BufferCin.getSize() > 0)
						BufferCin = String::fromUtf32(BufferCin.begin(), BufferCin.end() - 1);
					break;
				case 9:
					//block
					break;
				case 10:
					//block
					break;
				case 11:
					//block
					break;
				case 12:
					//block
					break;
				case 13:
#if textureArrow_
					BufferCin += '\n';
#endif
					break;
				case 14:
					//block
					break;
				case 15:
					//block
					break;
				case 16:
					//block
					break;
				case 17:
					//block
					break;
				case 18:
					//block
					break;
				case 19:
					//block
					break;
				case 20:
					//block
					break;
				case 21:
					//block
					break;
				case 22:
					BufferCin += Clipboard::getString();
					break;
				case 23:
					//block
					break;
				case 24:
					//block
					break;
				case 25:
					//block
					break;
				case 26:
					//block
					break;
				case 27:
					//block
					break;
				case 28:
					//block
					break;
				case 29:
					//block
					break;
				default:
					if (event.key.code < 2000) {
						BufferCin += static_cast<char32_t>(event.key.code);
					}
					else {
						// Time to consider sf::String or some other unicode-capable string
					}
					break;
				}
			}
		}

		mousePositionWindows = Vector2i(mouse.getPosition().x - window.getPosition().x, mouse.getPosition().y - window.getPosition().y);

		if (mousePositionWindows.x > 0 && mousePositionWindows.x < 1425 * k && mousePositionWindows.y > 0 && mousePositionWindows.y < 50 * k)
		{
			MousePositionTable = true;
		}
		else
		{
			MousePositionTable = false;
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape) || (close.active(mousePositionWindows) && Mouse::isButtonPressed(Mouse::Left)))
		{
			window.close();
			break;
		}

		if (MousePositionTable && Mouse::isButtonPressed(Mouse::Left) && !WindowsShift)
		{
			WindowsShift = true;
			StartPressedShiftWindows = mousePositionWindows;
		}

		if (WindowsShift && Mouse::isButtonPressed(Mouse::Left))
		{
			window.setPosition(Vector2i(mouse.getPosition().x - StartPressedShiftWindows.x, mouse.getPosition().y - StartPressedShiftWindows.y));
		}
		else 
		{
			WindowsShift = false;
		}

		/*if (bo)
		{
			window.setSize(Vector2u(window.getSize().x + 100, window.getSize().y));

			KResize.x = (float)OldSizeWindows.x / (OldSizeWindows.x + 100);

			shape.setScale(Vector2f(0.9056603773584905660377358490566, KResize.y));
		}
		else
		{
			window.setSize(Vector2u(window.getSize().x - 100, window.getSize().y));
			KResize.x = 1;
			shape.setScale(Vector2f(KResize.x, KResize.y));
			//window.setView = VideoMode(1425 * k, window.getSize().y);
			//shape.setSize(Vector2f(475, window.getSize().y));
		}

		bo = !bo;


		*/

		//string str = to_string(mousePositionWindows.x) + ";" + to_string(mousePositionWindows.y) + "\n" + to_string(close.active(mousePositionWindows));

		if (CinActive)
		{
			if (Keyboard::isKeyPressed(Keyboard::Enter))
			{
				if (BufferCin != "")
				{
					Message message;
					message.content = BufferCin;
					Messages.push_back(message);
					BufferCin = "";
				}
			}

			textCin.setString(BufferCin);
			spriPChat.setPosition(Vector2f(500 * k, textCin.getPosition(text).y - 8));
#if textureArrow_
			if (BufferCin != "")
			{
				spriPChat.setSize(Vector2f(825 * k, (972 * k) - spriPChat.getPosition().y));
				spriArrow.setPosition(Vector2f(1350 * k, spriPChat.getPosition().y + (spriPChat.getSize().y / 2) - (25 * k)));
			}
			else
			{
				spriPChat.setSize(Vector2f(900 * k, (972 * k) - spriPChat.getPosition().y));
			}
#else
			spriPChat.setSize(Vector2f(900 * k, (972 * k) - spriPChat.getPosition().y));
#endif 

		}

		window.clear(Color(72, 70, 79));

		//интерфейс/////////////////////
		window.draw(shape);
		window.draw(shape2);
		window.draw(shape3);
		window.draw(shape4);
		window.draw(shape6);
		
		window.draw(spriGlobalChannel);
		window.draw(spriGlobalAvatar);
		window.draw(spriUserAvatar);
		window.draw(spriUserDate);
		window.draw(spriPChat);

		close.draw(window);
		window.draw(spriClose);
		////////////////////////////////
		textCin.draw(window, text);
#if textureArrow_
		if (textCin.getString() != "")
		{
			window.draw(spriArrow);
		}
#endif

		float time = clock.getElapsedTime().asMicroseconds() / 1000;

		if (time < 500)
		{
			RectangleShape sha(Vector2f(1, 25 * k));
			if (text.getGlobalBounds().width == 0)
			{
				sha.setPosition(Vector2f(525 * k + text.getGlobalBounds().width, 930 * k));
			}
			else
			{
				sha.setPosition(Vector2f(530 * k + text.getGlobalBounds().width, 930 * k));
			}
			window.draw(sha);
		}
		else
		{
			if (time > 1000)
			{
				clock.restart();
			}
		}
		////////////////////////////////
		text.setString("FunTemis");
		text.setPosition(Vector2f(142 * k, 899 * k));
		window.draw(text);

		text.setString("Online");
		text.setPosition(Vector2f(142 * k, 939 * k));
		window.draw(text);

		text.setString("Global channel");
		text.setPosition(Vector2f(142 * k, 188 * k));
		window.draw(text);

		text.setString("Servel online");
		text.setPosition(Vector2f((237.5 * k) - (text.getLocalBounds().width / 2), 75 * k));
		window.draw(text);

		text.setString("Global channel");
		text.setPosition(Vector2f(500 * k, 75 * k));
		window.draw(text);

		if (Messages.size() != 0)
		{
			Vector2f StartPositionMessage(525 * k, 847 * k);
			for (int i = 0; i < Messages.size(); i++)
			{
				Text text3;
				TextTable textMessage(Font_normal, Vector2f(StartPositionMessage.x + (75 * k), StartPositionMessage.y + (25 * k)), 25 * k, k, false);
				textMessage.setString(Messages[i].content);

				int line = textMessage.getLine();

				if (line != 0)
				{
					StartPositionMessage.y -= ((25 * k) + text3.getGlobalBounds().height);
					textMessage.setPosition(Vector2f(StartPositionMessage.x + (75 * k), StartPositionMessage.y + (25 * k)));
				}

				textMessage.draw(window, text3);

				Sprite MessageAuthorAvatar;
				MessageAuthorAvatar.setTexture(textureUserAvatar);
				MessageAuthorAvatar.setPosition(Vector2f(StartPositionMessage.x, StartPositionMessage.y));
				MessageAuthorAvatar.setScale(Vector2f((50.0 / textureUserAvatar.getSize().x)* k, (50.0 / textureUserAvatar.getSize().y)* k));
				window.draw(MessageAuthorAvatar);

				Sprite MessageAuthorAvatarDate;
				MessageAuthorAvatarDate.setTexture(textureUserDateAvatar);
				MessageAuthorAvatarDate.setPosition(Vector2f(StartPositionMessage.x, StartPositionMessage.y));
				MessageAuthorAvatarDate.setScale(Vector2f(k, k));
				window.draw(MessageAuthorAvatarDate);

				Text text2;
				text2.setFont(Font_bold);
				text2.setString("FunTemis");
				text2.setPosition(Vector2f(StartPositionMessage.x + (75 * k), StartPositionMessage.y));
				text2.setCharacterSize(25 * k);
				window.draw(text2);

				if (line != 0)
				{
					StartPositionMessage.y -= ((50 * k) + text3.getGlobalBounds().height);
				}
				else
				{
					StartPositionMessage.y -= ((75 * k) + text3.getGlobalBounds().height);
				}
			}
		}
		////////////////////////////////

		window.display();
	}
#else

	TextTable textCinLogin(Font_normal, Vector2f(531 * k, 383 * k), Vector2i(400 * k, 50 * k), Vector2f(506 * k, 385 * k), 25 * k, k, false);
	textCinLogin.setActive(true);

	TextTable textCinPassword(Font_normal, Vector2f(531 * k, 458 * k), Vector2i(400 * k, 50 * k), Vector2f(506 * k, 450 * k), 25 * k, k, false);
	textCinPassword.setActive(false);

	while (window.isOpen())
	{
		bool backspase = false;

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
			if (event.type == Event::TextEntered)
			{
				cout << event.key.code << endl;

				switch (event.key.code)
				{
				case 1:
					//block
					break;
				case 2:
					//block
					break;
				case 3:
					//block
					break;
				case 4:
					//block
					break;
				case 5:
					//block
					break;
				case 6:
					//block
					break;
				case 7:
					//block
					break;
				case 8:
					if (BufferCin.getSize() > 0)
						BufferCin = String::fromUtf32(BufferCin.begin(), BufferCin.end() - 1);
					break;
				case 9:
					//block
					break;
				case 10:
					//block
					break;
				case 11:
					//block
					break;
				case 12:
					//block
					break;
				case 13:
#if textureArrow_
					BufferCin += '\n';
#endif
					break;
				case 14:
					//block
					break;
				case 15:
					//block
					break;
				case 16:
					//block
					break;
				case 17:
					//block
					break;
				case 18:
					//block
					break;
				case 19:
					//block
					break;
				case 20:
					//block
					break;
				case 21:
					//block
					break;
				case 22:
					BufferCin += Clipboard::getString();
					break;
				case 23:
					//block
					break;
				case 24:
					//block
					break;
				case 25:
					//block
					break;
				case 26:
					//block
					break;
				case 27:
					//block
					break;
				case 28:
					//block
					break;
				case 29:
					//block
					break;
				default:
					if (event.key.code < 2000) {
						BufferCin += static_cast<char32_t>(event.key.code);
					}
					else {
						// Time to consider sf::String or some other unicode-capable string
					}
					break;
				}

				if (event.key.code == 8)
				{
					backspase = true;
				}
			}
		}

		if (event.key.code != 8)
		{
			backspase = false;
		}

		mousePositionWindows = Vector2i(mouse.getPosition().x - window.getPosition().x, mouse.getPosition().y - window.getPosition().y);

		if (mousePositionWindows.x > 0 && mousePositionWindows.x < 1425 * k && mousePositionWindows.y > 0 && mousePositionWindows.y < 50 * k)
		{
			MousePositionTable = true;
		}
		else
		{
			MousePositionTable = false;
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape) || (close.active(mousePositionWindows) && Mouse::isButtonPressed(Mouse::Left)))
		{
			window.close();
			break;
		}

		if (MousePositionTable && Mouse::isButtonPressed(Mouse::Left) && !WindowsShift)
		{
			WindowsShift = true;
			StartPressedShiftWindows = mousePositionWindows;
		}

		if (WindowsShift && Mouse::isButtonPressed(Mouse::Left))
		{
			window.setPosition(Vector2i(mouse.getPosition().x - StartPressedShiftWindows.x, mouse.getPosition().y - StartPressedShiftWindows.y));
		}
		else
		{
			WindowsShift = false;
		}

		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (textCinLogin.active(mousePositionWindows))
			{
				BufferCin = "";
				textCinLogin.setActive(true);
			}
			else
			{
				textCinLogin.setActive(false);
			}

			if (textCinPassword.active(mousePositionWindows))
			{
				BufferCin = "";
				textCinPassword.setActive(true);
			}
			else
			{
				textCinPassword.setActive(false);
			}
		}


		window.clear(Color(72, 70, 79));
		window.draw(BackGraund);
		if ((mousePositionWindows.x - 506 * k) > 0 && (mousePositionWindows.x - 506 * k) < 400 * k && (mousePositionWindows.y - 525 * k) > 0 && (mousePositionWindows.y - 525 * k) < 50 * k)
		{
			window.draw(RegisterTableActive);
			if (Mouse::isButtonPressed(Mouse::Left))
			{
				if (textCinLogin.getString() != "" && textCinPassword.getString() != "")
				{
					string login = textCinLogin.getString();
					string password = textCinPassword.getString();
					cout << "Login    : " << login << endl;
					cout << "Password : " << password << endl;
					String str = "";
					textCinPassword.setString(str);
				}
			}
		}
		else
		{
			window.draw(RegisterTable);
		}
		window.draw(shape2);
		close.draw(window);
		window.draw(spriClose);

		if (textCinLogin.getActive())
		{
			String str = textCinLogin.getString();
			if (backspase && str.getSize() != 0)
			{
				str = String::fromUtf32(str.begin(), str.end() - 1);
			}
			str += BufferCin;
			BufferCin = "";
			textCinLogin.setString(str);
		}
		textCinLogin.draw(window, text);

		if (textCinPassword.getActive())
		{
			String str = textCinPassword.getString();
			if (backspase && str.getSize() != 0)
			{
				str = String::fromUtf32(str.begin(), str.end() - 1);
			}
			str += BufferCin;
			BufferCin = "";
			textCinPassword.setString(str);
		}
		textCinPassword.draw(window, text);

		text.setString(L"Авторизация");
		text.setPosition(Vector2f(706 * k - (text.getGlobalBounds().width / 2), 335 * k));
		window.draw(text);

		Text text2;
		text2.setString(L"Логин");
		text2.setFont(Font_normal);
		text2.setPosition(Vector2f(531 * k, 378 * k));
		text2.setCharacterSize(15 * k);
		text2.setFillColor(Color(200,200,200));
		window.draw(text2);

		text2.setString(L"Пароль");
		text2.setFont(Font_normal);
		text2.setPosition(Vector2f(531 * k, 453 * k));
		text2.setCharacterSize(15 * k);
		text2.setFillColor(Color(200, 200, 200));
		window.draw(text2);

		text2.setString(L"Вход");
		text2.setFont(Font_normal);
		text2.setCharacterSize(25 * k);
		text2.setPosition(Vector2f(706 * k - (text2.getGlobalBounds().width / 2), 533 * k));
		text2.setFillColor(Color(255, 255, 255));
		window.draw(text2);

		text2.setString(L"Не верный логин или пароль");
		text2.setFont(Font_normal);
		text2.setCharacterSize(15 * k);
		text2.setPosition(Vector2f(706 * k - (text2.getGlobalBounds().width / 2), 358 * k));
		text2.setFillColor(Color(255, 100, 100));
		window.draw(text2);

		window.display();
	}
#endif
#endif
	//system("pause");
	return 0;
}