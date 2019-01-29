#!/bin/bash

# Go to master
git checkout master

# Merge develop
git merge develop

# Push commits
git push

# Return to develop
git checkout develop
