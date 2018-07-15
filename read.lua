function main(args)
	local payload = {
		collection="requests",
		query={
			message="Hello World"
		}
	}

	return call("mongo", "list", payload)
end
