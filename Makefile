VERSION := 0.1.0
SHELL := /bin/bash
UNAME := $(shell uname -s)
BUILD_TYPE ?= Release
.DEFAULT_GOAL = help

##==================================================================================================
##@ Repo initialization

deps:  ## Install repo deps
	pip3 install poetry
	poetry install
.PHONY: deps

pre-commit:  ## Install pre-commit
	poetry run pre-commit install
	poetry run pre-commit install -t commit-msg
.PHONY: pre-commit

init: deps pre-commit  ## Initialize repo by executing above commands
.PHONY: init

##==================================================================================================
##@ Building with CMake

build:  ## Build project
	poetry run conan install . \
		--build=missing \
		--profile=conanprofile.txt \
		--settings=compiler.cppstd=gnu20 \
		--settings=build_type=$(BUILD_TYPE) \
	&& cd build \
	&& cmake .. \
		-DCMAKE_TOOLCHAIN_FILE=$(BUILD_TYPE)/generators/conan_toolchain.cmake \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	&& cmake --build .
.PHONY: build

tests:  ## Run tests
	cd build && ctest
.PHONY: tests

##==================================================================================================
##@ Cleaning

clean: ## Delete junk files
	find . | grep -E "\.o" | xargs rm -rf
	find . | grep -E "\.exe" | xargs rm -rf
	rm -rf build
.PHONY: clean

##==================================================================================================
##@ Miscellaneous

conan-profile:  ## Guess a configuration set (compiler, build configuration, architecture, shared or static libraries, etc.)
	poetry run conan profile detect --force | grep -v "Detected profile:" > conanprofile.txt
	perl -pi -e 'chomp if eof' conanprofile.txt
.PHONY: conan-profile

conan-lock:  ## Lock C++ dependencies
	poetry run conan lock create .
.PHONY: conan-lock

update-pre-commit-hooks:  ## Update pre-commit hooks
	poetry run pre-commit autoupdate
.PHONY: update-pre-commit-hooks

create-secrets-baseline:  ## Create secrets baseline file
	poetry run detect-secrets scan > .secrets.baseline
.PHONY: create-secrets-baseline

audit-secrets-baseline:  ## Check updated .secrets.baseline file
	poetry run detect-secrets audit .secrets.baseline
	git commit .secrets.baseline --no-verify -m "build(security): update secrets.baseline"
.PHONY: audit-secrets-baseline

##==================================================================================================
##@ Helper

help:  ## Display help
	@awk 'BEGIN {FS = ":.*##"; printf "\nUsage: \033[36m\033[0m\n"} /^[a-zA-Z\.\%-]+:.*?##/ { printf "  \033[36m%-24s\033[0m %s\n", $$1, $$2 } /^##@/ { printf "\n\033[1m%s\033[0m\n", substr($$0, 5) } ' $(MAKEFILE_LIST)
.PHONY: help
