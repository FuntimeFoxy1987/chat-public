#pragma once
#ifndef BaseDate_
#define BaseDate_

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/istreamwrapper.h"

struct icon
{
	string file;
	string url;
};

using namespace rapidjson;

class BaseDate
{
public:
	BaseDate(string PuthFileJson, bool upDateSet)
	{
		Puth = PuthFileJson;
		this->upDateSet = upDateSet;
		loadDate();
	}
	string json()
	{
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		documentFromFile.Accept(writer);

		return buffer.GetString();
	}
	string get(vector<string> element)
	{
		loadDate();
		Value &value = documentFromFile[element[0].c_str()];
		if (value.GetType() == Type::kNumberType)
			return to_string(value.GetInt());
		else if (value.GetType() == Type::kStringType)
			return value.GetString();
		else if (value.GetType() == Type::kArrayType)
			return "[Array]";
		else if (value.GetType() == Type::kObjectType)
			return getObject(value, element, element.size(), 0);
		return "Unknown";
	}

	void set(vector<string> element, int date)
	{
		Value &value = documentFromFile[element[0].c_str()];
		if (value.GetType() == Type::kArrayType)
		{
			setArray();
		}
		else if (value.GetType() == Type::kObjectType)
		{
			setObject(value, element, element.size(), 0, date);
		}
		else if (value.GetType() == Type::kNumberType)
		{
			value.SetInt(date);
		}
		else
		{
			cout << "Error set value : Unknow value" << endl;
			return;
		}
		if (upDateSet)
		{
			uploadDate();
		}
	}
	void set(vector<string> element, __int64 date)
	{
		Value& value = documentFromFile[element[0].c_str()];
		if (value.GetType() == Type::kArrayType)
		{
			setArray();
		}
		else if (value.GetType() == Type::kObjectType)
		{
			setObject(value, element, element.size(), 0, date);
		}
		else if (value.GetType() == Type::kNumberType)
		{
			value.SetInt64(date);
		}
		else
		{
			cout << "Error set value : Unknow value" << endl;
			return;
		}
		if (upDateSet)
		{
			uploadDate();
		}
	}
	void set(vector<string> element, string &date)
	{
		Value &value = documentFromFile[element[0].c_str()];
		if (value.GetType() == Type::kArrayType)
		{
			setArray();
		}
		else if (value.GetType() == Type::kObjectType)
		{
			setObject(value, element, element.size(), 0, date);
		}
		else if (value.GetType() == Type::kStringType)
		{
			value.SetString(date.c_str(), date.size());
		}
		else
		{
			cout << "Error set value : Unknow value" << endl;
			return;
		}
		if (upDateSet)
		{
			uploadDate();
		}
	}

	void uploadDate()
	{
		ofstream ofs(Puth);
		if (!ofs.is_open())
		{
			cout << "Error save BseDate: file don't open" << endl;
		}
		else 
		{
			ofs << json();
		}
	}
public:
	void loadDate()
	{
		ifstream ifs(Puth);
		IStreamWrapper isw(ifs);

		documentFromFile.ParseStream(isw);
	}
	string getObject(Value &value, vector<string> &vec, int i, int f)
	{
		if (i == 1)
		{
			return getObjectType(value);
		}
		else
		{
			i--;
			f++;
			return getObject(value[vec[f].c_str()], vec, i, f);
		}
	}
	string getObjectType(Value &value)
	{
		if (value.GetType() == Type::kNumberType)
			return to_string(value.GetInt());
		else if (value.GetType() == Type::kStringType)
			return value.GetString();
		else if (value.GetType() == Type::kArrayType)
			return "[Array]";
		else if (value.GetType() == Type::kObjectType)
			return "[Object]";
		return "[Unknown]";
	}
	
	void setObject(Value& value, vector<string>& vec, int i, int f, int date)
	{
		if (i == 1)
		{
			return setObjectTypeInt(value, date);
		}
		else
		{
			i--;
			f++;
			return setObject(value[vec[f].c_str()], vec, i, f, date);
		}
	}
	void setObject(Value& value, vector<string>& vec, int i, int f, __int64 date)
	{
		if (i == 1)
		{
			return setObjectTypeInt64(value, date);
		}
		else
		{
			i--;
			f++;
			return setObject(value[vec[f].c_str()], vec, i, f, date);
		}
	}
	void setObject(Value& value, vector<string>& vec, int i, int f, string &date)
	{
		if (i == 1)
		{
			return setObjectTypeString(value, date);
		}
		else
		{
			i--;
			f++;
			return setObject(value[vec[f].c_str()], vec, i, f, date);
		}
	}

	void setObjectTypeString(Value &value, string &date)
	{
		if (value.GetType() == Type::kStringType)
			value.SetString(date.c_str(), date.size());
		else if (value.GetType() == Type::kArrayType)
			cout << "Error set array";
		else if (value.GetType() == Type::kObjectType)
			cout << "Error set object";
		else
		{
			cout << "Error set Unknown";
		}
	}
	void setObjectTypeInt(Value &value, int &date)
	{
		if (value.GetType() == Type::kNumberType)
			value.SetInt(date);
		else if (value.GetType() == Type::kArrayType)
			cout << "Error set array";
		else if (value.GetType() == Type::kObjectType)
			cout << "Error set object";
		else
		{
			cout << "Error set Unknown";
		}
	}
	void setObjectTypeInt64(Value &value, __int64 &date)
	{
		if (value.GetType() == Type::kNumberType)
			value.SetInt64(date);
		else if (value.GetType() == Type::kArrayType)
			cout << "Error set array";
		else if (value.GetType() == Type::kObjectType)
			cout << "Error set object";
		else
		{
			cout << "Error set Unknown";
		}
	}

	void setArray()
	{

	}
	string Puth;
	Document documentFromFile;
	bool upDateSet;
};
#endif