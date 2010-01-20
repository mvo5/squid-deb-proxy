#/bin/sh

CURL="curl -s -I"

function assert_ok() {
    if ! $CURL $1 | grep -q "HTTP/1.0 200 OK"; then
	echo "ASSERT FAILURE for $@, assumed 200 OK"
    fi
}

function assert_forbidden() {
    if ! $CURL $1 | grep -q "HTTP/1.0 403 Forbidden"; then
	echo "ASSERT FAILURE for $@, assumed 403 Forbidden"
    fi
}

http_proxy=http://localhost:8000/
echo "Starting tests for $http_proxy"

assert_ok "http://archive.ubuntu.com"
assert_ok "http://de.archive.ubuntu.com/ubuntu/"
assert_ok "http://changelogs.ubuntu.com/meta-release"

assert_forbidden "http://example.com"
assert_forbidden "http://example.com:21"

echo "Test finished"