function main(args)
	payload = {
		collection="requests",
		data={
			message="Hello World"
		}
	}
	return call("mongo", "create", payload)
end
