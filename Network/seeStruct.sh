#!/bin/bash
echo "#include <$1>" | gcc -E - | less
