SHELL := /bin/bash
.DEFAULT_GOAL = help

export BUILD_TYPE := Release
export VERSION := $(shell grep -m 1 version pyproject.toml | grep -e '\d.\d.\d' -o)
UNAME := $(shell uname)
DOCKER_BUILDKIT := 1

##==================================================================================================
##@ Repo initialization

prerequisite: ## Install prerequisite tools
ifeq ($(UNAME), Darwin)
	brew install llvm@18
endif

deps:  ## Install repo deps
	poetry install
.PHONY: deps

pre-commit:  ## Install pre-commit
	poetry run pre-commit install
	poetry run pre-commit install -t commit-msg
.PHONY: pre-commit

local-init: prerequisite deps pre-commit  ## Initialize local environment for development
.PHONY: local-init

##==================================================================================================
##@ Macros

build:  ## Build project
	docker compose -f docker-compose.yaml up --build
.PHONY: build

run:  ## Run bash in container
	docker run -v $(PWD):/app -it canonical-huffman-archiver:$(VERSION) /bin/bash
.PHONY: run

test:  ## Run tests
	docker run -v $(PWD):/app -it canonical-huffman-archiver:0.1.0 bash -c "cd build/tests && ctest"
.PHONY: test

##==================================================================================================
##@ Conan

conan-profile:  ## Guess a configuration set (compiler, build configuration, architecture, shared or static libraries, etc.)
	poetry run conan profile detect --force | grep -v "Detected profile:" > conanprofile.txt
	perl -pi -e 'chomp if eof' conanprofile.txt
.PHONY: conan-profile

conan-build:  ## Build project with Conan
	poetry run conan install . \
		--build=missing \
		--profile=conanprofile.txt \
		--settings=compiler.cppstd=gnu23 \
		--settings=build_type=$(BUILD_TYPE) \
	&& cd build \
	&& cmake .. \
		-DCMAKE_TOOLCHAIN_FILE=$(BUILD_TYPE)/generators/conan_toolchain.cmake \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	&& cmake --build .
.PHONY: conan-build

conan-lock:  ## Lock C++ dependencies
	poetry run conan lock create .
.PHONY: conan-lock

##==================================================================================================
##@ Miscellaneous

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

clean: ## Delete junk files
	rm -rf build .cache .venv CMakeUserPresets.json conanprofile.txt
.PHONY: clean

##==================================================================================================
##@ Helper

help:  ## Display help
	@awk 'BEGIN {FS = ":.*##"; printf "\nUsage: \033[36m\033[0m\n"} /^[a-zA-Z\.\%-]+:.*?##/ { printf "  \033[36m%-24s\033[0m %s\n", $$1, $$2 } /^##@/ { printf "\n\033[1m%s\033[0m\n", substr($$0, 5) } ' $(MAKEFILE_LIST)
.PHONY: help
