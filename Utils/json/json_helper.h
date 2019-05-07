#ifndef JSON_HELPER_H
#define JSON_HELPER_H
#include"json.h"

namespace json_helper
{
	template <typename T>
	static bool readValue(const nlohmann::json &j, const std::string &key, T &v)
	{
		if (j.find(key) == j.end())
			return false;

		v = j[key].get<T>();
		return true;
	}


	/*
	������ȡ�̶������/���ޣ���Ϊ 0 ʱ�������ù̶�
	*/
	static bool readVector3f(const nlohmann::json &j, const std::string &key, float* vec)
	{
		if (j.find(key) == j.end())
			return false;

		std::vector<float> values = j[key].get<std::vector<float>>();
		for (unsigned int i = 0; i < values.size(); i++)
		{
			if (i == 3) break;	// ���ֻ��ȡ 4 ������

			vec[i] = values[i];
		}
			
		return true;
	}
}

#endif // !JSON_HELPER_H
