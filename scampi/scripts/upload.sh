#!/bin/sh

echo "$ALTOOL_PASSWORD" | xcrun altool --upload-app -t ios -f scampi.ipa -u "$ALTOOL_USERNAME"
