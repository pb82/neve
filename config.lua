-- Constants for the logger config
local LogLevel = {
	Silent = 1,
	Error =	2,
	Warn = 3,
	Info = 4,
	Debug = 5
}

Config "server" {
	ipaddress="0.0.0.0",
	port=8080
}

Config "logger" {
	level = LogLevel.Debug
}

Config "persistence" {
	path="./plugins/default/mongo/plugin_mongo.so",
	connectionString="mongodb://localhost:27017",
	database="neve"
}

Plugin "mongo" {
	path="./plugins/default/mongo/plugin_mongo.so",
	connectionString="mongodb://localhost:27017",
	database="userdata"
}

Plugin "skeleton" {
	path="./plugins/default/skeleton/plugin_skeleton.so",
}
