#!/bin/bash

set -e

cmake --build . --target doc
cd doc/YES
git init .
git remote add origin https://${GH_REPO_TOKEN}@github.com/${TRAVIS_REPO_SLUG}
git add ./*
git commit -m "Auto-deployed documentation from Travis build: ${TRAVIS_BUILD_NUMBER}" -m "Commit: ${TRAVIS_COMMIT}"
git branch -m gh-pages
git push origin gh-pages -f
