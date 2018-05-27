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
	level = LogLevel.Info
}

Plugin "mongo" {
	file="./plugins/default/mongo/plugin_mongo.so",
	connectionString="mongodb://database:27017",
	database="mongoplugin"
}
