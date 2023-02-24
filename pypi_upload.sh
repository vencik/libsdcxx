#!/bin/sh

set -e

rm -f pysdcxx-*.tar.gz
./build.sh -ug
twine upload pysdcxx-*.tar.gz
