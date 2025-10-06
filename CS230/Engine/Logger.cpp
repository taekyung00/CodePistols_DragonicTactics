/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Logger.cpp
Project:    CS230 Engine
Author:     Jonathan Holmes, Taekyung Ho
Created:    March 4, 2025
Updated:    March 4, 2025
*/
#include <iostream>
#include <map>
#include "Logger.h"

CS230::Logger::Logger(Logger::Severity severity, bool use_console, std::chrono::system_clock::time_point start_time) :
	min_level(severity),
	out_stream("Trace.log"),
	start_time(start_time)
{
	SetTraceLogLevel(LOG_NONE);
	if (use_console == true) {
		out_stream.set_rdbuf(std::cout.rdbuf());
	}
}
CS230::Logger::~Logger() {
	out_stream.flush();
	out_stream.close();
}

void CS230::Logger::log(CS230::Logger::Severity severity, std::string message) {
	//std:: string  Severity_string[] = {
	//		"Verbose",  //Minor messages
	//		"Debug",    //Only used while actively debugging
	//		"Event",    //General event, like key press or state change
	//		"Error"     //Errors, such as file load errors
	//};
	// std::string answer = Severity_string[int(severity)] +"\t"+ message;
	//=============array_version

	std::map< CS230::Logger::Severity, std::string> get_error_level = {
		{CS230::Logger::Severity::Verbose,"Verbose"},
		{CS230::Logger::Severity::Debug,"Debug"},
		{CS230::Logger::Severity::Event,"Event"},
		{CS230::Logger::Severity::Error,"Error"}
	};
	std::string answer = get_error_level[severity] + "\t" + message;
	//===========map_version
	

	if (int(CS230::Logger::min_level) <= int(severity)) {

		out_stream.precision(4);
		out_stream << '[' << std::fixed << seconds_since_start() << "]\t";
		out_stream  << answer << "\n";
	}
	return;
}

double CS230::Logger::seconds_since_start(){
	return std::chrono::duration<double>(std::chrono::system_clock::now() -start_time).count();
}
