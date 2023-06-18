VERSION := 0.0.0
SHELL := /bin/bash
UNAME := $(shell uname -s)
.DEFAULT_GOAL = help

##==================================================================================================
##@ Repo initialization

repo-prerequisites:  ## Install prerequisites
ifeq ($(UNAME), Darwin)
	brew install llvm
endif
.PHONY: repo-prerequisites

repo-deps:  ## Install repo deps
	pip install poetry
	poetry config virtualenvs.in-project true
	poetry install
.PHONY: repo-deps

repo-pre-commit:  ## Install pre-commit
	poetry run pre-commit install
	poetry run pre-commit install -t commit-msg
.PHONY: repo-pre-commit

repo-init: repo-prerequisites repo-deps repo-pre-commit  ## Initialize repo by executing above commands
.PHONY: repo-init

##==================================================================================================
##@ Building with CMake

.ONESHELL:
build:  ## Build project
	poetry run conan install . \
		--build=missing \
		--profile=conanprofile.txt \
		--settings=compiler.cppstd=gnu20
	cd build
	source Release/generators/conanbuild.sh
	cmake .. \
		-DCMAKE_TOOLCHAIN_FILE=Release/generators/conan_toolchain.cmake \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	cmake --build .
	source Release/generators/deactivate_conanbuild.sh
.PHONY: build

tests:  ## Run tests
	cd build && ctest
.PHONY: tests

##==================================================================================================
##@ Cleaning

clean: ## Delete junk files
	find . | grep -E "\.o" | xargs rm -rf
	find . | grep -E "\.exe" | xargs rm -rf
.PHONY: clean

##==================================================================================================
##@ Miscellaneous

conan-profile:  ## Guess a configuration set (compiler, build configuration, architecture, shared or static libraries, etc.)
	poetry run conan profile detect --force | grep -v "Detected profile:" > conanprofile.txt
.PHONY: conan-profile

conan-lock:  ## Lock dependencies
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
