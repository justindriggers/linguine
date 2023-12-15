#!/bin/sh

xcrun devicectl device install app --device "$DEVICE_ID" scampi.app
xcrun devicectl device process launch --device "$DEVICE_ID" com.justindriggers.scampi
