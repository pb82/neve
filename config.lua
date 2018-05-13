-- Constants for the logger config
local LogLevel = {
	Silent = 1,
	Error =	2,
	Warn = 3,
	Info = 4,
	Debug = 5
}

Config "logger" {
	level = LogLevel.Info
}
