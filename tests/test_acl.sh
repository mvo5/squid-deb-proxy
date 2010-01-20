#!/bin/sh

CURL="curl -s -I"
count=0
failure=0

assert_ok() {
    if ! $CURL $1 | grep -q "HTTP/1.0 200 OK"; then
	echo "ASSERT FAILURE for $@, assumed 200 OK"
	failure=$(( $failure + 1 ))
    fi
    count=$(( $count + 1 ))
}

assert_forbidden() {
    if ! $CURL $1 | grep -q "HTTP/1.0 403 Forbidden"; then
	echo "ASSERT FAILURE for $@, assumed 403 Forbidden"
	failure=$(( $failure + 1 ))
    fi
    count=$(( $count + 1 ))
}


http_proxy=http://localhost:8000/
echo "Starting tests for $http_proxy"

assert_ok "http://archive.ubuntu.com"
assert_ok "http://de.archive.ubuntu.com/ubuntu/"
assert_ok "http://changelogs.ubuntu.com/meta-release"

assert_forbidden "http://wiki.ubuntu.com"
assert_forbidden "http://www.ubuntu.com"
assert_forbidden "http://example.com"
assert_forbidden "http://example.com:21"

echo "Test finished, tests failed: $failure, tests run: $count"