#!/bin/bash

cppcheck \
    --project=build/compile_commands.json \
    --enable=all
