#include "../vendor/catch.hpp"
#include "../http/path.hpp"
#include "../http/request.hpp"

TEST_CASE("HTTP/Path", "[path]") {
	Path a(GET, "/a", 0);

	HttpRequest req;
	req.url = "/a";
	req.method = GET;
	REQUIRE(a.match(&req) == true);

	req.url = "/b";
	req.method = GET;
	REQUIRE(a.match(&req) == false);
}
