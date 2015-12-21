#!/bin/bash
if [ ! -d "gtest" ]; then
	# Control will enter here if $DIRECTORY doesn't exist.
	mkdir gtest
	cd gtest
	cmake ../../third_party/gtest
	make
fi
