#include "Assistant.h"



UserPrefs Assistant::engage()
{
	UserPrefs prefs;

	std::cout << "Hi, do you want to generate a new model or load an existing one?" << std::endl;
	std::cout << "1. Generate new" << std::endl;
	std::cout << "2. Load existing" << std::endl;

	if (Assistant::get_user_number() >= 2)
	{
		std::cout << "Please enter the path to the file: " << std::endl;
		prefs.src_file = Assistant::get_user_string();
		prefs.import = true;
	}
	else
	{
		std::cout << "Please provide details for model generation:" << std::endl;
		std::cout << "Crater density: ";
		prefs.crater_density = Assistant::get_user_number();
		std::cout << std::endl;

		std::cout << "Dome density: ";
		prefs.dome_density = Assistant::get_user_number();
		std::cout << std::endl;

		std::cout << "Ridge density: ";
		prefs.ridge_density = Assistant::get_user_number();
		std::cout << std::endl;

		prefs.import = false;
	}

	return prefs;
}

std::string Assistant::get_user_string()
{
	std::string res;
	std::getline(std::cin, res);
	return res;
}

int Assistant::get_user_number()
{
	int result = 0;

	bool repeat = true;
	while (repeat)
	{
		std::string res;
		std::getline(std::cin, res);
		try
		{
			result = std::stoi(res);
			repeat = false;
		}
		catch (std::exception)
		{
			std::cout << "Incorrect format! Please provide a number:" << std::endl;
			repeat = true;
		}
	}
	return result;
}

Assistant::Assistant()
{
}


Assistant::~Assistant()
{
}
