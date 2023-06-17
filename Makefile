VERSION := 0.0.0
SHELL := /bin/bash

##==================================================================================================
##@ Helper

.PHONY: help
help:  ## Display help
	@awk 'BEGIN {FS = ":.*##"; printf "\nUsage: \033[36m\033[0m\n"} /^[a-zA-Z\.\%-]+:.*?##/ { printf "  \033[36m%-24s\033[0m %s\n", $$1, $$2 } /^##@/ { printf "\n\033[1m%s\033[0m\n", substr($$0, 5) } ' $(MAKEFILE_LIST)

##==================================================================================================
##@ Repo initialization

repo-deps:  ## Install repo deps
	pip install poetry
	poetry config virtualenvs.in-project true
	poetry install

repo-pre-commit:  ## Install pre-commit
	poetry run pre-commit install
	poetry run pre-commit install -t commit-msg

repo-conan-profile:  ## Define a configuration set (compiler, build configuration, architecture, shared or static libraries, etc.
	conan profile detect --force

repo-init: repo-deps repo-pre-commit  repo-conan-profile  ## Initialize repo by executing above commands

##==================================================================================================
##@ Building with CMake

.ONESHELL:
build:  ## Build project
	conan install . --output-folder=build --build=missing
	cd build
	source conanbuild.sh
	cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
	cmake --build .
	source deactivate_conanbuild.sh

test:  ## Run tests
	cd build && ctest

##==================================================================================================
##@ Cleaning

clean: ## Delete junk files
	find . | grep -E "\.o" | xargs rm -rf
	find . | grep -E "\.exe" | xargs rm -rf

##==================================================================================================
##@ Miscellaneous

update-pre-commit-hooks:  ## Update pre-commit hooks
	pre-commit autoupdate

create-secrets-baseline:  ## Create secrets baseline file
	poetry run detect-secrets scan > .secrets.baseline

audit-secrets-baseline:  ## Check updated .secrets.baseline file
	poetry run detect-secrets audit .secrets.baseline
	git commit .secrets.baseline --no-verify -m "build(security): update secrets.baseline"
