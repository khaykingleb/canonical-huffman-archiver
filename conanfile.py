from conan import ConanFile

class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("boost/1.82.0")
        self.requires("gtest/1.13.0")

    def build_requirements(self):
        self.tool_requires("cmake/3.27.0")
