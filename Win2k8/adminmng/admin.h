#pragma once
#include <iostream>
#using <mscorlib.dll>

std::string systemInfoNetwork();

void createSysConf();

std::string cpuModel();

void initSystem(bool reset);

void reboot();

void shutdown();

void sysInfo(const std::string &cpu_usage);

void createSupportUser(const std::string &username, const std::string &password);

void removeSupportUser(const std::string &username);