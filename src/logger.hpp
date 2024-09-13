
#pragma once

#include <iostream>
#include <string>
#include <fstream>

namespace li2
{
	class Logger
	{
	public:
		~Logger() = default;

		Logger()
		{
			m_file.open("log.txt", std::ios::out | std::ios::trunc);
		}

		template <class ... T>
		void Log(T ... args)
		{
			// ((m_file << args << ' '), ...) << '\n';
			((std::cout << args << ' '), ...) << '\n';
		}

		template <class ... T>
		void Error(T ... args)
		{
			Log("Error: ", args...);
		}

		std::string WtoA(const std::wstring& wstr)
		{
			std::string str(wstr.begin(), wstr.end());
			return str;
		}

	private:
		std::ofstream m_file;
	};

	extern Logger logger;
}
