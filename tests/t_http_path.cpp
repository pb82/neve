#include "../vendor/catch.hpp"
#include "../http/path.hpp"
#include "../http/request.hpp"
#include "../json/value.hpp"

TEST_CASE("HTTP/Path", "[path]") {
	Path simple(GET, "/a", 0);
	HttpRequest req;

	SECTION("Simple path matching") {
		// Match same path
		req.url = "/a";
		req.method = GET;
		REQUIRE(simple.match(&req) == true);

		// Fail different path
		req.url = "/b";
		req.method = GET;
		REQUIRE(simple.match(&req) == false);

		// Fail different http method
		req.url = "/a";
		req.method = POST;
		REQUIRE(simple.match(&req) == false);

		// Match simple query args
		req.url = "/a?block=true";
		req.method = GET;
		REQUIRE(simple.match(&req) == true);
		REQUIRE(req.params.size() == 1);
		REQUIRE(req.params["block"].is(JSON::JSON_STRING));
		REQUIRE(req.params["block"].as<std::string>().compare("true") == 0);

		// Match multiple query args
		req.url = "/a?block=true&count=3";
		req.method = GET;
		REQUIRE(simple.match(&req) == true);
		REQUIRE(req.params.size() == 2);
		REQUIRE(req.params["block"].is(JSON::JSON_STRING));
		REQUIRE(req.params["block"].as<std::string>().compare("true") == 0);
		REQUIRE(req.params["count"].is(JSON::JSON_STRING));
		REQUIRE(req.params["count"].as<int>() == 3);

		// Match query arg as flag
		req.url = "/a?block";
		req.method = GET;
		REQUIRE(simple.match(&req) == true);
		REQUIRE(req.params.size() == 1);
		REQUIRE(req.params["block"].is(JSON::JSON_BOOL));
		REQUIRE(req.params["block"].as<bool>() == true);

		// Match multiple flags
		req.url = "/a?block&count";
		req.method = GET;
		REQUIRE(simple.match(&req) == true);
		REQUIRE(req.params.size() == 2);
		REQUIRE(req.params["block"].is(JSON::JSON_BOOL));
		REQUIRE(req.params["block"].as<bool>() == true);
		REQUIRE(req.params["count"].is(JSON::JSON_BOOL));
		REQUIRE(req.params["count"].as<bool>() == true);
	}

	Path variable(GET, "/a/:id/b", 0);

	SECTION("Path variables") {
		// Match same path
		req.url = "/a/hello/b";
		req.method = GET;
		REQUIRE(simple.match(&req) == true);
	}
}
