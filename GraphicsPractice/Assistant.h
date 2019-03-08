#pragma once

#include <iostream>
#include <string>

#include "UserPrefs.h"

class Assistant
{
public:
	static UserPrefs engage();

	static std::string get_user_string();
	static int get_user_number();

	Assistant();
	~Assistant();
};

