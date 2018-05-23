function main(args)
	if args.name == nil then
		args.name = "World"
	end

	return "Hello " .. args.name
end
