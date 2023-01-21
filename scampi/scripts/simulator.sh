#!/bin/sh

trap 'ctrl_c $1' INT

ctrl_c() {
  xcrun simctl logverbose "$1" disable
  xcrun simctl shutdown "$1"
}

xcrun simctl boot "$1"
xcrun simctl logverbose "$1" enable
open -a Console
open -a Simulator

tail -f "$HOME/Library/Logs/CoreSimulator/$1/system.log"
